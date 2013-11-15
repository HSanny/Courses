#include "vm/swap.h"
#include "vm/frame.h"
#include "vm/page.h"
#include "threads/synch.h"
#include "userprog/pagedir.h"
#include "userprog/exception.h"
#include "devices/block.h"
#include "lib/kernel/bitmap.h"

#define SECTOR_PER_PAGE (PGSIZE/BLOCK_SECTOR_SIZE)

struct swap_pool swap_table;

void swap_init (void)
{
    // initialize the lock for the swap space
    lock_init (&swap_table.swap_lock);
    // initialize the bitmap for the swap space
    bitmap_create (1024);
    // initialize the number of available slots
    struct block * swap_block = block_get_role (BLOCK_SWAP);
    size_t sector_per_page = PGSIZE / BLOCK_SECTOR_SIZE;
    swap_table.num_swap_slots = block_size (swap_block) / sector_per_page;
}

void * swap_get_frame (struct FTE * evict)
{
    ;
    
    
}

bool swap_out (struct FTE * fte, bool dirty) 
{
    
    
    
}

/* write an evictable frame to the swap pool */
bool swap_pool_write (struct FTE * frame) 
{
    // target a slot with exception handling
    size_t swap_slot = bitmap_scan_and_flip (swap_table.swap_map, 0, 1, false);
    if (swap_slot == BITMAP_ERROR) {
        return false;
    }

    // keep track of the swapping position in SPTE
    frame->supplementary_page->swap_slot = swap_slot;
    // get the global swap partition
    struct block* swap_block = block_get_role(BLOCK_SWAP);
    void* base = frame->paddr;

    // write the given frame to the swapping space sector by sector
    int i;
    for (i = 0; i < SECTOR_PER_PAGE; i++) {
        void* data = (char*) base + BLOCK_SECTOR_SIZE * i;
        uint32_t sector = (swap_slot * SECTOR_PER_PAGE) + i;
        // one-block data writen to the swap space
        block_write (swap_block, sector, data);
    }

    return true;
}

/* Read from the swap pool in response to a page fault */
struct FTE * swap_pool_read (struct SP * fault_page) 
{


}

