/**********************************************************************
 * DESCRIPTION:
 *   Serial Concurrent Wave Equation - C Version
 *   This program implements the concurrent wave equation
 *********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAXPOINTS 1000000
#define MAXSTEPS 1000000
#define MINPOINTS 20
#define PI 3.14159265

void check_param(void);
//void init_line(void);
//void update (void);
void printfinal (void);
void InitUpdateOnDevice(void );
__global__ void updateKer(float* , int ,int );

int nsteps,                 	/* number of time steps */
    tpoints, 	     		/* total points along string */
    rcode;                  	/* generic return code */
float  values[MAXPOINTS+2], 	/* values at time t */
       oldval[MAXPOINTS+2], 	/* values at time (t-dt) */
       newval[MAXPOINTS+2]; 	/* values at time (t+dt) */


/**********************************************************************
 *	Checks input values from parameters
 *********************************************************************/
void check_param(void)
{
   char tchar[20];

   /* check number of points, number of iterations */
   while ((tpoints < MINPOINTS) || (tpoints > MAXPOINTS)) {
      printf("Enter number of points along vibrating string [%d-%d]: "
           ,MINPOINTS, MAXPOINTS);
      scanf("%s", tchar);
      tpoints = atoi(tchar);
      if ((tpoints < MINPOINTS) || (tpoints > MAXPOINTS))
         printf("Invalid. Please enter value between %d and %d\n", 
                 MINPOINTS, MAXPOINTS);
   }
   while ((nsteps < 1) || (nsteps > MAXSTEPS)) {
      printf("Enter number of time steps [1-%d]: ", MAXSTEPS);
      scanf("%s", tchar);
      nsteps = atoi(tchar);
      if ((nsteps < 1) || (nsteps > MAXSTEPS))
         printf("Invalid. Please enter value between 1 and %d\n", MAXSTEPS);
   }

   printf("Using points = %d, steps = %d\n", tpoints, nsteps);

}

/**********************************************************************
 *     Initialize points on line
 *********************************************************************/

/**********************************************************************
 *      Calculate new values using wave equation
 *********************************************************************/


/**********************************************************************
 *     Update all values along line a specified number of times
 *********************************************************************/

void InitUpdateOnDevice()
{
	float *vald;
	long long size = tpoints * sizeof(float);
	
	// allocate mem space and write to device
	cudaMalloc(&vald, size);
	//cudaMemcpy(vald, values, size, cudaMemcpyHostToDevice);

	// kernel function
    int dimBlock = 20;
    int dimGrid = tpoints / dimBlock;
	updateKer<<<dimGrid, dimBlock>>>(vald, nsteps, tpoints);

	// read values from device
	cudaMemcpy(values, vald, size, cudaMemcpyDeviceToHost);
	// free mem space
	cudaFree(vald);
}

__global__ void updateKer(float* vald, int nsteps, int tpoints)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x + 1, n;
	float newval, oldval, val;
	__shared__ float fac, k, tmp;
	__shared__ float dtime, c, dx;
	__shared__ float tau, sqtau;

	fac = 2.0 * PI; k = i - 1; tmp = tpoints - 1;
    
    val = sin(fac * k / tmp);

    dtime = 0.3;c = 1.0;dx = 1.0;
    tau = (c * dtime / dx); sqtau = tau * tau;
	//val = vald[i];
    oldval = val;


	for(n = 1;n<=nsteps;n++)
	{
		if((i == 1) || (i == tpoints))
			newval = 0.0;
		else{
			newval = (2.0 * val) - oldval + (sqtau *  (-2.0)*val);
        }
		oldval = val;
		val = newval;
	}

    vald[i] = val;

}

/**********************************************************************
 *     Print final results
 *********************************************************************/
void printfinal()
{
   int i;

   for (i = 1; i <= tpoints; i++) {
      printf("%6.4f ", values[i]);
      if (i%10 == 0)
         printf("\n");
   }
}


/**********************************************************************
 *	Main program
 *********************************************************************/
int main(int argc, char *argv[])
{
	sscanf(argv[1],"%d",&tpoints);
	sscanf(argv[2],"%d",&nsteps);
	check_param();
	printf("Initializing points on the line...\n");
	//init_line();
	printf("Updating all points for all time steps...\n");
	InitUpdateOnDevice();
	printf("Printing final results...\n");
	printfinal();
	printf("\nDone.\n\n");
	
	return 0;
}