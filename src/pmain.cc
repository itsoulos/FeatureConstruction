# include <stdio.h>
# include <stdlib.h>
# include <problem.h>
# include <math.h>
# include <unistd.h>
# include <sys/times.h>
# include <get_options.h>
# include <mpi.h>
# include <pthread.h>
# include <string.h>
# include <protocol.h>
#include <population.h>
#include <nnprogram.h>
# include <string>
# include <sstream>
using namespace std;

//# define BATCH

double	server_best_error=1e+100;
double	server_xx1=0.0;
double	server_xx2=0.0;
int	server_tries=0;

void	sendMinimum();
Data best_train_x;
double best_train_error=1e+100;
int	batch_tries=0;

struct Minimum
{
	Data x;
	double y;
	int generations;
};

/** Table value holds all the minimum values **/
vector<Minimum> value;

/** Global Variables **/
int num_processes=0;
int myid=0;
int problem_dimension;
double	*tempx,*tempg;
Population *opt=NULL;
vector<bool> finishedClient;
bool allFinishedFlag=true;

char*	makeCsv(int generation,double minimum,Data x)
{
	stringstream stream;
	stream<<generation;
	stream<<",";
	for(int i=0;i<x.size();i++) stream<<x[i]<<",";
	stream<<minimum<<",";
	string s=stream.str();
	char *result=new char[s.size()+1];
	strcpy(result,s.c_str());
	return result;
}

void	decodeCsv(int &generation,double &minimum,Data &x,char *str)
{
	string input = string(str);
	istringstream ss(input);
	string token;
	getline(ss,token,',');
	generation=atoi(token.c_str());
	x.resize(problem_dimension);
	for(int i=0;i<problem_dimension;i++)
	{
		getline(ss,token,',');
		x[i]=atoi(token.c_str());	
	}
	getline(ss,token,',');
	minimum=atof(token.c_str());
}

NNprogram *myproblem;
/*
	This function starts the mpi interface
*/
void	BootMPI(int argc,char **argv)
{
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&num_processes);
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);	
	finishedClient.resize(num_processes-1);
	for(int i=0;i<finishedClient.size();i++)
		finishedClient[i]=false;
}

/*
	This function finishes the mpi interface
*/
void	FinishMPI()
{
	MPI_Finalize();
}

/** Send terminate status to the server **/
void	sendTerminate()
{
	int message=CLIENT_FINISHED;
	MPI_Send(&message,1,MPI_INT,0,CLIENT_TAG_SEND,MPI_COMM_WORLD);
	opt->newGenerationFlag=false;
	sendMinimum();
}

void	sendMinimum()
{
	Data x;
	double y;
	int message=CLIENT_SEND_MINIMUM;
	MPI_Send(&message,1,MPI_INT,0,CLIENT_TAG_SEND,MPI_COMM_WORLD);
	int generation=opt->getGeneration();
	x.resize(problem_dimension);
	opt->getMinimum(x,y);
#ifdef BATCH
	if(fabs(y)<best_train_error) 
	{
		best_train_error=fabs(y);
		best_train_x=x;		
	}
	else 
	{
		x=best_train_x;
		y=-best_train_error;
	}
#endif
	char *strMessage=makeCsv(generation,y,x);
	int strLen=strlen(strMessage);
	MPI_Send(&strLen,1,MPI_INT,0,CLIENT_TAG_LENGTH,MPI_COMM_WORLD);
	MPI_Send(strMessage,strLen+1,MPI_CHAR,0,CLIENT_TAG_STR,MPI_COMM_WORLD);
	opt->newGenerationFlag=false;
	delete[] strMessage;
}

void	recvMinimum(int client,int &generation,Data &x,double &y)
{
	int strLen;
	MPI_Status status;
  	MPI_Recv(&strLen,1,MPI_INT,client,CLIENT_TAG_LENGTH,MPI_COMM_WORLD,&status);
  	char *strMessage=new char[strLen+1];
  	MPI_Recv(strMessage,strLen+1,MPI_CHAR,client,CLIENT_TAG_STR,MPI_COMM_WORLD,&status);
  	decodeCsv(generation,y,x,strMessage);
  	delete[] strMessage;
}

void	sendMinimumToClient(int client,Data &x,double &y)
{
	int message=CLIENT_SEND_MINIMUM;
	MPI_Send(&message,1,MPI_INT,client,SERVER_TAG_MESSAGE,MPI_COMM_WORLD);
	int generation=1;
	char *strMessage=makeCsv(generation,y,x);
	int strLen=strlen(strMessage);
	MPI_Send(&strLen,1,MPI_INT,client,CLIENT_TAG_LENGTH,MPI_COMM_WORLD);
	MPI_Send(strMessage,strLen+1,MPI_CHAR,client,CLIENT_TAG_STR,MPI_COMM_WORLD);
	delete[] strMessage;
}


void	terminateAll()
{
	for(int i=1;i<num_processes;i++)
	{
		int message=SERVER_TERMINATE;
		MPI_Send(&message,1,MPI_INT,i,SERVER_TAG_MESSAGE,MPI_COMM_WORLD);
	}
}

void	Communicator()
{
	int message;
	problem_dimension=length * pattern_dimension;
	MPI_Status status;
	char *strMessage;
	int strLen;
	Data x;
	x.resize(problem_dimension);
	double y;
	int generation;
	bool termFlag;
	message=0;
	value.resize(num_processes-1);
	double variance=0.0,stopat=0.0;
	server_tries=0;
	server_xx1=0;
	server_xx2=0;
	server_best_error=1e+100;
	for(int i=0;i<value.size();i++)
	{
		value[i].y=1e+10;
		value[i].x.resize(problem_dimension);
		value[i].generations=0;
		finishedClient[i]=false;
	}

	while(true)
	{
		if(myid==0)
		{
			allFinishedFlag=true;
			for(int i=1;i<num_processes;i++)
			{
				if(finishedClient[i-1])
				{ 
					continue;
				}
				else allFinishedFlag=false;
				MPI_Recv(&message,1,MPI_INT,i,CLIENT_TAG_SEND,MPI_COMM_WORLD,&status);
				switch(message)
				{
					case CLIENT_SEND_MINIMUM:
					  recvMinimum(i,generation,x,y);
					  if(verbose==1) 
						printf("Server: received minimum from client %d is %10.5lg. Generation: %4d\n",i,fabs(y),generation);
					  value[i-1].x=x;
					  value[i-1].y=y;
					  value[i-1].generations=generation;
					  if(fabs(y)<server_best_error)
					  {
						server_best_error=fabs(y);
						stopat=variance/2.0;
					  }
					  server_xx1+=server_best_error;
					  server_xx2+=server_best_error * server_best_error;
					  server_tries++;
					  variance=server_xx2/server_tries-(server_xx1/server_tries)*(server_xx1/server_tries);
					  if(server_tries==1 || stopat<1e-10) stopat=variance/2.0;
					  if(variance<stopat && server_tries>5) {terminateAll();break;}
					  break;
					case CLIENT_FINISHED:
					 finishedClient[i-1]=true;
					 if(verbose==1) 
						printf("Server: received minimum from client %d is %10.5lg. Generation: %4d\n",i,fabs(y),generation);
					MPI_Recv(&message,1,MPI_INT,i,CLIENT_TAG_SEND,MPI_COMM_WORLD,&status);
					 recvMinimum(i,generation,x,y);
					 value[i-1].x=x;
					 value[i-1].y=y;
					 value[i-1].generations=generation;
					break;
				}
			}
			if(allFinishedFlag) 
			{
				terminateAll();
				break;
			}
		}
	}
}


void	runGenetic()
{
	MPI_Status status;
	int message;
	problem_dimension=length * pattern_dimension;
	myproblem=new NNprogram(pattern_dimension,train_file);
	Data x;
	x.resize(problem_dimension);
	double y;

	opt=new Population(chromosome_count,pattern_dimension*length,myproblem);
	opt->setSelectionRate(selection_rate);
	opt->setMutationRate(mutation_rate);
	opt->setLocalSearchParams(iterations_local_search,chromosome_local_search);
	myproblem->getModel()->setPatternDimension(pattern_dimension);
        myproblem->getModel()->setNumOfWeights(num_weights);	
	opt->Solve();
	opt->newGenerationFlag=true;
	opt->getMinimum(x,y);
	MPI_Recv(&message,1,MPI_INT,0,SERVER_TAG_MESSAGE,MPI_COMM_WORLD,&status);
	delete myproblem;
}

/** Change the random seed.**/
void	randomize()
{
	srand(myid+random_seed);
	srand48(myid+random_seed);
}


void	printRbf()
{
	const int total_runs=30;
	Rbf *rbf = new Rbf(myproblem->getMapper());
        rbf->readPatterns(train_file);
        rbf->setPatternDimension(pattern_dimension);
        for(int w=1;w<=20;w++)
        {       
                double avg_train_error=0.0;
                double avg_test_error=0.0;
                double avg_class_error=0.0;
                double avg_precision=0.0;
                double avg_recall=0.0;
                rbf->setNumOfWeights(w);
                for(int i=1;i<=total_runs;i++)
                {       
                        double d;
                        double precision=0.0,recall=0.0;
                        srand48(i);
                        d=rbf->train2();
                        avg_train_error+=d;
                        avg_test_error+=rbf->testError(test_file);
                        avg_class_error+=rbf->classTestError(test_file,precision,recall);
                        avg_precision+=precision;
                        avg_recall+=recall;
                }
                printf("TRAIN[%2d]=%10.04lg TEST[%2d]=%10.04lg CLASS[%2d]=%10.04lg%% \n",
                                w,
                                avg_train_error/total_runs,
                                w,
                                avg_test_error/total_runs,
                                w,
                                avg_class_error*100.0/total_runs);
        }
        delete rbf;

}


/** Report the train and test error **/
void	printErrors(Data &genome)
{
	/** prepei na fortosei kai o main thread to problem **/
	myproblem=new NNprogram(pattern_dimension,train_file);
	myproblem->fitness(genome);
	string program=myproblem->printF(genome);
	printf("Constructed program:\n%s",program.c_str());
	printf("Rbf error\n");
	printRbf();
	if(strlen(output_directory)>0)
	{
		char trainsave[1024];
		char testsave[1024];
		string basefile="";
        	int k=0;
        	for(k=strlen(train_file)-1;train_file[k]!='/' && k>=0;k--);
        	k=k+1;
        	for(;train_file[k]!='.';k++) basefile=basefile+train_file[k];
		sprintf(trainsave,"%s/%s%d.train",output_directory,basefile.c_str(),pattern_dimension);
        	sprintf(testsave, "%s/%s%d.test",output_directory,basefile.c_str(),pattern_dimension);
		myproblem->getModel()->print(trainsave,test_file,testsave);
	}
	delete myproblem;
}


/** Print report to the screen **/
void	printReport()
{
	double avgGeneration=0;
	double minValue=1e+100;
	double maxValue=-1e+100;
	int bestFound=0;
	printf("Report**************************************************************************\n");
	for(int i=0;i<value.size();i++)
	{
		avgGeneration+=value[i].generations;
		if(value[i].y<minValue) minValue=value[i].y;
		if(value[i].y>maxValue) maxValue=value[i].y;
	}
	for(int i=0;i<value.size();i++) 
		if(fabs(value[i].y-minValue)<1e-5) 
			bestFound++;
	printf("Average Number of generations: %20.2lf\n",avgGeneration/value.size());
	printf("Maximum Value		     : %20.5lf\n",maxValue);
	printf("Minimum Value                : %20.5lf\n",minValue);
	printf("Minimum found		     : %20.5lf%%\n",bestFound*100.0/value.size());
	printf("End Report***********************************************************************\n");
}

void	findBest(Data &x,double &y,double &generations)
{
	int minFound=0;
	y=fabs(value[0].y);
	x.resize(value[0].x.size());
	generations=value[0].generations;
	for(int i=1;i<value.size();i++)
	{
		if(fabs(value[i].y)<y)
		{
			y=fabs(value[i].y);
			minFound=i;
		}
		generations+=value[i].generations;
		printf("BEST[%d]=%lf\n",i,y);
	}
	x=value[minFound].x;
	generations/=value.size();	
}

void	printSimpleReport()
{
	Data x;
	double y;
	double generations;
	findBest(x,y,generations);	
	printf("Average number of Generations 	%8.2lf\n",generations);
	printf("Minimum fitness value 		%8.5lf\n",y);
	printErrors(x);
	
}

int batch_main(int argc, char **argv)
{
	BootMPI(argc,argv);
	parse_cmd_line(argc,argv);

	for(batch_tries=1;batch_tries<=10;batch_tries++)
	{
		random_seed=batch_tries;
		randomize();
		if(myid==0)
		{
			Communicator();
		}
		else runGenetic();
		if(myid!=0)
		{
			Data x;
			x.resize(problem_dimension);
			double y;
			opt->getMinimum(x,y);
			if( fabs(y)<best_train_error) 
			{
				best_train_error=fabs(y);
				best_train_x.resize(x.size());
				best_train_x=x;
				printf("New minimum %lf \n",y);
			}
			printf("RANDOM SEED: %4d CURRENT VALUE: %20.10lf\n",random_seed,best_train_error);
			delete opt;
			opt=NULL;
		}
	}
	if(myid==0) printSimpleReport();
	FinishMPI();
	return 0;
}

/** The main function **/
int main(int argc, char **argv)
{
#ifdef BATCH
	return batch_main(argc,argv);
#endif
	BootMPI(argc,argv);
	parse_cmd_line(argc,argv);
	randomize();
	if(myid==0)
	{
		Communicator();
	}
	else 
	{
		runGenetic();
	}
	FinishMPI();
	if(myid!=0)
	{
		delete opt;
		opt=NULL;
		pthread_exit(0);
	}
	else 
	{
		printSimpleReport();
	}
	return 0;
}
