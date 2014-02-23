// 
// Michael Tostenson 
//

using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string>
#include <limits.h>
#include <sys/time.h>

#include "helper-routines.h"

/*Define Global Variables*/
pid_t   childpid;
timeval t1, t2;
int numtests;
double elapsedTime;
double totalTime= 0;
double minTime = 0;
double maxTime = 0;

void sigusr1_handler(int sig) 
{	
	
}

void sigusr2_handler(int sig) 
{ 
	
}

void sigint_handler(int sig)
{
	
}

// printing helper routine
void printResults(int childpid, 
				  int pid, 
                  int gid, 
				  double min, 
			      double max, 
                  double total)
{
	(childpid == 0)?printf("Child"):printf("Parent");
	printf("'s Results for Pipe IPC mechanisms\n");
	printf("Process ID is %d, Group ID is %d\n", pid, gid);
	printf("Round trip times\n");
	printf("Average %f\n", total/numtests);
	printf("Maximum %f\nMinimum %f\n", max, min);
	printf("Elapsed Time %f\n", total);
}

//
// main - The main routine 
//
int main(int argc, char **argv)
{
	//Initialize Constants here
	
    //variables for Pipe
	int fd1[2],fd2[2], nbytes;	
	//byte size messages to be passed through pipes	
	char    childmsg[] = "1";
	char    parentmsg[] = "2";
	char    quitmsg[] = "q";
    
	char readbuffer[1];
	// pipe initialization
	pipe(fd1);
	pipe(fd2);
	//

    /*Three Signal Handlers You Might Need
     *
     *I'd recommend using one signal to signal parent from child
     *and a different SIGUSR to signal child from parent
     */
    Signal(SIGUSR1,  sigusr1_handler); //User Defined Signal 1
    Signal(SIGUSR2,  sigusr2_handler); //User Defined Signal 2
    Signal(SIGINT, sigint_handler);
    
    //Default Value of Num Tests
    numtests=10;
    //Determine the number of messages was passed in from command line arguments
    //Replace default numtests w/ the commandline argument if applicable 
    if(argc<2)
	{
		printf("Not enough arguments\n");
		exit(0);
	}
    
    printf("Number of Tests %d\n", numtests);
    // start timer
	// gettimeofday(&t1, NULL); 
	if(strcmp(argv[1],"-p")==0)
	{
		int currentTest = 0;
		//code for benchmarking pipes over numtests
		if((childpid = fork()) == -1)
		{
			perror("fork");
			exit(1);
		}
		if(childpid != 0)
		{
			close(fd1[1]); // Parent reads from fd1
			close(fd2[0]); // Parent writes to fd2
		}
		else
		{	
			close(fd1[0]); // Child writes to fd1
			close(fd2[1]); // Child reads from fd2
		}
		if(childpid != 0)
		{	
			while(currentTest < numtests)
			{
				if(currentTest == 2)
					write(fd2[1], quitmsg, 2);
				printf("Parent test %d\n", currentTest);
				gettimeofday(&t1, NULL); 
				//printf("Child sending message...\n");
				write(fd2[1], parentmsg, 2);
				nbytes = read(fd1[0], readbuffer, 1);	
				//printf("Parent received msg: %s\n", readbuffer);
				currentTest++;
			}
			printf("QUITTING\n");
			write(fd2[1], quitmsg, 2);
		}
		else
		{
			while(true)
			{
				printf("Child test %d\n", currentTest);
				nbytes = read(fd2[0], readbuffer, 1);
				printf("readbuffer: %s\n",readbuffer);				
				if(strcmp(readbuffer, quitmsg)==0)
				{
					printf("quitmsg received\n");
					break;
				}
				//printf("Child received msg: %s\n", readbuffer);
				gettimeofday(&t1, NULL); 
				//printf("Child sending message...\n");
				write(fd1[1], childmsg, 2);
				currentTest++;
			}
		}		
		printf("END\n");
		// stop timer
		gettimeofday(&t2, NULL);
		// compute and print the elapsed time in millisec
		elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
		elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
		//printf("Elapsed Time %f\n", elapsedTime);
		totalTime += elapsedTime;
		if(elapsedTime > maxTime)
			maxTime = elapsedTime;
		if(elapsedTime < minTime || minTime == 0)
			minTime = elapsedTime;
			
		//printResults(childpid, getpid(), getgid(), minTime, maxTime, totalTime);
	}


	if(strcmp(argv[1],"-s")==0)
	{
		//code for benchmarking signals over numtests
		
		
		// stop timer
		gettimeofday(&t2, NULL);

		// compute and print the elapsed time in millisec
		elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
		elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
		printf("Elapsed Time %f\n", elapsedTime);
	}
}
  










