/*
 * File: multi-lookup.c
 * Author: Michael Tostenson
 * Create Date: 04/09/2014
 */

#include "multi-lookup.h"

void* PushFileNames(void* filename)
{
	fprintf(stdout, " Thread holds filename: %s\n", (char*)filename);
	while(queue_is_full(&q))
		usleep((rand()%100)*10000+1000000);	
	
	queue_push(&q, filename);

	/* Exiting */
	return NULL;
}

int main(int argc, char* argv[])
{
	/* void unused vars */
	(void)argc;
	(void)argv;

	int i;
	/* Thread variables */
	
	int rc;

	/* Check Arguments */
	if(argc < MINARGS)
    {
        fprintf(stderr, "Not enough arguments: %d\n", (argc-1));
        fprintf(stderr, "Usage:\n %s %s\n", argv[0], USAGE);
        return EXIT_FAILURE;
    }

	/* Initialize queue */
	i = queue_init(&q, MAX_INPUT_FILES);


	for(i = 0; i<argc-2; i++)
	{
		rc = pthread_create(&(req_threads[i]), NULL, PushFileNames, argv[i+1]);
		if(rc)
			fprintf(stderr, "Error creating thread %d.\n", i);
		else
			fprintf(stdout, "Successfully created thread %d.\n", i);
	}
	for(i = 0; i<argc-2; i++)
		pthread_join(req_threads[i], NULL);
    return EXIT_SUCCESS;
}


/* THIS HAPPENS LATER
FILE* inputfp = fopen(*filename, "r");
if(!inputfp)
{
	sprintf(errorstr, "\nError opening input file: \"%s\"", argv[i]);
	perror(errorstr);
	continue;
}*/

/* Setup Output File
outputfp = fopen(argv[argc-1], "w");
if(!outputfp)
{
	perror("Error opening output file.");
	return EXIT_FAILURE;
}



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

