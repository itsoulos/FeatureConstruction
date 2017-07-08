# include <unistd.h>
# include <string.h>
# include <stdio.h>
# include <stdlib.h>

int	chromosome_count;
int	iterations_local_search;
int	chromosome_local_search;
int	length;
double	selection_rate;
double	mutation_rate;
int	pattern_dimension;
int	num_weights;
char	train_file[1024];
char	test_file[1024];
int	generations;
int 	random_seed;
int	verbose;
char 	output_directory[1024];
void	parse_cmd_line(int argc,char **argv);

const char *short_options="c:l:s:m:d:w:i:p:t:g:o:r:v:";

void	print_usage()
{
	printf("\t-h	Print this help screen.\n"
			"\t-c	Specify population count. Default value: 500\n"
			"\t-d	Specify pattern dimension. Default value: 1\n"
			"\t-f	Specify chromosomes for local search. Default value: 20\n"
			"\t-g	Specify number of generations. Default value: 200\n"
			"\t-i	Specify iterations for local search application. Default value: 50\n"
			"\t-l	Specify	population length. Default value: 100\n"
			"\t-m	Specify mutation rate. Default value: 0.05\n"
			"\t-o	Specify output directory. Default value: \n"
			"\t-p	Specify train file. Default value: \n"
			"\t-s	Specify selection rate. Default value: 0.1\n"
			"\t-t	Specify test file. Default value: \n"
			"\t-w	Specify number of weights. Default value: 1\n"
			"\t-v	Specify verbose output. Default value: 0\n"
			"\t-r	Specify random seed. Default value: 1\n");
}

void	parse_cmd_line(int argc,char **argv)
{
	if(argc==1)
	{
		print_usage();
		exit(1);
	}
	int next_option;
	/*	DEFAULT VALUES
	 * */
	chromosome_count=500;
	length=100;
	selection_rate=0.1;mutation_rate=0.05;
	pattern_dimension=1;
	num_weights=1;
	strcpy(train_file,"");
	strcpy(test_file,"");
	strcpy(output_directory,"");
	generations=200;
	random_seed= 1;
	iterations_local_search=50;
	chromosome_local_search=20;
	verbose=0;
	do
	{
		next_option=getopt(argc,argv,short_options);
		switch(next_option)
		{
			case 'c':
				chromosome_count=atoi(optarg);
				break;
			case 'l':
				length=atoi(optarg);
				break;
			case 'f':
				chromosome_local_search=atoi(optarg);
				break;
			case 's':
				selection_rate=atof(optarg);
				break;
			case 'm':
				mutation_rate=atof(optarg);
				break;
			case 'd':
				pattern_dimension=atoi(optarg);
				break;
			case 'i':
				iterations_local_search=atoi(optarg);
				break;
			case 'g':
				generations=atoi(optarg);
				break;
			case 'r':
				random_seed=atoi(optarg);
				break;
			case 'w':
				num_weights=atoi(optarg);
				break;
			case 'p':
				strcpy(train_file,optarg);
				break;
			case 't':
				strcpy(test_file,optarg);
				break;
			case 'o':
				strcpy(output_directory,optarg);
				break;
			case 'v':
				verbose=atoi(optarg);
				break;
			case -1:
				break;
			case '?':
				print_usage();
				exit(1);
				break;
			default:
				print_usage();
				exit(1);
				break;
		}
	}while(next_option!=-1);
}
