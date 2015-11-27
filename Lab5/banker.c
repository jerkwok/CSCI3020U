/*
 * Banker's Algorithm for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
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

// Put any other macros or constants here using #define
// May be any values >= 0
#define NUM_CUSTOMERS 5
#define NUM_RESOURCES 3

void *customer(void *arg);
bool check_safe(int available[], int allocation[][NUM_RESOURCES], int need[][NUM_RESOURCES]);


typedef struct
{
    int thread_num;
    int max[NUM_RESOURCES];
}customer_struct;

sem_t sem;

// Put global environment variables here
// Available amount of each resource
int available[NUM_RESOURCES];

// Maximum demand of each customer
int maximum[NUM_CUSTOMERS][NUM_RESOURCES];

// Amount currently allocated to each customer
int allocation[NUM_CUSTOMERS][NUM_RESOURCES];

// Remaining need of each customer
int need[NUM_CUSTOMERS][NUM_RESOURCES];

// Define functions declared in banker.h here
bool request_res(int n_customer, int request[])
{
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
      return false;
    }
  }

  for(int k = 0; k < NUM_RESOURCES; k++)
  {
      available[k] -= request[k];
      allocation[n_customer][k] += request[k];
      need[n_customer][k] -= request[k];
  }

  if(check_safe(available,allocation,need))
  {
    for(int l = 0; l < NUM_RESOURCES; l++)
    {
      maximum[n_customer][l] = 0;
    }
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

    return false;
  }
}

// Release resources, returns true if successful
bool release_res(int n_customer, int release[])
{
  for(int i = 0; i < NUM_RESOURCES; i++)
  {
    available[i] += release[i];
    allocation[n_customer][i] = 0;
    maximum[n_customer][i] = 0;
  }

  return true;
}

int main(int argc, char *argv[])
{
    // ==================== YOUR CODE HERE ==================== //

    // Read in arguments from CLI, NUM_RESOURCES is the number of arguments
    // Allocate the available resources

    if(argc -1 != NUM_RESOURCES)
    {
      printf("ERROR: Expected %d reources, recieved %d\n", NUM_RESOURCES, argc);
      exit(EXIT_FAILURE);
    }

    for(int i = 0; i < NUM_RESOURCES; i++)
    {
      available[i] = (int) argv[i+1];
    }

    // Initialize the pthreads, locks, mutexes, etc.
    pthread_t customers[NUM_CUSTOMERS]; //create array of threads
    sem_init(&sem,0,1); //init the semaphore

    // Run the threads and continually loop

    for (int i = 0; i < NUM_CUSTOMERS; i++)
    {
      customer_struct s;

      s.thread_num = i;

      for(int j = 0; j < NUM_RESOURCES; j++)
      {
          int resource_max = rand() % (int) argv[j];

          s.max[j] = resource_max;

          allocation[i][j] = 0;
          maximum[i][j] = resource_max;
          need[i][j] = resource_max;
      }

      pthread_create(&customers[i], 0, customer , (void *) &s);
    }

    //Join all threads
    for (int k = 0; k < NUM_CUSTOMERS; k++)
    {
      pthread_join(customers[k],0);
    }

    // The threads will request and then release random numbers of resources

    // If your program hangs you may have a deadlock, otherwise you *may* have
    // implemented the banker's algorithm correctly

    // If you are having issues try and limit the number of threads (NUM_CUSTOMERS)
    // to just 2 and focus on getting the multithreading working for just two threads

    sem_destroy(&sem); //destroy the semaphore

    return EXIT_SUCCESS;
}

void *customer(void *arg)
{
  customer_struct* s = (customer_struct*) arg;
  int has[NUM_RESOURCES] = {0};

  bool complete = false;

  while(!complete)
  {
    sleep(1);

    //create request array and generate random request
    int req[NUM_RESOURCES];

    for(int i = 0; i < NUM_RESOURCES; i++)
    {
      req[i] = rand() % (s->max[i] - has[i]);
    }

    //Wait for semaphore and then issue a request
    sem_wait(&sem);
    bool request_granted = request_res(s->thread_num, req);
    sem_post(&sem);

    if(request_granted)
    {
      //Update has array.
      for(int j = 0; j < NUM_RESOURCES; j++)
      {
        has[j] += req[j];
      }

      //Check if customer has all required resources by comparing has array to max array
      for(int k = 0; k < NUM_RESOURCES; k++)
      {
        if(has[k] != s->max[k])
        {
          break;
        }

        //If hasn't called break by last iteration of k, all are equal and release all resources
        if(k == NUM_RESOURCES - 1)
        {
          //Wait for semaphore and then issue a release
          sem_wait(&sem);
          release_res(s->thread_num, has);
          sem_post(&sem);

          complete = true;
        }
      }
    }
  }

  return (void *) NULL;
}

bool check_safe(int available[], int allocation[][NUM_RESOURCES], int need[][NUM_RESOURCES])
{
  int work[NUM_RESOURCES];
  bool finish[NUM_CUSTOMERS] = {false};

  for(int i = 0; i < NUM_RESOURCES; i++)
  {
    work[i] = available[i];
  }

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

    bool can_finish = false;

    for(int j = 0; j < NUM_CUSTOMERS; j++)
    {
      for(int k = 0; k < NUM_RESOURCES; k++)
      {
        if(work[k] < need[j][k])
        {
          break;
        }
        if(k == NUM_RESOURCES - 1)
        {
          can_finish = true;
        }
      }
      if(can_finish)
      {
        for(int l = 0; l < NUM_RESOURCES; l++)
        {
          work[l] += allocation[j][l];
          allocation[j][l] = 0;
        }

        finish[j] = true;
        break;
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
