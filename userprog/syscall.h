#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#define MAX_ARGS 3
#define USER_VADDR_BOTTOM ((void *) 0x08048000)

void syscall_init (void);
void exit (int status);
const void* check_valid_uaddr(const void * uaddr, int size);


#endif /* userprog/syscall.h */
