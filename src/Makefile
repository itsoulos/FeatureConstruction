include ../Makefile.inc
INCLUDE = -I../include $(CFLAGS)

LIB = 
OBJS_FEAT =  symbol.o rule.o problem.o small_tolmin.o tolmin.o program.o population.o doublestack.o cprogram.o fparser.o mapper.o model.o neural.o kmeans.o matrix_functions.o Rbf.o rbf_model.o nnprogram.o gensolver.o get_options.o knn.o  pmain.o 
TARGET=fc

all:  $(TARGET)



.cc.o: 
	$(CXX) $(INCLUDE) -c $<

$(TARGET): $(OBJS_FEAT)
	$(CXX) $(OBJS_FEAT) $(LIB) -o $(TARGET)
	cp $(TARGET) ../bin

clean:
	rm -f *~   $(OBJS_FEAT) $(TARGET)
