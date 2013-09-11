#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

const int MAX = 13;

static void doFib(int n, int doPrint);

// global variables for computation of Fib sequences.
int LAST_FIB = 0; // also for the 0th Fib number
int CURT_FIB = 1; // also for the 1st Fib number

/*
 * unix_error - unix-style error routine.
 */
inline static void 
unix_error(char *msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}


int main(int argc, char **argv)
{
  int arg;
  int print;

  if(argc != 2){
    fprintf(stderr, "Usage: fib <num>\n");
    exit(-1);
  }

  if(argc >= 3){
    print = 1;
  }

  arg = atoi(argv[1]);
  if(arg < 0 || arg > MAX){
    fprintf(stderr, "number must be between 0 and %d\n", MAX);
    exit(-1);
  }

  // For the output in special case
  if (arg == 0) printf("%d\n", LAST_FIB);
  else if (arg == 1) printf("%d\n", CURT_FIB);
  else {
    // add the offset in Fib sequence
    doFib(arg - 1, 1);
  }
  return 0;
}

/* 
 * Recursively compute the specified number. If print is
 * true, print it. Otherwise, provide it to my parent process.
 *
 * NOTE: The solution must be recursive and it must fork
 * a new child for each call. Each process should call
 * doFib() exactly once.
 */
static void 
doFib(int n, int doPrint)
{
    int status;
    // create a new process
    int child = fork();

    // No need for the input check since the argument is verified
    // in the main() function.

    if (child == 0) {
        // for the child process
        if (n == 0) {
            // the current number is what we want.
            // end of the recursion.
            printf("%d\n", CURT_FIB);
            exit(1); // normal exit
        } else {
            // Computer the update of fib number
            int temp = CURT_FIB; 
            CURT_FIB += LAST_FIB; 
            LAST_FIB = temp;
            // Continue to compute the next Fib in the new process.
            doFib(n - 1, doPrint);
            waitpid(child, &status, 0);
        }
    } else {
        // wait for the termination of child process
        waitpid(child, &status, 0);
        exit(1);
    }

    return ;
}


