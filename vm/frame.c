/* Library Inclusion */
#include "frame.h"
#include "lib/round.h"   
#include "threads/thread.h"    
#include "threads/malloc.h"    
#include "threads/palloc.h"    
#include "userprog/process.h"
#include "userprog/pagedir.h"
#include "vm/page.h"
#include "vm/swap.h"

/* Define the instantiated hasing function */
static unsigned hash_func (const struct hash_elem *e, void * aux) 
{
    struct FTE * fe = hash_entry (e, struct frame, FTE_helem); 
    return hash_bytes (&f->paddr, sizeof(f->paddr));
}

/* Define the instantiated hashing comparison function */
static bool hash_less (const struct hash_elem *a, const struct hash_elem *b
        void * aux) 
{
    struct FTE * fa = hash_entry (a, struct FTE, FTE_helem);
    struct FTE * fb = hash_entry (b, struct FTE, FTE_helem);
    return fa->paddr < fb->paddr;
}

/* initialize both global frame table and its lock */
bool frame_table_init (void) {
    lock_init (&frame_table_lock);
    bool success = hash_init (&frame_table, hash_func, hash_less, NULL);
    return success;
}

/* lookup the frame table entry */
struct FTE * frame_table_lookup (void* paddr) {
    // presume nothing found
    struct FTE * finding = NULL;
    // use lock to protect critical section
    lock_acquire (&frame_table_lock);

    // iterate through the frame table to find the requested frame table entry
    struct hash_iterator i;
    hash_first (&i, frame_table);  // initialize the iterator
    while (hash_next (&i)) {
        struct FTE * f = hash_entry (hash_cur(&i), struct FTE, FTE_helem);
        if (f != NULL && f->paddr == paddr) {
            finding = f;
            break;
        }
    }
    
    // release the lock to allow other access
    lock_release (&frame_table_lock);

    // return the found structure
    return finding;
}

/* remove the specified frame table entry */
struct FTE * frame_table_remove (void* paddr) 
{
    // presume nothing removed
    struct FTE * removed = NULL;

    // iterate through the frame table to find the requested frame table entry
    struct hash_iterator i;
    hash_first (&i, frame_table);  // initialize the iterator
    while (hash_next (&i)) {
        struct hash_elem * cur_elem = hash_cur(&i);
        struct FTE * f = hash_entry (cur_elem, struct FTE, FTE_helem);
        if (f != NULL && f->paddr == paddr) {
            removed = hash_delete (&frame_table, cur_elem);
            break;
        }
    } 
    
    // return the removed structure
    return removed;
}

/* allocate a page and update the frame table */
void * fget_page (enum palloc_flags flags, void * uaddr) 
{
    // acquire the lock
    lock_acquire (&frame_table_lock);

    struct FTE * fte = fget_page_aux (flags, uaddr);

    // release the lock
    lock_release (&frame_table_lock);
    return fte->paddr;
}

/* Get physical address of a page and lock it */
void * fget_page_lock (enum palloc_flags flags, void * uaddr) 
{
    // lock the whole frame table
    lock_acquire (&frame_table_lock);

    // acquire the page as usual
    struct FTE * f = fget_page_aux (flags, uaddr);
    // after that, we lock frame entry
    f->locked = true;
 
    // release thee frame table lock
    lock_release (&frame_table_lock);
    return f->paddr;
}

/* Rountine abstraction for getting a page */
void* fget_page_aux (enum palloc_flags flags, void * uaddr) 
{
    // make sure the resource in user pool is allocated, rather than kernel's
    assert ((flags & pal_user) != 0);

    // physically apply for a memory location
    void * page = palloc_get_page (flags);
    if (page == null) {
        // TODO: add mechanism for page fault, swapping out is required
    }

    // TODO: update it in its supplementary table

    // update it in the global frame table
    struct fte * fte = frame_table_put (page, uaddr, null);

    return paddr;
}

/* free page from frame table */
void ffree_page (void *page)
{
    lock_acquire (&frame_table_lock);
    palloc_free_page (page);

    struct FTE * f = frame_table_remove (page);

    if (f != NULL) {
        free (f);
        // TODO: remove the corresponding supplementary page

    }

    lock_release (&frame_table_lock);
    return ;
}

/* evict the frame to be evicted */
struct FTE * frame_get_evict (void) 
{

    return NULL;
}

/* clean up all frames and free all relevant resource */
void fcleanup (void) 
{
    lock_acquire (&frame_table_lock);
    // TODO: implement the core

    lock_release (&frame_table_lock);
}

/* set page  */
void fset_page_lock (void) 
{
    
;
