/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <Alexandar Mihaylov, Jeremy Kwok, Taylor Smith, Elias Amal, Luisa Rojas>
 * All rights reserved.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

// Define your FIFO queue functions here, these will most likely be the
// push and pop functions that you declared in your header file

node_t *push(node_t *tail, proc process) {
	
	// Create new node
	node_t *new_node;
	new_node = (node_t*)malloc(sizeof(node_t));
	if (node_t == NULL) {
		fprintf(stderr, "Unable to malloc\n");
		exit(0);
	}

	// Fill values
	new_node->val = val;
	new_node->next = NULL;

	// Add at END of queue (tail)
	if (some_queue->tail != NULL) {
		some_queue->tail->next = new_node;
		some_queue->tail = new_node;
	}
	else {
		some_queue->head = new_node;
		some_queue->tail = new_node;
	}

	return *new_node;
}

node_t *pop(node_t *tail) {

	node_t *next_node;
	proc temp_proc;

	if (*some_queue->head == NULL) {
		// Queue is empty.
		return 0;
	}
	else {
		// Return first element and remove it
		next_node = (*some_queue)->head->next;
		temp_proc = (*some_queue)->head->val;
		free((*some_queue)->head);
		(*some_queue)->head = next_node;
	}
	return *next_node;

}
