/*
 * The code is not part of the real application, and just used to 
 * illustrate the bounded-buffer problem using Semaphore and/or mutexes. 
 * Detailed requirements please refer to the assignment documentation.
 * 
*/

#include <stdio.h> 
#include <stdlib.h> 
#include "bbuffer.h"
#include <semaphore.h>

sem_t mutex; //declares semaphores before initialization
sem_t empty;
sem_t full;

void initilization()
{
    sem_init(&empty, 0, BUFFER_SIZE); //empty and full semaphores are created, only empty needs the BUFER_SIZE upper bound
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1); //set up binary semaphore lock
    buffer.next_in = 0; //resets insert and remove pointers
    buffer.next_out = 0;
}

/* *
 * insert_item - thread safe(?) function to insert items to the bounded buffer
 * @param item the value to be inserted
 * @return 0 in case of sucess -1 otherwise
 */
  
int insert_item(int item, long int id)
{
    sem_wait(&empty); //waits for status of empty
    sem_wait(&mutex); //sets up lock
    if(buffer.next_in < BUFFER_SIZE) {  //extra precaution in case buffer.next_in ever goes out of bounds
        buffer.value[buffer.next_in] = item; //adds item and prints statement
        printf("producer %ld: inserted item %d into buffer index %d\n", id, item, buffer.next_in);
        if(buffer.next_in == (BUFFER_SIZE - 1)) //if next_in is at end of buffer, reset to index 0
            buffer.next_in = 0;
        else
            buffer.next_in = (buffer.next_in + 1) % BUFFER_SIZE; //otherwise increment to next buffer index
    }
    else //otherwise can't insert, aborts procedure
        return -1;
    sem_post(&mutex); //unlocks method
    sem_post(&full);   //increments full
    return 0;
}

/**
 * remove_item - thread safe(?) function to remove items to the bounded buffer
 * @param item the address of the variable that the removed value will be written
 * @return 0 in case of sucess -1 otherwise
 */
int remove_item(int *item, long int id)
{
    sem_wait(&full); //waits for status for full
    sem_wait(&mutex); //sets up lock
    if(buffer.next_out > -1) { //extra precaution in case buffer.next_out ever goes out of bounds
        *item = buffer.value[buffer.next_out]; //stores item info for printing
        buffer.value[buffer.next_out] = -1;    //overwrites item and prints statement
        printf("consumer %ld: removed item %d from buffer index %d\n", id, *item, buffer.next_out); 
        if(buffer.next_out == (BUFFER_SIZE - 1)) //if next_out is at end of buffer, reset to index 0
            buffer.next_out = 0;
        else //otherwise increment to next buffer index
            buffer.next_out = (buffer.next_out + 1) % BUFFER_SIZE;
    }
    else //otherwise can't remove, abort procedure
        return -1;
    sem_post(&mutex); //unlocks method
    sem_post(&empty); //increments empty
    return 0;
}
//The other functions are essentially untouched, they were completed as part of the assignment and don't directly have to do with semaphore usage

/**
 * producer - will iterate PRODUCER_ITERATION times and call the corresponding
 * function to insert an integer to the bounded buffer
 * @param param an integer id of the producer used to distinguish between the
 * multiple producer threads
 * @return nothing
 */
void * producer(void *param)
{
    int item, i;
    long int id = (long int)param;

    printf("producer %d started!\n", id);
    i = PRODUCER_ITERATIONS;
    while (i--) {
	  sleep(rand() % 3);

	item = rand() % 10000;
	if (insert_item(item, id))
	    printf("Error while inserting to buffer\n");
    }

    pthread_exit(0);
}

/**
 * consumer - will iterate CONSUMER_ITERATION times and call the corresponding
 * function to remove an integer from the bounded buffer
 * @param param an integer id of the producer used to distinguish between the
 * multiple consumer threads
 * @return nothing
 */
void * consumer(void *param)
{
    int item, i;
    long int id = (long int)param;

    printf("consumer %d started!\n", id);
    i = CONSUMER_ITERATIONS;
    while (i--) {
	sleep(rand() % 3);

	if (remove_item(&item, id))
	    printf("Error while removing from buffer\n");
    }

    pthread_exit(0);
}
