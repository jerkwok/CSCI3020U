#CSCI 3020U
#Lab #4: Host Dispatcher Shell Project
#####Alexandar Mihaylov 100536396
#####Jeremy Kwok - 100341977
#####Taylor Smith - 100372402
#####Elias Amal - 100494613
#####Luisa Rojas - 100518772

--------------------------------

####Objective
Create a C program that dispatches and schedules the execution of processes in multiple queues.

####Program Flow

######The Job Dispatch List:

The Host Dispatcher program will initially take in a list of processes and their requirements from a file. The requirements outline the amount of memory and other I/O devices that the process requires. Each process also outlines the arrival time, runtime and priority of each process. The program loads each process and it's outline into a queue, placing the processes one at a time into the data structure. Once the dispatch list has been fully loaded into the dispatch queue, the program starts its execution. Every second, the dispatch queue iterates through each of the processes in its queue. If the arrival time of a process in the dispatch queue is equal to the amount of seconds passed, the dispatch queue will put that process in its proper queue depending on the priority value of the process. The dispatcher program continues execution second by second until the dispatch queue is empty and all the processes have been run to completion. 

######Multiple Job Queues:

Each process in the dispatch queue has a priority associated with it. When a process comes off the dispatch queue, the dispatcher will put it into one of four queues depending on its priority. Processes with a priority of 0 will be put into the Real Time queue, and processes with priorities 1-3 will be placed into the priority 1-3 queue.

######Memory Allocation:

The free memory that we have devoted to our dispatcher is represented by an integer array, where a 0 represents free memory and a 1 represents currently occupied memory. When a non real time process runs for 1 second, it is put into a lower queue and the memory is still occupied. The memory is only freed up once the process runs to completion, and because of this, we could have insufficient memory for a process to execute when it is on the front of the queue. To check if we have sufficient memory, we use a First Fit memory allocation scheme. When a process requires an amount of memory, the program will analyze our memory array, and find the first continuous amount of memory that the process requires. The benefit to this allocation scheme is that the allocation is very fast, as the dispacther stops as soon as it finds a large enough continious section of memory; it doesn't spend a long time analyzing the avaliable memory. The downside to a First Fit memory allocation scheme is that as multiple processes start and run to execution, the memory will be allocated and deallocated many times, and since we only perform a cursory analysis of the avaliable memory, we can end with with many small chunks of memory that are unable to be allocated, causing inefficiencies.   

######Multilevel Dispatching Schemes: