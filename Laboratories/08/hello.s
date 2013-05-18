.data 
hellostr:
    .string "Hellow World!\n"
.text
.globl _start 
_start:
    movl $4,%eax # write(1,hellostr,13) 
    movl $1,%ebx
    movl $hellostr,%ecx
    movl $13,%edx 
    int $0x80
    movl $1,%eax # exit(0) 
    movl $0,%ebx
    int $0x80
