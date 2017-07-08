# ifndef __FC_GETOPTIONS__H
# define __FC_GETOPTIONS__H
extern int	chromosome_count;
extern int	length;
extern int	iterations_local_search;
extern int 	chromosome_local_search;
extern double	selection_rate;
extern double	mutation_rate;
extern int	pattern_dimension;
extern int	num_weights;
extern char	train_file[1024];
extern char	test_file[1024];
extern char	output_directory[1024];
extern int	generations;
extern int	random_seed;
extern int	verbose;
extern void	parse_cmd_line(int argc,char **argv);
# endif
