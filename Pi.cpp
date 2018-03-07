#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <thread>

long thread_count;
long long number_in_circle = 0;
long long n;
pthread_mutex_t mutex;
double pi = 0.0, avg = 0.0;
void* toss(void* rank);

int main(int argc, char* argv[])
{
    pthread_t* thread_handles;
    int thread;
    /* 
    system argument
        argv[1]: number of tosses
        argv[2]: threadcount
    */
    n = strtoll(argv[1], NULL, 10);

    if(argv[2] == NULL){
        thread_count = std::thread::hardware_concurrency();
        printf("Default: %d thread running.\nU can add second argument as number of threads.\n", thread_count);
    }else
		thread_count = strtol(argv[2], NULL, 10);

    pthread_mutex_init(&mutex, NULL);

	thread_handles = (pthread_t*)malloc(thread_count*sizeof(pthread_t));

	for(thread = 0;thread<thread_count;thread++)
        pthread_create(&thread_handles[thread],NULL, toss, (void*)thread);


	for(thread = 0;thread<thread_count;thread++)
        pthread_join(thread_handles[thread], NULL);

	pi = (double)4.0*number_in_circle/n;
	printf("pi: %.7f\n", pi);

    free(thread_handles);

    pthread_mutex_destroy(&mutex);

	return 0;
}

void* toss(void* rank)
{
    double dis;
    long number_in_circle_loc = 0;
    long localToss = n/thread_count;

    srand((unsigned)time(NULL)); // random seed

    for(long i = 0;i<localToss;i++){

        double x = (double)(rand()/(RAND_MAX+1.0))*2.0-1.0;
        double y = (double)(rand()/(RAND_MAX+1.0))*2.0-1.0;
        dis = x*x + y*y;
        if(dis <= 1){
            number_in_circle_loc++;     // # in circle at local
        }
    }
    // add local toss to global
    pthread_mutex_lock(&mutex);
    number_in_circle += number_in_circle_loc;
    pthread_mutex_unlock(&mutex);

    return NULL;
}
