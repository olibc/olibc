/* Generated by gensyscalls.py. Do not edit. */

#include <private/bionic_asm.h>

ENTRY(sendmmsg)
    ldr     ip, =__NR_sendmmsg
    b       __bionic_syscall_eabi
END(sendmmsg)
