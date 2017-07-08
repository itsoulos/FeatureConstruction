# include <rbf_model.h>
//# define CLASSMODEL

Rbf::Rbf(Mapper *m)
	:Model(m)
{
	centers = NULL;
	variances = NULL;
	weights = NULL;
	input  = NULL;
	weight.resize(0);
}

double *Output;

double Rbf::train1()
{
	int noutput=1;
	#ifdef CLASS
		noutput=2;
	#endif
	if(weight.size() != noutput * num_weights)
	{
		weight.resize(num_weights*noutput);
		setDimension(num_weights*noutput);
		if(centers)
		{
		delete[] centers;
		delete[] variances;
		delete[] weights;
		delete[] input;
		}
		centers = new double[num_weights * pattern_dimension];
		variances = new double[num_weights * pattern_dimension];
		weights = new double[num_weights*noutput];
		input = new double[pattern_dimension*xpoint.size()];
	}
	Output=new double[noutput * xpoint.size()];

	Matrix xx;
	xx.resize(original_dimension);
	for(int i=0;i<xpoint.size();i++) 
	{
		int d=mapper->map(origx[i],xpoint[i]);
		if(!d) 
		{
			delete[] Output;
			return 1e+8;
		}
		for(int j=0;j<pattern_dimension;j++)
		{
			
			input[i*pattern_dimension+j]=xpoint[i][j];
			if(fabs(xpoint[i][j])>=1e+10 || isnan(xpoint[i][j]) || isinf(xpoint[i][j]))
			{
				delete[] Output;
				return 1e+8;
			}
		}
		if(!d)  
		{
			delete[] Output;
			return 1e+8;
		}
#ifndef CLASS
		Output[i]=ypoint[i];
#endif
	}
#ifdef CLASS
	for(int i=0;i<ypoint.size();i++)
	{
		Output[i*2+0]=ypoint[i]>0?0:1;
		Output[i*2+1]=ypoint[i]>0?1:0;
	}
#endif


        Kmeans(input,centers,variances,
			xpoint.size(),pattern_dimension,num_weights);
	
        int icode=train_rbf(pattern_dimension,num_weights,noutput,xpoint.size(),
			centers,variances,weights,input,Output);
	double v =0.0;
#ifndef CLASS
	#ifndef CLASSMODEL
	v=funmin(weight);
	#else
		v=classTrainError();
	#endif
#else
	for(int i=0;i<ypoint.size();i++)
	{
		double pattern[2];
		if(ypoint[i]>0) {pattern[0]=0;pattern[1]=1;}
		else            {pattern[0]=1;pattern[1]=0;}
		
		double outv[noutput];
		double *xt=new double[pattern_dimension];
		for(int j=0;j<pattern_dimension;j++) xt[j]=xpoint[i][j];
		create_rbf(pattern_dimension,num_weights,noutput,
				centers,variances,weights,xt,outv);
		v+=pow(outv[0]-pattern[0],2.0)+pow(outv[1]-pattern[1],2.0);
		delete[] xt;
	}
#endif
	delete[] Output;
	if(icode==1) return 1e+8;
	/**/
	/**/
	return v;
}

double Rbf::train2()
{
	return train1();
}

double Rbf::output(Matrix x)
{
	int noutput=1;
#ifdef CLASS
	noutput=2;
#endif
	double v[noutput];
	double *xt=new double[x.size()];
	double penalty=0.0;
	for(int i=0;i<x.size();i++) 
	{
		xt[i]=x[i];
		penalty+=(fabs(x[i])>=10.0)*1000.0;
	}
	create_rbf(pattern_dimension,num_weights,noutput,
			centers,variances,weights,xt,v);
	delete[] xt;
#ifndef CLASS
	return v[0];
#else
	return (v[0]>v[1]?0:1);	
#endif
}

void   Rbf::getDeriv(Matrix x,Matrix &g)
{
}

Rbf::~Rbf()
{
	delete[] centers;
	delete[] variances;
	delete[] weights;
	delete[] input;

}
