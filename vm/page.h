#ifndef VM_PAGE_H
#define VM_PAGE_H

/* Library Inclusion */
#include <debug.h>
#include "lib/kernel/hash.h"
#include "filesys/filesys.h"
#include "threads/thread.h"

/* Defining data structure representing page in VAS */
struct SP {
    /* Basic identification */
    struct thread * owner;  
    struct hash_elem SP_helem;  /* element for hashing */
    void * vaddr;   /* virtual address as identification */

    /* Loading Information */
    uint32_t * pagedir;  /* page directory */
    uint8_t * ppage;     /* physical address */
    off_t offset;        /* offset of a file */
    int page_read_bytes; /* # of bytes to read */

    /* Control Bits */
    bool evicted;   /* residency in main memory */
    bool modified;  /* dirty bit */
    bool executable; /* code segment page or not */
    bool writable;  /* modify the bits that we could produce */
};

unsigned sp_hash (const struct hash_elem * elem, void * aux UNUSED);

bool sp_hash_less (const struct hash_elem * a, const struct hash_elem * b,
       void * aux UNUSED);

void sp_hash_destruct (struct hash_elem * elem, void * aux UNUSED);

bool sp_table_init (struct hash * spt);

struct SP * sp_table_put (struct hash * page_table, void * vaddr);

struct SP * sp_table_find (struct hash * page_table, void * vaddr);

struct SP * sp_table_remove (struct hash * page_table, void * vaddr);

#endif /* vm/page.h */
