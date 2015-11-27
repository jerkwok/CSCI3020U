/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#ifndef UTILITY_H_
#define UTILITY_H_

#include "queue.h"

// The amount of available memory
#define MEMORY 1024
#define MAX_LENGTH 256


// Resources structure containing integers for each resource constraint and an
// array of 1024 for the memory
typedef struct {
 	// printer, scanner, modem and CD drives will be stored here
 	int printers;
 	int scanners;
 	int modems;
 	int cd_drives;
 	// memory array
 	int avail_mem[MEMORY];
} resources;

// Processes structure containing all of the process details parsed from the 
// input file, should also include the memory address (an index) which indicates
// where in the resources memory array its memory was allocated
typedef struct {
	int arrival;
	int priority;
	int runtime;
	int memory;
	int printers;
	int scanners;
	int modems;
	int cd_drives;
	int address;
	int pid;
} proc;

typedef struct node{
  proc val;
  struct node* next;
} node_t;

typedef struct{
  node_t *head;
  node_t *tail;
} queue;

// Include your relevant functions declarations here they must start with the 
// extern keyword such as in the following examples:

// Function to allocate a contiguous chunk of memory in your resources structure
// memory array, always make sure you leave the last 64 values (64 MB) free, should
// return the index where the memory was allocated at
extern int alloc_mem(resources *res, int size);

// Function to free the allocated contiguous chunk of memory in your resources
// structure memory array, should take the resource struct, start index, and 
// size (amount of memory allocated) as arguments
extern void free_mem(resources *res, int index, int size);

// Function to parse the file and initialize each process structure and add
// it to your job dispatch list queue (linked list)
extern void readFile(queue** p1);

extern void dispatch(queue** dispatcher, queue** realtime_queue, queue** prior1_queue, queue** prior2_queue, queue** prior3_queue, int time);

extern char** tokenize2(char *input, char *delim);

extern void push(queue **q1, proc val);

extern proc pop(queue **q1);

extern void print_list(queue *q1);

extern int freeMemoryAmount(int memory[],int length);

extern void print_memory(int memory[],int length);

#endif /* UTILITY_H_ */
