/* Generated by gensyscalls.py. Do not edit. */

#include <private/bionic_asm.h>

ENTRY(prlimit64)
    movq    %rcx, %r10
    movl    $__NR_prlimit64, %eax
    syscall
    cmpq    $-MAX_ERRNO, %rax
    jb      1f
    negl    %eax
    movl    %eax, %edi
    call    __set_errno
    orq     $-1, %rax
1:
    ret
END(prlimit64)

    .globl _C_LABEL(prlimit)
    .equ _C_LABEL(prlimit), _C_LABEL(prlimit64)
