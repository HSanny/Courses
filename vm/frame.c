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

    // make sure the resource in user pool is allocated, rather than kernel's
    ASSERT ((flags & PAL_USER) != 0);

    void * page = palloc_get_page (flags);
    if (page == NULL) {
        // TODO: add mechanism for page fault, swapping out is required
    }

    // TODO: update it in its supplementary table

    // update it in the global frame table
    struct FTE * fte = frame_table_put (page, uaddr, NULL);

    // release the lock
    lock_release (&frame_table_lock);
    return fte;
}

void * fget_locked_page (void * uaddr) 
{
    return NULL;
}

/* free page from frame table */
void ffree_page (void uaddr)
{
    
    return ;
}

/* evict one table */
struct FTE * frame_get_evict (void) 
{
    return NULL;
}

/* clean up all frames */
void fcleanup (void);

/* set page  */
void fset_page_lock (void) 
{
    
;
