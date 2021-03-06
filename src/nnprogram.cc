# include <nnprogram.h>
NNprogram::NNprogram(int pdimension,char *filename)
{
	FILE *fp=fopen(filename,"r");
	if(!fp) return;
	int d;
	fscanf(fp,"%d",&d);
	mapper=new Mapper(d);
	fclose(fp);	
	model = new Rbf(mapper);
	pattern_dimension = pdimension;
	program = new Cprogram(d,pdimension);
	setStartSymbol(program->getStartSymbol());
	model->readPatterns(filename);
	pstring.resize(pattern_dimension);
	pgenome.resize(0);
}

string	NNprogram::printF(vector<int> &genome)
{
	string ret="";
	if(pgenome.size()!=genome.size()/pattern_dimension)
		pgenome.resize(genome.size()/pattern_dimension);
	for(int i=0;i<pattern_dimension;i++)
	{
		for(int j=0;j<genome.size()/pattern_dimension;j++)
			pgenome[j]=genome[i*genome.size()/pattern_dimension+j];
		int redo=0;
		pstring[i]=printRandomProgram(pgenome,redo);
		if(redo>=REDO_MAX) return "";
		char str[10];
		sprintf(str,"f%d(x)=",i+1);
		ret+=str;
		ret+=pstring[i];
		ret+="\n";
	}
	return ret;
}

double 	NNprogram::fitness(vector<int> &genome)
{
	double value=0.0;
	if(pgenome.size()!=genome.size()/pattern_dimension)
		pgenome.resize(genome.size()/pattern_dimension);
	for(int i=0;i<pattern_dimension;i++)
	{
		for(int j=0;j<pgenome.size();j++)
			pgenome[j]=genome[i*genome.size()/pattern_dimension+j];
		int redo=0;
		pstring[i]=printRandomProgram(pgenome,redo);
		if(redo>=REDO_MAX) return -1e+8;
	}
	mapper->setExpr(pstring);
	extern int random_seed;
	srand48(random_seed);
	model->setPatternDimension(pattern_dimension);
	if(model_type==MODEL_NEURAL) model->randomizeWeights();
	value=model->train1();
	if(isnan(value) || isinf(value)) return -1e+8;
	return -value;
}

Model	*NNprogram::getModel()
{
	return model;
}

Mapper	*NNprogram::getMapper()
{
	return mapper;
}

NNprogram::~NNprogram()
{
	delete program;
	delete model;
	delete mapper;
}

