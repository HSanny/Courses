/* Library Inclusion */
#include "vm/frame.h"

#include <debug.h>
#include "lib/round.h"   
#include "threads/thread.h"    
#include "threads/malloc.h"    
#include "threads/palloc.h"    
#include "userprog/process.h"
#include "userprog/pagedir.h"
#include "userprog/exception.h"

#include "vm/page.h"
#include "vm/swap.h"
#include "lib/random.h"
#include "lib/kernel/hash.h"


struct FTE * fget_page_aux (enum palloc_flags flags, void * vaddr);

/* Define the instantiated hasing function */
static unsigned fte_hash_func (const struct hash_elem *e, void * aux UNUSED) 
{
    struct FTE * f = hash_entry (e, struct FTE, FTE_helem); 
    return hash_bytes (&f->paddr, sizeof(f->paddr));
}

/* Define the instantiated hashing comparison function */
static bool fte_hash_less (const struct hash_elem *a, const struct hash_elem *b,
        void * aux UNUSED) 
{
    struct FTE * fa = hash_entry (a, struct FTE, FTE_helem);
    struct FTE * fb = hash_entry (b, struct FTE, FTE_helem);
    return fa->paddr < fb->paddr;
}

/* initialize both global frame table and its lock */
bool frame_table_init (void) 
{
//    printf("init\n");
    lock_init (&frame_table_lock);
    bool success = hash_init (&frame_table, fte_hash_func, fte_hash_less,
            NULL);
    return success;
}

/* find the frame table entry */
struct FTE * frame_table_find (void* paddr) 
{
//    printf("find\n");
    // presume nothing found
    struct FTE * finding = NULL;

    // define the target
    struct FTE temp;
    temp.paddr = (void *) ROUND_DOWN ( (uint64_t) paddr, (uint64_t) PGSIZE);

    // use lock to protect critical section
    lock_acquire (&frame_table_lock);

    // search the hash table entry
    struct hash_elem * felem = hash_find (&frame_table, &(temp.FTE_helem));
    finding = hash_entry (felem, struct FTE, FTE_helem);
    
    // release the lock to allow other access
    lock_release (&frame_table_lock);

    // return the found structure
    return finding;
}

/* add one new entry to the frame table */
struct FTE * frame_table_put (void *paddr, void *vaddr, struct SP * page)
{
//    printf("put\n");
    // create a new_fte frame table entry structure
    struct FTE * new_fte = (struct FTE *) malloc (sizeof (struct FTE));
    if (new_fte == NULL) return NULL;
    new_fte->owner = thread_current ()->tid;
    new_fte->paddr = paddr;
    new_fte->vaddr = vaddr;
    new_fte->supplementary_page = page;
    new_fte->locked = false;

    // insert that new_fte entry to the global frame table
    struct hash_elem *helem = hash_insert (&frame_table, &new_fte->FTE_helem);

    // return the new_ftely created structure if succeed, null return expected
    if (helem == NULL) return new_fte;
    else return NULL;  // skip if pre-exist
}

/* remove the specified frame table entry */
struct FTE * frame_table_remove (void* paddr) 
{
    // presume nothing removed
    struct FTE * removed = NULL;

    // define the target stucture
    struct FTE temp;
    temp.paddr = (void *) ROUND_DOWN ((int) paddr, PGSIZE);

    // delete the targeted hash table entry
    struct hash_elem * delem = hash_delete (&frame_table, &temp.FTE_helem);

    // return the removed structure
    if (delem != NULL) {
        removed = hash_entry (delem, struct FTE, FTE_helem);
        return removed;
    }
    else return NULL;
}

/* allocate a frame and update the frame table */
void * fget_page (enum palloc_flags flags, void * vaddr) 
{
    // acquire the lock
    lock_acquire (&frame_table_lock);

    // acquire the correponding physical address
    struct FTE * fte = fget_page_aux (flags, vaddr);
    fte->locked = false;

    // release the lock
    lock_release (&frame_table_lock);
    // printf("paddr: %x\n", fte->paddr);

    return fte->paddr;
}

/* Get physical address of a page and lock it */
void * fget_page_lock (enum palloc_flags flags, void * vaddr) 
{
    // lock the whole frame table
    lock_acquire (&frame_table_lock);

    // acquire the page as usual
    struct FTE * fte = fget_page_aux (flags, vaddr);
    // after that, we lock frame entry
    fte->locked = true;
 
    // release thee frame table lock
    lock_release (&frame_table_lock);

    return fte->paddr;
}

/* Rountine abstraction for getting a page */
struct FTE * fget_page_aux (enum palloc_flags flags, void * vaddr) 
{
    // make sure the resource in user pool is allocated, rather than kernel's
    ASSERT ((flags & PAL_USER) != 0);

    // physically apply for a memory location
    void * paddr = palloc_get_page (flags);
    if (paddr == NULL) {
        // physical memory full: swapping out is required

        struct FTE * kicked_out = fget_evict ();
        paddr = swap_out (kicked_out);
        // printf ("kick paddr: %x, vaddr: %x, evicted: %d \n"
        //      , kicked_out->paddr, kicked_out->vaddr, kicked_out->supplementary_page->evicted);
        if (!kicked_out->supplementary_page->executable || 
                kicked_out->supplementary_page->modified) {
            kicked_out->supplementary_page->evicted = true;
        } 
        struct FTE * temp = frame_table_remove(kicked_out->paddr);
        free (temp);
    }

    // get the page table owned by this process
    struct hash * spt = thread_current()->spt;
    // add new supplementary page into that page table
    struct SP * sp = sp_table_put (spt, vaddr);

    // update it in the global frame table
    struct FTE * fte = frame_table_put (paddr, vaddr, sp);

    // printf("finished aux\n");

    return fte;
}

/* deallocate one frame */
void ffree_page (void *paddr)
{
    ASSERT ((int) paddr % PGSIZE == 0);

    struct thread * cur = thread_current();

    lock_acquire (&frame_table_lock);

    // remove the entry in the global frame table
    struct FTE * fte = frame_table_remove (paddr);
    if (fte != NULL) {
        // remove the entry in the supplementary page table
        lock_acquire (&cur->spt_lock);
        struct SP * page = sp_table_remove (cur->spt, fte->vaddr);
        lock_release (&cur->spt_lock);
        // free those memory allocation of entries in two hash tables
        if (page != NULL) free (page);
        free (fte);
    }
    // free the phsical memory
    palloc_free_page (paddr);

    lock_release (&frame_table_lock);

    return ;
}

/* clock algorithm: evict the frame to be evicted */
struct FTE * fget_evict (void) 
{
    // infinite loop until find a valid evicted frame
    while (true)  {
        // go through the whole frame table
        struct hash_iterator i;
        hash_first(&i, &frame_table);
        while (hash_next(&i)) {
            struct FTE* fte = hash_entry(hash_cur(&i), struct FTE, FTE_helem);
            // check the reference bit in the built-in page table
            if (pagedir_is_accessed(fte->supplementary_page->pagedir, fte->vaddr))
                // sweep the reference bit
                pagedir_set_accessed(fte->supplementary_page->pagedir, fte->vaddr, false);
            else if(!fte->locked) { // locked frame is not evictable
                // since frame f is to be evicted (important operation) 
                // we should lock it at the moment
                fte->locked = true;
                return fte;
            }
        }
    }
    // useless return
    return NULL;
}

/* clean up all frames and free all relevant resource */
void fcleanup (void) 
{
    struct thread * cur = thread_current();
    lock_acquire (&frame_table_lock);

   // struct hash_elem * delete_record [100];
    struct FTE *delete_record[hash_size(&frame_table)];
    int s;
    for (s = 0; s < hash_size(&frame_table); s ++) {
        delete_record[s] = NULL;
    }

    // iterate through all frame table entry
    struct hash_iterator i;
    hash_first (&i, &frame_table);
    s = 0;
    while (hash_next (&i)) {
      struct FTE * fte = hash_entry (hash_cur (&i), struct FTE, FTE_helem);
        if (fte->owner == cur->tid) {
            delete_record[s] = fte;
            s ++;
        }
    }
    int j;
    // remove recorded hash element
    for (j = 0; j < s; j ++) {
        if (delete_record[j] == NULL) break;
        else {
         //   hash_delete(&frame_table, delete_record[s]);
         frame_table_remove(delete_record[j]->paddr);
        }
    }

    lock_release (&frame_table_lock);
}


