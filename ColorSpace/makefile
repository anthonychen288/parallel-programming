#testing makefile
CFLAGS = -fopenmp -lpthread
INCLUDE = ./BMPlib

ctomp: ct_omp.cpp MyBMP.cpp
	g++ -g -I$(INCLUDE) ct_omp.cpp MyBMP.cpp -o ctomp $(CFLAGS)
	
clean:
	rm -f ctomp