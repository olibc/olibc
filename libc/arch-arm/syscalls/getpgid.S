/* Generated by gensyscalls.py. Do not edit. */

#include <private/bionic_asm.h>

ENTRY(getpgid)
    ldr     ip, =__NR_getpgid
    b       __bionic_syscall_eabi
END(getpgid)
