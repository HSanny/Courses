#include "vm/swap.h"
#include "vm/frame.h"
#include "vm/page.h"
#include "threads/synch.h"
#include "userprog/pagedir.h"
#include "userprog/exception.h"
#include "devices/block.h"
#include "lib/kernel/bitmap.h"
#include "userprog/pagedir.h"

#define SECTOR_PER_SLOT (PGSIZE/BLOCK_SECTOR_SIZE)

struct lock swap_lock;
struct bitmap * swap_map;

void swap_init (void)
{
    // initialize the lock for the swap space
    lock_init (&swap_lock);
    // initialize the number of available slots
    struct block * swap_block = block_get_role (BLOCK_SWAP);
    size_t num_swap_slots = block_size (swap_block) / SECTOR_PER_SLOT;
    // initialize the bitmap for the swap space
    swap_map = bitmap_create (num_swap_slots);
}

void * swap_out (struct FTE * evict)
{
    ASSERT (evict != NULL);
    lock_acquire (&swap_lock);

    // obtain necessary information
    void * ppage = evict->paddr;
    uint32_t pagedir = evict->supplementary_page->pagedir;
    void * vaddr = evict->vaddr;
    // find the dirty property of that provided frame
    bool dirty = pagedir_is_dirty (pagedir, vaddr);
    // clear up the page in physical memory
    pagedir_clear_page(pagedir, vaddr);

    if(!swap_out_core(evict, dirty)) 
        PANIC("swap space full..");

    lock_release (&swap_lock);
    return ppage;
}

bool swap_out_core (struct FTE * fte, bool dirty) 
{
    // for executable code segment
    if (fte->supplementary_page->executable) {
        if (fte->supplementary_page->modified || dirty) {
            fte->supplementary_page->modified = true;
            return swap_pool_write(fte);
        }
        else return true;
    }  
    // for general data segment
    else {
        return swap_pool_write(fte);
    }
}

/* write an evictable frame to the swap pool */
bool swap_pool_write (struct FTE * frame) 
{
    // target one swap slot with exception handling
    size_t swap_slot = bitmap_scan_and_flip (swap_map, 0, 1, false);
    if (swap_slot == BITMAP_ERROR) {
        return false;
    }

    // keep track of the swapping position in SPTE
    frame->supplementary_page->swap_slot = swap_slot;
    // get the global swap partition
    struct block * swap_block = block_get_role (BLOCK_SWAP);
    void * data_base = frame->paddr;

    // write the given frame to the swapping space sector by sector
    int i;
    for (i = 0; i < SECTOR_PER_SLOT; i++) {
        void* data = (char*) data_base + BLOCK_SECTOR_SIZE * i;
        uint32_t sector = (swap_slot * SECTOR_PER_SLOT) + i;
        // one-block data writen to the swap space
        block_write (swap_block, sector, data);
    }

    return true;
}

/* Read from the swap pool in response to a page fault */
struct FTE * swap_pool_read (struct SP * fault_page) 
{
    // the fault page must be currently evicted
    ASSERT (fault_page->evicted == true);

    // obtain the swapped slot index and swap disk block
    size_t swap_slot = fault_page->swap_slot;
    struct block * swap_block = block_get_role (BLOCK_SWAP);

    // mutual exclusion: prevent simultaneous writing
    lock_acquire (&swap_lock);

    // get a not evictable page 
    void * ppage = fget_page_lock (PAL_USER, fault_page->vaddr);
    int i;
    for (i = 0; i < SECTOR_PER_SLOT; i ++) {
        void * paddr = ppage + BLOCK_SECTOR_SIZE * i;
        uint32_t sector = (swap_slot * SECTOR_PER_SLOT) + i;
        block_read (swap_block, sector, paddr);
    }

    // now it is memory resident
    fault_page->evicted = false;
    // update the frame address of that faulted page
    fault_page->ppage = ppage;
    // update the bitmap: flip the bit associted with the swapped-in slot
    bitmap_flip (swap_map, swap_slot);
    lock_release (&swap_lock);

    return frame_table_find(ppage);
}

