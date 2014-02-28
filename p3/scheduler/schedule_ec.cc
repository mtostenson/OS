#include "schedule_ec.h"
#include <stdlib.h>

//delete this crap
#include <stdio.h>

// Globals
struct node* root;
struct node* cur;

struct node
{
	int value;
	int pri;
	struct node* next;
	struct node* prev;
};

void print()
{
	if (root)
	{
		cur = root;
		while (cur)
		{
			printf("[%d, %d] -> ", cur->value, cur->pri);
			cur = cur->next;
		}
		printf("\n");
	}
}
/*
 * Function to add a process to the scheduler
 * @Param tid - the ID for the process/thread to be added to the 
 *      scheduler queue
 * @Param priority - the priority of the given process
 * @return true/false response for if the addition was successful
 */
int addProcessEC(int tid, int priority)
{
	struct node* temp = (struct node*)malloc(sizeof(struct node));
	temp->value = tid;
	temp->pri = priority;
	if (!root)
		root = temp;
	else
	{
		if (priority < root->pri)
		{
			temp->next = root;
			root->prev = temp;
			root = temp;
		}
		else
		{
			cur = root;
			while (cur->next && cur->next->pri <= priority)
			{
				cur = cur->next;
			}
			if (cur->next)
			{
				cur->next->prev = temp;
				temp->next = cur->next;
			}
			temp->prev = cur;
			cur->next = temp;
		}
	}
	print();
	return 1;
}
/*
 * Function to remove a process from the scheduler queue
 * @Param tid - the ID for the process/thread to be removed from the
 *      scheduler queue
 * @Return true/false response for if the removal was successful
 */
int removeProcessEC(int tid)
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
int nextProcessEC()
{
	if (root)
	{
		struct node* temp = cur = root;
		root = root->next;
		root->prev = NULL;
		addProcessEC(temp->value, temp->pri);
		return temp->value;
	}
	else
	{
		return -1;
	}
}
