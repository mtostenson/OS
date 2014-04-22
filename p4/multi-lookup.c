/*
 * File: multi-lookup.c
 * Author: Michael Tostenson
 * Create Date: 04/09/2014
 * Update Date: 04/11/2014
 */

#include "multi-lookup.h"

// Global queue
int running = 1;
queue q;

int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;

	// Check command line arguments
	if(argc < MINARGS)
	{
        fprintf(stderr, "Not enough arguments: %d\n", (argc-1));
        fprintf(stderr, "Usage:\n %s %s\n", argv[0], USAGE);
        return EXIT_FAILURE;
	}
	
	// Open file for writing output
	outputfp = fopen(argv[argc-1], "w");
	if(!outputfp)
	{
		perror("Error opening output file.");
		return EXIT_FAILURE;
	}

	// Initializing queue
	if(queue_init(&q, QUEUE_LENGTH) == QUEUE_FAILURE)
		fprintf(stderr, "Queue initialization failed.\n");
	
	// Initialize queue mutex
	pthread_mutex_init(&queue_lock, NULL);

	// Set up requester thread pool
	for(i = 0; i< argc-2; i++)
	{
		rc = pthread_create(&(req_threads[i]), NULL, Requester, argv[i+1]);
		if(rc)
		{
			fprintf(stderr, "Error creating requester thread %d.\n", i);
			return EXIT_FAILURE;
		}
	}

	// Initialize output file mutex
	pthread_mutex_init(&output_lock, NULL);

	// Set up resolver thread pool
	for(i = 0; i < NUM_THREADS; i++)
	{
		rc = pthread_create(&(res_threads[i]), NULL, Resolver, outputfp);
		if(rc)
		{
			fprintf(stderr, "Error creating resolver thread %d.\n", i);
			return EXIT_FAILURE;
		}
	}

	// Wait for requester threads to finish
	for(i = 0; i < NUM_THREADS; i++)
		pthread_join(req_threads[i], NULL);

	running = 0;

	// Wait for resolver threads to finish
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
		
		// Wait if queue is full
		while(queue_is_full(&q))
			usleep((rand()%100)*10000+1000000);
		
		// Lock the queue for push
		pthread_mutex_lock(&queue_lock);
		queue_push(&q, new_string);
		pthread_mutex_unlock(&queue_lock);
	}
	fclose(inputfp);
	return EXIT_SUCCESS;
}

void* Resolver(void* outfile)
{
	while(running == 1 || !queue_is_empty(&q))
	{
		if(!queue_is_empty(&q))
		{
			// Lock the queue for pop
			pthread_mutex_lock(&queue_lock);
			char* hostname = queue_pop(&q);
			pthread_mutex_unlock(&queue_lock);
			
			// Lookup
			if(hostname == NULL)
				return EXIT_SUCCESS;
		    if(dnslookup(hostname, firstipstr, sizeof(firstipstr)) == UTIL_FAILURE)
		    {
				fprintf(stderr, "dnslookup error: %s\n", hostname);
				strncpy(firstipstr, "", sizeof(firstipstr));
			}
			
			// Lock output file while writing
			pthread_mutex_lock(&output_lock);
			fprintf((FILE*)outfile, "%s %s\n", hostname, firstipstr);
			pthread_mutex_unlock(&output_lock);
		}
		else
			usleep((rand()%100)*10000+1000000);			
	}
	return EXIT_SUCCESS;
}