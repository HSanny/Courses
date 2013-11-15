// libraries
#include "userprog/process.h"
#include <debug.h>
#include <inttypes.h>
#include <round.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "userprog/gdt.h"
#include "userprog/pagedir.h"
#include "userprog/tss.h"
#include "filesys/directory.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "threads/flags.h"
#include "threads/init.h"
#include "threads/interrupt.h"
#include "threads/palloc.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "lib/kernel/hash.h"
#ifdef VM
#include "vm/frame.h"
#include "vm/page.h"
#endif

// macros
#define ERROR -1
#define THREAD_MAGIC 0xcd6abf4b
#define MAX_STACK_PAGES 2048

// global variable
static bool TEST = 0;

// signature
static thread_func start_process NO_RETURN;
static bool load (const char *cmdline, void (**eip) (void), void **esp);

// ------------------------------------------------------------------------
// Project 4: new functions
/* install the faulted page into page directory */
bool install_fault_page (struct SP * fp) 
{
    return (pagedir_get_page (fp->pagedir, fp->vaddr) == NULL
            && pagedir_set_page (fp->pagedir, fp->vaddr, fp->ppage, fp->writable));
}

/* load segment for swapping in the page on demand */
struct FTE * load_segment_on_demand (struct SP * fault_page, struct file * file) 
{
    // allocate a new frame entry
    fault_page->ppage = fget_page_lock (PAL_USER, fault_page->vaddr);
    // install that faulted page
    install_fault_page (fault_page);

    /* Load this page. */
    // set to previous file position
    file_seek (file, fault_page->offset);
    // execute further read operation
    int page_read_bytes = fault_page->page_read_bytes;
    int page_zero_bytes = PGSIZE - page_read_bytes;
    if (page_read_bytes > 0)
        file_read (file, fault_page->ppage, page_read_bytes);
    // set zero to remaining byte
    memset (fault_page->ppage + page_read_bytes, 0, page_zero_bytes);

    struct FTE * loaded_frame = frame_table_find (fault_page->ppage);
    return loaded_frame;
}

void grow_stack (struct thread * t, struct intr_frame * f, void * fault_addr) 
{
    // compute the bottom address of the existing stack segment
    void * stack_bottom = (void *) (PHYS_BASE - t->num_stack_pages *PGSIZE);   
    // round up to PGSIZE (4096)
    int addr_dist = ROUND_UP ((uint32_t) stack_bottom - (uint32_t)fault_addr, PGSIZE);
    // compute the number of page need to grow
    int num_new_page = addr_dist / PGSIZE;
    // check for page overflow
    if (t->num_stack_pages + num_new_page > MAX_STACK_PAGES) {
        PANIC ("Stack size too big, beyond 8MB. \n");
    }
    // allocate new page one by one
    int i = 0;
    for (;i < num_new_page; i++) {
        void * new_upage = (void*) ((int) stack_bottom - ((i+1) *PGSIZE));
        void * new_ppage = fget_page (PAL_USER, new_upage);
        install_page (new_upage, new_ppage, true);
        t->num_stack_pages ++;
    }
}
// ---------------------------------------------------------------------------

/* Starts a new thread running a user program loaded from
   FILENAME.  The new thread may be scheduled (and may even exit)
   before process_execute() returns.  Returns the new process's
   thread id, or TID_ERROR if the thread cannot be created. */
    tid_t
process_execute (const char *file_name) 
{
    char *fn_copy;
    tid_t tid;

    /* Make a copy of FILE_NAME.
       Otherwise there's a race between the caller and load(). */
    fn_copy = palloc_get_page (0);
    if (fn_copy == NULL)
        return TID_ERROR;
    strlcpy (fn_copy, file_name, PGSIZE);

    // ************************************************************
    // initialize the first argument
    char * first_arg;
    first_arg = palloc_get_page (0);
    if (first_arg == NULL)
        return TID_ERROR;
    strlcpy (first_arg, file_name, PGSIZE);
    // fn_copy is actually treated as an array containing arguments
    char * delimiters = " \n";  // delimiter
    char * save_ptr;  // allocate memory for that
    // string separation for file_name
    first_arg = strtok_r (first_arg, delimiters, &save_ptr);  

    // ************************************************************

    /* Create a new thread to execute FILE_NAME. */
    tid = thread_create (first_arg, PRI_DEFAULT, start_process, fn_copy);
    if (tid == TID_ERROR) {
        palloc_free_page (fn_copy);  
        palloc_free_page (first_arg);  
    }

    return tid;
}

/* A thread function that loads a user process and starts it
   running. */
    static void
start_process (void *file_name_)
{
    char * file_name = file_name_;
    struct intr_frame if_;
    bool success;

    // ***************************************************
    // Local Variable declaration
    char * delimiters = " \n";  // delimiter
    char * save_ptr;  // allocate memory for that

    // arguments storage
    int MAX_ARGS = 100;  // upper bound of arguments is 10
    char * args [MAX_ARGS];  
    int i = 0; // indexing for the array args
    for (; i < MAX_ARGS; i ++ ) { args[i] = NULL; }  // initialization
    i = 0; // reset the iterator

    // derive the program name
    char * fname = strtok_r (file_name, delimiters, &save_ptr);
    args[i++] = fname;  // set it as the first argument
    int num_args = 0;
    int len_all_args = 0;  // the total length of all arguments, \0 included
    // ***************************************************

    /* Initialize interrupt frame and load executable. */
    memset (&if_, 0, sizeof if_);
    if_.gs = if_.fs = if_.es = if_.ds = if_.ss = SEL_UDSEG;
    if_.cs = SEL_UCSEG;
    if_.eflags = FLAG_IF | FLAG_MBS;

    success = load (fname, &if_.eip, &if_.esp);

    if (success) {
        // printf ("load succeed !\n");
        struct thread * cur = thread_current();
        // change the loading status
        cur->interrupt = &if_;
        cur->isLoaded = LOADED;
        struct file *holder = filesys_open (fname);
        cur->file_deny_execute = holder;
        file_deny_write(holder);
        if (cur->parent != cur->tid)
        {
            cur->depth = search_thread_by_tid(cur->parent)->depth +1;
            if (cur->depth > 31)
                thread_exit();
        }
        /* successful page allocation */
        // establish a pointer array to restore arguments
        char * argtok;
        while ((argtok = strtok_r (NULL, delimiters, &save_ptr)) != NULL) {
            if (i < MAX_ARGS) {
                args[i++] = argtok;
                if (TEST) {
                    printf("argtok: %s \n", argtok);
                }
            }
        }
        // get the number of arguments
        for (i = 0; i < MAX_ARGS; i ++) {
            if (args[i] != NULL) num_args++;
        }
        // copy the data into the stack
        char * stack_addr [MAX_ARGS];
        for (i = 0; i < MAX_ARGS; i ++ ) { stack_addr[i] = NULL; }  // initialization

        if (TEST) {
            printf("eip: %X  esp: %X  \n", (unsigned int) if_.eip, (unsigned int) if_.esp);
        }

        for (i = MAX_ARGS - 1; i >= 0; i --) {
            if (args[i] != NULL) {
                int len_arg = strlen(args[i]) + 1;  // includes null char
                if_.esp -= len_arg;  // move the esp, the position seeker
                memcpy (if_.esp, args[i], len_arg);  // copy content
                len_all_args += len_arg;  // accumulate length of args for padding
                stack_addr[i] = if_.esp;  // record the address of arg in stack
                if (TEST) {
                    printf("addr: %X, Name: args[%d][...], Data: %s, Type: char[%d]\n",
                            (unsigned int) if_.esp, i, (char *) if_.esp, len_arg);
                }
            }
        }
        // apply word alignment
        int alignment = 4 - (len_all_args % 4);
        if_.esp -= alignment;  

        if (TEST) {
            printf("eip: %X  esp: %X  \n", (unsigned int) if_.eip, 
                    (unsigned int) if_.esp); 
        } 
        // get the size of a char pointer
        int LEN_POINTER = sizeof (char *);
        // push null pointer as argv array terminator
        if_.esp -= LEN_POINTER;
        int null_p = 0;
        memcpy (if_.esp, &null_p, LEN_POINTER);  
        if (TEST) {
            printf("Null Pointer: addr %X \n", (unsigned int) if_.esp);
        }
        // push the address of string to stack
        char * addr_first_arg;
        for (i = MAX_ARGS - 1; i >= 0; i --) {
            if (args[i] != NULL) {
                if_.esp -= LEN_POINTER;  // move esp pointer
                memcpy (if_.esp, &stack_addr[i], LEN_POINTER);  // copy arg address
                if (i == 0) { // record the stack address of the first argument
                    addr_first_arg = if_.esp;
                }
                if (TEST) {
                    printf("addr: %X, Name: args[%d], Data: %x, Type: char *\n",
                            (unsigned int) if_.esp, i, (unsigned int) stack_addr[i]);
                }
            }
        }

        // push the char ** argv
        if_.esp -= LEN_POINTER;
        memcpy (if_.esp, &addr_first_arg, LEN_POINTER);
        if (TEST) {
            printf("addr_argv: %X  data_argv: %X\n", 
                    (unsigned int) if_.esp, (unsigned int)addr_first_arg);
            printf("After argv pushed: eip: %X  esp: %X\n", 
                    (unsigned int) if_.eip, (unsigned int) if_.esp);
        }

        // push the number of arguments
        if_.esp -= sizeof (int);
        memcpy (if_.esp, &num_args, sizeof(int));
        if (TEST) {
            printf("Number of argument: %d\n", num_args);
            printf("After argc pushed: eip: %X  esp: %X  \n",
                    (unsigned int)if_.eip, (unsigned int) if_.esp);
        }

        // reserve the place for the return address
        if_.esp -= LEN_POINTER;
        int return_addr = 0;
        memcpy (if_.esp, &return_addr, LEN_POINTER);
        if (TEST) {
            printf("After return address pushed: eip: %X  esp: %X  \n",
                    (unsigned int)if_.eip, (unsigned int) if_.esp); 
        } 

        //FIXME: buggy - wait-killed test case
        cur->stack_track = if_.esp;
        
        // dump for manual checking of the established stack
        if (TEST)
            hex_dump ((unsigned int)if_.esp, if_.esp, PHYS_BASE-(unsigned int)if_.esp, 1);

        // Synchornization: yield back to parent process by default
        sema_down(&cur->sema);
        // printf ("set argument done ..\n");
    }
    // ***************************************************

    /* If load failed, quit. */
    palloc_free_page (file_name);
    if (!success) {
        thread_current()->isLoaded = LOADING_FAIL;
        thread_yield();
        thread_exit ();
    }
    /* Start the user process by simulating a return from an
       interrupt, implemented by intr_exit (in
       threads/intr-stubs.S).  Because intr_exit takes all of its
       arguments on the stack in the form of a `struct intr_frame',
       we just point the stack pointer (%esp) to our stack frame
       and jump to it. */
    // printf ("start exec\n");
    asm volatile ("movl %0, %%esp; jmp intr_exit" : : "g" (&if_) : "memory");
    NOT_REACHED ();
}


/* Waits for thread TID to die and returns its exit status.  If
   it was terminated by the kernel (i.e. killed due to an
   exception), returns -1.  If TID is invalid or if it was not a
   child of the calling process, or if process_wait() has already
   been successfully called for the given TID, returns -1
   immediately, without waiting.

   This function will be implemented in problem 2-2.  For now, it
   does nothing. */
    int
process_wait (tid_t child_tid) 
{
    // Jimmy's driving
    struct thread * cur = thread_current();
    struct thread * t = search_thread_by_tid (child_tid);
    int exit_status;
    if (t == NULL  // no found, given tid is invalid
            || t->parent != cur->tid)  // not child of calling process
    { 
        // perhaps the child process exit already
        if (cur->exit_value != NOT_EXIT) {
            exit_status = cur->exit_value;
            cur->exit_value = NOT_EXIT;
            return exit_status; 
        }
        // no child process has exited before
        return ERROR;
    }
    // If the child process is waiting for execution, let it go!
    struct semaphore *sema = &t->sema;
    if (sema != NULL)
        sema_up (sema);
    // busy waiting for the termination of child process
    while (t != NULL && t->status != THREAD_DYING && t->magic == THREAD_MAGIC) { 
        thread_yield(); 
    }
    // process the exit value of given child process
    if (cur->exit_value != NOT_EXIT) exit_status = cur->exit_value;
    else exit_status = ERROR;
    cur->exit_value = NOT_EXIT;
    return exit_status;
}

/* Free the current process's resources. */
    void
process_exit (void)
{
    struct thread *cur = thread_current ();
    uint32_t *pd;

    /* Destroy the current process's page directory and switch back
       to the kernel-only page directory. */
    pd = cur->pagedir;
    palloc_free_page(cur->file_name);
    if (pd != NULL) 
    {
        /* Correct ordering here is crucial.  We must set
           cur->pagedir to NULL before switching page directories,
           so that a timer interrupt can't switch back to the
           process page directory.  We must activate the base page
           directory before destroying the process's page
           directory, or our active page directory will be one
           that's been freed (and cleared). */
        cur->pagedir = NULL;
        pagedir_activate (NULL);
        pagedir_destroy (pd);
    }
    // clean up the supplemental page table and all the entries
    hash_clear (cur->spt, sp_hash_destruct);
}

/* Sets up the CPU for running user code in the current
   thread.
   This function is called on every context switch. */
    void
process_activate (void)
{
    struct thread *t = thread_current ();

    /* Activate thread's page tables. */
    pagedir_activate (t->pagedir);

    /* Set thread's kernel stack for use in processing
       interrupts. */
    tss_update ();
}

/* We load ELF binaries.  The following definitions are taken
   from the ELF specification, [ELF1], more-or-less verbatim.  */

/* ELF types.  See [ELF1] 1-2. */
typedef uint32_t Elf32_Word, Elf32_Addr, Elf32_Off;
typedef uint16_t Elf32_Half;

/* For use with ELF types in printf(). */
#define PE32Wx PRIx32   /* Print Elf32_Word in hexadecimal. */
#define PE32Ax PRIx32   /* Print Elf32_Addr in hexadecimal. */
#define PE32Ox PRIx32   /* Print Elf32_Off in hexadecimal. */
#define PE32Hx PRIx16   /* Print Elf32_Half in hexadecimal. */

/* Executable header.  See [ELF1] 1-4 to 1-8.
   This appears at the very beginning of an ELF binary. */
struct Elf32_Ehdr
{
    unsigned char e_ident[16];
    Elf32_Half    e_type;
    Elf32_Half    e_machine;
    Elf32_Word    e_version;
    Elf32_Addr    e_entry;
    Elf32_Off     e_phoff;
    Elf32_Off     e_shoff;
    Elf32_Word    e_flags;
    Elf32_Half    e_ehsize;
    Elf32_Half    e_phentsize;
    Elf32_Half    e_phnum;
    Elf32_Half    e_shentsize;
    Elf32_Half    e_shnum;
    Elf32_Half    e_shstrndx;
};

/* Program header.  See [ELF1] 2-2 to 2-4.
   There are e_phnum of these, starting at file offset e_phoff
   (see [ELF1] 1-6). */
struct Elf32_Phdr
{
    Elf32_Word p_type;
    Elf32_Off  p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
};

/* Values for p_type.  See [ELF1] 2-3. */
#define PT_NULL    0            /* Ignore. */
#define PT_LOAD    1            /* Loadable segment. */
#define PT_DYNAMIC 2            /* Dynamic linking info. */
#define PT_INTERP  3            /* Name of dynamic loader. */
#define PT_NOTE    4            /* Auxiliary info. */
#define PT_SHLIB   5            /* Reserved. */
#define PT_PHDR    6            /* Program header table. */
#define PT_STACK   0x6474e551   /* Stack segment. */

/* Flags for p_flags.  See [ELF3] 2-3 and 2-4. */
#define PF_X 1          /* Executable. */
#define PF_W 2          /* Writable. */
#define PF_R 4          /* Readable. */

static bool setup_stack (void **esp);
static bool validate_segment (const struct Elf32_Phdr *, struct file *);
static bool load_segment (struct file *file, off_t ofs, uint8_t *upage,
        uint32_t read_bytes, uint32_t zero_bytes,
        bool writable);

/* Loads an ELF executable from FILE_NAME into the current thread.
   Stores the executable's entry point into *EIP
   and its initial stack pointer into *ESP.
   Returns true if successful, false otherwise. */
    bool
load (const char *file_name, void (**eip) (void), void **esp) 
{
    // printf("loading starts..\n");
    struct thread *t = thread_current ();
    struct Elf32_Ehdr ehdr;
    struct file *file = NULL;
    off_t file_ofs;
    bool success = false;
    int i;

    thread_current()->file_deny_execute = NULL;
    /* Allocate and activate page directory. */
    t->pagedir = pagedir_create ();
    if (t->pagedir == NULL) 
        goto done;
    process_activate ();

    /* Open executable file. */
    file = filesys_open (file_name);
    //printf("test:%s\n", file_name);
    if (file == NULL) 
    {
        printf ("load: %s: open failed\n", file_name);
        thread_current()->isLoaded = LOADING_FAIL;
        goto done; 
    }

    /* Read and verify executable header. */
    if (file_read (file, &ehdr, sizeof ehdr) != sizeof ehdr
            || memcmp (ehdr.e_ident, "\177ELF\1\1\1", 7)
            || ehdr.e_type != 2
            || ehdr.e_machine != 3
            || ehdr.e_version != 1
            || ehdr.e_phentsize != sizeof (struct Elf32_Phdr)
            || ehdr.e_phnum > 1024) 

    {
        thread_current()->isLoaded = LOADING_FAIL;
        printf ("load: %s: error loading executable\n", file_name);
        goto done; 
    }

    /* Read program headers. */
    file_ofs = ehdr.e_phoff;
    for (i = 0; i < ehdr.e_phnum; i++) 
    {
        struct Elf32_Phdr phdr;

        if (file_ofs < 0 || file_ofs > file_length (file))
            goto done;
        file_seek (file, file_ofs);

        if (file_read (file, &phdr, sizeof phdr) != sizeof phdr)
            goto done;
        file_ofs += sizeof phdr;
        switch (phdr.p_type) 
        {
            case PT_NULL:
            case PT_NOTE:
            case PT_PHDR:
            case PT_STACK:
            default:
                /* Ignore this segment. */
                break;
            case PT_DYNAMIC:
            case PT_INTERP:
            case PT_SHLIB:
                goto done;
            case PT_LOAD:
                if (validate_segment (&phdr, file)) 
                {
                    bool writable = (phdr.p_flags & PF_W) != 0;
                    uint32_t file_page = phdr.p_offset & ~PGMASK;
                    uint32_t mem_page = phdr.p_vaddr & ~PGMASK;
                    uint32_t page_offset = phdr.p_vaddr & PGMASK;
                    uint32_t read_bytes, zero_bytes;
                    if (phdr.p_filesz > 0)
                    {
                        /* Normal segment.
                           Read initial part from disk and zero the rest. */
                        read_bytes = page_offset + phdr.p_filesz;
                        zero_bytes = (ROUND_UP (page_offset + phdr.p_memsz, PGSIZE)
                                - read_bytes);
                    }
                    else 
                    {
                        /* Entirely zero.
                           Don't read anything from disk. */
                        read_bytes = 0;
                        zero_bytes = ROUND_UP (page_offset + phdr.p_memsz, PGSIZE);
                    }
                    if (!load_segment (file, file_page, (void *) mem_page,
                                read_bytes, zero_bytes, writable))
                        goto done;
                }
                else
                    goto done;
                break;
        }
    }

    /* Set up stack. */
    if (!setup_stack (esp))
    {
        goto done;
    }

    /* Start address. */
    *eip = (void (*) (void)) ehdr.e_entry;

    success = true;

done:
    /* We arrive here whether the load is successful or not. */
    file_close (file);
    return success;
}

/* load() helpers. */


/* Checks whether PHDR describes a valid, loadable segment in
   FILE and returns true if so, false otherwise. */
    static bool
validate_segment (const struct Elf32_Phdr *phdr, struct file *file) 
{
    /* p_offset and p_vaddr must have the same page offset. */
    if ((phdr->p_offset & PGMASK) != (phdr->p_vaddr & PGMASK)) 
        return false; 

    /* p_offset must point within FILE. */
    if (phdr->p_offset > (Elf32_Off) file_length (file)) 
        return false;

    /* p_memsz must be at least as big as p_filesz. */
    if (phdr->p_memsz < phdr->p_filesz) 
        return false; 

    /* The segment must not be empty. */
    if (phdr->p_memsz == 0)
        return false;

    /* The virtual memory region must both start and end within the
       user address space range. */
    if (!is_user_vaddr ((void *) phdr->p_vaddr))
        return false;
    if (!is_user_vaddr ((void *) (phdr->p_vaddr + phdr->p_memsz)))
        return false;

    /* The region cannot "wrap around" across the kernel virtual
       address space. */
    if (phdr->p_vaddr + phdr->p_memsz < phdr->p_vaddr)
        return false;

    /* Disallow mapping page 0.
       Not only is it a bad idea to map page 0, but if we allowed
       it then user code that passed a null pointer to system calls
       could quite likely panic the kernel by way of null pointer
       assertions in memcpy(), etc. */
    if (phdr->p_vaddr < PGSIZE)
        return false;

    /* It's okay. */
    return true;
}

/* Loads a segment starting at offset OFS in FILE at address
   UPAGE.  In total, READ_BYTES + ZERO_BYTES bytes of virtual
   memory are initialized, as follows:

   - READ_BYTES bytes at UPAGE must be read from FILE
   starting at offset OFS.

   - ZERO_BYTES bytes at UPAGE + READ_BYTES must be zeroed.

   The pages initialized by this function must be writable by the
   user process if WRITABLE is true, read-only otherwise.

   Return true if successful, false if a memory allocation error
   or disk read error occurs. */
    static bool
load_segment (struct file *file, off_t ofs, uint8_t *upage,
        uint32_t read_bytes, uint32_t zero_bytes, bool writable) 
{
    ASSERT ((read_bytes + zero_bytes) % PGSIZE == 0);
    ASSERT (pg_ofs (upage) == 0);
    ASSERT (ofs % PGSIZE == 0);

    file_seek (file, ofs);
    while (read_bytes > 0 || zero_bytes > 0) 
    {
        /* Calculate how to fill this page.
           We will read PAGE_READ_BYTES bytes from FILE
           and zero the final PAGE_ZERO_BYTES bytes. */
        size_t page_read_bytes = read_bytes < PGSIZE ? read_bytes : PGSIZE;
        size_t page_zero_bytes = PGSIZE - page_read_bytes;

        // -----------------------------------------------------------------
        // Our implementation for demand paging
        // create supplemental page entry first
        struct thread * cur = thread_current ();
        struct hash * spt = cur->spt;
        struct SP * page = sp_table_put (spt, upage);
        page->executable = true;
        page->writable = writable;
        page->page_read_bytes = page_read_bytes;
        page->ppage = 0;
        page->offset = file_tell (file);
        file_seek (file, file_tell(file) + page_read_bytes);
       //  printf ("%x %d read:%d\n", upage, page->offset, page_read_bytes);
        // -----------------------------------------------------------------
        /* Advance. */
        read_bytes -= page_read_bytes;
        zero_bytes -= page_zero_bytes;
        upage += PGSIZE;
    }
    return true;
}


/* Create a minimal stack by mapping a zeroed page at the top of
   user virtual memory. */
    static bool
setup_stack (void **esp) 
{
    uint8_t *kpage;
    bool success = false;

    uint8_t * vaddr = ((uint8_t *) PHYS_BASE) - PGSIZE;
#ifndef VM
    // routine for user program project
    kpage = palloc_get_page (PAL_USER | PAL_ZERO);
#else 
    // routine for virtual memory project
    kpage = fget_page (PAL_USER | PAL_ZERO, (void *) vaddr);
#endif

    if (kpage != NULL) 
    {
        success = install_page (vaddr, kpage, true);
        if (success) {
            *esp = PHYS_BASE;
            // ---------------------------------------------------
            // notify the sp data structure saying that it is writable
            struct hash * spt = thread_current()->spt;
            struct SP * sp = sp_table_find (spt, vaddr);
            sp->writable = true;
            // ---------------------------------------------------
        }
        else {
#ifndef VM
            palloc_free_page (kpage);
#else
            ffree_page (kpage);
#endif
        }
    }
    return success;
}

/* Adds a mapping from user virtual address UPAGE to kernel
   virtual address KPAGE to the page table.
   If WRITABLE is true, the user process may modify the page;
   otherwise, it is read-only.
   UPAGE must not already be mapped.
   KPAGE should probably be a page obtained from the user pool
   with palloc_get_page().
   Returns true on success, false if UPAGE is already mapped or
   if memory allocation fails. */
    bool
install_page (void *upage, void *kpage, bool writable)
{
    struct thread *t = thread_current ();

    /* Verify that there's not already a page at that virtual
       address, then map our page there. */
    return (pagedir_get_page (t->pagedir, upage) == NULL
            && pagedir_set_page (t->pagedir, upage, kpage, writable));
}
