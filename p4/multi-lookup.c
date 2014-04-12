/*
 * File: multi-lookup.c
 * Author: Michael Tostenson
 * Create Date: 04/09/2014
 */

#include "multi-lookup.h"

int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;
	if(argc < MINARGS)
	{
        fprintf(stderr, "Not enough arguments: %d\n", (argc-1));
        fprintf(stderr, "Usage:\n %s %s\n", argv[0], USAGE);
        return EXIT_FAILURE;
	}
	outputfp = fopen(argv[argc-1], "w");
	
	if(!outputfp)
	{
		perror("Error opening output file.");
		return EXIT_FAILURE;
	}

	if(queue_init(&q, QUEUE_LENGTH) == QUEUE_FAILURE)
		fprintf(stderr, "Queue initialization failed.\n");
	
	pthread_mutex_init(&queue_lock, NULL);	
	
	for(i = 0; i<argc-2; i++)
	{
		rc = pthread_create(&(req_threads[i]), NULL, Requester, argv[i+1]);
		if(rc)
		{
			fprintf(stderr, "Error creating requester thread %d.\n", i);
			return EXIT_FAILURE;
		}
	}
	
	pthread_mutex_init(&output_lock, NULL);
	
	for(i = 0; i < THREAD_MAX; i++)
	{
		res_id[i] = i;
		rc = pthread_create(&(res_threads[i]), NULL, Resolver, &res_id[i]);
		if(rc)
		{
			fprintf(stderr, "Error creating resolver thread %d.\n", i);
			return EXIT_FAILURE;
		}
	}

	for(i = 0; i<argc-2; i++)
		pthread_join(req_threads[i], NULL);
	
	for(i = 0; i<THREAD_MAX; i++)
		pthread_join(res_threads[i], NULL);

    return EXIT_SUCCESS;
}

void* Requester(void* filename)
{
	FILE* inputfp = fopen((char*)filename, "r");
	char hostname[MAX_NAME_LENGTH];
	if(!inputfp)
	{
		fprintf(stderr, "Error opening input file: \"%s\"\n", (char*)filename);
		return NULL;
	}
	while(fscanf(inputfp, INPUTFS, hostname) > 0)
	{
		if(!queue_is_full(&q))
		{
			//pthread_mutex_lock(&queue_lock);
			queue_push(&q, &hostname);
			fprintf(stdout, "Pushed address \"%s\" to the queue.\n", hostname);
			//pthread_mutex_unlock(&queue_lock);
		}
		else
			usleep((rand()%100)*10000+1000000);
	}
	return NULL;
}

void* Resolver(void* arg)
{
	(void)arg;
	usleep(30000);
	fprintf(stdout, "Resolver thread %d created.\n", *(int*)arg);
	while(!queue_is_empty(&q))
	{
		//pthread_mutex_lock(&queue_lock);
		char* hostname = queue_pop(&q);
		fprintf(stdout, "Resolver thread %d popped address %s\n", *(int*)arg, hostname);
		//free(hostname);
		//pthread_mutex_unlock(&queue_lock);
	}
	fprintf(stdout, "Empty queue, thread %d quitting.", *(int*)arg);
	return NULL;
}
/* Setup Output File
outputfp = fopen(argv[argc-1], "w");




while(fscanf(inputfp, INPUTFS, hostname) > 0)
{
	dnslookup(hostname, ipstr, sizeof(ipstr));
	fprintf(outputfp, "%s: %s\n", hostname, ipstr);
}


FILE* outputfp = NULL;
char hostname[MAX_NAME_LENGTH];
char errorstr[MAX_NAME_LENGTH];
char ipstr[INET6_ADDRSTRLEN];
int i;

*/

