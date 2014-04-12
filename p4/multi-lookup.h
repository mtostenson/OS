#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "util.h"
#include "queue.h"

#define USAGE "<inputFilePath> <outputFilePath>"
#define MIN_RESOLVER_THREADS 2
#define MAX_NAME_LENGTH 1025
#define MAX_INPUT_FILES 10
#define INPUTFS "%1024s"
#define QUEUE_LENGTH 50
#define THREAD_MAX 5
#define MINARGS 3

pthread_t req_threads[MAX_INPUT_FILES];
pthread_t res_threads[THREAD_MAX];

pthread_mutex_t queue_lock;
pthread_mutex_t output_lock;

FILE* outputfp;

queue q;
int res_id[THREAD_MAX];
int rc;
int i;

void* Requester(void*);
void* Resolver(void*);