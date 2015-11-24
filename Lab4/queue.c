/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <Alexandar Mihaylov, Jeremy Kwok, Taylor Smith, Elias Amal, Luisa Rojas>
 * All rights reserved.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "queue.h"

// Define your FIFO queue functions here, these will most likely be the
// push and pop functions that you declared in your header file

proc pop(queue **q1){
  node_t* next_node = NULL;
  proc popped_val;

  if ((*q1)->tail != NULL){  
    next_node = (*q1)->head->next;
    popped_val = (*q1)->head->val;
    free((*q1)->head);
    (*q1)->head = next_node;
  }
  return popped_val;
}

void push(queue **q1, proc val){
  //create a new node
  node_t* newNode = malloc(sizeof(node_t));
  newNode->val = val;
  newNode->next = NULL;

  if ((*q1)->head == NULL){
    //q1 is empty
    (*q1)->head = newNode;
    (*q1)->tail = newNode;
  }else{
    (*q1)->tail->next = newNode;
    (*q1)->tail = newNode;
  }
}
