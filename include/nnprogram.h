# ifndef __NNPROGRAM__H
# include <program.h>
# include <cprogram.h>
# include <model.h>
# include <neural.h>
# include <rbf_model.h>
# include <knn.h>
# include <mapper.h>
# include <vector>
using namespace std;

# define MODEL_NEURAL		1
# define MODEL_RBF		2
# define MODEL_KNN		3

class NNprogram	:public Program
{
	private:
		vector<string> pstring;
		vector<int>    pgenome;
		int	model_type;
		int	pattern_dimension;
		Cprogram *program;
		Model	 *model;
		Mapper	 *mapper;
	public:
		NNprogram(int pdimension,char *filename);
		string	printF(vector<int> &genome);
		virtual double 	fitness(vector<int> &genome);
		Model	*getModel();
		Mapper	*getMapper();
		~NNprogram();
};
# define __NNPROGRAM__H
# endif
