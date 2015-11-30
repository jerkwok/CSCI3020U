/*
 * Banker's Algorithm for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, Taylor Smith
 * All rights reserved.
 *
 */
#ifndef BANKER_H_
#define BANKER_H_

#include <stdbool.h>

// Add any additional data structures or functions you want here
// perhaps make a clean bank structure instead of several arrays...

// May be any values >= 0
#define NUM_CUSTOMERS 5
#define NUM_RESOURCES 3

typedef struct
{
    int thread_num;
    int max[NUM_RESOURCES];
}customer_struct;

// Request resources, returns true if successful
extern bool request_res(int n_customer, int request[]);

// Release resources, returns true if successful
extern bool release_res(int n_customer, int release[]);

extern void *customer(void *arg);
extern bool check_safe(int available[], int allocation[][NUM_RESOURCES], int need[][NUM_RESOURCES]);
extern void print_struct(customer_struct s);
extern void print_array(char* title, int a[], int length, bool newline);
extern void print_matricies();

#endif /* BANKER_H_ */
