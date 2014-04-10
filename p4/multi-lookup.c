/*
 * File: multi-lookup.c
 * Author: Michael Tostenson
 * Create Date: 04/09/2014
 */

#include "multi-lookup.h"

int main(int argc, char* argv[])
{
    FILE* inputfp = NULL;
    FILE* outputfp = NULL;
    char hostname[SBUFSIZE];
    char errorstr[SBUFSIZE];
    char ipstr[INET6_ADDRSTRLEN];
    int i;
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

    for(i=1; i<argc-1; i++)
    {
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