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
#include <semaphore.h>
#include "banker.h"
#include <unistd.h>

pthread_mutex_t mutex;

int available[NUM_RESOURCES];                 // Available amount of each resource
int maximum[NUM_CUSTOMERS][NUM_RESOURCES];    // Maximum demand of each customer
int allocation[NUM_CUSTOMERS][NUM_RESOURCES]; // Amount currently allocated to each customer
int need[NUM_CUSTOMERS][NUM_RESOURCES];       // Remaining need of each customer

int main(int argc, char *argv[])
{
    /*I*/ // Initialization: Check to make sure user entered the correct amount
    /*I*/ // of resources and loads them into the available array and Initializes
    /*I*/ // the mutex lock.
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
    /*C*/pthread_mutex_destroy(&mutex);
    /*C*/
    /*C*/return EXIT_SUCCESS;
} // main

void *customer(void *arg)
{
  /*I*/ // Intialization: typecast the struct passed as an argument to the thread
  /*I*/ // as well the has array, a local copy of this threads allocated resources
  /*I*/ // and sets the looping variable to false.
  /*I*/customer_struct* s = (customer_struct*) arg;
  /*I*/int has[NUM_RESOURCES] = {0};
  /*I*/bool complete = false;

  /*W*/ // Loop the thread until it is complete by aquiring all needed resources.
  /*W*/ // Waits for user to hit enter before proceeding, then generates a random
  /*W*/ // request that ranges from [0,max needed] and then aquires the mutex to
  /*W*/ // issue a request. If the request is granted
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
  /*W*/    //Check if customer has all required resources by comparing has array to max array
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
  printf("Proc%d Req", n_customer);
  print_array("", request, NUM_RESOURCES, true);

  print_array("Available", available, NUM_RESOURCES, true);
  puts("");

  for(int i = 0; i < NUM_RESOURCES; i++)
  {
    if(request[i] > need[n_customer][i])
    {
      printf("ERROR: Thread number %d requested more resources than maximum\n", n_customer);
      exit (EXIT_FAILURE);
    }
  }

  for(int j = 0; j < NUM_RESOURCES; j++)
  {
    if(request[j] > available[j])
    {
      printf("DENIED: Resources not available\n");
      puts("==============================");
      return false;
    }
  }

  //apply
  for(int k = 0; k < NUM_RESOURCES; k++)
  {
      available[k] -= request[k];
      allocation[n_customer][k] += request[k];
      need[n_customer][k] -= request[k];
  }

  if(check_safe(available,allocation,need))
  {
    printf("GRANTED: %d now has", n_customer);
    print_array("", allocation[n_customer], NUM_RESOURCES, true);
    puts("==============================");

    return true;
  }
  else
  {
    //revert
    for(int m = 0; m < NUM_RESOURCES; m++)
    {
        available[m] += request[m];
        allocation[n_customer][m] -= request[m];
        need[n_customer][m] += request[m];
    }

    printf("DENIED: Unsafe state!\n");
    puts("==============================");

    return false;
  }
}

// Release resources, returns true if successful
bool release_res(int n_customer, int release[])
{
  printf("Releasing all resources from process %d.\n", n_customer);

  for(int i = 0; i < NUM_RESOURCES; i++)
  {
    available[i] += release[i];
    allocation[n_customer][i] = 0;
    maximum[n_customer][i] = 0;
  }

  return true;
}

bool check_safe(int available[], int allocation[][NUM_RESOURCES], int need[][NUM_RESOURCES])
{
  //printf("check_safe called\n");

  int work[NUM_RESOURCES] = {0};
  int finish[NUM_CUSTOMERS] = {0};
  int local_allocation[NUM_CUSTOMERS][NUM_RESOURCES];
  int local_need[NUM_CUSTOMERS][NUM_RESOURCES];

  for(int e = 0; e < NUM_CUSTOMERS; e++)
  {
    for(int f = 0; f < NUM_RESOURCES; f++)
    {
      local_allocation[e][f] = allocation[e][f];
      local_need[e][f] = need[e][f];
    }
  }

  for(int i = 0; i < NUM_RESOURCES; i++)
  {
    work[i] = available[i];
  }

  printf("#   [Require]   [ Alloc ]   [  Max  ]\n");

  print_matricies();

  printf("\n");
  print_array("Post Request", work, NUM_RESOURCES, true);
  printf("\n");

  bool printed = false;

  while(1)
  {
    int n_start_finished_processes = 0;
    int n_end_finished_processes = 0;

    for(int n = 0; n < NUM_CUSTOMERS; n++)
    {
        if(finish[n])
        {
          n_start_finished_processes++;
        }
    }
    if(!printed)
    {
      printf("#   [ Avail ]   [Require]   [ Alloc ]   [    Fin    ]\n");
      printed = true;
    }
    for(int j = 0; j < NUM_CUSTOMERS; j++)
    {
      bool can_finish = false;

      if(finish[j] == 0)
      {
        for(int k = 0; k < NUM_RESOURCES; k++)
        {
          if(work[k] < local_need[j][k])
          {
            break;
          }

          if(k == NUM_RESOURCES - 1)
          {
            can_finish = true;
          }
        }

        printf("%d", j);
        print_array("", work, NUM_RESOURCES, false);
        print_array("", local_need[j], NUM_RESOURCES, false);
        print_array("", local_allocation[j],NUM_RESOURCES, false);
        print_array("", finish, NUM_CUSTOMERS, false);

        if(can_finish)
        {
          printf(" | Process %d can finish\n", j);

          for(int l = 0; l < NUM_RESOURCES; l++)
          {
            work[l] += local_allocation[j][l];
            local_allocation[j][l] = 0;
          }

          finish[j] = 1;
        }
        else
        {
          printf(" | Process %d can't finish\n", j);
        }
      }
    }

    for(int m = 0; m < NUM_CUSTOMERS; m++)
    {
      if(finish[m])
      {
        n_end_finished_processes++;
      }
    }

    if(n_end_finished_processes == n_start_finished_processes)
    {
      return false;
    }
    if(n_end_finished_processes == NUM_CUSTOMERS)
    {
      return true;
    }
  }
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

void print_need()
{

  puts("Need Arrays");

  for(int i = 0; i < NUM_CUSTOMERS; i++)
  {
    printf("%d : [ ", i);

    for(int j = 0; j < NUM_RESOURCES; j++)
    {
      printf("%d ", need[i][j]);
    }

    puts("]");
  }
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
