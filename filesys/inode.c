#include "filesys/inode.h"
#include <list.h>
#include <debug.h>
#include <round.h>
#include <string.h>
#include "filesys/filesys.h"
#include "filesys/free-map.h"
#include "threads/malloc.h"
#include "threads/synch.h"
/* Identifies an inode. */
#define INODE_MAGIC 0x494e4f44

#define DIRECT_BLOCKS 4
#define INDIRECTION_BLOCKS 9
#define DOUBLE_INDIRECTION_BLOCKS 1

#define DIRECT_INDEX 0
#define INDIRECTION_INDEX 4
#define DOUBLE_INDIRECTION_INDEX 13

#define INDIRECTION_BLOCK_POINTERS 128
#define INODE_BLOCK_POINTERS 14

/* 8 megabyte file size limit */
#define MAX_FILE_SIZE 8980480

/* On-disk inode.
   Must be exactly BLOCK_SECTOR_SIZE bytes long. */
struct inode_disk
{
    off_t length;                        /* File size in bytes. */
    unsigned magic;                      /* Magic number. */
    uint32_t unused[107];                /* Not used. */
    uint32_t direct_index;               /* stores how many pointers has been allocated inside this inode sector*/
    uint32_t indirection_index;             /* stores how many indirection pointers has been allocated inside 1st level indirectionion block */
    uint32_t double_indirection_index;      /* stores how many indirection pointers has been allocated inside 2nd level indirectionion block */
    bool isdir;                           /* stores if this is a directory or not */
    block_sector_t parent;                /* stores the inumber of its parent */
    block_sector_t pointer[INODE_BLOCK_POINTERS]; /* Pointers to blocks */
};

/* structure for a block full of pointers */
struct indirection_block
{
    block_sector_t pointer[INDIRECTION_BLOCK_POINTERS];
};

bool inode_alloc (struct inode_disk *disk_inode);
off_t inode_expand (struct inode *inode, off_t new_length);
size_t inode_expand_indirection_block (struct inode *inode,
        size_t new_data_sectors);
size_t inode_expand_double_indirection_block (struct inode *inode,
        size_t new_data_sectors);
size_t inode_expand_double_indirection_block_lvl_two (struct inode *inode,
        size_t new_data_sectors,
        struct indirection_block *outer_block);

void inode_dealloc (struct inode *inode);
void inode_dealloc_indirection_block (block_sector_t *pointer, size_t data_pointers);
void inode_dealloc_double_indirection_block (block_sector_t *pointer,
        size_t indirection_pointers,
        size_t data_pointers);


// ========================================================================
/* Returns the number of sectors to allocate for an inode SIZE
   bytes long. */
static inline size_t bytes_to_data_sectors (off_t size)
{
    return DIV_ROUND_UP (size, BLOCK_SECTOR_SIZE);
}

/* Returns the number of indirection pointers to allocate for an inode SIZE
   bytes long. */
static inline size_t bytes_to_indirection_sectors (off_t size)
{
    if (size <= BLOCK_SECTOR_SIZE*DIRECT_BLOCKS)
    {
        return 0;
    }
    size -= BLOCK_SECTOR_SIZE*DIRECT_BLOCKS;
    return DIV_ROUND_UP(size, BLOCK_SECTOR_SIZE*INDIRECTION_BLOCK_POINTERS);
}

/* Returns the number of double indirection pointers to allocate for an inode SIZE
   bytes long. */
static inline size_t bytes_to_double_indirection_sector (off_t size)
{
    if (size <= BLOCK_SECTOR_SIZE*(DIRECT_BLOCKS +
                INDIRECTION_BLOCKS*INDIRECTION_BLOCK_POINTERS))
        return 0;
    return DOUBLE_INDIRECTION_BLOCKS;
}
// ========================================================================

/* In-memory inode. */
struct inode
{
    struct list_elem elem;                /* Element in inode list. */
    block_sector_t sector;                /* Sector number of disk location. */
    int open_cnt;                         /* Number of openers. */
    bool removed;                         /* True if deleted, false otherwise. */
    int deny_write_cnt;                   /* 0: writes ok, >0: deny writes. */
    off_t length;                         /* File size in bytes. */
    off_t read_length;                    /* for reading files after file size, implementaion for file growth */
    size_t direct_index;                  /* stores how many pointers has been allocated inside this inode sector*/
    size_t indirection_index;                /* stores how many indirection pointers has been allocated inside 1st level indirectionion block */
    size_t double_indirection_index;         /* stores how many indirection pointers has been allocated inside 2nd level indirectionion block */
    bool isdir;                           /* store if this file is diretory or not */
    block_sector_t parent;                /* store the inumber of its parent */
    struct lock lock;                     /* lock on inode to achieve synchronization */
    block_sector_t pointer[INODE_BLOCK_POINTERS]; /* Pointers to blocks */
};

/* Returns the block device sector that contains byte offset POS
   within INODE.
   Returns -1 if INODE does not contain data for a byte at offset
   POS. */
    static block_sector_t
byte_to_sector (const struct inode *inode, off_t length, off_t pos)
{
    ASSERT (inode != NULL);
    if (pos < length)
    {
        uint32_t idx;
        uint32_t indirection_block[INDIRECTION_BLOCK_POINTERS];
        if (pos < BLOCK_SECTOR_SIZE*DIRECT_BLOCKS)
        {
            return inode->pointer[pos / BLOCK_SECTOR_SIZE];
        }
        else if (pos < BLOCK_SECTOR_SIZE*(DIRECT_BLOCKS +
                    INDIRECTION_BLOCKS*INDIRECTION_BLOCK_POINTERS))
        {
            pos -= BLOCK_SECTOR_SIZE*DIRECT_BLOCKS;
            idx = pos / (BLOCK_SECTOR_SIZE*INDIRECTION_BLOCK_POINTERS) + DIRECT_BLOCKS;
            block_read(fs_device, inode->pointer[idx], &indirection_block);
            pos %= BLOCK_SECTOR_SIZE*INDIRECTION_BLOCK_POINTERS;
            return indirection_block[pos / BLOCK_SECTOR_SIZE];
        }
        else
        {
            block_read(fs_device, inode->pointer[DOUBLE_INDIRECTION_INDEX],
                    &indirection_block);
            pos -= BLOCK_SECTOR_SIZE*(DIRECT_BLOCKS +
                    INDIRECTION_BLOCKS*INDIRECTION_BLOCK_POINTERS);
            idx = pos / (BLOCK_SECTOR_SIZE*INDIRECTION_BLOCK_POINTERS);
            block_read(fs_device, indirection_block[idx], &indirection_block);
            pos %= BLOCK_SECTOR_SIZE*INDIRECTION_BLOCK_POINTERS;
            return indirection_block[pos / BLOCK_SECTOR_SIZE];
        }
    }
    else
    {
        return -1;
    }
}

/* List of open inodes, so that opening a single inode twice
   returns the same `struct inode'. */
static struct list open_inodes;

/* Initializes the inode module. */
    void
inode_init (void)
{
    list_init (&open_inodes);
}

/* Initializes an inode with LENGTH bytes of data and
   writes the new inode to sector SECTOR on the file system
   device.
   Returns true if successful.
   inode->
   Returns false if memory or disk allocation fails. */
    bool
inode_create (block_sector_t sector, off_t length, bool isdir)
{
    struct inode_disk *disk_inode = NULL;
    bool success = false;

    ASSERT (length >= 0);

    /* If this assertion fails, the inode structure is not exactly
       one sector in size, and you should fix that. */
    ASSERT (sizeof *disk_inode == BLOCK_SECTOR_SIZE);

    disk_inode = calloc (1, sizeof *disk_inode);
    if (disk_inode != NULL)
    {
        disk_inode->length = length;
        if (disk_inode->length > MAX_FILE_SIZE)
        {
            disk_inode->length = MAX_FILE_SIZE;
        }
        disk_inode->magic = INODE_MAGIC;
        disk_inode->isdir = isdir;
        disk_inode->parent = ROOT_DIR_SECTOR;
        if (inode_alloc(disk_inode))
        {
            block_write (fs_device, sector, disk_inode);
            success = true;
        }
        free (disk_inode);
    }
    return success;
}

/* Reads an inode from SECTOR
   and returns a `struct inode' that contains it.
   Returns a null pointer if memory allocation fails. */
    struct inode *
inode_open (block_sector_t sector)
{
    struct list_elem *e;
    struct inode *inode;

    /* Check whether this inode is already open. */
    for (e = list_begin (&open_inodes); e != list_end (&open_inodes);
            e = list_next (e))
    {
        inode = list_entry (e, struct inode, elem);
        if (inode->sector == sector)
        {
            inode_reopen (inode);
            return inode;
        }
    }

    /* Allocate memory. */
    inode = malloc (sizeof *inode);
    if (inode == NULL)
        return NULL;

    /* Initialize. */
    list_push_front (&open_inodes, &inode->elem);
    inode->sector = sector;
    inode->open_cnt = 1;
    inode->deny_write_cnt = 0;
    inode->removed = false;
    lock_init(&inode->lock);
    struct inode_disk data;
    block_read(fs_device, inode->sector, &data);
    inode->length = data.length;
    inode->read_length = data.length;
    inode->direct_index = data.direct_index;
    inode->indirection_index = data.indirection_index;
    inode->double_indirection_index = data.double_indirection_index;
    inode->isdir = data.isdir;
    inode->parent = data.parent;
    memcpy(&inode->pointer, &data.pointer, INODE_BLOCK_POINTERS*sizeof(block_sector_t));
    return inode;
}

/* Reopens and returns INODE. */
    struct inode *
inode_reopen (struct inode *inode)
{
    if (inode != NULL)
        inode->open_cnt++;
    return inode;
}

/* Returns INODE's inode number. */
    block_sector_t
inode_get_inumber (const struct inode *inode)
{
    return inode->sector;
}

/* Closes INODE and writes it to disk.
   If this was the last reference to INODE, frees its memory.
   If INODE was also a removed inode, frees its blocks. */
    void
inode_close (struct inode *inode)
{
    /* Ignore null pointer. */
    if (inode == NULL)
        return;

    /* Release resources if this was the last opener. */
    if (--inode->open_cnt == 0)
    {
        /* Remove from inode list and release lock. */
        list_remove (&inode->elem);

        /* Deallocate blocks if removed. */
        if (inode->removed)
        {
            free_map_release (inode->sector, 1);
            inode_dealloc(inode);
        }
        else
        {
            struct inode_disk disk_inode = {
                .length = inode->length,
                .magic = INODE_MAGIC,
                .direct_index = inode->direct_index,
                .indirection_index = inode->indirection_index,
                .double_indirection_index = inode->double_indirection_index,
                .isdir = inode->isdir,
                .parent = inode->parent,
            };
            memcpy(&disk_inode.pointer, &inode->pointer,
                    INODE_BLOCK_POINTERS*sizeof(block_sector_t));
            block_write(fs_device, inode->sector, &disk_inode);
        }
        free (inode);
    }
}

/* Marks INODE to be deleted when it is closed by the last caller who
   has it open. */
    void
inode_remove (struct inode *inode)
{
    ASSERT (inode != NULL);
    inode->removed = true;
}

/* Reads SIZE bytes from INODE into BUFFER, starting at position OFFSET.
   Returns the number of bytes actually read, which may be less
   than SIZE if an error occurs or end of file is reached. */
    off_t
inode_read_at (struct inode *inode, void *buffer_, off_t size, off_t offset)
{
    uint8_t *buffer = buffer_;
    off_t bytes_read = 0;

    off_t length = inode->read_length;

    if (offset >= length)
    {
        return bytes_read;
    }

    while (size > 0)
    {
        /* Disk sector to read, starting byte offset within sector. */
        block_sector_t sector_idx = byte_to_sector (inode, length, offset);
        int sector_ofs = offset % BLOCK_SECTOR_SIZE;

        /* Bytes left in inode, bytes left in sector, lesser of the two. */
        off_t inode_left = length - offset;
        int sector_left = BLOCK_SECTOR_SIZE - sector_ofs;
        int min_left = inode_left < sector_left ? inode_left : sector_left;

        /* Number of bytes to actually copy out of this sector. */
        int chunk_size = size < min_left ? size : min_left;
        if (chunk_size <= 0)
            break;

        uint8_t temp[BLOCK_SECTOR_SIZE];
        block_read(fs_device, sector_idx, &temp);
        memcpy (buffer + bytes_read, (uint8_t *) &temp + sector_ofs,
                chunk_size);


        //c->accessed = true;
        // c->open_cnt--;

        /* Advance. */
        size -= chunk_size;
        offset += chunk_size;
        bytes_read += chunk_size;
    }

    return bytes_read;
}

/* Writes SIZE bytes from BUFFER into INODE, starting at OFFSET.
   Returns the number of bytes actually written, extend file is implemented */
    off_t
inode_write_at (struct inode *inode, const void *buffer_, off_t size,
        off_t offset)
{
    const uint8_t *buffer = buffer_;
    off_t bytes_written = 0;
    bool sync = false;

    if (inode->deny_write_cnt)
        return 0;

    if (offset + size >inode_length(inode))
    {
        if (!inode->isdir)
        {
            inode_lock(inode);
            sync = true;
        }
        inode->length = inode_expand(inode, offset + size);
        struct inode_disk disk_inode = {
                .length = inode->length,
                .magic = INODE_MAGIC,
                .direct_index = inode->direct_index,
                .indirection_index = inode->indirection_index,
                .double_indirection_index = inode->double_indirection_index,
                .isdir = inode->isdir,
                .parent = inode->parent,
            };
            memcpy(&disk_inode.pointer, &inode->pointer,
                    INODE_BLOCK_POINTERS*sizeof(block_sector_t));
            block_write(fs_device, inode->sector, &disk_inode);
   }

    while (size > 0)
    {
        /* Sector to write, starting byte offset within sector. */
        block_sector_t sector_idx = byte_to_sector (inode,
                inode_length(inode),
                offset);
        int sector_ofs = offset % BLOCK_SECTOR_SIZE;

        /* Bytes left in inode, bytes left in sector, lesser of the two. */
        off_t inode_left = inode_length(inode) - offset;
        int sector_left = BLOCK_SECTOR_SIZE - sector_ofs;
        int min_left = inode_left < sector_left ? inode_left : sector_left;

        /* Number of bytes to actually write into this sector. */
        int chunk_size = size < min_left ? size : min_left;
        if (chunk_size <= 0)
            break;

        uint8_t temp[BLOCK_SECTOR_SIZE];
        block_read(fs_device, sector_idx, &temp);


        memcpy ((uint8_t *) &temp + sector_ofs, buffer + bytes_written,
                chunk_size);
        block_write(fs_device, sector_idx, &temp);

        /* Advance. */
        size -= chunk_size;
        offset += chunk_size;
        bytes_written += chunk_size;
    }

    inode->read_length = inode_length(inode);
    if (!inode->isdir && sync)
        {
            inode_unlock(inode);
        }
 
    return bytes_written;
}

/* Disables writes to INODE.
   May be called at most once per inode opener. */
    void
inode_deny_write (struct inode *inode)
{
    inode->deny_write_cnt++;
    ASSERT (inode->deny_write_cnt <= inode->open_cnt);
}

/* Re-enables writes to INODE.
   Must be called once by each inode opener who has called
   inode_deny_write() on the inode, before closing the inode. */
    void
inode_allow_write (struct inode *inode)
{
    ASSERT (inode->deny_write_cnt > 0);
    ASSERT (inode->deny_write_cnt <= inode->open_cnt);
    inode->deny_write_cnt--;
}

/* Returns the length, in bytes, of INODE's data. */
    off_t
inode_length (struct inode *inode)
{
    return inode->length;
}

/* free inode */
void inode_dealloc (struct inode *inode)
{
    size_t data_sectors = bytes_to_data_sectors(inode->length);
    size_t indirection_sectors = bytes_to_indirection_sectors(inode->length);
    size_t double_indirection_sector = bytes_to_double_indirection_sector(
            inode->length);
    unsigned int idx = 0;
    while (data_sectors && idx < INDIRECTION_INDEX)
    {
        free_map_release (inode->pointer[idx], 1);
        data_sectors--;
        idx++;
    }
    while (indirection_sectors && idx < DOUBLE_INDIRECTION_INDEX)
    {
        size_t data_pointers = data_sectors < INDIRECTION_BLOCK_POINTERS ? \
                           data_sectors : INDIRECTION_BLOCK_POINTERS;
        inode_dealloc_indirection_block(&inode->pointer[idx], data_pointers);
        data_sectors -= data_pointers;
        indirection_sectors--;
        idx++;
    }
    if (double_indirection_sector)
    {
        inode_dealloc_double_indirection_block(&inode->pointer[idx],
                indirection_sectors,
                data_sectors);
    }
}

/* free indirection block */
void inode_dealloc_indirection_block (block_sector_t *pointer,
        size_t data_pointers)
{
    unsigned int i;
    struct indirection_block block;
    block_read(fs_device, *pointer, &block);
    for (i = 0; i < data_pointers; i++)
    {
        free_map_release(block.pointer[i], 1);
    }
    free_map_release(*pointer, 1);
}

/* free double indirection block */
void inode_dealloc_double_indirection_block (block_sector_t *pointer,
        size_t indirection_pointers,
        size_t data_pointers)
{
    unsigned int i;
    struct indirection_block block;
    block_read(fs_device, *pointer, &block);
    for (i = 0; i < indirection_pointers; i++)
    {
        size_t data_per_block = data_pointers < INDIRECTION_BLOCK_POINTERS ? data_pointers : \
                                INDIRECTION_BLOCK_POINTERS;
        inode_dealloc_indirection_block(&block.pointer[i], data_per_block);
        data_pointers -= data_per_block;
    }
    free_map_release(*pointer, 1);
}

/* grow file */
off_t inode_expand (struct inode *inode, off_t new_length)
{
    static char zeros[BLOCK_SECTOR_SIZE];
    size_t new_data_sectors = bytes_to_data_sectors(new_length) - \
                              bytes_to_data_sectors(inode->length);

    if (new_data_sectors == 0)
    {
        return new_length;
    }

    // this is the case when only direct pointers are allocated
    while (inode->direct_index < INDIRECTION_INDEX)
    {
        free_map_allocate (1, &inode->pointer[inode->direct_index]);
        block_write(fs_device, inode->pointer[inode->direct_index], zeros);
        inode->direct_index++;
        new_data_sectors--;
        if (new_data_sectors == 0)
        {
            return new_length;
        }
    }

    // this is the case when direct pointers and indirection pointers are allocated
    while (inode->direct_index < DOUBLE_INDIRECTION_INDEX)
    {
        new_data_sectors = inode_expand_indirection_block(inode, new_data_sectors);
        if (new_data_sectors == 0)
        {
            return new_length;
        }
    }

    // this is the case when all pointers are allocated
    if (inode->direct_index == DOUBLE_INDIRECTION_INDEX)
    {
        new_data_sectors = inode_expand_double_indirection_block(inode,
                new_data_sectors);
    }
    return new_length - new_data_sectors*BLOCK_SECTOR_SIZE;
}

/* grow file using indirection block */
size_t inode_expand_indirection_block (struct inode *inode,
        size_t new_data_sectors)
{
    static char zeros[BLOCK_SECTOR_SIZE];
    struct indirection_block block;
    if (inode->indirection_index == 0)
    {
        free_map_allocate(1, &inode->pointer[inode->direct_index]);
    }
    else
    {
        block_read(fs_device, inode->pointer[inode->direct_index], &block);
    }
    while (inode->indirection_index < INDIRECTION_BLOCK_POINTERS)
    {
        free_map_allocate(1, &block.pointer[inode->indirection_index]);
        block_write(fs_device, block.pointer[inode->indirection_index], zeros);
        inode->indirection_index++;
        new_data_sectors--;
        if (new_data_sectors == 0)
        {
            break;
        }
    }
    block_write(fs_device, inode->pointer[inode->direct_index], &block);
    if (inode->indirection_index == INDIRECTION_BLOCK_POINTERS)
    {
        inode->indirection_index = 0;
        inode->direct_index++;
    }
    return new_data_sectors;
}


/* grow file using double indirection block */
size_t inode_expand_double_indirection_block (struct inode *inode,
        size_t new_data_sectors)
{
    struct indirection_block block;
    if (inode->double_indirection_index == 0 && inode->indirection_index == 0)
    {
        free_map_allocate(1, &inode->pointer[inode->direct_index]);
    }
    else
    {
        block_read(fs_device, inode->pointer[inode->direct_index], &block);
    }
    while (inode->indirection_index < INDIRECTION_BLOCK_POINTERS)
    {
        new_data_sectors = inode_expand_double_indirection_block_lvl_two(inode,
                new_data_sectors, &block);
        if (new_data_sectors == 0)
        {
            break;
        }
    }
    block_write(fs_device, inode->pointer[inode->direct_index], &block);
    return new_data_sectors;
}

/* helper function for grow file using double indirection block */
size_t inode_expand_double_indirection_block_lvl_two (struct inode *inode,
        size_t new_data_sectors,
        struct indirection_block* outer_block)
{
    static char zeros[BLOCK_SECTOR_SIZE];
    struct indirection_block inner_block;
    if (inode->double_indirection_index == 0)
    {
        free_map_allocate(1, &outer_block->pointer[inode->indirection_index]);
    }
    else
    {
        block_read(fs_device, outer_block->pointer[inode->indirection_index],
                &inner_block);
    }
    while (inode->double_indirection_index < INDIRECTION_BLOCK_POINTERS)
    {
        free_map_allocate(1, &inner_block.pointer[inode->double_indirection_index]);
        block_write(fs_device, inner_block.pointer[inode->double_indirection_index],
                zeros);
        inode->double_indirection_index++;
        new_data_sectors--;
        if (new_data_sectors == 0)
        {
            break;
        }
    }
    block_write(fs_device, outer_block->pointer[inode->indirection_index], &inner_block);
    if (inode->double_indirection_index == INDIRECTION_BLOCK_POINTERS)
    {
        inode->double_indirection_index = 0;
        inode->indirection_index++;
    }
    return new_data_sectors;
}

/* allocate an inode given an inode on disk */
bool inode_alloc (struct inode_disk *disk_inode)
{
    struct inode inode = {
        .length = 0,
        .direct_index = 0,
        .indirection_index = 0,
        .double_indirection_index = 0,
    };
    inode_expand(&inode, disk_inode->length);
    disk_inode->direct_index = inode.direct_index;
    disk_inode->indirection_index = inode.indirection_index;
    disk_inode->double_indirection_index = inode.double_indirection_index;
    memcpy(&disk_inode->pointer, &inode.pointer,
            INODE_BLOCK_POINTERS*sizeof(block_sector_t));
    return true;
}

/* return true if this inode stores a file directory */
bool inode_is_dir (const struct inode *inode)
{
    return inode->isdir;
}

/* returns how many process is accessing this inode */
int inode_get_open_cnt (const struct inode *inode)
{
    return inode->open_cnt;
}

/* returns inumber of its parent */
block_sector_t inode_get_parent (const struct inode *inode)
{
    return inode->parent;
}

/* set the varaible to store inumber of its parent */
void inode_set_parent (struct inode * in, block_sector_t parent_inumber)
{
    in->parent = parent_inumber;
}

/* add a parent to a child inode */
bool inode_add_parent (block_sector_t parent_sector,
        block_sector_t child_sector)
{
    // get the child inode using child inumber
    struct inode* inode = inode_open(child_sector);
    // if the child does not exist, return false
    if (!inode)
    {
        return false;
    }

    //stores the child's parent inside child's PARENT variable
    inode->parent = parent_sector;
    inode_close(inode);
    return true;
}

// acquire a lock on thsi inode
void inode_lock (const struct inode *inode)
{
    lock_acquire(&((struct inode *)inode)->lock);
}

// release the lock on this inode
void inode_unlock (const struct inode *inode)
{
    lock_release(&((struct inode *) inode)->lock);
}

// set the bool variable to store if it is a directory or not
void inode_set_isdir (struct inode * inode, bool isdir)
{
    inode->isdir = isdir;
}

//indicate whether this inode is removed 
bool inode_is_removed (struct inode * inode)
{
    return inode->removed != 0;
}
