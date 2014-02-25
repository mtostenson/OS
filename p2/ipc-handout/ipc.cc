// 
// Michael Tostenson 
// Operating Systems: Project 2
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
bool looping = true;
sigset_t sigmask;

void sigusr1_handler(int sig) // Child catches this
{
    gettimeofday(&rt2, NULL);
    rtTime = (rt2.tv_sec - rt1.tv_sec)*1000.0;
    rtTime += (rt2.tv_usec - rt1.tv_usec)/1000.0;
    totalTripTime += rtTime;
    if(rtTime > maxTime)
        maxTime = rtTime;
    if(rtTime < minTime || minTime == 0)
        minTime = rtTime;
    gettimeofday(&rt1, NULL);
    kill(getppid(), SIGUSR2);
}

void sigusr2_handler(int sig) // Parent catches this
{
	gettimeofday(&rt2, NULL);
	rtTime = (rt2.tv_sec - rt1.tv_sec)*1000.0;
	rtTime += (rt2.tv_usec - rt1.tv_usec)/1000.0;
	totalTripTime += rtTime;
	if(rtTime > maxTime)
		maxTime = rtTime;
	if(rtTime < minTime || minTime == 0)
		minTime = rtTime;
	gettimeofday(&rt1, NULL);
	kill(childpid, SIGUSR1);
}

void sigint_handler(int sig)
{
	looping = false;
	// Exit(0) will happen after it prints
}

// printing helper routine
void printResults(int childpid, 
		  int pid, 
                  int gid, 
		  double min, 
	          double max, 
		  double elapsed,
		  bool testType)
{
	(childpid == 0)?printf("Child"):printf("Parent");
	printf("'s Results for ");
	testType?printf("Symbol"):printf("Pipe");
	printf(" IPC mechanisms\n");
	printf("Process ID is %d, Group ID is %d\n", pid, gid);
	printf("Round trip times\n");
	printf("Average %f\n", elapsed/numtests);
	printf("Maximum %f\nMinimum %f\n", max, min);
	printf("Elapsed Time %f\n", elapsed);
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
	if(argc > 2)
		numtests = atoi(argv[2]);
	else
	    numtests=10000;
    //Determine the number of messages was passed in from command line arguments
    //Replace default numtests w/ the commandline argument if applicable 
    if(argc<2)
	{
		printf("Not enough arguments\n");
		exit(0);
	}
    
    printf("Number of Tests %d\n", numtests);
    // start timer
	gettimeofday(&t1, NULL);
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
		if(childpid == 0)
		{	
			gettimeofday(&rt1, NULL);
			while(strcmp(readbuffer, quitmsg) != 0)
			{
				nbytes = read(fd2[0], readbuffer, sizeof(readbuffer));
				if(strcmp(readbuffer, parentmsg)==0)
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
					gettimeofday(&rt1, NULL);
					write(fd1[1], childmsg, strlen(readbuffer)+1);
				}
			}
		}
		else
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
					if(currentTest == numtests)
	     				write(fd2[1], quitmsg, strlen(readbuffer)+1);
					else
     					write(fd2[1], parentmsg, strlen(readbuffer)+1);
				}
			}
			wait(0);
		}	
		gettimeofday(&t2, NULL);
		elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
		elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
		printResults(childpid, 
			     getpid(), 
			     getgid(), 
			     minTime, 
			     maxTime, 
			     elapsedTime,
			     false);
		exit(0);	
	}

	if(strcmp(argv[1],"-s")==0)
	{
		int currentTest = 0;
		if ((childpid = fork()) == -1)
		{
			perror("fork");
			exit(1);
		}
		if (childpid == 0)
		{
			sigaddset(&sigmask, SIGUSR2);
		}
		else
		{
			sigaddset(&sigmask, SIGUSR1);
			sigaddset(&sigmask, SIGINT);
		}
		if(childpid == 0)
		{
			maxTime = 0;
			gettimeofday(&rt1, NULL);
			while(looping)
			{
				sigsuspend(&sigmask);
			}
		}
		else
		{
			maxTime = 0;
			gettimeofday(&rt1, NULL);
			kill(childpid, SIGUSR1);
			while(currentTest < numtests)
			{
				sigsuspend(&sigmask);
				currentTest++;
			}
			kill(childpid, SIGINT);
			wait(0);
		}
		//code for benchmarking signals over numtests		
		// stop timer
		gettimeofday(&t2, NULL);
		// compute and print the elapsed time in millisec
		elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
		elapsedTime += (t2.tv_usec - t1.tv_usec)/1000.0;   // us to ms
		printResults(childpid,
			     getpid(),
			     getgid(),
			     minTime,
			     maxTime,
			     elapsedTime,
			     true);
		exit(0);
	}
}
