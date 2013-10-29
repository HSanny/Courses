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

/* initialize the frame table */
bool frame_table_init (void);

/* lookup the frame table entry */
struct FTE * frame_table_lookup (void* paddr);

/* remove the specified frame table entry */
struct FTE * frame_table_remove (void* paddr);

/* get page from frame table */
void * fget_page (void * uaddr);
void * fget_locked_page (void * uaddr);

/* free page from frame table */
void ffree_page (void uaddr);

/* evict one table */
struct FTE * frame_get_evict (void);

/* clean up all frames */
void fcleanup (void);

/* set page  */
void fset_page_lock (void);
