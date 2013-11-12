#ifndef VM_SWAP_H
#define VM_SWAP_H

#include "threads/synch.h"
#include "vm/frame.h"

struct swap_pool {
    struct lock lock;
    size_t swap_num_slots;
    struct bitmap * swap_map;
};

void swap_init (void);

void * swap_get_frame (struct frame * evict);

bool swap_out (struct FTE * fte, bool dirty);

struct FTE * read_from_swap (struct SP * fault_page);


#endif /* vm/swap.h */
