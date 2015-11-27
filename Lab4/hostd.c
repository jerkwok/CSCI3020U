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

  int current_time = 0;

  // 4 queues: real time, and priority 1 - 3:
  // Processes are executed immediately until their runtime is completed.
  // Are processes on a first-come-first-served basis.
  queue *realtime_queue = malloc(sizeof(queue));
  realtime_queue->head = NULL;
  realtime_queue->tail = NULL;

  // After a process has been run for 1 second, its removed from the queue and
  // added to the next lower priority queue
  queue *prior1_queue = malloc(sizeof(queue));
  prior1_queue->head = NULL;
  prior1_queue->tail = NULL;

  // After a process has been run for 1 second, its removed from the queue and
  // added to the next lower priority queue
  queue *prior2_queue = malloc(sizeof(queue));
  prior2_queue->head = NULL;
  prior2_queue->tail = NULL;

  // Once processes are added to it, they are run for 1 second then added back
  // to the priority 3 queue
  queue *prior3_queue = malloc(sizeof(queue));
  prior3_queue->head = NULL;
  prior3_queue->tail = NULL;

  queue *dispatcher = malloc(sizeof(queue));
  dispatcher->head = NULL;
  dispatcher->tail = NULL;
    
  // Load the dispatchlist
  // Add each process structure instance to the job dispatch list queue
  readFile(&dispatcher);
  // print_list(dispatcher);

  // Iterate through each item in the job dispatch list, add each process
  // to the appropriate queues

  // Allocate the resources for each process before it's executed
  resources res_avail;
  res_avail.printers = 2;
  res_avail.scanners = 1;
  res_avail.modems = 1;
  res_avail.cd_drives = 1;
  for (int i = 0; i < MEMORY;i++){
    res_avail.avail_mem[i] = 0;
  }

  node_t *dispatch_head = dispatcher->head;
  node_t *temp_real_time = realtime_queue->head;
  node_t *temp_1 = prior1_queue->head;
  node_t *temp_2 = prior2_queue->head;
  node_t *temp_3 = prior3_queue->head;
  while ((temp_real_time != NULL) || (temp_1 != NULL) || (temp_2 != NULL) || (temp_3 != NULL) || (dispatcher->head != NULL)){

    //   puts("-------------");
    // printf("Dispatcher Queue PRE:\n");
    // print_list(dispatcher);
    // puts("-------------");

    //dispatch any processes that have arrival times into the queues.
    dispatch(&dispatcher,&realtime_queue,&prior1_queue,&prior2_queue,&prior3_queue,current_time);

    temp_real_time = realtime_queue->head;
    temp_1 = prior1_queue->head;
    temp_2 = prior2_queue->head;
    temp_3 = prior3_queue->head;

    if(dispatcher->head == NULL){
      printf("%s\n","<=DISPATCHER NULL=>" );
    }else{
      printf("-----------\n");
      printf("Dispatcher Queue:\n");
      print_list(dispatcher);
      printf("-----------\n");
    }

    if(temp_real_time == NULL){
      printf("%s\n","<=RT NULL=>" );
    }else{
      printf("-----------\n");
      printf("Real Time Queue:\n");
      print_list(realtime_queue);
      printf("-----------\n");
    }

    if(temp_1 == NULL){
      printf("%s\n","<=1 NULL=>" );
    }else{
      printf("-----------\n");
      printf("1 Queue:\n");
      print_list(prior1_queue);
      printf("-----------\n");
    }

    if(temp_2 == NULL){
      printf("%s\n","<=2 NULL=>" );
    }else{
      printf("-----------\n");
      printf("2 Queue:\n");
      print_list(prior2_queue);
      printf("-----------\n");
    }

    if(temp_3 == NULL){
      printf("%s\n","<=3 NULL=>" );
    }else{
      printf("-----------\n");
      printf("3 Queue:\n");
      print_list(prior3_queue);
      printf("-----------\n");
    }



    //redundant but needed 
    node_t *dispatch_head = dispatcher->head;
    node_t *temp_real_time = realtime_queue->head;
    node_t *temp_1 = prior1_queue->head;
    node_t *temp_2 = prior2_queue->head;
    node_t *temp_3 = prior3_queue->head;

    // Execute the process binary using fork and exec

    if (temp_real_time != NULL){
      //pop the head from the queue
      proc popped_proc= pop(&realtime_queue);

      //get resources required
      int printers_req = popped_proc.printers;
      int scanners_req = popped_proc.scanners;
      int modems_req = popped_proc.modems;
      int cd_drives_req = popped_proc.cd_drives;

      //check if there is enough memory for the process
      int mem_index = alloc_mem(&res_avail, popped_proc.memory);
      if (mem_index == -1){
	//not enough memory, must return the given memory
	free_mem(&res_avail, mem_index, popped_proc.memory);
      }else{
	//actually need the allocated memory, print an update
	//printf("mem_index:%d\n",mem_index);
	print_memory(res_avail.avail_mem,MEMORY);
      }
      //check if enough resources and memory are available
      if (res_avail.printers >= printers_req &&
	  res_avail.scanners >= scanners_req &&
	  res_avail.modems >= modems_req &&
	  res_avail.cd_drives >= cd_drives_req &&
	  mem_index >= 0){
	//there are enough resources and memory for the resorce
	//EXECUTE THE NEXT RESOURCE
	pid_t pid = fork();
	if (pid == 0){
	  //child process
	  execlp("./process",NULL);
	  exit(0);
	}else if(pid > 0){
	  //parent process
	  printf("[parent Q0] waiting %d second(s)...:\n",popped_proc.runtime);
	  sleep(popped_proc.runtime); //sleep for the needed runtime
	  current_time+=popped_proc.runtime;

	  puts("[parent] Sending SIGINT...");
	  kill(pid,SIGINT);
	  waitpid(pid,0,0);
	  //done with executing the proces----------------
	  //free the memory
	  free_mem(&res_avail, mem_index, popped_proc.memory);
	  //return the resources back to 
	  res_avail.printers += printers_req;
	  res_avail.scanners += scanners_req;
	  res_avail.modems += modems_req;
	  res_avail.cd_drives += cd_drives_req;
	}else{
	  //fork failed
	  puts("fork failed");
	}
      }
      temp_real_time = realtime_queue->head;
    }

    //Priority 1 Queue
    if (temp_1 != NULL){
      //pop the head from the queue
      proc popped_proc= pop(&prior1_queue);

      //get resources required
      int printers_req = popped_proc.printers;
      int scanners_req = popped_proc.scanners;
      int modems_req = popped_proc.modems;
      int cd_drives_req = popped_proc.cd_drives;

      int mem_index = -1;
      if (popped_proc.suspended == false){
	//first time process is run, need to allocate memory
	//check if there is enough memory for the process
	mem_index = alloc_mem(&res_avail, popped_proc.memory);
	if (mem_index == -1){
	  //not enough memory, must return the given memory
	  free_mem(&res_avail, mem_index, popped_proc.memory);
	  //check if there are enough resources
	}else if(res_avail.printers >= printers_req &&
		 res_avail.scanners >= scanners_req &&
		 res_avail.modems >= modems_req &&
		 res_avail.cd_drives >= cd_drives_req ){
	  //enough resources for the process
	  printf("[Q1: allocating %d memory\n]", popped_proc.memory);
	  //actually need the allocated memory, print an update
	  print_memory(res_avail.avail_mem,MEMORY);
	  //allocate the needed resources
	  res_avail.printers -= printers_req; 
	  res_avail.scanners -= scanners_req;
	  res_avail.modems -= modems_req;
	  res_avail.cd_drives -= cd_drives_req;
	  //process is ready for execution
	  popped_proc.suspended = true;
	}
      }
      //run process only if it has resources + memory allocated
      if (popped_proc.suspended == true){
	//there are enough resources and memory for the resorce
	//EXECUTE THE NEXT RESOURCE
	pid_t pid = fork();
	if (pid == 0){
	  //child process
	  execlp("./process",NULL);
	  exit(0);
	}else if(pid > 0){
	  //parent process
	  printf("[parent Q1] waiting %d second...:\n",1);
	  sleep(1); //sleep for only 1 second
	  current_time++;

	  puts("[parent Q1] Sending SIGINT...");
	  kill(pid,SIGINT);
	  waitpid(pid,0,0);

	  popped_proc.runtime--;
	  //check if process is done execution
	  if (popped_proc.runtime == 0){
	    //process is done executing, must deallocate memory
	    free_mem(&res_avail, mem_index, popped_proc.memory);
	    //return the resources back to 
	    res_avail.printers += printers_req;
	    res_avail.scanners += scanners_req;
	    res_avail.modems += modems_req;
	    res_avail.cd_drives += cd_drives_req;
	  }
	  //push the process to the next queue
	  if (popped_proc.runtime >0){
	    push(&prior2_queue, popped_proc);
	  }
	}else{
	  //fork failed
	  puts("fork failed");
	}
      }
      temp_1 = prior1_queue->head;
    }

    //Priority 2 Queue
    if (temp_2 != NULL){
      //pop the head from the queue
      proc popped_proc= pop(&prior2_queue);

      //get resources required
      int printers_req = popped_proc.printers;
      int scanners_req = popped_proc.scanners;
      int modems_req = popped_proc.modems;
      int cd_drives_req = popped_proc.cd_drives;

      int mem_index = -1;
      if (popped_proc.suspended == false){
	//first time process is run, need to allocate memory
	//check if there is enough memory for the process
	mem_index = alloc_mem(&res_avail, popped_proc.memory);
	if (mem_index == -1){
	  //not enough memory, must return the given memory
	  free_mem(&res_avail, mem_index, popped_proc.memory);
	  //check if there are enough resources
	}else if(res_avail.printers >= printers_req &&
		 res_avail.scanners >= scanners_req &&
		 res_avail.modems >= modems_req &&
		 res_avail.cd_drives >= cd_drives_req ){
	  //enough resources for the process
	  printf("[Q2: allocating %d memory\n]", popped_proc.memory);
	  //actually need the allocated memory, print an update
	  print_memory(res_avail.avail_mem,MEMORY);
	  //allocate the needed resources
	  res_avail.printers -= printers_req; 
	  res_avail.scanners -= scanners_req;
	  res_avail.modems -= modems_req;
	  res_avail.cd_drives -= cd_drives_req;
	  //process is ready for execution
	  popped_proc.suspended = true;
	}
      }
      //run process only if it has resources + memory allocated
      if (popped_proc.suspended == true){
	//there are enough resources and memory for the resorce
	//EXECUTE THE NEXT RESOURCE
	pid_t pid = fork();
	if (pid == 0){
	  //child process
	  execlp("./process",NULL);
	  exit(0);
	}else if(pid > 0){
	  //parent process
	  printf("[parent Q2] waiting %d second...:\n",1);
	  sleep(1); //sleep for only 1 second
	  current_time++;

	  puts("[parent Q2] Sending SIGINT...");
	  kill(pid,SIGINT);
	  waitpid(pid,0,0);

	  popped_proc.runtime--;
	  //check if process is done execution
	  if (popped_proc.runtime == 0){
	    //process is done executing, must deallocate memory
	    free_mem(&res_avail, mem_index, popped_proc.memory);
	    //return the resources back to 
	    res_avail.printers += printers_req;
	    res_avail.scanners += scanners_req;
	    res_avail.modems += modems_req;
	    res_avail.cd_drives += cd_drives_req;
	  }
	  //push the process to the next queue
	  if (popped_proc.runtime >0){
	    push(&prior3_queue, popped_proc);
	  }
	}else{
	  //fork failed
	  puts("fork failed");
	}
      }
      temp_2 = prior2_queue->head;
    }

    //Priority 3 Queue
    if (temp_3 != NULL){
      //pop the head from the queue
      proc popped_proc= pop(&prior3_queue);

      //get resources required
      int printers_req = popped_proc.printers;
      int scanners_req = popped_proc.scanners;
      int modems_req = popped_proc.modems;
      int cd_drives_req = popped_proc.cd_drives;

      int mem_index = -1;
      if (popped_proc.suspended == false){
	//first time process is run, need to allocate memory
	//check if there is enough memory for the process
	mem_index = alloc_mem(&res_avail, popped_proc.memory);
	if (mem_index == -1){
	  //not enough memory, must return the given memory
	  free_mem(&res_avail, mem_index, popped_proc.memory);
	  //check if there are enough resources
	}else if(res_avail.printers >= printers_req &&
		 res_avail.scanners >= scanners_req &&
		 res_avail.modems >= modems_req &&
		 res_avail.cd_drives >= cd_drives_req ){
	  //enough resources for the process
	  printf("[Q3: allocating %d memory\n]", popped_proc.memory);
	  //actually need the allocated memory, print an update
	  print_memory(res_avail.avail_mem,MEMORY);
	  //allocate the needed resources
	  res_avail.printers -= printers_req; 
	  res_avail.scanners -= scanners_req;
	  res_avail.modems -= modems_req;
	  res_avail.cd_drives -= cd_drives_req;
	  //process is ready for execution
	  popped_proc.suspended = true;
	}
      }
      //run process only if it has resources + memory allocated
      if (popped_proc.suspended == true){
	//there are enough resources and memory for the resorce
	//EXECUTE THE NEXT RESOURCE
	pid_t pid = fork();
	if (pid == 0){
	  //child process
	  execlp("./process",NULL);
	  exit(0);
	}else if(pid > 0){
	  //parent process
	  printf("[parent Q3] waiting %d second...:\n",1);
	  sleep(1); //sleep for only 1 second
	  current_time++;

	  puts("[parent Q3] Sending SIGINT...");
	  kill(pid,SIGINT);
	  waitpid(pid,0,0);

	  popped_proc.runtime--;
	  //check if process is done execution
	  if (popped_proc.runtime == 0){
	    //process is done executing, must deallocate memory
	    free_mem(&res_avail, mem_index, popped_proc.memory);
	    //return the resources back to 
	    res_avail.printers += printers_req;
	    res_avail.scanners += scanners_req;
	    res_avail.modems += modems_req;
	    res_avail.cd_drives += cd_drives_req;
	  }
	  //push the process to the next queue
	  if (popped_proc.runtime >0){
	    push(&prior3_queue, popped_proc);
	  }
	}else{
	  //fork failed
	  puts("fork failed");
	}
      }
      temp_3 = prior3_queue->head;
    }

    //    return 0;//temp for debugging


    // Perform the appropriate signal handling / resource allocation and de-alloaction
        
    // Repeat until all processes have been executed, all queues are empty
    // while ((temp_real_time != NULL) && (temp_1 != NULL) && (temp_2 != NULL) && (temp_3 != NULL)){
    //     if (temp_real_time != NULL){
    //         temp_real_time = temp_real_time -> next;
    //     }

    //     if (temp_1 != NULL){
    //         temp_1 = temp_1 -> next;
    //     }

    //     if (temp_2 != NULL){
    //         temp_2 = temp_2 -> next;
    //     }

    //     if (temp_3 != NULL){
    //         temp_3 = temp_3 -> next;
    //     }
    // }
    printf("<=========CURRENT TIME:%d========>\n",current_time );
    // int c;
    // c = getchar( );
  }

     
  return EXIT_SUCCESS;
}
