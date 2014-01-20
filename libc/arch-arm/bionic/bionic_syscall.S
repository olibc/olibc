/*
 * Copyright (C) 2013 olibc developers
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <private/bionic_asm.h>

ENTRY_PRIVATE(__bionic_syscall_eabi)
    /* Swap ip and r7 */
    eor     ip, ip, r7
    eor     r7, ip, r7
    eor     ip, ip, r7

    swi     #0
    mov     r7, ip
    cmn     r0, #(MAX_ERRNO + 1)
    bxls    lr
    neg     r0, r0
    b       __set_errno
END(__bionic_syscall_eabi)

ENTRY_PRIVATE(__bionic_long_syscall_eabi)
    .save   {r4, r5, r6, r7}
    stmfd   sp!, {r4, r5, r6, r7}

    /* Move the system call number to r7 */
    mov     r7, ip
    add     ip, sp, #16
    ldmfd   ip, {r4, r5, r6}
    swi     #0
    ldmfd   sp!, {r4, r5, r6, r7}
    cmn     r0, #(MAX_ERRNO + 1)
    bxls    lr
    neg     r0, r0
    b       __set_errno
END(__bionic_long_syscall_eabi)
