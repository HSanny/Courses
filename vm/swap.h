#ifndef VM_SWAP_H
#define VM_SWAP_H

#include "threads/synch.h"
#include "vm/frame.h"

void swap_init (void);

void * swap_get_frame (struct FTE * evict);

bool swap_out (struct FTE * fte, bool dirty);

bool swap_pool_write (struct FTE * frame);

struct FTE * swap_pool_read (struct SP * fault_page); 

#endif /* vm/swap.h */
