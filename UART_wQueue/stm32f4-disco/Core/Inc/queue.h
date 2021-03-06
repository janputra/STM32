/*
 * queue
 *
 *  Created on: Jul 3, 2021
 *      Author: janpo
 */

#ifndef __QUEUE_H__
#define __QUEUE_H__


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#define SIZE 256

typedef struct Queue{

	int16_t Front;
	int16_t Rear;
	uint8_t Buffer[SIZE];
} Queue;

Queue Ser_Queue;
void reset_Queue(Queue *Q);
void enQueue(Queue *Q,uint8_t element);
uint8_t deQueue(Queue *Q);


#endif /* __QUEUE_H__ */
