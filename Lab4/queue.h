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
typedef struct node{
  proc val;
  struct node* next;
} node_t;

typedef struct{
  node_t *head;
  node_t *tail;
} queue;


// Include your relevant FIFO queue functions declarations here they must start 
// with the extern keyword

// Add a new process to the queue, returns the address of the next node added
// to the linked list
extern void push(queue **q1, proc val);
extern proc pop(queue **q1);

#endif /* QUEUE_H_ */