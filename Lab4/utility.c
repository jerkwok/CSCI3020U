/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "utility.h"
#include "queue.h"

#define FILE_LENGTH 10

// Define your utility functions here, you will likely need to add more...

int alloc_mem(resources res, int size)
{
  bool free = false;
  for (int i = 0; i < 1024 - 64; i++){
    if ((res.avail_mem[i] == 0) && (i + 64 < 1024)){
      free = true;
      for (int j = i; j < size; j++){
        if (res.avail_mem[j] == 0){
          free = false;
        }
      }
      if (free == true){
        return i;
      }
    }
  }

  //not sure what to return on not founds
	return 0;
}

void free_mem(resources res, int index, int size)
{
	for(int i = index; i < index+size; i++){
    res.avail_mem[i] = 0;
  }
}

int freeMemoryAmount(int memory[],int length){
  int freeMemory = 0;
  for (int i = 0; i < length; i++){
    if (memory[i] == 0){
      freeMemory++;
    }
  }
  return freeMemory;
}

void print_memory(int memory[],int length){
  printf("[");
  for (int i = 0; i < length; i++){
    if (memory[i] == 1){
      printf("|");
    }else{
      printf("_");
    }
  }
  printf("]\n");
}

void dispatch(queue** dispatcher, queue** realtime_queue, queue** prior1_queue, queue** prior2_queue, queue** prior3_queue,int time){
  	queue *temp = *dispatcher;
	// print_list(temp);
	node_t *current = temp->head;

	while (current != NULL){
		//realtime 
    if (current->val.arrival <= time){

      if(current->val.priority == 0){
        push(realtime_queue,current->val);
      }else if(current->val.priority == 1){
        push(prior1_queue,current->val);
      }else if(current->val.priority == 2){
        push(prior2_queue,current->val);
      }else if(current->val.priority == 3){
        push(prior3_queue,current->val);      
      }
    }
      current=current->next;
  }
	printf("Real Time:\n");
	print_list(*realtime_queue);
	printf("P1:\n");
	print_list(*prior1_queue);
	printf("P2:\n");
	print_list(*prior2_queue);
	printf("P3:\n");
	print_list(*prior3_queue);
}

void readFile(queue** p1){
  //if priority_filter = -1 -> load all processes
  //if priority_filter = 0  -> load processes with priority = 0
  //if priority_filter = 1  -> load all processes with priority != 0
  char buffer[MAX_LENGTH] = {0};
  FILE *f1 = fopen("dispatchlist","r");
  if (f1 == NULL){
    perror("Error opening file\n");
    return;    
  }

  for (int i = 0; i < FILE_LENGTH; i++){
    fgets(buffer,MAX_LENGTH, f1);
    proc *temp_proc = (proc *)  malloc(sizeof(proc));
    char **tokenized = tokenize2(buffer,", ");
    //priority is in tokenized[1]
  // if ((atoi(tokenized[1]) == 0 && priority_filter == 0)|| //only priority == 0
  // (atoi(tokenized[1]) != 0 && priority_filter == 1)|| //only priority != 0
  // (atoi(tokenized[1]) != 0 && priority_filter == 2)|| //only priority != 0
  // (atoi(tokenized[1]) != 0 && priority_filter == 3)|| //only priority != 0
  // (priority_filter == -1)) //all
  // if (atoi(tokenized[1]) == priority_filter)
  // {
	  temp_proc->arrival = atoi(tokenized[0]); //arrival
      temp_proc->priority = atoi(tokenized[1]);//priority
      temp_proc->runtime = atoi(tokenized[2]); //runtime
      temp_proc->memory = atoi(tokenized[3]);//memory
      temp_proc->printers = atoi(tokenized[4]);//memory
      temp_proc->scanners = atoi(tokenized[5]);//memory
      temp_proc->modems = atoi(tokenized[6]);//memory
      temp_proc->cd_drives = atoi(tokenized[7]);//memory
      temp_proc->pid = 0;//pid
      temp_proc->address = 0; //address

      //push process onto queue
      push(p1,*temp_proc);
    // }
  }
  fclose(f1);
}

void print_list(queue *q1){
  //  node_t *current = head;
  node_t *current = q1->head;
  
  //check if list is empty
  if(q1->head == NULL){
    printf("<empty>\n");
    return;
  }
  
  while(current != NULL){
  //temp
  /*  char name[CHAR_LENGTH];
  int priority;
  int pid;
  int address;
  int memory;
  int runtime;
  bool suspended;*/
  //end temp
  printf("arrival: %d, priority: %d, runtime: %d, memory: %d, printers: %d, scanners: %d, modems: %d, cd_drives: %d\n",
  	current->val.arrival,
	current->val.priority,
	current->val.runtime,
	current->val.memory,
	current->val.printers,
	current->val.scanners,
	current->val.modems,
	current->val.cd_drives
	 );
  //iterate to next item
    current = current->next;
  }
}


char** tokenize2(char *input, char *delim){
  //takes an input string with some delimiter and returns an array
  //with all the tokens split by the provided delimiter

  //Sample usage:
  //char buffer[] = "a b c";
  //char **user_output;
  //user_output = tokenize(buffer, " ");

  char** tokens = 0;
  size_t num_elements = 0;
  size_t tokens_index  = 0; //keep tracks of the  tokens offset when adding them
  char* input_cpy = input;
  char *input_cpy2 = malloc(1 + strlen(input)); //used with strtok
  if (input_cpy2){
    strcpy(input_cpy2, input);
  }else{
    printf("error copying input\n");
  }

  //iterate through the intput and count # of delims
  while (*input_cpy != NULL){
    if (*delim == *input_cpy){
      num_elements++;
    }input_cpy++;
  }

  num_elements++; //for last object
  num_elements++; //for null terminating value

  //create enough memory for all the elements
  tokens = malloc(sizeof(char*) * num_elements);
  char* token = strtok(input_cpy2, delim);
  while (token){
    //store the token in the tokens array
    *(tokens + tokens_index++) = strdup(token);   //strdup duplicates the string
    token = strtok(0, delim); //next token
  }
  //finally add null value at the end
  *(tokens + tokens_index) = 0;
  return tokens;
}

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
