#include "lib/round.h"
#include "lib/kernel/hash.h"
#include "vm/page.h"
#include "vm/frame.h"
#include "vm/swap.h"
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

/* Define the destructor for the supplemental page table */
void sp_hash_destruct (struct hash_elem * elem, void * aux UNUSED)
{
    struct SP * sp = hash_entry (elem, struct SP, SP_helem);
    free (sp); 
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
    lock_acquire (&thread_current()->spt_lock);
    // printf ("vaddr: %x\n", vaddr);
    void * page = (void *) ROUND_DOWN ((uint64_t) vaddr, (uint64_t) PGSIZE);
    // printf ("page: %x\n", page);
    if (hash_empty(page_table)) return NULL;   

    struct SP temp;
    temp.vaddr = page;

    struct hash_elem * helem = hash_find (page_table, &temp.SP_helem);
    if (helem != NULL) {
        finding = hash_entry (helem, struct SP, SP_helem);
       // printf ("find_addr: %x\n", finding->vaddr);
    }

    lock_release (&thread_current()->spt_lock);
    return finding;
}

struct SP * sp_table_remove (struct hash * page_table, void * vaddr)
{
    lock_acquire (&thread_current()->spt_lock);
    struct SP * removed = NULL;
    if (hash_empty(page_table)) return NULL;   

    // define the target
    struct SP temp;
    temp.vaddr = vaddr;

    // remove the element in sp table
    struct hash_elem * helem = hash_delete (page_table, &temp.SP_helem);
    lock_release (&thread_current()->spt_lock);

    if (helem != NULL) {
        removed = hash_entry (helem, struct SP, SP_helem);
    }
    return removed;
}

struct FTE* supplementary_page_load (struct SP* fault_page, bool locked)
{
    struct FTE* frame = NULL;
    /*if (fault_page->mmentry != NULL) 
    {
        frame = lazy_load_segment(fault_page, fault_page->mmentry->backup_file);
    } 
    else*/ 
    if(fault_page->executable && !fault_page->modified)
    {
        frame = load_segment_on_demand(fault_page, fault_page->owner->file_deny_execute);
        frame->locked = locked;
    } 
    else if (fault_page->evicted)
    {
        if(fault_page->executable && !fault_page->modified) PANIC("EVICT EXEC\n");
        frame = swap_pool_read(fault_page);
        frame->locked = locked;
    }
    return frame;
}
