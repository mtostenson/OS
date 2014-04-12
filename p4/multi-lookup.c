/*
 * File: multi-lookup.c
 * Author: Michael Tostenson
 * Create Date: 04/09/2014
 */

#include "multi-lookup.h"

queue q;

int main(int argc, char* argv[])
{
	/* Checking arguments */
	(void)argc;
	(void)argv;
	if(argc < MINARGS)
	{
        fprintf(stderr, "Not enough arguments: %d\n", (argc-1));
        fprintf(stderr, "Usage:\n %s %s\n", argv[0], USAGE);
        return EXIT_FAILURE;
	}
	
	/* Setting up output file */
	outputfp = fopen(argv[argc-1], "w");
	if(!outputfp)
	{
		perror("Error opening output file.");
		return EXIT_FAILURE;
	}

	/* Initializing queue */
	if(queue_init(&q, QUEUE_LENGTH) == QUEUE_FAILURE)
		fprintf(stderr, "Queue initialization failed.\n");
	
	/* Setting up requester thread pool */
	for(i = 0; i< argc-2; i++)
	{
		rc = pthread_create(&(req_threads[i]), NULL, Requester, argv[i+1]);
		if(rc)
		{
			fprintf(stderr, "Error creating requester thread %d.\n", i);
			return EXIT_FAILURE;
		}
	}
	
	/* Setting up resolver thread pool */
	for(i = 0; i < NUM_THREADS; i++)
	{
		rc = pthread_create(&(res_threads[i]), NULL, Resolver, outputfp);
		if(rc)
		{
			fprintf(stderr, "Error creating resolver thread %d.\n", i);
			return EXIT_FAILURE;
		}
	}

	/* Waiting for requester threads to finish */
	for(i = 0; i < NUM_THREADS; i++)
		pthread_join(req_threads[i], NULL);

	/* Waiting for resolver threads to finish */
	for(i = 0; i < NUM_THREADS; i++)
		pthread_join(res_threads[i], NULL);

	queue_cleanup(&q);
	fclose(outputfp);    
	return EXIT_SUCCESS;
}

void* Requester(void* filename)
{
	char hostname[MAX_NAME_LENGTH];
	char* new_string;
	FILE* inputfp = fopen((char*)filename, "r");	
	if(!inputfp)
	{
		fprintf(stderr, "Error opening input file: \"%s\"\n", (char*)filename);
		return NULL;
	}
	
	while(fscanf(inputfp, INPUTFS, hostname) > 0)
	{
		new_string = malloc(MAX_NAME_LENGTH);
		strncpy(new_string, hostname, MAX_NAME_LENGTH);
		while(queue_is_full(&q))
		{
			usleep((rand()%100)*10000+1000000);
			fprintf(stdout, "Full queue\n");
		}
		queue_push(&q, new_string);
		fprintf(stdout, "Pushed \"%s\"\n", new_string);
	}
	fclose(inputfp);
	return EXIT_SUCCESS;
}

void* Resolver(void* arg)
{
	(void)arg;
	usleep(10000);
	while(!queue_is_empty(&q))
	{
		char* hostname = queue_pop(&q);
		if(hostname == NULL)
			return EXIT_SUCCESS;
		dnslookup(hostname, firstipstr, sizeof(firstipstr));
		fprintf(stdout, "%s %s\n", hostname, firstipstr);
	}
	return EXIT_SUCCESS;
}