#ifndef FILESYS_FILESYS_H
#define FILESYS_FILESYS_H

#include <stdbool.h>
#include "filesys/off_t.h"
#include "threads/synch.h"


/* Sectors of system file inodes. */
#define FREE_MAP_SECTOR 0       /* Free map file inode sector. */
#define ROOT_DIR_SECTOR 1       /* Root directory file inode sector. */

/* Block device that contains the file system. */
struct block *fs_device;

struct lock filesys_lock;

// ---------------------------------------------------
struct dir * ROOT_DIR;  /* the root directory */
struct inode;
char ** separate_pathname (const char *);
// ---------------------------------------------------

void filesys_init (bool format);
void filesys_done (void);
struct file *filesys_open (const char *name);
bool filesys_remove (const char *name);
// ---------------------------------------------------
bool filesys_create (const char *name, off_t initial_size);
bool filesys_mkdir (const char * dirname);
bool filesys_lookup (const char * name, struct inode **);
// ---------------------------------------------------

#endif /* filesys/filesys.h */
