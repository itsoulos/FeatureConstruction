# ifndef __NNPROGRAM__H
# include <program.h>
# include <problem.h>
# include <vector>
using namespace std;

typedef void(*TRANSFORM_FUNCTION)(int,int,double*,double*);

class MetaProgram	:public Program, Problem
{
	private:
		/*
			W: είναι ο πίνακας των βαρών
			pattern: είναι ο πίνακας των προτύπων μειωμένης διαστάσεως
			cat: είναι ο πίνακας των κατηγοριών
			original: είναι τα αρχικά πρότυπα σε κανονική διάσταση
			pattern_dimension: είναι η μειωμένη διάσταση
			count_patterns: είναι το πλήθος των προτύπων
		*/
		Matrix	W;
		double	**pattern;
		double	*cat;
		double	**original;
		int	pattern_dimension;
		int	original_dimension;
		int	count_patterns;
	public:
		MetaProgram(int wcount);
		int	readFromFile(char *filename);
		void	setPatternDimension(int d);
		void	replacePattern(int pos,double *p,double c);
		int	getCountPatterns()    const;
		int	getPatternDimension() const;
		double	output(double *p);
		void	getDeriv(double *p,Matrix &g);
		string	printF(vector<int> &genome);
		double	originalError(int w);
		double	trainError(TRANSFORM_FUNCTION tf);
		double	testError(vector<int> &genome,string test_file);
		double	testError(TRANSFORM_FUNCTION tf,string test_file);
		double	testError(string test_file);

		virtual	double 	funmin(Matrix x);
		virtual void	granal(Matrix x,Matrix &g);
		virtual double 	fitness(vector<int> &genome);
		void	setNodes(Matrix x);	
		~MetaProgram();
};
# define __NNPROGRAM__H
# endif
