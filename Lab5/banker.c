/*
 * Banker's Algorithm for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 *
 */
#define _XOPEN_SOURCE 700 // required for barriers to work
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

typedef struct
{
    int thread_num;
    int max[NUM_RESOURCES];
}customer_struct;

void *customer(void *arg);
bool check_safe(int available[], int allocation[][NUM_RESOURCES], int need[][NUM_RESOURCES]);
void print_available();
void print_struct(customer_struct s);

pthread_mutex_t mutex;

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

  printf("Req#%d: [ ", n_customer);

  for(int p = 0; p < NUM_RESOURCES; p++)
  {
    printf("%d ", request[p]);
  }

  printf("]\n");

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
  printf("release call\n");

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

    for(int a = 0; a < NUM_RESOURCES; a++)
    {
      available[a] = atoi(argv[a+1]);
    }

    print_available();

    // Initialize the pthreads, locks, mutexes, etc.
    pthread_t customers[NUM_CUSTOMERS]; //create array of threads
    pthread_mutex_init(&mutex,NULL);

    // Run the threads and continually loop

    puts("Before thread creation");
    puts("======================");

    for (int i = 0; i < NUM_CUSTOMERS; i++)
    {
      customer_struct s;

      s.thread_num = i - 1*i;

      for(int j = 0; j < NUM_RESOURCES; j++)
      {
          int resource_max = rand() % atoi(argv[j+1]);

          s.max[j] = resource_max;

          allocation[i][j] = 0;
          maximum[i][j] = resource_max;
          need[i][j] = resource_max;
      }
      print_struct(s);

      pthread_create(&customers[i], 0, customer, (void *) &s);
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

    pthread_mutex_destroy(&mutex); //destroy the mutex

    return EXIT_SUCCESS;
}

void *customer(void *arg)
{
  sleep(10);

  puts("Inside a thread");
  puts("===============");

  customer_struct* s = (customer_struct*) arg;
  int has[NUM_RESOURCES] = {0};

  print_struct(*s);

  bool complete = false;

  while(!complete)
  {
    sleep(2);

    printf("thread %d itterating press enter to continue\n", s->thread_num);
    getchar();

    //create request array and generate random request
    int req[NUM_RESOURCES];

    for(int i = 0; i < NUM_RESOURCES; i++)
    {
      printf("breaks here?\n");
      req[i] = rand() % (s->max[i] - has[i]);
    }

    //Wait for mutex and then issue a request
    printf("#%d waiting for mutex\n", s->thread_num);

    pthread_mutex_lock(&mutex);
    bool request_granted = request_res(s->thread_num, req);
    pthread_mutex_unlock(&mutex);

    if(request_granted)
    {
      printf("granted?");
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
          //Wait for mutex and then issue a release
          pthread_mutex_lock(&mutex);
          release_res(s->thread_num, has);
          pthread_mutex_unlock(&mutex);

          complete = true;
        }
      }
    }
  }

  printf("return from %d\n", s->thread_num);
  return (void *) NULL;
}

bool check_safe(int available[], int allocation[][NUM_RESOURCES], int need[][NUM_RESOURCES])
{
  printf("check_safe called\n");

  int work[NUM_RESOURCES];
  bool finish[NUM_CUSTOMERS] = {false};

  for(int h = 0; h < NUM_CUSTOMERS; h ++)
  {
    for(int g = 0; g < NUM_RESOURCES; g++)
    {
      if(need[h][g] != 0)
      {
        break;
      }

      if(g == NUM_RESOURCES -1)
      {
        finish[h] = true;
      }
    }
  }

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

void print_available()
{
  printf("Available: [");

  for(int p = 0; p < NUM_RESOURCES; p++)
  {
    printf(" %d ", available[p]);
  }

  printf("]\n");
}

void print_struct(customer_struct s)
{
  printf("Thread Num: %d\n", s.thread_num);

  printf("Max Resources: [ ");

  for(int p = 0; p < NUM_RESOURCES; p++)
  {
    printf("%d ", s.max[p]);
  }

  printf("]\n");
}
