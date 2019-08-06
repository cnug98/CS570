#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
	
    pid_t pid1, pid2; //pids for each child process
    int status1, status2;
    char *alph = "./testalphabet"; //path for each executable
    char *spec = "./testspecial";
    const char *empty = "5";

    if (!(pid1 == fork())) { //first fork, this produces the child that runs alphabetcount
        if(!(pid2 == fork())) { //second fork, this produces the child that runs specialcharcount
            printf("CHILD <PID: %d> is executing testspecial!\n", getpid()); //prints execution start + pid
            execl(spec, "", NULL); //runs testspecial
        }
        printf("CHILD <PID: %d> is executing testalphabet!\n", getpid()); //prints execution start + pid
        execl(alph, "", NULL); //runs testalphabet, execl allowed me to run without additional parameters
    }

    else if((pid1 > 0) && (pid2 > 0)){ //parent case, this was never fully finished as I couldn't figure out how to print after each child terminates
        wait(&status1); //both waits statements work as intended, with both finishing at expected runtimes
        printf("CHILD <PID: %d> is finished with testspecial!", getppid()); //both print statements never reach the console, but I decided to put them in to show my thought process
        wait(&status2);
        printf("CHILD <PID: %d> is finished with testalphabet!", getppid() - 1);
        
    }
	return 0;
	
}
        /*
        */
/*
 This program will fork two child processes running the two programs generated in programming zero in parallel.
 Hint: You mant need to use fork(), exec() family, wait(), exit(), getpid() and etc ...
 
 Requirements:
 
 1) Exactly two child processes are created, one to run testspecial program and the other is to run testalphabet program;
 2) When a program starts to run, print a message to the output screen showing which process (with PID) is running which program, for example:
    "CHILD <16741> process is executing testalphabet program!"
    
 3) When a program is done, the process exits and at the same time, a message should be print to the output screen showing which  process (with PID) is done with the program, for example:
    "CHILD <16741> process has done with testalphabet program !"
    
 4) The messages should match the real execution orders, i.e. when the testspecial program starts/ends, the right message should be print out. So you need to figure out how to get the starting/ending time of each 
 process.
   
   
 The expected screen print out should be similar as follows:
 
 CHILD <16741> process is executing testalphabet program!
 CHILD <16742> process is executing testspecial program!
, -> 745668
. -> 798072
... ...

CHILD <16742> process has done with testspecial program !
a -> 2973036
b -> 556908
... ...

CHILD <16741> process has done with testalphabet program !
 */

