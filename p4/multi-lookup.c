/*
 * File: multi-lookup.c
 * Author: Michael Tostenson
 * Create Date: 04/09/2014
 */

#include "multi-lookup.h"

void* QueueFile(void* filename)
{
	return NULL;
}

int main(int argc, char* argv[])
{
    FILE* inputfp = NULL;
    FILE* outputfp = NULL;
    char hostname[MAX_NAME_LENGTH];
    char errorstr[MAX_NAME_LENGTH];
    char ipstr[INET6_ADDRSTRLEN];
    int i;

	/* Thread variables */
	pthread_t threads[THREAD_MAX];
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

	/* Setup Output File */
    outputfp = fopen(argv[argc-1], "w");
    if(!outputfp)
    {
        perror("Error opening output file.");
        return EXIT_FAILURE;
    }

    for(i=1; i<argc-1; i++)
    {
		rc = pthread_create(&(threads[i]), NULL, QueueFile, hostname);

        inputfp = fopen(argv[i], "r");
        if(!inputfp)
        {
            sprintf(errorstr, "\nError opening input file: \"%s\"", argv[i]);
            perror(errorstr);
            continue;
        }

        fprintf(outputfp, "\nIP's from file \"%s\":\n", argv[i]);
        while(fscanf(inputfp, INPUTFS, hostname) > 0)
        {
            dnslookup(hostname, ipstr, sizeof(ipstr));
            fprintf(outputfp, "%s: %s\n", hostname, ipstr);
        }
    }
    return EXIT_SUCCESS;
}