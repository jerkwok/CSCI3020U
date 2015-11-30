/*
 * Banker's Algorithm for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, Taylor Smith
 * All rights reserved.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "banker.h"
#include <unistd.h>

/*G*/ // Declare all global variables.
/*G*/pthread_mutex_t mutex;
/*G*/
/*G*/int available[NUM_RESOURCES];
/*G*/int maximum[NUM_CUSTOMERS][NUM_RESOURCES];
/*G*/int allocation[NUM_CUSTOMERS][NUM_RESOURCES];
/*G*/int need[NUM_CUSTOMERS][NUM_RESOURCES];

int main(int argc, char *argv[])
{
    /*I*/ // Initialization: Check to make sure user entered the correct amount
    /*I*/ // of resources and loads them into the available array and
    /*I*/ // initializes the mutex lock.
    /*I*/system("clear");
    /*I*/
    /*I*/if(argc -1 != NUM_RESOURCES)
    /*I*/{
    /*I*/  printf("ERROR: Expected %d reources, recieved %d\n", NUM_RESOURCES, argc);
    /*I*/  exit(EXIT_FAILURE);
    /*I*/}
    /*I*/
    /*I*/for(int a = 0; a < NUM_RESOURCES; a++)
    /*I*/{
    /*I*/  available[a] = atoi(argv[a+1]);
    /*I*/}
    /*I*/
    /*I*/print_array("Available Resources", available, NUM_RESOURCES, true);
    /*I*/printf("\n");
    /*I*/
    /*I*/pthread_mutex_init(&mutex,NULL);

    /*G*/ // Generate the initial max values for the processes and store them in
    /*G*/ // an array. Max values have to be between 0 and the maximum available
    /*G*/ // of any given resource.
    /*G*/customer_struct processes[NUM_CUSTOMERS];
    /*G*/
    /*G*/for (int i = 0; i < NUM_CUSTOMERS; i++)
    /*G*/{
    /*G*/  customer_struct s;
    /*G*/
    /*G*/  s.thread_num = i;
    /*G*/
    /*G*/  for(int j = 0; j < NUM_RESOURCES; j++)
    /*G*/  {
    /*G*/      int resource_max = rand() % atoi(argv[j+1]);
    /*G*/
    /*G*/      s.max[j] = resource_max;
    /*G*/
    /*G*/      allocation[i][j] = 0;
    /*G*/      maximum[i][j] = resource_max;
    /*G*/      need[i][j] = resource_max;
    /*G*/  }
    /*G*/
    /*G*/  print_struct(s);
    /*G*/  processes[i] = s;
    /*G*/}

    /*P*/ // Print prompt so user knows how to use the system.
    /*P*/printf("\nPress enter to move to next step...\n");
    /*P*/printf("===================================\n");

    /*T*/ // Create each thread passing it a generated struct containing its
    /*T*/ // thread number and the maximum resources required for a process
    /*T*/ // to complete. Each thread executes the customer function which
    /*T*/ // continually generates random requests and submits them to the banker
    /*T*/ // until it recieves all required resources and then releases all of its
    /*T*/ // resources.
    /*T*/pthread_t customers[NUM_CUSTOMERS];
    /*T*/
    /*T*/for(int q = 0; q < NUM_CUSTOMERS; q++)
    /*T*/{
    /*T*/  pthread_create(&customers[q], 0, customer, (void *) &processes[q]);
    /*T*/}
    /*T*/
    /*T*/for (int k = 0; k < NUM_CUSTOMERS; k++)
    /*T*/{
    /*T*/  pthread_join(customers[k],0);
    /*T*/}

    /*C*/ // Clean-up
    /*C*/printf("\nAll processes completed program exitting\n\n");
    /*C*/
    /*C*/pthread_mutex_destroy(&mutex);
    /*C*/
    /*C*/return EXIT_SUCCESS;
}

void *customer(void *arg)
{
   /*I*/ // Intialization: typecast the struct passed as an argument to the
   /*I*/ // thread as well the has array, a local copy of this threads allocated
   /*I*/ // resources and sets the looping variable to false.
   /*I*/customer_struct* s = (customer_struct*) arg;
   /*I*/int has[NUM_RESOURCES] = {0};
   /*I*/bool complete = false;

   /*W*/ // Loop the thread until it completes by aquiring all needed resources.
   /*W*/ // Waits for user to hit enter before proceeding, then generates a
   /*W*/ // random request that ranges from [0,max needed] and then aquires the
   /*W*/ // mutex to issue a request. If the request is granted the has array is
   /*W*/ // updated and compared to max to determine if the process is complete.
   /*W*/while(!complete)
   /*W*/{
   /*W*/  getchar();
   /*W*/
   /*W*/  //create request array and generate random request
   /*W*/  int req[NUM_RESOURCES];
   /*W*/  for(int i = 0; i < NUM_RESOURCES; i++)
   /*W*/  {
   /*W*/    req[i] = rand() % (s->max[i] - has[i] + 1);
   /*W*/  }
   /*W*/
   /*W*/  pthread_mutex_lock(&mutex);
   /*W*/  bool request_granted = request_res(s->thread_num, req);
   /*W*/  pthread_mutex_unlock(&mutex);
   /*W*/
   /*W*/  if(request_granted)
   /*W*/  {
   /*W*/    //Update has array.
   /*W*/    for(int j = 0; j < NUM_RESOURCES; j++)
   /*W*/    {
   /*W*/      has[j] += req[j];
   /*W*/    }
   /*W*/
   /*W*/    //Check if customer has all required resources by comparing has
   /*W*/    //array to max array.
   /*W*/    complete = true;
   /*W*/    for(int k = 0; k < NUM_RESOURCES; k++)
   /*W*/    {
   /*W*/      if(has[k] != s->max[k])
   /*W*/      {
   /*W*/       complete = false;
   /*W*/      }
   /*W*/    }
   /*W*/  }
   /*W*/}

   /*C*/ // Clean-up: release all resources and exit the thread.
   /*C*/printf("Process %d completed.\n", s->thread_num);
   /*C*/
   /*C*/pthread_mutex_lock(&mutex);
   /*C*/release_res(s->thread_num, has);
   /*C*/pthread_mutex_unlock(&mutex);
   /*C*/
   /*C*/return (void *) NULL;
}

bool request_res(int n_customer, int request[])
{
   /*P*/ // Print the incomming request and current available resources.
   /*P*/printf("Proc%d Req", n_customer);
   /*P*/print_array("", request, NUM_RESOURCES, true);
   /*P*/
   /*P*/print_array("Available", available, NUM_RESOURCES, true);
   /*P*/puts("");

   /**/ // Check to see if the process requests more resources than its maximum
   /**/ // need in which case something has gone wrong and close the program.
   /**/ // Also checks if any of the resource requests are more than the current
   /**/ // remaining available resources and denies the request if they are.
   /**/for(int i = 0; i < NUM_RESOURCES; i++)
   /**/{
   /**/  if(request[i] > need[n_customer][i])
   /**/  {
   /**/    printf("ERROR: Thread number %d requested more resources than maximum\n", n_customer);
   /**/    exit (EXIT_FAILURE);
   /**/  }
   /**/}
   /**/
   /**/for(int j = 0; j < NUM_RESOURCES; j++)
   /**/{
   /**/  if(request[j] > available[j])
   /**/  {
   /**/    printf("DENIED: Resources not available\n");
   /**/    puts("===================================");
   /**/    return false;
   /**/  }
   /**/}

   /*U*/ // Assume the request will be granted and update the current available
   /*U*/ // resources, the allocated resources to the process and the need of
   /*U*/ // the process.
   /*U*/for(int k = 0; k < NUM_RESOURCES; k++)
   /*U*/{
   /*U*/    available[k] -= request[k];
   /*U*/    allocation[n_customer][k] += request[k];
   /*U*/    need[n_customer][k] -= request[k];
   /*U*/}

   /*S*/ // Check if the system will be in a safe state. If it will be return true
   /*S*/ // and don't update as the system is already up to date. However if it
   /*S*/ // will not be in a safe state revert all previous changes to the safe
   /*S*/ // state before the request was made.
   /*S*/if(check_safe(available,allocation,need))
   /*S*/{
   /*S*/  printf("\nGRANTED: Proc%d now has", n_customer);
   /*S*/  print_array("", allocation[n_customer], NUM_RESOURCES, true);
   /*S*/  puts("===================================");
   /*S*/
   /*S*/  return true;
   /*S*/}
   /*S*/else
   /*S*/{
   /*S*/  // Revert all changes made to the system state
   /*S*/  for(int m = 0; m < NUM_RESOURCES; m++)
   /*S*/  {
   /*S*/      available[m] += request[m];
   /*S*/      allocation[n_customer][m] -= request[m];
   /*S*/      need[n_customer][m] += request[m];
   /*S*/  }
   /*S*/
   /*S*/  printf("\nDENIED: Unsafe state!\n");
   /*S*/  puts("===================================");
   /*S*/
   /*S*/  return false;
   /*S*/}
}

bool release_res(int n_customer, int release[])
{
  /*P*/ // Print which process is returning its resources.
  /*P*/printf("Process %d Releasing all resources.\n", n_customer);
  /*P*/puts("===================================");

  /*R*/ // Release the resources from the completed process.
  /*R*/for(int i = 0; i < NUM_RESOURCES; i++)
  /*R*/{
  /*R*/  available[i] += release[i];
  /*R*/  allocation[n_customer][i] = 0;
  /*R*/  maximum[n_customer][i] = 0;
  /*R*/}

  return true;
}

bool check_safe(int available[], int allocation[][NUM_RESOURCES], int need[][NUM_RESOURCES])
{
  /*I*/ // Intialize variables needed for bankers algorithm as well as local
  /*I*/ // copies of need and allocation.
  /*I*/int work[NUM_RESOURCES] = {0};
  /*I*/int finish[NUM_CUSTOMERS] = {0};
  /*I*/int local_allocation[NUM_CUSTOMERS][NUM_RESOURCES];
  /*I*/int local_need[NUM_CUSTOMERS][NUM_RESOURCES];
  /*I*/
  /*I*/for(int e = 0; e < NUM_CUSTOMERS; e++)
  /*I*/{
  /*I*/  for(int f = 0; f < NUM_RESOURCES; f++)
  /*I*/  {
  /*I*/    local_allocation[e][f] = allocation[e][f];
  /*I*/    local_need[e][f] = need[e][f];
  /*I*/  }
  /*I*/}
  /*I*/
  /*I*/for(int i = 0; i < NUM_RESOURCES; i++)
  /*I*/{
  /*I*/  work[i] = available[i];
  /*I*/}

  /*P*/ // Print the required allocated and max of every process and the
  /*P*/ // remaining available after the request has been implemented. Then
  /*P*/ // print the header for the bankers algorithm loop.
  /*P*/print_matricies();
  /*P*/
  /*P*/printf("\n");
  /*P*/print_array("Post Request", work, NUM_RESOURCES, true);
  /*P*/printf("\n");
  /*P*/
  /*P*/printf("#   [ Avail ]   [Require]   [ Alloc ]   [    Fin    ]\n");

  /*W*/ // Repeat the algorithm until it is complete by looping over every
  /*W*/ // process and counting the completed processes before itterating
  /*W*/ // over every process as well as after. If they are equal return false
  /*W*/ // as no new processes have completed and never will. If the number
  /*W*/ // completed at the end is equal to the number of customers return true
  /*W*/ // as all processes are complete.
  /*W*/while(1)
  /*W*/{
  /*W*/  /*C*/ // Reset the conters to 0 and count the currently completed
  /*W*/  /*C*/ // processes.
  /*W*/  /*C*/int n_start_finished_processes = 0;
  /*W*/  /*C*/int n_end_finished_processes = 0;
  /*W*/  /*C*/
  /*W*/  /*C*/for(int n = 0; n < NUM_CUSTOMERS; n++)
  /*W*/  /*C*/{
  /*W*/  /*C*/    if(finish[n])
  /*W*/  /*C*/    {
  /*W*/  /*C*/      n_start_finished_processes++;
  /*W*/  /*C*/    }
  /*W*/  /*C*/}
  /*W*/
  /*W*/  /*L*/ // Loop over all customers and if they are unfinished, determine
  /*W*/  /*L*/ // if they can finish.
  /*W*/  /*L*/for(int j = 0; j < NUM_CUSTOMERS; j++)
  /*W*/  /*L*/{
  /*W*/  /*L*/  if(finish[j] == 0)
  /*W*/  /*L*/  {
  /*W*/  /*L*/    /*F*/ // Check if the need of a process for each resource is
  /*W*/  /*L*/    /*F*/ // greater than what is currently available. If it is go
  /*W*/  /*L*/    /*F*/ // to the next process as the current one cannot finish.
  /*W*/  /*L*/    /*F*/ // If all needed resources are less than or equal to
  /*W*/  /*L*/    /*F*/ // those available the process can finish.
  /*W*/  /*L*/    /*F*/ bool can_finish = false;
  /*W*/  /*L*/    /*F*/
  /*W*/  /*L*/    /*F*/ for(int k = 0; k < NUM_RESOURCES; k++)
  /*W*/  /*L*/    /*F*/ {
  /*W*/  /*L*/    /*F*/   if(work[k] < local_need[j][k])
  /*W*/  /*L*/    /*F*/   {
  /*W*/  /*L*/    /*F*/     break;
  /*W*/  /*L*/    /*F*/   }
  /*W*/  /*L*/    /*F*/
  /*W*/  /*L*/    /*F*/   if(k == NUM_RESOURCES - 1)
  /*W*/  /*L*/    /*F*/   {
  /*W*/  /*L*/    /*F*/     can_finish = true;
  /*W*/  /*L*/    /*F*/   }
  /*W*/  /*L*/    /*F*/ }
  /*W*/  /*L*/
  /*W*/  /*L*/    /*P*/ // Print the state for the current process being tested.
  /*W*/  /*L*/    /*P*/printf("%d", j);
  /*W*/  /*L*/    /*P*/print_array("", work, NUM_RESOURCES, false);
  /*W*/  /*L*/    /*P*/print_array("", local_need[j], NUM_RESOURCES, false);
  /*W*/  /*L*/    /*P*/print_array("", local_allocation[j],NUM_RESOURCES, false);
  /*W*/  /*L*/    /*P*/print_array("", finish, NUM_CUSTOMERS, false);
  /*W*/  /*L*/
  /*W*/  /*L*/    /*R*/ // If a process can finish print a confirmation message
  /*W*/  /*L*/    /*R*/ // and release all of the resources allocated to that
  /*W*/  /*L*/    /*R*/ // process back to the available. If the process cannot
  /*W*/  /*L*/    /*R*/ // finish print the matching can't finish prompt.
  /*W*/  /*L*/    /*R*/if(can_finish)
  /*W*/  /*L*/    /*R*/{
  /*W*/  /*L*/    /*R*/  //✓ - u2713
  /*W*/  /*L*/    /*R*/  printf(" | \u2713 Process %d can finish\n", j);
  /*W*/  /*L*/    /*R*/
  /*W*/  /*L*/    /*R*/  for(int l = 0; l < NUM_RESOURCES; l++)
  /*W*/  /*L*/    /*R*/  {
  /*W*/  /*L*/    /*R*/    work[l] += local_allocation[j][l];
  /*W*/  /*L*/    /*R*/    local_allocation[j][l] = 0;
  /*W*/  /*L*/    /*R*/  }
  /*W*/  /*L*/    /*R*/
  /*W*/  /*L*/    /*R*/  finish[j] = 1;
  /*W*/  /*L*/    /*R*/}
  /*W*/  /*L*/    /*R*/else
  /*W*/  /*L*/    /*R*/{
  /*W*/  /*L*/    /*R*/  //✗ - u2717
  /*W*/  /*L*/    /*R*/  printf(" | \u2717 Process %d can't finish\n", j);
  /*W*/  /*L*/    /*R*/}
  /*W*/  /*L*/  }
  /*W*/  /*L*/}
  /*W*/
  /*W*/  /*C*/ // Count the number of processes completed after looping over
  /*W*/  /*C*/ // every process.
  /*W*/  /*C*/for(int m = 0; m < NUM_CUSTOMERS; m++)
  /*W*/  /*C*/{
  /*W*/  /*C*/  if(finish[m])
  /*W*/  /*C*/  {
  /*W*/  /*C*/    n_end_finished_processes++;
  /*W*/  /*C*/  }
  /*W*/  /*C*/}
  /*W*/
  /*W*/  /*F*/ // Check if the nummber of processes completed at the start is
  /*W*/  /*F*/ // equal to the number that were completed at the start of the
  /*W*/  /*F*/ // loop. If so then no more of the processes have been able to
  /*W*/  /*F*/ // complete and the system is in an unsafe state. If the number
  /*W*/  /*F*/ // of processes at the end of the loop is equal to the number of
  /*W*/  /*F*/ // customers than all processes are completed.
  /*W*/  /*F*/if(n_end_finished_processes == n_start_finished_processes)
  /*W*/  /*F*/{
  /*W*/  /*F*/  return false;
  /*W*/  /*F*/}
  /*W*/  /*F*/
  /*W*/  /*F*/if(n_end_finished_processes == NUM_CUSTOMERS)
  /*W*/  /*F*/{
  /*W*/  /*F*/  return true;
  /*W*/  /*F*/}
  /*W*/
  /*W*/}
}

void print_struct(customer_struct s)
{
  printf("Process Num: %d\n", s.thread_num);

  printf("Max Resources: [ ");

  for(int p = 0; p < NUM_RESOURCES; p++)
  {
    printf("%d ", s.max[p]);
  }

  printf("]\n");
}

void print_array(char* title, int a[], int length, bool newline)
{
  printf("%s : [ ", title);

  for(int i = 0; i < length; i++)
  {
    printf("%d ", a[i]);
  }

  printf("]");

  if(newline)
  {
    printf("\n");
  }
}

void print_matricies()
{
  printf("#   [Require]   [ Alloc ]   [  Max  ]\n");

  for(int i = 0; i < NUM_CUSTOMERS; i++)
  {
    printf("%d : [ ", i);

    for(int j = 0; j < NUM_RESOURCES; j++)
    {
      printf("%d ", need[i][j]);
    }

    printf("] | [ ");

    for(int j = 0; j < NUM_RESOURCES; j++)
    {
      printf("%d ", allocation[i][j]);
    }

    printf("] | [ ");

    for(int j = 0; j < NUM_RESOURCES; j++)
    {
      printf("%d ", maximum[i][j]);
    }

    puts("]");
  }
}
