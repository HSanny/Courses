// ========================================================================
// UNCOMMENT THEMACROS TO ENABLE THE CORRESPONDING OUTPUT FOR TESTING
/*
 #define OPEN_TEST 
#define CREATE_TEST 
#define WRITE_TEST
#define CHDIR_TEST
#define READDIR_TEST
#define CLOSE_TEST
*/
// ========================================================================
#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <string.h>
#include <user/syscall.h>
#include "devices/input.h"
#include "devices/shutdown.h"
#include "threads/interrupt.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
// ========================================================================
#ifdef VM
#include "vm/frame.h"
#include "vm/page.h"
#endif
// ========================================================================
#ifdef FILESYS
#include "filesys/inode.h"
#include "filesys/file.h"
#include "filesys/directory.h"
#include "filesys/filesys.h"
#include "filesys/free-map.h"
#endif
// ========================================================================
#define ERROR -1
#define TEST 0
// ========================================================================
int open (const char * file);
bool create (const char *file, unsigned initial_size);
int read (int fd, void *buffer, unsigned size);
void close (int fd);
bool remove (const char * file);
tid_t exec (const char * cmdline);
int wait (tid_t pid);
void exit (int status);
// ========================================================================

// ORIGINALLY PROVIDED FUNCITON
static void syscall_handler (struct intr_frame *);

// NEW DATA STRUCTURE
struct process_file {
    struct file * file;
    struct dir * dir;
    bool isdir;
    int fd;
    struct list_elem elem;
};


// SYSTEM CALL IMPLEMENTATION
int open (const char * file);
bool create (const char *file, unsigned initial_size);
int read (int fd, void *buffer, unsigned size);
void close (int fd);
bool remove (const char * file);
tid_t exec (const char * cmdline);
int wait (tid_t pid);
void exit (int status);

// SYSTEM CALL FOR FILE SYSTEM
// ********************************************************************
bool chdir (const char * dir);
bool mkdir (const char * dir);
bool readdir (int fd, char * name);
bool isdir (int fd);
int inumber (int fd);
// AUXILIARY FUNCTIONS
int add_file (struct file *);
int add_dir (struct dir *d);
struct process_file * get_pf_by_fd (int fd);
// ********************************************************************

// AUXILIARY FUNCTIONS
struct file* get_file_by_fd (int fd);
int find_kernel_ptr(const void *vaddr);
void get_ptr(struct intr_frame *f, int *arg, int n);
void validate_ptr (const void *vaddr);
void check_buffer (void* buffer, unsigned size);
void close_file (int);
char * remove_multiple_slash (const char *); 

void
syscall_init (void) 
{
    intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

    static void
syscall_handler (struct intr_frame *f UNUSED) 
{
    int arg[MAX_ARGS];
    validate_ptr((const void*) f->esp);
    switch (* (int *) f->esp)
    {
        case SYS_HALT:
            {
                halt(); 
                break;
            }
        case SYS_EXIT:
            {   
                get_ptr(f, &arg[0], 1);
                exit (arg[0]);
                break;
            }
        case SYS_EXEC:
            {  
                get_ptr(f, &arg[0], 1);
                arg[0] = find_kernel_ptr((const void *) arg[0]);
                f->eax = exec((const char *) arg[0]); 
                break;
            }
        case SYS_WAIT:
            {
                get_ptr(f, &arg[0], 1);
                f->eax = wait(arg[0]);
                break;
            }
        case SYS_CREATE:
            {
                get_ptr(f, &arg[0], 2);
                f->eax = create((const char *)arg[0], (unsigned) arg[1]);
                break;
            }
        case SYS_REMOVE:
            {
                get_ptr(f, &arg[0], 1);
                f->eax = remove((const char *) arg[0]);
                break;
            }
        case SYS_OPEN:
            {
                get_ptr(f, &arg[0], 1);
                if (sp_table_find (thread_current()->spt, (char *)arg[0]) == NULL) exit (ERROR);
                f->eax = open((char *) arg[0]);
                break; 				
            }
        case SYS_FILESIZE:
            {
                get_ptr(f, &arg[0], 1);
                f->eax = filesize(arg[0]);
                break;
            }
        case SYS_READ:
            {
                get_ptr(f, &arg[0], 3);
                check_buffer((void *) arg[1], (unsigned) arg[2]);
                if (sp_table_find (thread_current()->spt, (char *)arg[1]) == NULL) exit (ERROR);
                f->eax = read(arg[0], (void *) arg[1],
                        (unsigned) arg[2]);
                break;
            }
        case SYS_WRITE:
            { 
                get_ptr(f, &arg[0], 3);
                check_buffer((void *) arg[1], (unsigned) arg[2]);
                f->eax = write(arg[0], (const void *) arg[1],
                        (unsigned) arg[2]);
                break;
            }
        case SYS_SEEK:
            {
                get_ptr(f, &arg[0], 2);
                seek((int)arg[0], (unsigned)arg[1]);
                break;
            } 
        case SYS_TELL:
            { 
                get_ptr(f, &arg[0], 1);
                f->eax = tell((int)arg[0]);
                break;
            }
        case SYS_CLOSE:
            {
                get_ptr(f, &arg[0], 1);
                close(arg[0]);
                break;
            }
        case SYS_MKDIR:
            { 
                get_ptr(f, &arg[0], 1);
                f->eax = mkdir((const char *) arg[0]);
                break;
            }
        case SYS_CHDIR:
            { 
                get_ptr(f, &arg[0], 1);
                f->eax = chdir((const char *)arg[0]);
                break;
            }
        case SYS_READDIR:
            { 
                get_ptr(f, &arg[0], 2);
                f->eax = readdir((const char *)arg[0], (char *) arg[1]);
                break;
            }
        case SYS_ISDIR:
            { 
                get_ptr(f, &arg[0], 1);
                f->eax = isdir((int) arg[0]);
                break;
            }
        case SYS_INUMBER:
            { 
                get_ptr(f, &arg[0], 1);
                f->eax = inumber((int) arg[0]);
                break;
            }
    }

}

/*
 * Halting System Call
 * */
void halt (void)
{
    shutdown_power_off();
}

/* 
 * Seek System Call
 * */
void seek (int fd, unsigned position)
{
    struct file *hold = get_file_by_fd(fd); 
    file_seek(hold, position);
}

/* 
 * Tell System Call
 * */
unsigned tell (int fd)
{
    struct file *hold = get_file_by_fd(fd); 
    return file_tell(hold);
}
/*
 * Exit System Call
 * */
void exit (int status)
{
    struct thread *cur = thread_current ();
    struct thread *parent = search_thread_by_tid (cur->parent);
    // signal the parent process
    if(parent != NULL) parent->exit_value = status;
    // print out the exit message
    printf ("%s: exit(%d)\n", cur->name, status);

    struct process_file *pf;
    struct list_elem *e; 

    // get the file_list occupied by current thread (process)
    struct list *file_list = &cur->file_list;
    // close all files recorded in the file_list
    if (file_list != NULL && !list_empty(file_list)) {
        for (e = list_begin(file_list); e != list_end(file_list); 
                e = list_next(e)) {
            pf = list_entry (e, struct process_file, elem);
            if (!pf->isdir) file_close(pf->file);
        }
    }

    if (file_list != NULL)
    {
        e = list_begin(file_list);
        while(!list_empty(file_list))
        {
            list_remove(e);
            e = list_next(e);
        }
        if (thread_current()->file_deny_execute != NULL)
        {
            // free the writing limitation
            file_allow_write(thread_current()->file_deny_execute);
            file_close(thread_current()->file_deny_execute);
        }
    }
    //-----------------------------------------------------
    // frame reclamation 
    fcleanup();
    //-----------------------------------------------------
    // invoke other operations about relevant deallocation
    thread_exit();
}

/* 
 * Get the file structure by using file descriptor
 * */
struct file* get_file_by_fd (int fd) 
{
    struct process_file *pf;
    struct list_elem *e; 

    // get the file_list occupied by current thread(process)
    struct list file_list = thread_current()->file_list;
    if (list_empty(&file_list) || fd < 0 || fd >= thread_current()->fd) {
        return NULL; 
    }
    // iterate through the whole list
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
 * Get the process_file structure by using file descriptor
 * */
struct process_file * get_pf_by_fd (int fd) 
{
    struct process_file *pf;
    struct list_elem *e; 

    // get the file_list occupied by current thread(process)
    struct list file_list = thread_current()->file_list;
    if (list_empty(&file_list) || fd < 0 || fd >= thread_current()->fd) {
        return NULL; 
    }
    // iterate through the whole list
    for (e = list_begin(&file_list); e != list_end(&file_list); 
            e = list_next(e)) {
        pf = list_entry (e, struct process_file, elem);
        if (TEST) printf("fd: %d \n", pf->fd);
        if (pf->fd == fd) return pf;
    } 
    // not found, return NULL
    return NULL;
}

/* 
 * Return the size of file, using fd.
 * INPUT: file descriptor 
 * OUTPUT: size of the file associated with the given descriptor
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
    // check the head and end of buffer is modifiable
    struct thread * cur = thread_current();
    if (!sp_table_find(cur->spt, buffer)->writable || 
            !sp_table_find(cur->spt, buffer+size)->writable ) {
        exit (ERROR);
    }

    unsigned read_byte = 0;
    // READ FROM THE EXTERNAL KEYBOARD
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
        // invalid file descriptor
        return ERROR;
    } 
    else {
        // get file structure
        struct process_file * pf = get_pf_by_fd (fd);

        // file not found
        if (pf == NULL || pf->isdir) {
            return ERROR; 
        }

        // get the size of requested file
        unsigned fsize = filesize (fd);
        if (TEST) printf("fsize: %d,  size: %d\n", fsize, size);
        if (TEST) printf("sbuffer: %d \n", sizeof buffer);
        // invalid if requested size is beyond the file size

        // read bytes from specified file
        lock_acquire(&filesys_lock);
        read_byte = file_read (pf->file, buffer, size);
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
    validate_ptr (buffer);
#ifdef WRITE_TEST
    printf ("*WRITE_SYSCALL* fd: %d, size: %d\n", fd, size);
#endif
    if (fd == STDOUT_FILENO)
    {
        putbuf(buffer, size);
        return size;
    }
    else if(fd == STDIN_FILENO){
        exit(ERROR);
    }
    else {
        // get the corresponding file structure
        struct process_file *pf = get_pf_by_fd (fd);
        // return error if required file is not found
        if (pf == NULL) return ERROR;
        // write file with mutual exclusion
        if (!pf->isdir && pf->file != NULL) {
            lock_acquire(&filesys_lock);
            int numOfbyte = file_write(pf->file, buffer, size);
            lock_release(&filesys_lock);
#ifdef WRITE_TEST
            printf ("*WRITE_SYSCALL* numOfbyte: %d\n", numOfbyte);
#endif
            return numOfbyte; 
        } else {
            // no write is allowed for directory
#ifdef WRITE_TEST
            printf ("*WRITE_SYSCALL* write to directory\n");
#endif
            return ERROR;
        }
    }
}

/*
 * Translate the virtual address to physical address which resides in main
 * memory
 * */
int find_kernel_ptr (const void *vaddr)
{
    validate_ptr (vaddr);
    void *ptr = pagedir_get_page (thread_current()->pagedir, vaddr);
    if (ptr == NULL)
        exit(ERROR);
    return (uint32_t) ptr;
}


/*
 * Acquire the arguments of currently involved system call from the interrupt
 * frame
 * */
void get_ptr (struct intr_frame *f, int *arg, int n)
{
    int i;
    int * ptr;
    int arg_num = 0;
    int * temp = (int *) f->esp;
    for (i = 0; i < n; i++)
    {
        ptr = temp + i + 1;
        validate_ptr ((const void *) ptr);
        arg_num++;
        if(arg_num > 3)break;
        arg[i] = *ptr;
    }
}


/* 
 * Check whether the given virtual address is valid
 * */
void validate_ptr (const void *vaddr)
{
    // check the validity of virtual address
    if (!is_user_vaddr(vaddr) || vaddr < USER_VADDR_BOTTOM || vaddr > PHYS_BASE)
    {
        if (TEST) printf("%s\n",thread_current()->name);
        exit(ERROR);
    }
    // check whether this virtual address is allocated
    struct thread * cur = thread_current();
    if (sp_table_find(cur->spt, vaddr) == NULL) exit(ERROR);
    // demand page if allocated but not loaded into physical memory
    check_valid_uaddr(vaddr, sizeof  (void *));
}


/* 
 * Check whether the given buffer is valid
 * */
void check_buffer (void* buffer, unsigned size)
{
    unsigned i;
    char* temp = (char *) buffer;
    // basic principle is to check the validity of all address in that buffer
    for (i = 0; i < size; i++)
    {
        validate_ptr ((const void*) temp);
        temp++;
    }
}

/* 
 * Create System Call
 * */
bool create (const char *file, unsigned initial_size)
{
    if (file == NULL) exit(ERROR);
    validate_ptr (file);
    if (strlen(file) == 0) return false;
#ifdef CREATE_TEST
    printf ("*CREATE_SYSCALL* filename: %s\n", file);
    printf ("*CREATE_SYSCALL* initial_size: %s\n", file);
#endif  

    // lock_acquire (&filesys_lock);
    bool success = filesys_create (file, initial_size);
    //lock_release (&filesys_lock);
    return success;
}


/* 
 * Open System Call
 * */
int open ( const char *file)
{
    //lock_acquire (&filesys_lock);
    if (file == NULL) exit (ERROR);
    validate_ptr (file);
    char * pro_file = remove_multiple_slash (file);

#ifdef OPEN_TEST
    printf ("*OPEN_SYSCALL* input_addr: %x\n", pro_file);
    printf ("*OPEN_SYSCALL* filename: %s\n", pro_file);
#endif

    struct thread * cur = thread_current();
    struct dir * cwd = cur->cwd;
    struct inode * inode;
    if (strcmp(pro_file, "/") == 0) {
#ifdef OPEN_TEST
        printf ("*OPEN_SYSCALL* open root\n");
#endif
        inode = dir_get_inode(dir_open_root());
    } else if (!filesys_lookup(pro_file, &inode)) {
#ifdef OPEN_TEST
        printf ("*OPEN_SYSCALL* file not found\n");
#endif
        return ERROR;
    }

#ifdef OPEN_TEST
    if (cur->cwd != NULL) printf ("*OPEN_SYSCALL* cur: %d\n",
            inode_get_inumber(dir_get_inode(cur->cwd)));
    if (inode != NULL) printf ("*OPEN_SYSCALL* inode: %d\n", 
            inode_get_inumber(inode));
#endif
    // for a directory
    if (inode_is_dir(inode)) {
        struct dir * new_dir = dir_open (inode);
        int fd = add_dir (new_dir);
        // lock_release(&filesys_lock);
#ifdef OPEN_TEST
        printf ("*OPEN_SYSCALL* open a directory\n");
#endif
        return fd;
    } else {
        // now for a simple file
        struct file *f = filesys_open(pro_file);
        if (f == NULL) {
            // release because of file-not-found error
#ifdef OPEN_TEST
            printf ("*OPEN_SYSCALL* open file failure\n");
#endif
        //    lock_release(&filesys_lock);
            return ERROR;
        }
        int fd = add_file(f);
        // release lock because of successful open
#ifdef OPEN_TEST
        printf ("*OPEN_SYSCALL* fd: %d\n", fd);
#endif
        //lock_release(&filesys_lock);
        return fd;
    }
}

/*
 * Process open a new file
 * */
int add_file (struct file *f)
{
    struct process_file *pf = (struct process_file *)malloc(sizeof(struct
                process_file)); 
    pf->isdir = false;
    pf->file = f;
    pf->fd = thread_current()->fd;
    thread_current()->fd++;
    list_push_back(&thread_current()->file_list, &pf->elem);
    return pf->fd;
}
/*
 * Process open a new directory 
 * */
int add_dir (struct dir *d)
{
    struct process_file *pf = (struct process_file *)malloc(sizeof(struct
                process_file)); 
    pf->isdir = true;
    pf->dir = d;
    pf->fd = thread_current()->fd;
    thread_current()->fd++;
    list_push_back(&thread_current()->file_list, &pf->elem);
    return pf->fd;
}
/*
 * Close System Call
 * */
void close (int fd)
{
#ifdef CLOSE_TEST
    printf ("*CLOSE_SYSCALL* fd: %d\n", fd);
#endif
    //lock_acquire(&filesys_lock);
    close_file(fd);
    //lock_release(&filesys_lock);
}

/*
 * Close the file occupied by a process
 * */
void close_file (int fd)
{
    struct thread *t = thread_current();
    struct list_elem *e;

#ifdef CLOSE_TEST
    printf ("list_size: %d\n", list_size(&t->file_list));
#endif
    for (e = list_begin(&t->file_list); e != list_end (&t->file_list); e = list_next(e))
    {
        struct process_file *pf = list_entry (e, struct process_file, elem);
        if (fd == pf->fd || fd == -1)
        {
            if (pf->isdir) dir_close (pf->dir);
            else file_close(pf->file);
            list_remove(&pf->elem);
            free(pf);
            break;
        }
    }
#ifdef CLOSE_TEST
    printf ("close file done\n");
#endif
}

/*
 * Remove System Call: remove a file from pintos' file system
 * */
bool remove (const char *file)
{
    validate_ptr (file);
    //lock_acquire(&filesys_lock);
    bool success = filesys_remove(file);
    //lock_release(&filesys_lock);
    return success;
}

/*
 * Exec System Call
 * */
tid_t exec (const char *cmdline) {
    // create new process to execute the given command line
    tid_t pid = process_execute(cmdline);
    // acquire the corresponding thread structure
    struct thread *cp = search_thread_by_tid(pid);
    if (cp == NULL) {
        return ERROR;
    }

    // yield to the newly created process, if not loaded yet 
    while (cp->isLoaded == NOT_LOADED){ 
        thread_yield();
    }

    // return pid until loading succeed
    if (cp->isLoaded == LOADED) {
        return pid;
    }
    return ERROR;
}

/* 
 * Wait for the termination of another process
 * */
int wait (tid_t pid)
{
    return process_wait(pid);
}

const void* check_valid_uaddr(const void * uaddr, int size) 
{
    if (uaddr == NULL) {
        return NULL;
    }

    struct hash* spt = thread_current()->spt;
    const void *usaddr = uaddr;  
    void *ueaddr = (void*)((char*)uaddr + size - 1); 

    uint32_t *pd = thread_current()->pagedir; 

    const void* cur;
    for (cur=usaddr; cur<ueaddr; cur+=4096) {
        struct SP* supp_page = sp_table_find(spt, cur);
        if (!is_user_vaddr(cur) || supp_page == NULL) return NULL;

        void* page = pagedir_get_page (pd, cur);
        if (page == NULL) supplementary_page_load(supp_page, false);
    }

    struct SP* supp_page = sp_table_find(spt, ueaddr);
    if (!is_user_vaddr(ueaddr) || supp_page == NULL) return NULL;

    void *keaddr = pagedir_get_page (pd, ueaddr);

    // one of these is out of the bounds 
    if (keaddr == NULL) {
        supplementary_page_load(supp_page, false);
    }

    return uaddr;
}

bool chdir (const char * dirname) 
{
    //lock_acquire (&filesys_lock);
    struct thread * cur = thread_current ();
    struct dir * old_dir = cur->cwd;

#ifdef CHDIR_TEST
    if (cur->cwd != NULL) printf ("chdir_pre: %d\n", inode_get_inumber(dir_get_inode(cur->cwd)));
#endif
    struct inode * inode;
    if (!filesys_lookup (dirname, &inode)) {
#ifdef CHDIR_TEST
        printf ("*CHDIR_CALL* %s failure\n", dirname);
#endif
       // lock_release (&filesys_lock);
        return false;
    }
    struct dir * new_dir = dir_open (inode);

    // update the cwd of current process
    cur->cwd = new_dir;
#ifdef CHDIR_TEST
    if (cur->cwd != NULL) printf ("chdir_post: %d\n", inode_get_inumber(dir_get_inode(cur->cwd)));
#endif
    //lock_release (&filesys_lock);
    return true;
};

bool mkdir (const char * dirname) 
{
    validate_ptr (dirname);
    // printf ("mkdir call: %s\n", dirname);
    //lock_acquire (&filesys_lock);
    bool success = filesys_mkdir (dirname);
    //lock_release (&filesys_lock);
    return success;
};

bool readdir (int fd, char * name)
{
   // lock_acquire (&filesys_lock);
    check_buffer(name, READDIR_MAX_LEN + 1);
#ifdef READDIR_TEST
    printf ("*READDIR_SYSCALL* fd: %d \n", fd);
#endif
    // get the file structure using file decriptor
    struct process_file * pf = get_pf_by_fd (fd);
    if (pf == NULL) {
        //lock_release (&filesys_lock);
        return ERROR;
    }
    if (pf->isdir && pf->dir != NULL) {
        bool success = dir_readdir(pf->dir, name);
        //lock_release (&filesys_lock);
#ifdef READDIR_TEST
        if (success) printf ("*READDIR_SYSCALL* succ\n");
        else printf ("*READDIR_SYSCALL* fail\n");
#endif
        return success;
    } else {
        //lock_release (&filesys_lock);
        return false;
    }
};

bool isdir (int fd)
{
    //lock_acquire (&filesys_lock);
    // get the file structure using file decriptor
    struct process_file * pf = get_pf_by_fd (fd);
    // no pf found
    if (pf == NULL) {
        //lock_release (&filesys_lock);
        return ERROR;
    }
    // return isdir from the data structure
    //lock_release (&filesys_lock);
    return pf->isdir;
};

int inumber (int fd) 
{
    //lock_acquire (&filesys_lock);
    // get the file structure using file decriptor
    struct process_file * pf = get_pf_by_fd (fd);
    // no pf found
    if (pf == NULL) {
        //lock_release (&filesys_lock);
        return ERROR;
    }
    // return inumber from the data structure
    if (pf->isdir) {
        if (pf->dir != NULL) {
            block_sector_t inumber = inode_get_inumber (dir_get_inode(pf->dir));
           // lock_release (&filesys_lock);
            return inumber;
        }
    } else {
        if (pf->file != NULL) {
            block_sector_t inumber = inode_get_inumber (file_get_inode(pf->file));
            //lock_release (&filesys_lock);
            return inumber;
        }
    }
};

/* address the multiple slash in a user-specified pathname */
char * remove_multiple_slash (const char * name)
{
    int i, j;
    int length = strlen(name);
    char * new_str = (char *) malloc (length+1);
    for (i = 0, j = 0; i < length; i ++) {
        if (*(name+i) == '/') {
            if (*(name+i+1) == '/') continue; // multiple-slash
            else {
                *(new_str+j) = *(name+i); // single-slash
                ++j;
            }
        } else { // other char
            *(new_str+j) = *(name+i);
            ++j;
        } 
    }
    *(new_str+j) = 0;
    // printf ("or_ms: %s, rm_ms: %s\n", name, new_str);
    return new_str;
} 
