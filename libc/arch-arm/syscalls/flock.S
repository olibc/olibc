/* Generated by gensyscalls.py. Do not edit. */

#include <private/bionic_asm.h>

ENTRY(flock)
    ldr     ip, =__NR_flock
    b       __bionic_syscall_eabi
END(flock)
