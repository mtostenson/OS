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
timeval t1, t2, rt1, rt2;
int numtests;
double elapsedTime = 0,
	   totalTripTime = 0,
	   rtTime = 0,
	   minTime = 0,
	   maxTime = 0;

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
                  double total,
				  double elapsed)
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
	char 	readbuffer[2];
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
			gettimeofday(&rt1, NULL);
			write(fd2[1], parentmsg, strlen(readbuffer)+1);
			while(currentTest < numtests)
			{
				nbytes = read(fd1[0], readbuffer, sizeof(readbuffer));
				if(strcmp(readbuffer, childmsg)==0)
				{
					gettimeofday(&rt2, NULL);
					memset(readbuffer,0,strlen(readbuffer));
					rtTime = (rt2.tv_sec - rt1.tv_sec)*1000.0;
					rtTime += (rt2.tv_usec - rt1.tv_usec)/1000.0;
					totalTripTime += rtTime;
					if(rtTime > maxTime)
						maxTime = rtTime;
					if(rtTime < minTime || minTime == 0)
						minTime = rtTime;
					currentTest++;						
					gettimeofday(&rt1, NULL);
     				write(fd2[1], parentmsg, strlen(readbuffer)+1);
				}
			}
			write(fd2[1], quitmsg, 2);
		}
		else
		{
			gettimeofday(&rt1, NULL);
			while(true)
			{
				if(strcmp(readbuffer, quitmsg)==0)
				{
					printf("QUITTING\n");
					break;
				}
				nbytes = read(fd2[0], readbuffer, sizeof(readbuffer));
				if(strcmp(readbuffer, parentmsg)==0)
				{
					printf("check\n");
					gettimeofday(&rt2, NULL);
					memset(readbuffer,0,strlen(readbuffer));
					rtTime = (rt2.tv_sec - rt1.tv_sec)*1000.0;
					rtTime += (rt2.tv_usec - rt1.tv_usec)/1000.0;
					totalTripTime += rtTime;
					if(rtTime > maxTime)
						maxTime = rtTime;
					if(rtTime < minTime || minTime == 0)
						minTime = rtTime;
					gettimeofday(&rt1, NULL);
					write(fd1[1], childmsg, strlen(readbuffer)+1);
				}
			}
		}		
		// stop timer
		gettimeofday(&t2, NULL);
		// compute and print the elapsed time in millisec
		elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
		elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
		printResults(childpid, getpid(), getgid(), minTime, maxTime, totalTripTime, elapsedTime);
		exit(0);	
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
  










