#include <stdio.h>
#include <math.h>
#include "mpi.h"

#define PI 3.1415926535

int main( int argc , char ** argv )
{
	int pnum, my_rank, tag = 1234, dest, src;
	MPI_Status status;
	
	long long i , num_intervals , itvl_num, start, end;
	double rect_width , area , my_sum , x_middle , sum;
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &pnum);
	
	sscanf( argv[1] , "%llu" , &num_intervals ) ;
	rect_width = PI / num_intervals ;
	
	itvl_num = num_intervals / pnum;
	
	start = 1 + itvl_num * my_rank;
	
	if(my_rank == (pnum - 1))		// 或是讓main process處理未處理的部分??
		end = num_intervals;
	else
		end = itvl_num * (my_rank + 1);
	
	//printf("%llu %llu\n", start, end);
	my_sum = 0;
	for ( i = start; i < end + 1; i ++) {
		
		/* find the middle of the interval on the X - axis . */
		
		x_middle = ( i - 0.5) * rect_width ;
		area = sin( x_middle ) * rect_width ;
		my_sum = my_sum + area ;
	}
	
	/*if(my_rank != 0){
		dest = 0;
		MPI_Send(&my_sum, 1, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD);
	}else
	{
		for(src = 1; src < pnum ;src++)
		{
			MPI_Recv(&other_sum, 1, MPI_DOUBLE, src, tag, MPI_COMM_WORLD, &status);
			my_sum += other_sum;
		}
	}*/
	
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Reduce(&my_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	
	if(my_rank == 0)
		printf( " The total area is : %f\n " , ( float ) sum ) ;
	
	MPI_Finalize();
	
	return 0;
}