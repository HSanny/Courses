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
#define TEST 0

// ORIGINALLY PROVIDED FUNCITON
static void syscall_handler (struct intr_frame *);

// GLOBAL VARIABLE
struct lock filesys_lock;
// NEW DATA STRUCTURE
struct process_file {
    struct file *file;
    int fd;
    struct list_elem elem;
};

// AUXILIARY FUNCTIONS
int process_add_file (struct file *);
struct file* get_file_by_fd (int fd);
int user_to_kernel_ptr(const void *vaddr);
void get_arg (struct intr_frame *f, int *arg, int n);
void check_valid_ptr (const void *vaddr);
void check_valid_buffer (void* buffer, unsigned size);
void process_close_file (int);
// SYSTEM CALL IMPLEMENTATION
int open (const char *);
bool create (const char *file, unsigned initial_size);
int read (int fd, void *buffer, unsigned size);
void close(int);
bool remove (const char *);
tid_t exec(const char *);
int wait (tid_t pid);

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
            get_arg(f, &arg[0], 1);
	    arg[0] = user_to_kernel_ptr((const void *) arg[0]);
	    f->eax = exec((const char *) arg[0]); 
	    break;
            }
         case SYS_WAIT:
            {
	    get_arg(f, &arg[0], 1);
	    f->eax = wait(arg[0]);
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
                 get_arg(f, &arg[0], 1);
	         arg[0] = user_to_kernel_ptr((const void *) arg[0]);
	         f->eax = remove((const char *) arg[0]);
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
                get_arg(f, &arg[0], 1);
                f->eax = filesize(arg[0]);
                break;
            }
        case SYS_READ:
            {
                get_arg(f, &arg[0], 3);
                check_valid_buffer((void *) arg[1], (unsigned) arg[2]);
                arg[1] = user_to_kernel_ptr((void *) arg[1]);
                f->eax = read(arg[0], (void *) arg[1],
                        (unsigned) arg[2]);
                break;
            }
        case SYS_WRITE:
            { 
                get_arg(f, &arg[0], 3);
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
              get_arg(f, &arg[0], 1);
	      close(arg[0]);
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
    struct thread *parent = search_thread_by_tid(cur->parent);
    if(parent != NULL) parent->exit_value = status;
    printf ("%s: exit(%d)\n", cur->name, status);
    thread_exit();
}

struct file* get_file_by_fd (int fd) 
{
    struct process_file *pf;
    struct list_elem *e; 

    // get the file_list occupied by current thread(process)
    struct list file_list = thread_current()->file_list;
    if (list_empty(&file_list) || fd < 0 || fd >= thread_current()->fd) {
        return NULL; 
    }
    for (e = list_begin(&file_list); e != list_end(&file_list); 
            e = list_next(e)) {
        pf = list_entry (e, struct process_file, elem);
        if (TEST) printf("fd: %d \n", pf->fd);
        if (pf->fd == fd) return pf->file;
    } 
    // not found, return NULL
    return NULL;
}


/* 
 * Return the size of file, opened as fd.
 * */
int filesize (int fd) 
{
    int fsize = 0;
    // acquire the lock to avoid race condition
    lock_acquire(&filesys_lock);
    // search the file structure having the file decriptor
    struct file * f = get_file_by_fd (fd);
    // compute the length of file
    fsize = file_length (f);
    // free the lock
    lock_release(&filesys_lock);

    if (TEST) printf("filsize: %d\n", fsize);
    // return the size of file
    return fsize;
}

/*
 * Read system call
 * */
int read (int fd, void *buffer, unsigned size) 
{
    // READ FROM THE EXTERNAL KEYBOARD
    unsigned read_byte = 0;
    if (fd == STDIN_FILENO) {
        uint8_t key;
        char * tb = (char *) buffer;
        // read byte-by-byte
        while (read_byte < size && (key=input_getc()) != '\0') {
            *tb = (char) key;
            tb ++;
            read_byte ++;
        }
    } 
    else if (fd == STDOUT_FILENO) {
        return -1;
    } 
    else {
        // get file structure
        struct file * f = get_file_by_fd (fd);

        // file not found
        if (f == NULL) return -1; 
#include "userprog/process.h"
        // get the size of requested file
        unsigned fsize = filesize (fd);
        if (TEST) printf("fsize: %d,  size: %d\n", fsize, size);
        if (TEST) printf("sbuffer: %d \n", sizeof buffer);
        // invalid if requested size is beyond the file size
        if (size > fsize)  return -1; 

        // read bytes from specified file
        lock_acquire(&filesys_lock);
        read_byte = file_read (f, buffer, size);
        lock_release(&filesys_lock);
    }
    if (TEST) printf("read_byte : %d\n", read_byte);
    return read_byte;
}


/*
 * Write system call
 * */
int write (int fd, const void *buffer, unsigned size)
{
    if (fd == STDOUT_FILENO)
    {
        putbuf(buffer, size);
        return size;
    }
    else if(fd == 0){
       exit(-1);
    }
    else{
    
    struct file *f = get_file_by_fd(fd);
    if (!f)
     {
      return ERROR;
     }
     lock_acquire(&filesys_lock);
      int numOfbyte = file_write(f, buffer, size);
     lock_release(&filesys_lock);
     return numOfbyte; 
     }
}


int user_to_kernel_ptr (const void *vaddr)
{
    check_valid_ptr (vaddr);
    void *ptr = pagedir_get_page (thread_current()->pagedir, vaddr);
    if (!ptr)
    {
        exit(ERROR);
    }
    return (int) ptr;
}


void get_arg (struct intr_frame *f, int *arg, int n)
{
    int i;
    int * ptr;
    for (i = 0; i < n; i++)
    {
        ptr = (int *) f->esp + i + 1;
        check_valid_ptr ((const void *) ptr);
        arg[i] = *ptr;
    }

}

void check_valid_ptr (const void *vaddr)
{
    if (!is_user_vaddr(vaddr) || vaddr < USER_VADDR_BOTTOM)
    {
        if(TEST) printf("%s\n",thread_current()->name);
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

void close (int fd)
{
  lock_acquire(&filesys_lock);
  process_close_file(fd);
  lock_release(&filesys_lock);
}

void process_close_file (int fd)
{
  struct thread *t = thread_current();
  struct list_elem *next, *e = list_begin(&t->file_list);

  while (e != list_end (&t->file_list))
    {
      next = list_next(e);
      struct process_file *pf = list_entry (e, struct process_file, elem);
      if (fd == pf->fd || fd == -1)
	{
	  file_close(pf->file);
	  list_remove(&pf->elem);
	  free(pf);
	  if (fd != -1)
	    {
	      return;
	    }
	}
      e = next;
    }
}

bool remove (const char *file)
{
  lock_acquire(&filesys_lock);
  bool success = filesys_remove(file);
  lock_release(&filesys_lock);
  return success;
}

tid_t exec(const char *cmdline){
  tid_t pid = process_execute(cmdline);
  struct thread *cp = search_thread_by_tid(pid);
  if(!cp)return ERROR;
  while(cp->isLoaded == NOT_LOADED) thread_yield();
  if(cp->isLoaded == LOADED) return pid;
  else return ERROR;
}

int wait (tid_t pid)
{
  return process_wait(pid);
}

