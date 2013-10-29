/* Library Inclusion */
#include "lib/kernel/hash.h"   /* for hash table */
#include "threads/palloc.h"    /* for memory allocation */
#include "threads/synch.h"     /* for lock mechanism */

/* data structure for the frame table entry */
struct FTE {
    int isResident;    /* valid bit */
    int isModified;    /* dirty bit */
    int locked;        /* evictable bit */

    void * paddr;      /* physical address */
    void * uaddr;      /* virtual address */

    struct hash_elem FTE_helem; 
};

/* the lock used in the frame table */
struct lock frame_table_lock;

/* the frame table modeled as a hash table */
struct hash frame_table;

/* initialize the frame table */
bool frame_table_init (void);

/* lookup the frame table entry */
struct FTE * frame_table_lookup (void* paddr);

/* remove the specified frame table entry */
struct FTE * frame_table_remove (void* paddr);

/* get page from frame table */
void * fget_page (void * uaddr);
void * fget_locked_page (void * uaddr);

/* free page from frame table */
void ffree_page (void uaddr);

/* evict one table */
struct FTE * frame_get_evict (void);

/* clean up all frames */
void fcleanup (void);

/* set page  */
void fset_page_lock (void);
