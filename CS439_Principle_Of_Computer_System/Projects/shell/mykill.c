#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>


int main(int argc, char **argv)
{
    // argument verification
    if (argc != 2) {
        fprintf(stderr, "Error input.\n");
        exit(-1);
    }

    // processing for the raw input
    int pid = atoi(*(argv + 1));
    if (! (pid > 0)) {
        fprintf(stderr, "Process Number Must be Non-negative.\n");
    }

    // type cast
    pid_t objpid = (pid_t) pid;

    // send the signal to other process by using kill function
    kill(objpid, SIGUSR1);

    return 0;
}
