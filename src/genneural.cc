# include <genneural.h>
# include <math.h>
# include <gensolver.h>
static double sig(double x)
{
	return 1.0/(1.0+exp(-x));
}

GenNeural::GenNeural(Mapper *m):Model(m)
{
}

double GenNeural::train1()
{
	weight.resize((pattern_dimension+2)*num_weights);
	setDimension(weight.size());
	for(int i=0;i<weight.size();i++) weight[i]=2.0*drand48()-1.0;
	for(int i=0;i<xpoint.size();i++) mapper->map(origx[i],xpoint[i]);
	MinInfo Info;
	Info.p=this;
	double v;
	v=small_tolmin(weight,Info);
	GenSolve(this,weight,v);
	return v;
}

double GenNeural::train2()
{
	weight.resize((pattern_dimension+2)*num_weights);
	setDimension(weight.size());
	for(int i=0;i<weight.size();i++) weight[i]=2.0*drand48()-1.0;
	for(int i=0;i<xpoint.size();i++) mapper->map(origx[i],xpoint[i]);
	MinInfo Info;
	Info.p=this;
	double v;
	v=tolmin(weight,Info);
//	GenSolve2(this,weight,v);
	return v;
}

double GenNeural::output(double *x)
{
	double arg=0.0,per=0.0;
	int dimension = pattern_dimension;
        for(int i=1;i<=weight.size()/(pattern_dimension+2);i++)
        {
              arg = 0.0;
              for(int j=1;j<=dimension;j++)
              {
                  int pos=(dimension+2)*i-(dimension+1)+j-1;
                  arg+=weight[pos]*x[j-1];
              }
              arg+=weight[(dimension+2)*i-1];
              per+=weight[(dimension+2)*i-(dimension+1)-1]*sig(arg);
        }
        return per;
}

void   GenNeural::getDeriv(double *x,Matrix &g)
{
  	double arg;
        double s;
        int nodes=weight.size()/(pattern_dimension+2);
	int dimension = pattern_dimension;
        double f,f2;
        for(int i=1;i<=nodes;i++)
        {
                arg = 0.0;
                for(int j=1;j<=dimension;j++)
                {
                        arg+=weight[(dimension+2)*i-(dimension+1)+j-1]*x[j-1];
                }
                arg+=weight[(dimension+2)*i-1];
                f=sig(arg);
                f2=f*(1-f);
                g[(dimension+2)*i-1]=weight[(dimension+2)*i-(dimension+1)-1]*f2;
                g[(dimension+2)*i-(dimension+1)-1]=f;
                for(int k=1;k<=dimension;k++)
                {
                        g[(dimension+2)*i-(dimension+1)+k-1]=
                             x[k-1]*f2*weight[(dimension+2)*i-(dimension+1)-1];
                }
        }
}

GenNeural::~GenNeural()
{
}
