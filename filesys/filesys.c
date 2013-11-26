// ==================================================================
// uncomment to enable the tests
// #define FS_CREATE_TEST
// #define FS_LOOKUP_TEST
// ==================================================================
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
// ==================================================================
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

struct dir * enter_dir (char ** ptr)
{
    int level = 0;
    // struct dir * cwd = cur->cwd;
    struct thread * cur = thread_current();
    struct dir * cwd = cur->cwd;
    struct dir * tmp_dir;
    if (cwd != NULL) tmp_dir = cur->cwd;
    else tmp_dir = dir_open_root();
    
    struct inode * inode;
    while (*(ptr + level) != NULL && level < MAX_LEVEL-1) {
        char * dirname = * (ptr+level);
        // printf ("str: %s\n", dirname);
        if (strcmp("/", dirname) == 0) {
            tmp_dir = dir_open_root();
            goto levelup;  // skip the checking procedure
        }
        if (strcmp(".", dirname) == 0) {
            // remain the current directory
            goto levelup;
        }
        if (strcmp("..", dirname) == 0) {
            // back to the parent directory
            struct dir * parent_dir = dir_get_parent (tmp_dir);
            dir_close (tmp_dir);
            tmp_dir = parent_dir;
            goto levelup;
        }

        if (!dir_lookup (tmp_dir, dirname, &inode)) return false;
        // each level must be a directory
        if (!inode_is_dir(inode)) return false;
        if (tmp_dir != ROOT_DIR && tmp_dir != cwd) dir_close(tmp_dir);
        tmp_dir = dir_open (inode);
levelup:
        ++ level;
    }
    return tmp_dir;
}

bool filesys_lookup (const char * name, struct inode ** inode) 
{
    // 0-length file name
    if (strlen(name) == 0) return false;
    // address the root directory
    if (!strcmp(name, "/")) {
        *inode = dir_get_inode (dir_open_root());
        return true;
    }
    struct thread * cur = thread_current();
    if (inode_is_removed (dir_get_inode (cur->cwd))) {
#ifdef FS_LOOKUP_TEST
        printf ("*filesys_create* current dir removed\n");
#endif
        return false;
    }
    // parse the input name
    char ** ptr = separate_pathname (name);
    char * filename = * (ptr+MAX_LEVEL-1);
    if (ptr == NULL && filename == NULL) return false;

    // enter specified directory
    struct dir * tmp_dir = enter_dir (ptr);
    if (tmp_dir == NULL) return false;

    // deal with . and ..
    if (strcmp(filename, ".") == 0) {
        *inode = dir_get_inode (tmp_dir);
        return true;
    } else if (strcmp(filename, "..") == 0) {
        *inode = dir_get_inode (dir_get_parent (tmp_dir));
        return true;
    }

    return dir_lookup (tmp_dir, filename, inode);
}

bool filesys_mkdir (const char * name)
{
    // printf ("length:%d\n", strlen(name));
    // printf ("name: %s\n", name);
    if (strlen(name) == 0) 
        return false;

    char ** ptr = separate_pathname (name);
    if (ptr == NULL) return false;
    struct dir * tmp_dir = enter_dir (ptr);
 
    char * final_level = *(ptr+MAX_LEVEL-1);
    // printf ("filename: %s\n", final_level);
    // then, create a directory for the final_level 

    // cached variable
    block_sector_t inode_sector;
    if (tmp_dir == NULL) {
        tmp_dir = ROOT_DIR;
    }
    struct inode * tmp_inode;
    if (dir_lookup (tmp_dir, final_level, &tmp_inode)) {
        return false;
    }

    // FIXME: how to determine the count and entries?
    int count = 1;
    int entries = 10;
    bool success = (tmp_dir != NULL
            && free_map_allocate (count, &inode_sector)
            && dir_create (inode_sector, entries)
            && dir_add (tmp_dir, final_level, inode_sector));
    if (!success && inode_sector != 0) {
        return success;
        // free_map_release (inode_sector, 1);
    }

    // if (tmp_dir != ROOT_DIR && cwd != tmp_dir) dir_close (tmp_dir);
    // printf ("root: %d\n", inode_get_inumber(dir_get_inode(ROOT_DIR)));
    // if (cur->cwd != NULL) printf ("cur: %d\n", inode_get_inumber(dir_get_inode(cur->cwd)));

    // update the inode data structure
    struct inode * in = inode_open (inode_sector);
    inode_set_parent (in, inode_get_inumber(dir_get_inode(tmp_dir)));
    inode_set_isdir (in, true);

    return success;
}

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
    ROOT_DIR = dir_open_root();
    inode_set_isdir(dir_get_inode(ROOT_DIR), true);
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
    //struct dir *dir = dir_open_root ();
    // ========================================================
    char ** ptr = separate_pathname (name);
    if (ptr == NULL) return false;
    struct dir * tmp_dir = enter_dir (ptr); 

    char * new_file_name = *(ptr+MAX_LEVEL-1);
     // printf ("filename_len: %d\n", strlen(new_file_name));
     // printf ("filename: %s\n", new_file_name);
    if (strlen (new_file_name) > MAX_LENGTH_EACH_LEVEL) return false;
    // ========================================================
    if (tmp_dir == NULL) {
#ifdef FS_CREATE_TEST
        printf ("*filesys_create* use root\n");
#endif
        tmp_dir = dir_open_root();
    }
    if (inode_is_removed (dir_get_inode (tmp_dir))) {
#ifdef FS_CREATE_TEST
        printf ("*filesys_create* inode null\n");
#endif
        return false;
    }
    
    block_sector_t inode_sector;
    bool success = (tmp_dir != NULL
            && free_map_allocate (1, &inode_sector)
            && inode_create (inode_sector, initial_size, false)
            && dir_add (tmp_dir, new_file_name, inode_sector));
    if (!success && inode_sector != 0) 
        free_map_release (inode_sector, 1);
    // if (tmp_dir != ROOT_DIR && cwd != tmp_dir) dir_close (tmp_dir);

#ifdef FS_CREATE_TEST
    printf ("root: %d\n", inode_get_inumber(dir_get_inode(ROOT_DIR)));
    printf ("tmp_dir: %d\n", inode_get_inumber(dir_get_inode(tmp_dir)));
    if (cur->cwd != NULL) printf ("cur: %d\n", inode_get_inumber(dir_get_inode(cur->cwd)));
#endif
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
    struct thread * cur = thread_current();
    char ** ptr = separate_pathname (name);
    if (ptr == NULL) return false;
    struct dir *dir = (cur->cwd != NULL)? enter_dir (ptr) : dir_open_root();
    
    struct inode *inode = NULL;
    char * file_name = *(ptr+MAX_LEVEL-1);

    if (dir != NULL)
        dir_lookup (dir, file_name, &inode);
    // if (dir != ROOT_DIR) dir_close (dir);

    return file_open (inode);
}

/* Deletes the file named NAME.
   Returns true if successful, false on failure.
   Fails if no file named NAME exists,
   or if an internal memory allocation fails. */
    bool
filesys_remove (const char *name) 
{
    // removal of root dir is disallowed
    if (strcmp(name, "/") == 0) return false;

    // removal of parent dir is disallowed
    struct thread * cur = thread_current (); 
   // printf ("*remove* cwd: %s \n", cur->cwd);

    char ** ptr = separate_pathname (name);
    if (ptr == NULL) return false;

    struct dir * dir = enter_dir (ptr); 
    char * rm_file = *(ptr+MAX_LEVEL-1);  // name of file to be removed
    struct inode * inode;
    bool allowed = true;  // allow to remove it by default
    if (dir_lookup (dir, rm_file, &inode)) {
        if (inode_is_dir(inode)) {
            block_sector_t rm_dir_i = inode_get_inumber (inode);
            // check if the directory is parent of cwd
            struct dir * temp = dir_get_parent(cur->cwd);
            while (true) {
                block_sector_t i = inode_get_inumber(dir_get_inode(temp)); 
                if (i == 1) break;  // root dir
                else if (i == rm_dir_i) {
                    allowed = false; // not allowed
                    break;
                }
                struct dir *del = dir_get_parent(temp);
                dir_close (temp);
                temp = del;
            }
        }
    }


    if (!allowed) return false;

    bool success = dir != NULL && dir_remove (dir, rm_file);

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
