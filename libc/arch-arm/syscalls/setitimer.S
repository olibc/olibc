/* Generated by gensyscalls.py. Do not edit. */

#include <private/bionic_asm.h>

ENTRY(setitimer)
    ldr     ip, =__NR_setitimer
    b       __bionic_syscall_eabi
END(setitimer)
