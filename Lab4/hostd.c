/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <Alexandar Mihaylov, Jeremy Kwok, Taylor Smith, Elias Amal, Luisa Rojas>
 * All rights reserved.
 * 
 */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "queue.h"
#include "utility.h"
#include "hostd.h"

// Put macros or constants here using #define
#define MEMORY 1024

// Put global environment variables here

// Define functions declared in hostd.h here

int main(int argc, char *argv[]) {
    // ==================== YOUR CODE HERE ==================== //

    // 4 queues: real time, and priority 1 - 3:
    // Processes are executed immediately until their runtime is completed.
    // Are processes on a first-come-first-served basis.
    queue_t *realtime_queue = malloc(sizeof(queue_t));
    realtime_queue->head = NULL;
    realtime_queue->tail = NULL;

    // After a process has been run for 1 second, its removed from the queue and
    // added to the next lower priority queue
    queue_t *prior1_queue = malloc(sizeof(queue_t));
    prior1_queue->head = NULL;
    prior1_queue->tail = NULL;

    // After a process has been run for 1 second, its removed from the queue and
    // added to the next lower priority queue
    queue_t *prior2_queue = malloc(sizeof(queue_t));
    prior2_queue->head = NULL;
    prior2_queue->tail = NULL;

    // Once processes are added to it, they are run for 1 second then added back
    // to the priority 3 queue
    queue_t *prior3_queue = malloc(sizeof(queue_t));
    prior3_queue->head = NULL;
    prior3_queue->tail = NULL;
    
    // Load the dispatchlist
    
    // Add each process structure instance to the job dispatch list queue

    // Iterate through each item in the job dispatch list, add each process
    // to the appropriate queues

    // Allocate the resources for each process before it's executed

    // Execute the process binary using fork and exec

    // Perform the appropriate signal handling / resource allocation and de-alloaction

    // Repeat until all processes have been executed, all queues are empty
     
    return EXIT_SUCCESS;
}
