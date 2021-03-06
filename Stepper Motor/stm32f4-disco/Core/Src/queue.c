/*
 * queue
 *
 *  Created on: Jul 3, 2021
 *      Author: janpo
 */

#include "queue.h"


uint8_t isQ_Full(Queue *Q)
{
	if ((Q->Front==Q->Rear+1)||(Q->Front==0&&Q->Rear==SIZE-1))
	{
		return 1;
	}
	return 0;
}

uint8_t isQ_Empty(Queue *Q)
{
	if (Q->Front==-1) return 1;
	return 0;

}

void reset_Queue(Queue *Q)
{
	Q->Front=-1;
	Q->Rear=-1;
}


void enQueue(Queue *Q,uint8_t element)
{
	if (!isQ_Full(Q))
	{
		if (Q->Front==-1){Q->Front=0;}

		Q->Rear= (Q->Rear+1)%SIZE;
		Q->Buffer[Q->Rear]= element;


	}

}

uint8_t deQueue(Queue *Q)
{
	uint8_t element;

	if (!isQ_Empty(Q))
	{
		element =Q->Buffer[Q->Front];
		if (Q->Front==Q->Rear)
		{
			reset_Queue(Q);
		}
		else
		{
		Q->Front= (Q->Front+1)%SIZE;
		}

		return element;
	}
	else { return (-1);}
}
