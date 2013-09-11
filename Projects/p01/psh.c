/* 
 * psh - A prototype tiny shell program with job control
 * 
 * <Put your name and login ID here>
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "util.h"



/* Global variables */
int verbose = 0;            /* if true, print additional output */

extern char **environ;      /* defined in libc */
static char prompt[] = "psh> ";    /* command line prompt (DO NOT CHANGE) */
int MAX_NUM_ARGS = 10;
int MAX_NUM_ENVS = 10;
/* End global variables */


/* Function prototypes */

/* Here are the functions that you will implement */
void eval(char *cmdline);
void rmNewLine(char *str);
void rmWhiteSpaces(char *str);
int builtin_cmd(char **argv);

/* Here are helper routines that we've provided for you */
void usage(void);
void sigquit_handler(int sig);



/*
 * main - The shell's main routine 
 */
int main(int argc, char **argv) 
{
    char c;
    char cmdline[MAXLINE];
    int emit_prompt = 1; /* emit prompt (default) */

    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);

    /* Parse the command line */
    while ((c = getopt(argc, argv, "hvp")) != EOF) {
        switch (c) {
        case 'h':             /* print help message */
            usage();
	    break;
        case 'v':             /* emit additional diagnostic info */
            verbose = 1;
	    break;
        case 'p':             /* don't print a prompt */
            emit_prompt = 0;  /* handy for automatic testing */
	    break;
	default:
            usage();
	}
    }


    /* This one provides a clean way to kill the shell */
    Signal(SIGQUIT, sigquit_handler); 

    /* Execute the shell's read/eval loop */
    while (1) {

	/* Read command line */
	if (emit_prompt) {
	    printf("%s", prompt);
	    fflush(stdout);
	}
	if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
	    app_error("fgets error");
	if (feof(stdin)) { /* End of file (ctrl-d) */
	    fflush(stdout);
	    exit(0);
	}

	/* Evaluate the command line */
	eval(cmdline);
	fflush(stdout);
	fflush(stdout);
    } 

    exit(0); /* control never reaches here */
}
  
/*
 * eval - Evaluate the command line that the user has just typed in
 * 
 * If the user has requested a built-in command (quit)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return. 
*/
void eval(char *cmdline) 
{
    rmNewLine(cmdline);
    rmWhiteSpaces(cmdline);
    //printf("%s\n",cmdline);

    if (!builtin_cmd(&cmdline)) {
        // Initialiization of execution information
        char * filename;
        char * args[MAX_NUM_ARGS];
        char * envs[MAX_NUM_ENVS];
        
        char * delimiter = " "; // separator between command and arguments

        // read in the first argument: name of the file to be executed
        filename = strtok(cmdline, delimiter);

        // configure the arguments
        int i = 0;
        args[i++] = filename;
        char * pch;
        while ((pch = strtok(NULL, delimiter)) != NULL) {
            if (!(i < MAX_NUM_ARGS)) {
                fprintf(stderr, "Too many arguments..");
                break;
            }
            args[i] = pch;
            // printf("%s\n", pch);
            i ++;
        }
        // configure the environment where the execution relies on
        envs[0] = getenv("home");
        envs[1] = getenv("logname");
        envs[2] = NULL;
        // execution
        execve(filename, args, envs);
    }

    return;
}

/*
 * Remove the Newline (\n) caused by Enter Press 
 */
void rmNewLine(char * str) {
    int i; 
    for (i = 0; i < strlen(str); i ++) {
        if (*(str + i) == '\n') {
            *(str + i) = 0;
        }
    }
}

/* 
 * Remove the whitespaces in the front of command
 */
void rmWhiteSpaces(char * str) {
    // initialization
    int len = strlen(str);
    int iter, newiter;
    // ignore the front 'blank' characters
    for (iter = 0, newiter = 0; iter < len; iter++) {
        if (*(str + iter) == ' ') {
            continue;
        } else {
            break;
        }
    }
    // copy mediate characters
    for ( ; iter < len; iter ++, newiter ++) {
        *(str + newiter) = *(str + iter);
    }
    // set the rear characters to empty 
    for ( ; newiter < len; newiter ++) {
        *(str + newiter) = 0;
    }
    return ;
}


/* 
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately. 
 * Return 1 if a builtin command was executed; return 0
 * if the argument passed in is *not* a builtin command.
 */
int builtin_cmd(char **argv) 
{ 
    // quit processing
    if (strcmp(*argv, "quit") == 0) {
        exit(1); // user-specified quit 
    }

    return 0;     /* not a builtin command */
}



/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print a help message
 */
void usage(void) 
{
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig) 
{
    printf("Terminating after receipt of SIGQUIT signal\n");
    exit(1);
}



