#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  printf ("system call!\n");
  thread_exit ();
}

void exit (int status) {
    struct thread * current = thread_current();
    // TODO: 
    // if this is a kernel thread, do not print the message, 
    // but if user process terminated, print the result
    printf("%s: exit(%d)\n", current->file_name, status);
    process_exit();
}
