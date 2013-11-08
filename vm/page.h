#ifndef VM_PAGE_H
#define VM_PAGE_H

/* Library Inclusion */
#include <debug.h>
#include "lib/kernel/hash.h"
#include "filesys/filesys.h"
#include "threads/thread.h"

/* Defining data structure representing page in VAS */
struct SP {
    void * vaddr;   /* virtual address as identification */
    tid_t process; 

    bool evicted;   /* residency in main memory */
    bool writable;  /* modify the bits that we could produce */

    struct hash_elem SP_helem;  
};

unsigned sp_hash (const struct hash_elem * elem, void * aux UNUSED);

bool sp_hash_less (const struct hash_elem * a, const struct hash_elem * b,
       void * aux UNUSED);

bool sp_table_init (struct hash * spt);

struct SP * sp_table_put (struct hash * page_table, void * vaddr);

struct SP * sp_table_find (struct hash * page_table, void * vaddr);

struct SP * sp_table_remove (struct hash * page_table, void * vaddr);

#endif /* vm/page.h */
