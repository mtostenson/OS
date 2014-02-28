// Michael Tostenson
// Operating Systems
// Assignment 3

#include "schedule.h"
#include <stdlib.h>

// Globals
struct node* root;
struct node* cur;

struct node
{
	int value;
	struct node* next;
	struct node* prev;
};

/*
 * Function to add a process to the scheduler
 * @Param tid - the ID for the process/thread to be added to the 
 *      scheduler queue
 * @return true/false response for if the addition was successful
 */
int addProcess(int tid)
{
	if (!root)
	{
		root = (struct node*)malloc(sizeof(struct node));
		cur = root;
	}
	else
	{
		cur = root;
		while (cur->next)
		{
			cur = cur->next;
		}
		cur->next = (struct node*)malloc(sizeof(struct node));
		cur->next->prev = cur;
		cur = cur->next;
	}
	cur->value = tid;
	return 1;
}
/*
 * Function to remove a process from the scheduler queue
 * @Param tid - the ID for the process/thread to be removed from the
 *      scheduler queue
 * @Return true/false response for if the removal was successful
 */
int removeProcess(int tid)
{
	if (root)
	{
		cur = root;
		do
		{
			if (cur->value == tid)
			{
				if (cur->next)
				{
					if (cur->prev)
					{
						cur->prev->next = cur->next;
						cur->next->prev = cur->prev;
					}
					else
					{
						root = cur->next;
						root->prev = NULL;
					}
				}
				else
				{
					if (cur->prev)
					{
						cur->prev->next = NULL;
					}
					else
					{
						root = NULL;
					}
				}
				free(cur);
				return 1;
			}
			else
			{
				cur = cur->next;
			}
		} while (cur);
	}
	return 0;
}
/*
 * Function to get the next process from the scheduler
 * @Return returns the thread id of the next process that should be 
 *      executed, returns -1 if there are no processes
 */
int nextProcess()
{
	if (root)
	{
		struct node* temp = cur = root;
		root = root->next;
		root->prev = NULL;
		while (cur->next)
		{
			cur = cur->next;
		}
		cur->next = temp;
		temp->next = NULL;
		temp->prev = cur;
		return temp->value;
	}
	else
	{
		return -1;
	}
}