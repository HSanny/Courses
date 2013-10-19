#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <user/syscall.h>
#include "devices/input.h"
#include "devices/shutdown.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "threads/interrupt.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#define ERROR -1
#define MAX_ARGS 3
#define USER_VADDR_BOTTOM ((void *) 0x08048000)


struct lock filesys_lock;

struct process_file {
    struct file *file;
    int fd;
    struct list_elem elem;
};

int process_add_file (struct file *);
int open(const char *);

static void syscall_handler (struct intr_frame *);
int user_to_kernel_ptr(const void *vaddr);
void get_arg (struct intr_frame *f, int *arg, int n);
void check_valid_ptr (const void *vaddr);
void check_valid_buffer (void* buffer, unsigned size);


bool create (const char *file, unsigned initial_size);

    void
syscall_init (void) 
{
    lock_init(&filesys_lock);
    intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

    static void
syscall_handler (struct intr_frame *f UNUSED) 
{
    int arg[MAX_ARGS];
    check_valid_ptr((const void*) f->esp);
    switch (* (int *) f->esp)
    {
        case SYS_HALT:
            {
                halt(); 
                break;
            }
        case SYS_EXIT:
            {
                get_arg (f, &arg[0], 1);
                exit (arg[0]);
                break;
            }
        case SYS_EXEC:
            {

                break;
            }
        case SYS_WAIT:
            {

                break;
            }
        case SYS_CREATE:
            {
               get_arg(f, &arg[0], 2);
	       arg[0] = user_to_kernel_ptr((const void *) arg[0]);
	       f->eax = create((const char *)arg[0], (unsigned) arg[1]);
                break;
            }
        case SYS_REMOVE:
            {

                break;
            }
        case SYS_OPEN:
            {
               get_arg(f, &arg[0], 1);
	       arg[0] = user_to_kernel_ptr((const void *) arg[0]);
	       f->eax = open((const char *) arg[0]);
	break; 				
            }
        case SYS_FILESIZE:
            {

                break;
            }
        case SYS_READ:
            {

                break;
            }
        case SYS_WRITE:
            { 
                get_arg(f, &arg[0],3);
                check_valid_buffer((void *) arg[1], (unsigned) arg[2]);
                arg[1] = user_to_kernel_ptr((const void *) arg[1]);
                f->eax = write(arg[0], (const void *) arg[1],
                        (unsigned) arg[2]);
                break;
            }
        case SYS_SEEK:
            {

                break;
            } 
        case SYS_TELL:
            { 

                break;
            }
        case SYS_CLOSE:
            { 

                break;
            }
    }
}

void halt (void)
{
    shutdown_power_off();
}

void exit (int status)
{
    struct thread *cur = thread_current();
    printf ("%s: exit(%d)\n", cur->name, status);
    thread_exit();
}


int write (int fd, const void *buffer, unsigned size)
{
    if (fd == STDOUT_FILENO)
    {
        putbuf(buffer, size);
        return size;
    }
    // need to handle write file problem
    return -1;
}


int user_to_kernel_ptr(const void *vaddr)
{
    check_valid_ptr(vaddr);
    void *ptr = pagedir_get_page(thread_current()->pagedir, vaddr);
    if (!ptr)
    {
        exit(ERROR);
    }
    return (int) ptr;
}


void get_arg (struct intr_frame *f, int *arg, int n)
{
    int i;
    int *ptr;
    for (i = 0; i < n; i++)
    {
        ptr = (int *) f->esp + i + 1;
        check_valid_ptr((const void *) ptr);
        arg[i] = *ptr;
    }
}

void check_valid_ptr (const void *vaddr)
{
    if (!is_user_vaddr(vaddr) || vaddr < USER_VADDR_BOTTOM)
    {
        exit(ERROR);
    }
}

void check_valid_buffer (void* buffer, unsigned size)
{
    unsigned i;
    char* local_buffer = (char *) buffer;
    for (i = 0; i < size; i++)
    {
        check_valid_ptr((const void*) local_buffer);
        local_buffer++;
    }
}

bool create (const char *file, unsigned initial_size)
{
  lock_acquire(&filesys_lock);
  bool success = filesys_create (file, initial_size);
  lock_release(&filesys_lock);
  return success;
}

int open (const char *file)
{
  lock_acquire(&filesys_lock);
  struct file *f = filesys_open(file);
  if (!f)
    {
      lock_release(&filesys_lock);
      return ERROR;
    }
  int fd = process_add_file(f);
  lock_release(&filesys_lock);
  return fd;
}

int process_add_file (struct file *f)
{
  struct process_file *pf=(struct process_file *)malloc(sizeof(struct process_file));
  pf->file = f;
  pf->fd = thread_current()->fd;
  thread_current()->fd++;
  list_push_back(&thread_current()->file_list, &pf->elem);
  return pf->fd;
}
