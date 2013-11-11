#ifndef VM_FRAME_H
#define VM_FRAME_H

/* Library Inclusion */
#include "lib/kernel/hash.h"   /* for hash table */
#include "threads/palloc.h"    /* for memory allocation */
#include "threads/synch.h"     /* for lock mechanism */
#include "threads/thread.h"
#include "vm/page.h"

#ifndef PGSIZE
#define PGSIZE 4096
#endif

/* data structure for the frame table entry */
struct FTE {
    bool locked;        /* evictable bit */

    tid_t owner;

    void * paddr;      /* physical address */
    void * vaddr;      /* virtual address */

    struct SP * supplementary_page;  /* corresponding page */
    struct hash_elem FTE_helem;   /* identify the hash element */
};

/* the global lock used in the frame table */
struct lock frame_table_lock;

/* the global frame table modeled as a hash table */
struct hash frame_table;

/* initialize the frame table */
bool frame_table_init (void);

/* find the frame table entry */
struct FTE * frame_table_find (void *paddr);

/* add new entry to the frame table */
struct FTE * frame_table_put (void *paddr, void *vaddr, struct SP *page);

/* remove the specified frame table entry */
struct FTE * frame_table_remove (void * paddr);

/* get page from frame table */
void * fget_page (enum palloc_flags flags, void * vaddr);

/* get page from frame table and lock acquired entry hereafter */
void * fget_page_lock (enum palloc_flags flags, void * vaddr);

/* free page from frame table */
void ffree_page (void * vaddr);

/* evict one table */
struct FTE * fget_evict (void);

/* clean up all frames */
void fcleanup (void);

/* set page  */
void fset_page_lock (void);

#endif /* vm/frame.h */
