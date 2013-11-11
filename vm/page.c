#include "vm/page.h"
#include "vm/frame.h"
#include "threads/malloc.h"
#include "threads/thread.h"

/* Define hash function */
unsigned sp_hash (const struct hash_elem * elem, void * aux UNUSED) 
{
    struct SP * page = hash_entry (elem, struct SP, SP_helem);
    return hash_bytes (&(page->vaddr), sizeof (page->vaddr));
}

/* Define the comparison function */
bool sp_hash_less (const struct hash_elem *a, const struct hash_elem *b,
        void * aux UNUSED) 
{
    struct SP * spa = hash_entry (a, struct SP, SP_helem);
    struct SP * spb = hash_entry (b, struct SP, SP_helem);
    return spa->vaddr < spb->vaddr;
}

bool sp_table_init (struct hash * page_table) 
{
    bool success;
    success = hash_init (page_table, sp_hash, sp_hash_less, NULL);
    return success;
}

struct SP * sp_table_put (struct hash * page_table, void * vaddr)
{
    // must secure given addr is multiple of PGSIZE
    // perhaps round down first
    ASSERT ((int) vaddr % PGSIZE == 0);

    struct thread * cur = thread_current ();
    lock_acquire (&cur->spt_lock);

    struct SP * new = (struct SP*) malloc (sizeof (struct SP));
    // assignment basic identification
    new->owner = cur;
    new->vaddr = vaddr;
    new->pagedir = cur->pagedir;

    // assignment for control bits
    new->executable = false;
    new->writable = true;
    new->evicted = false;
    new->modified = false;

    // insert into the page table 
    struct hash_elem * helem = hash_insert (page_table, &new->SP_helem);
    
    lock_release (&cur->spt_lock);

    if (helem != NULL) { 
        // equal element exist, thus return that pre-existing element
        return hash_entry (helem, struct SP, SP_helem);
    } else {
        return new;  // sucessful insertion
    }
}

struct SP * sp_table_find (struct hash * page_table, void * vaddr)
{
    struct SP * finding = NULL;
    if (hash_empty(page_table)) return NULL;   

    struct SP temp;
    temp.vaddr = vaddr;

    struct hash_elem * helem = hash_find (page_table, &temp.SP_helem);
    finding = hash_entry (helem, struct SP, SP_helem);

    return finding;
}

struct SP * sp_table_remove (struct hash * page_table, void * vaddr)
{
    struct SP * removed = NULL;
    if (hash_empty(page_table)) return NULL;   

    // define the target
    struct SP temp;
    temp.vaddr = vaddr;

    // remove the element in sp table
    struct hash_elem * helem = hash_delete (page_table, &temp.SP_helem);

    if (helem != NULL) {
        removed = hash_entry (helem, struct SP, SP_helem);
        return removed;
    }
    else return NULL;
}
