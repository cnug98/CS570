/*
 *  Implementation of FIFO and LRU page replacement algorithm
 *  Please add appropriate level of comments in this file 
 */

#include "vmalgorithm.h"


/* Generate an access pattern
 * Example: 3, 2, 2, 1, 1  indicates the page 3, 2, 2, 1, 1 in order
 */
void generateAccessPattern()
{
   int i;
   srand(time(0));
   accessPattern = (int *)malloc( sizeof(int) * AccessPatternLength);   
   printf("The randomized Access Pattern: ");
   for(i=0; i< AccessPatternLength; i++)
   {
		     accessPattern[i] = rand() % ReferenceSZ;
        printf("%d ", accessPattern[i]);       
   }
   printf("\n");
}

/*
 * Initialize the parameters of simulated memory
 */
void initializePageFrame()
{
   int i;
   memory.PageFrameList = (int *)malloc( sizeof(int)* FrameNR );    // dynamic allocated FrameNR frames to be used in memory
   memory.nextToReplaced =0;          // indicate the new frame to be replaced as 0
   for(i=0; i< FrameNR; i++)
   {
			memory.PageFrameList[i] = -1;  // initialization to -1 indicating all frames are unused 
   }

}

// Print the pages loaded in memory
void printPageFrame()
{
   int i;
   for(i=0; i< FrameNR; i++)
   {
			printf("%2d ",memory.PageFrameList[i]);       
   }
   printf("\n");
}


/*
 *  Print the access pattern in order
 */
void printAccessPattern()
{
   int i;
   printf("The Same Access Pattern: ");
   for(i=0; i< AccessPatternLength; i++)
   {
        printf("%d ", accessPattern[i]);       
   }
   printf("\n");

}

void printFirstAccessPattern() {
   int i;
   printf("The Access Pattern: ");
   for(i=0; i< AccessPatternLength; i++){
        printf("%d ", accessPattern[i]);       
   }
   printf("\n");
}


/*
 * Return: number of the page fault using FIFO page replacement algorithm
 */
int FIFO() {
   printFirstAccessPattern(); //prints the first statement, testvm doesn't do this on its own
   int page_faults = 0, k = 0; //initializes page fault counter along with loop counters and flags
   int m, n, s;
   
   for(m = 0; m < ReferenceSZ; m++) { //cycle through full page range
        s = 0;
        for(n = 0; n < FrameNR; n++) { //cycles through each frame for page hit
            if(accessPattern[m] == memory.PageFrameList[n]){ //if found
                s++; //skip other statements
                k = m; //initialize new position for m
                page_faults--; //prevent page fault from incrementing
            }
        }     
        page_faults++;
        if((page_faults < FrameNR) && (s == 0)) { //if we haven't used all page frames yet
            if(k != 0) { //if a page hit has occurred before we have used all page frames
                memory.PageFrameList[k] = accessPattern[m]; //add value to correct page frame index
                k = m; //re-syncs k to the current index
            }
            else //otherwise add value as normal
                memory.PageFrameList[m] = accessPattern[m];
        }   
        else if(s == 0) { //if all page frames have been used
            memory.PageFrameList[(page_faults - 1) % FrameNR] = accessPattern[m]; //find the first used index and replace the value. This will account for all page frame indexes
        }
        for(n = 0; n < FrameNR; n++){  //print current page frames   
            printf("%d  ", memory.PageFrameList[n]);
        }
        printf("\n");
    } 
    return page_faults; //return the amount of page faults
}



/*
 * Return: number of the page fault using LRU page replacement algorithm
 */

int LRU() {

      int temp[FrameNR]; //initialized temp array to hold lru index for replacement
      int m, n, k, l, position; //initializes loop counters and flags
      int a = 0, b = 0, page_faults = 0;
      for(n = 0; n < ReferenceSZ; n++){ //cycle through page range
            a = 0, b = 0;
            for(m = 0; m < FrameNR; m++){
                  if(memory.PageFrameList[m] == accessPattern[n]) { //if page hit is found
                        a = 1; //clears all flags and continues from next index
                        b = 1;
                        break;
                  }
            }
            if(a == 0){ //if page fault occurs but there are unused page frames
                  for(m = 0; m < FrameNR; m++) {
                        if(memory.PageFrameList[m] == -1) {
                              memory.PageFrameList[m] = accessPattern[n]; //rewrite page frame value to current page index
                              b = 1; //clears additional statement and breaks from loop
                              page_faults++; //increment page fault count
                              break;
                        }
                  }
            }
            if(b == 0){ //if all page frames are used
                  for(m = 0; m < FrameNR; m++) { //iniitalize temp array values to check for least recently used value
                        temp[m] = 0;
                  }
                  for(k = n - 1, l = 1; l < FrameNR; l++, k--) { //cycles back from current page index - 1 to start, bounded by frame amount
                        for(m = 0; m < FrameNR; m++){ //cycles through current page frame list
                              if(memory.PageFrameList[m] == accessPattern[k]) { //if page frame value and current reference value match
                                    temp[m] = 1; //set off temp flag
                              }
                        }
                  }
                  for(m = 0; m < FrameNR; m++){ //cycle through temp array
                        if(temp[m] == 0) //if value has not been used
                        position = m; //set new overwrite value to temp's index
                  }
                  memory.PageFrameList[position] = accessPattern[n]; //overwrite page frame value with new value
                  page_faults++; //increment page fault count
            }
            
            for(m = 0; m < FrameNR; m++){ //print current page frames
                  printf("%d  ", memory.PageFrameList[m]);
            }
            printf("\n");
      }
      return page_faults; //return total page fault count
}

