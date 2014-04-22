/*
 * File: multi-lookup.c
 * Author: Michael Tostenson
 * Create Date: 04/09/2014
 * Update Date: 04/11/2014
 */

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
#define QUEUE_LENGTH 10
#define NUM_THREADS 5
#define MINARGS 3

pthread_t req_threads[NUM_THREADS];
pthread_t res_threads[NUM_THREADS];

pthread_mutex_t queue_lock;
pthread_mutex_t output_lock;

FILE* outputfp;

char firstipstr[INET6_ADDRSTRLEN];
int rc;
int i;

void* Requester(void*);
void* Resolver(void*);