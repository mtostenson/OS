#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

#define QUEUE_SIZE 10
#define SBUFSIZE 1025

int main(int argc, char* argv[])
{
	queue q;
	char hostname[SBUFSIZE];
	FILE* inputfp = fopen("input/names1.txt", "r");
	
	queue_init(&q, QUEUE_SIZE);
	
	while(fscanf(inputfp, INPUTFS, hostname) > 0)
	{
		queue_push(&q, &hostname);
		fprintf(stdout, "Pushing: \"%s\"\n", hostname);
	}
	
	return EXIT_SUCCESS;
}