/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <Alexandar Mihaylov, Jeremy Kwok, Taylor Smith, Elias Amal, Luisa Rojas>
 * All rights reserved.
 * 
 */
#ifndef QUEUE_H_
#define QUEUE_H_

#include "utility.h"

#define MAX_LENGTH 256

// Your linked list structure for your queue
typedef struct {
	char name[MAX_LENGTH];
	int priority;
	int pid;
	int address;
	int memory;
	int runtime;
	bool suspended;
} proc;

typedef struct node{
  proc val;
  struct node* next;
} node_t;

typedef struct queue {
  node_t *head;
  node_t *tail;
} queue_t;

// Include your relevant FIFO queue functions declarations here they must start 
// with the extern keyword

// Add a new process to the queue, returns the address of the next node added
// to the linked list
extern node_t *push(node_t *tail, proc process);
extern node_t *pop(node_t *tail);

#endif /* QUEUE_H_ */