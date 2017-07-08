# ifndef __GENNEURAL__H
# define __GENNEURAL__H
# include <model.h>
class GenNeural :public Model
{
	public:
		GenNeural(Mapper *m);
		virtual double train1();
		virtual double train2();
		virtual double output(double *x);
		virtual void   getDeriv(double *x,Matrix &g);
		~GenNeural();
};

# endif
