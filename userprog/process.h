#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"
#ifdef VM
#include "vm/page.h"
#endif

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

struct frame * pg_load_segment (struct file *file, struct SP *fault_page);

#endif /* userprog/process.h */
