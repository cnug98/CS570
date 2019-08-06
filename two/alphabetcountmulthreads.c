/*
 * alphabetcountmulthreads.c - this file implements the alphabetcountmulthreads function.
 */
#include <dirent.h>
#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include "count.h"
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int file_index = 0;
char *pat; //both pat and temparr are replicas of path and alphabetfreq used in the thread function, then copies the results to alphabetfreq afterwards
long temparr[ALPHABETSIZE] = {0};
struct Data {
    char files [255];           
};

typedef struct thread_param_s { //this struct stores the info of files from data, being their number and their names
     struct Data* e; // each thread will get an array of files from data
     int n;    // number of elements
} thread_param_t;


void *threadstuff(void *param) { 

    thread_param_t* thread_param = (thread_param_t*)param; //initializes the data needed to access by this specific thread
    int j;
    for(j = 0; j < thread_param->n; j++) { //loops through total number of elements
        char p[256];
        struct Data *temp = thread_param->e + j;
        strcpy(p, pat); //concatenates the full data directory to p
        strcat(p, "/");
        strcat(p, temp->files);
        FILE *f = fopen(p, "r");
        if(f == NULL) { //can't open file, abort
            return 0;
        }
        printf("Thread id = %ld is processing  %s\n", pthread_self(), temp->files); //print file processed
        int c = fgetc(f); //grabs the first character
        while(!feof(f)) {
            if((c >= 65) && (c <= 90)) //checks if char's ascii value is uppercase, and changes it to lowercase
                c = c + 32;
            if((c >= 97) && (c <= 122)) { //if alphabet letter is found 
                pthread_mutex_lock(&mutex); //locks to prevent interference from other threads
                temparr[c - 97] = (long) temparr[c - 97] + 1; //increments desired character counter. c-97 corresponds to each lowercase's location in the array
                pthread_mutex_unlock(&mutex);
            }
            c = fgetc(f); //re-initializes fgetc for next character in while loop
        }
        fclose(f);
    }

    printf("Thread id = %ld is done!\n", pthread_self()); //prints finishing statement
    pthread_exit(NULL);

}


void alphabetcountmulthreads(char *path, char *filetowrite, long alphabetfreq[], int num_threads) //IMPORTANT: The algorithm I used to store the filenames from prog0 doesn't store them in order like in data, so while the indexes processed are correct, the actual files don't line up with their index (i.e. input_09.txt ends up in index 15, but is still processed by a thread correctly). Keep that in mind when grading this program
{
	DIR *d;
    char *filename;
    char *fil;
    int start = 0; //index trackers
    int end = 0;
    struct dirent *dir; //directory opening process
    d = opendir(path);
    struct Data fil_names[255];
    pat = path;
    while((dir = readdir(d)) != NULL)
    {
            char *filename = dir->d_name;
            size_t t = strlen(filename) - 3; //checks for .txt extentsion
                int ctr = 0;
                while(t < strlen(filename)) {
                    if(!(filename[t] == 't' || filename[t] == 'x'))
                        continue;
                    else {
                        ctr++; //adds the current letter to a counter
                    }
                    t++;
                }
                if(ctr == 3) {
                    fil = dir->d_name; //immediately stores validated file to be read
                    strcpy(fil_names[file_index].files, fil); // store file name into fil_names array;
                    file_index++;   // increase the file index for next file.
                }

    }

    pthread_t tid_array[num_threads]; //initializes the numer of threads needed in an array of threads
    thread_param_t thread_param[num_threads];
    int thread_counter = 0;

    int set = file_index / num_threads; // number of files for each thread
    int i;

    for(i = 0; i < num_threads - 1; i++) { // create all threads
       thread_param[i].n = set; //sets element numbers to set
       thread_param[i].e = fil_names + set * i; //grabs the correct data set correspoding to the thread
       end = start + thread_param[i].n - 1; //sets the correct end index for the first runthrough, will be incremented by start later
       pthread_create(&tid_array[i], NULL, threadstuff, thread_param + i); //initializes thread
       printf("Thread id = %ld is processing indexes %d to %d\n", tid_array[i], start, end); //prints what indexes the thread will be processing
       start = end + 1; //aligns start correctly after the first pass
    }
    // the last thread may get more data if the round is not zero
    thread_param[i].n = file_index - set * i; //grabs the remaining extra files and adds them to the threads element count
    thread_param[i].e = fil_names + set * i;
    end = start + thread_param[i].n - 1;
    pthread_create(&tid_array[i], NULL, threadstuff, thread_param + i); //creates thread and prints index range
    printf("Thread id = %ld is processing indexes %d to %d\n", tid_array[i], start, end);

    for(int i = 0; i< num_threads; i++) { //joins all threads after completion
        pthread_join(tid_array[i], NULL);
    }
    for (int i = 0; i < 26; i++) { //copies the results from threads back to alphabetfreq to coincide with test function
        alphabetfreq[i] = temparr[i];
    }
    
    FILE *g = fopen(filetowrite, "w"); //opens result.txt for writing
    for(int i = 0; i < 26; i++) { //loops through entire alphabetfreq
        fprintf(g, "%c -> %ld\n", (char)(i+97), alphabetfreq[i]); //formatted writing
    }   
    
    fclose(g); //close write file
    
    closedir(d); //close directory  		
	
}



/**
  The alphabetcountmulthreads function counts the frequency of each alphabet letter (a-z, case insensitive) in all the .txt files under
  directory of the given path and write the results to a file named as filetowrite. Different with programming assignment#0, you need to implement the program using mutithreading.
  
  Input: 
      path - a pointer to a char string [a character array] specifying the path of the directory; and
      filetowrite - a pointer to a char string [a character array] specifying the file where results should be written in.
      alphabetfreq - a pointer to a long array storing the frequency of each alphabet letter from a - z, which should be already up-to-date;
      num_threads - number of the threads running in parallel to process the files
      
       
  Output: a new file named as filetowrite with the frequency of each alphabet letter written in
  
  Requirements:
1)	Multiple threads are expected to run in parallel to share the workload, i.e. suppose 3 threads to process 30 files, then each thread should process 10 files;
2)	When a thread is created, a message should be print out showing which files this thread will process, for example:
Thread id = 274237184 starts processing files with index from 0 to 10!
3)	When a file is being processed, a message should be print out showing which thread (thread_id = xxx) is processing this file, for example:
Thread id = 265844480 is processing file input_11.txt
4)	When a thread is done with its workload, a message should be print out showing which files this thread has done with work, for example:
      Thread id = 274237184 is done !
5)	The array: long alphabetfreq[ ] should always be up-to-date, i.e. it always has the result of all the threads counted so far.  [You may need to use mutexes to protect this critical region.]


You should have the screen printing should be similar as follows:

 Thread id = 274237184 starts processing files with index from 0 to 10!
 Thread id = 265844480 starts processing files with index from 11 to 22!
 Thread id = 257451776 starts processing files with index from 23 to 31!

 Thread id = 265844480 is processing file input_11.txt
 Thread id = 257451776 is processing file input_22.txt
 Thread id = 274237184 is processing file input_00.txt
  … … 

 Thread id = 274237184 is done !
 Thread id = 265844480 is done !
 Thread id = 257451776 is done !

 The results are counted as follows:
 a -> 2973036
 b -> 556908
 c -> 765864
 …  … 
*/
