#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

int isprime(int n)
{
	int i, squareroot;
	if(n > 10)
	{
		squareroot = (int) sqrt(n);
		for(i = 3;i <= squareroot;i = i + 2)
			if((n%i) == 0) return 0;
		
		return 1;
	}else
		return 0;
}

int main(int argc, char *argv[])
{
	int my_rank, pnum, src, dest, tag = 1234;
	int interval, lower, upper;		// calculating interval
	MPI_Status status;
	
	int my_pc,			// prime counter
		pc,
		my_foundone,	// most recent prime found
		foundone;
		
	long long int n , limit;
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &pnum);
	
	sscanf( argv[1] , "%llu", &limit );
	
	interval = (limit - 11) / pnum;
	
	lower = 11 + my_rank*interval;	// must be odd??
	if(lower%2 == 0)
		lower++;
	
	if(my_rank == (pnum - 1))
		upper = limit;
	else
		upper = 11 + (my_rank+1)*interval;
	
	
	if(my_rank == 0)
	{
		printf(" Starting . Numbers to be scanned = %lld\n", limit );
		my_pc =4; /* Assume (2 ,3 ,5 ,7) are counted here */
	}else
		my_pc = 0;
	
	for ( n =lower; n <= upper ; n = n +2) {
		if ( isprime( n ) ) {
			my_pc ++;
			my_foundone = n ;
		}
	}
	//printf("%d %d\n", my_pc, my_foundone);
	/*if(my_rank != 0)
	{
		dest = 0;
		MPI_Send(&my_pc, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
		// Sending pc
		if(my_rank == (pnum - 1))
			MPI_Send(&my_foundone, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
	}else
	{
		for(src = 1;src < pnum;src++)
		{
			// receive pc
			MPI_Recv(&other_pc, 1, MPI_INT, src, tag, MPI_COMM_WORLD, &status);
			// summing pc
			my_pc += other_pc;
			
		}
		// receiving max foundone
		MPI_Recv(&my_foundone, 1, MPI_INT, (pnum - 1), tag, MPI_COMM_WORLD, &status);
	}*/
	
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Reduce(&my_pc, &pc, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&my_foundone, &foundone, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
	
	if(my_rank == 0)
		printf( " Done . Largest prime is %d \n Total primes %d\n", foundone , pc ) ;
	
	MPI_Finalize();
	return 0;

}