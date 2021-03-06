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
void grow_stack (struct thread * t, struct intr_frame * f, void * fault_addr);
struct FTE * load_segment_on_demand (struct SP * fault_page, struct file * file);
bool install_page (void *upage, void *kpage, bool writable);
bool install_fault_page (struct SP * fp);

#endif /* userprog/process.h */
