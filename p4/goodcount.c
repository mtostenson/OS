/*****************************************************************************
This program shows how to correct the race condition in badcount.c.
The program increments a shared counter (sum) in parallel with
multiple threads (executing count()).  Access to sum is protected with
the use of lock which guarantees mutual exclusion and hence sum can not
be simultaneously incremented.

The program is implemented using Pthreads.
Jeremy R. Johnson
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#define LIMIT 100
#define NUMCOUNTERS 10
#define MAXCOUNTERS 100

int sum = 0;
pthread_mutex_t lock;

void count(int *arg){
	int i;
	for(i = 0; i<*arg; i++){
		pthread_mutex_lock(&lock);
		sum++;
		pthread_mutex_unlock(&lock);
	}
}

int main(int argc, char **argv){
	int error, i;
	int numcounters = NUMCOUNTERS;
	int limit = LIMIT;
	pthread_t tid[MAXCOUNTERS];


	if(argc == 1){
		printf("usage: goodcount numcounters limit\n");
		exit(1);
	}

	if(argc == 2){
		numcounters = atoi(argv[1]);
	}
	if(argc == 3){
		numcounters = atoi(argv[1]);
		limit = atoi(argv[2]);
	}

	printf("numcounters = %d, limit = %d\n", numcounters, limit);
	pthread_setconcurrency(numcounters);
	pthread_mutex_init(&lock, NULL);

	for(i = 1; i<=numcounters; i++){
		error = pthread_create(&tid[i], NULL, (void *(*)(void *))count, &limit);
	}
	for(i = 1; i<=numcounters; i++){
		error = pthread_join(tid[i], NULL);
	}
	printf("Counters finished with count = %d\n", sum);
	printf("Count should be %d X %d = %d\n", numcounters, limit, numcounters*limit);
	return 0;
}