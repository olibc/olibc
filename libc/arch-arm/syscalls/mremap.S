/* Generated by gensyscalls.py. Do not edit. */

#include <private/bionic_asm.h>

ENTRY(mremap)
    ldr     ip, =__NR_mremap
    b       __bionic_syscall_eabi
END(mremap)
