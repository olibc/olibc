/* Generated by gensyscalls.py. Do not edit. */

#include <private/bionic_asm.h>

ENTRY(__waitid)
    ldr     ip, =__NR_waitid
    b       __bionic_long_syscall_eabi
END(__waitid)
