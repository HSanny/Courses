#include "filesys/filesys.h"
#include <debug.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "filesys/file.h"
#include "filesys/free-map.h"
#include "filesys/inode.h"
#include "filesys/directory.h"
#include "threads/thread.h"

#define MAX_LEVEL 15 
#define MAX_LENGTH_EACH_LEVEL 15
/* Partition that contains the file system. */
struct block *fs_device;
static void do_format (void);

// ==================================================================
char ** separate_pathname (const char * path) {
    if (path == NULL) return NULL;
    char ** ptr = (char**) malloc (MAX_LEVEL * sizeof(char*));
    int i = 0;
    for (; i < MAX_LEVEL; i ++ ) {
        *(ptr+i) = NULL;
    }
    int last_pos = 0;  // position of last separation
    int pos = 0; // current pointer position
    int level = 0;
    char * str;
    while (*(path+pos) != 0) {
        char curchar = *(path+pos);
        char nextchar = *(path+pos+1);
        if (curchar == '/' && nextchar != '/') {
            // separation occurs here..
            if (pos - last_pos > MAX_LENGTH_EACH_LEVEL) PANIC ("path too long!");
            if (pos == 0) {
                str = (char *) malloc (2 * sizeof(char));
                memcpy (str, path, 1);
                *(str+1) = 0; // null terminator
                last_pos = 1;
            } else { 
                int size = pos-last_pos;
                str = (char *) malloc ((size+1) * sizeof(char));
                memcpy (str, path+last_pos, size);
                *(str+size) = 0;
            }
            *(ptr+level) = str; // append a new string
            last_pos = pos + 1;
            ++ level;
        } else if (curchar == '/' && nextchar == '/') {
            // no separation and skip the double slash
            ++ pos;
        }
        ++ pos;
    }

    if (pos > last_pos) {
        int size = pos-last_pos;
        str = (char *) malloc ((size+1) * sizeof(char));
        memcpy (str, path+last_pos, size);
        *(str+size) = 0;
        *(ptr+MAX_LEVEL-1) = str; // append a new string
    }

    return ptr;
}
// =================================================================

/* Initializes the file system module.
   If FORMAT is true, reformats the file system. */
    void
filesys_init (bool format) 
{
    fs_device = block_get_role (BLOCK_FILESYS);
    if (fs_device == NULL)
        PANIC ("No file system device found, can't initialize file system.");

    inode_init ();
    free_map_init ();

    if (format) 
        do_format ();

    free_map_open ();

    // ---------------------------------------------------
    ROOT_DIR = dir_open_root();
    // ---------------------------------------------------
}

/* Shuts down the file system module, writing any unwritten data
   to disk. */
    void
filesys_done (void) 
{
    free_map_close ();
}

/* Creates a file named NAME with the given INITIAL_SIZE.
   Returns true if successful, false otherwise.
   Fails if a file named NAME already exists,
   or if internal memory allocation fails. */
    bool
filesys_create (const char *name, off_t initial_size) 
{
    block_sector_t inode_sector = 0;
    //struct dir *dir = dir_open_root ();
    // ========================================================
    struct thread * cur = thread_current (); 
    char ** ptr = separate_pathname (name);
    if (ptr == NULL) return false;
    int level = 0;
    struct dir * tmp_dir = cur->cwd;
    struct inode * inode;
    while (*(ptr + level) != NULL && level < MAX_LEVEL-1) {
        char * dirname = * (ptr+level);
    //    printf ("str: %s\n", dirname);
        if (!dir_lookup (tmp_dir, dirname, &inode)) return false;
        if (tmp_dir != ROOT_DIR) dir_close(tmp_dir);
        tmp_dir = dir_open (inode);
        ++ level;
    } 

    char * new_file_name = *(ptr+MAX_LEVEL-1);
  //  printf ("filename: %s\n", new_file_name);
    // ========================================================
    if (tmp_dir == NULL) tmp_dir = dir_open_root();
    bool success = (tmp_dir != NULL
            && free_map_allocate (1, &inode_sector)
            && inode_create (inode_sector, initial_size)
            && dir_add (tmp_dir, new_file_name, inode_sector));
    if (!success && inode_sector != 0) 
        free_map_release (inode_sector, 1);
    if (tmp_dir != ROOT_DIR) dir_close (tmp_dir);
    // ========================================================

    // ========================================================
    return success;
}

/* Opens the file with the given NAME.
   Returns the new file if successful or a null pointer
   otherwise.
   Fails if no file named NAME exists,
   or if an internal memory allocation fails. */
    struct file *
filesys_open (const char *name)
{
    struct dir *dir = dir_open_root ();
    struct inode *inode = NULL;

    if (dir != NULL)
        dir_lookup (dir, name, &inode);
    dir_close (dir);

    return file_open (inode);
}

/* Deletes the file named NAME.
   Returns true if successful, false on failure.
   Fails if no file named NAME exists,
   or if an internal memory allocation fails. */
    bool
filesys_remove (const char *name) 
{
    struct dir *dir = dir_open_root ();
    bool success = dir != NULL && dir_remove (dir, name);
    dir_close (dir); 

    return success;
}

/* Formats the file system. */
    static void
do_format (void)
{
    printf ("Formatting file system...");
    free_map_create ();
    if (!dir_create (ROOT_DIR_SECTOR, 16))
        PANIC ("root directory creation failed");
    free_map_close ();
    printf ("done.\n");
}
