/***************************************************************************
 Copyright (c) 2009-2012 The Linux Foundation. All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of The Linux Foundation nor the names of its contributors may
       be used to endorse or promote products derived from this software
       without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
  ***************************************************************************/

/***************************************************************************
 *  Neon memmove: Attempts to do a memmove with Neon registers if possible,
 *     Inputs:
 *        dest: The destination buffer
 *        src: The source buffer
 *        n: The size of the buffer to transfer
 *     Outputs:
 *
 ***************************************************************************/

#include <machine/cpu-features.h>

	/*
	 * These can be overridden in:
	 *   device/<vendor>/<board>/BoardConfig.mk
	 * by setting the following:
	 *   TARGET_USE_KRAIT_BIONIC_OPTIMIZATION := true
	 *   TARGET_USE_KRAIT_PLD_SET := true
	 *   TARGET_KRAIT_BIONIC_PLDOFFS := <pldoffset>
	 *   TARGET_KRAIT_BIONIC_PLDSIZE := <pldsize>
	 *   TARGET_KRAIT_BIONIC_PLDTHRESH := <pldthreshold>
	 */
#ifndef PLDOFFS
#define PLDOFFS	(10)
#endif
#ifndef PLDTHRESH
#define PLDTHRESH (PLDOFFS)
#endif
#if (PLDOFFS < 5)
#error Routine does not support offsets less than 5
#endif
#if (PLDTHRESH < PLDOFFS)
#error PLD threshold must be greater than or equal to the PLD offset
#endif
#ifndef PLDSIZE
#define PLDSIZE (64)
#endif
#define NOP_OPCODE (0xe320f000)

	.code 32
	.align 5
	.global memmove
	.type memmove, %function

	.global bcopy
	.type bcopy, %function

bcopy:
	mov	r12, r0
	mov	r0, r1
	mov	r1, r12
	.balignl 64, NOP_OPCODE, 4*2
memmove:
.Lneon_memmove_cmf:
	subs	r12, r0, r1
	bxeq	lr
	cmphi	r2, r12
	bls	memcpy	/* Use memcpy for non-overlapping areas */

	push	{r0}

.Lneon_back_to_front_copy:
	add	r0, r0, r2
	add	r1, r1, r2
	cmp	r2, #4
	bgt	.Lneon_b2f_gt4
	cmp	r2, #0
.Lneon_b2f_smallcopy_loop:
	beq	.Lneon_memmove_done
	ldrb	r12, [r1, #-1]!
	subs	r2, r2, #1
	strb	r12, [r0, #-1]!
	b	.Lneon_b2f_smallcopy_loop
.Lneon_b2f_gt4:
	sub	r3, r0, r1
	cmp	r2, r3
	movle	r12, r2
	movgt	r12, r3
	cmp	r12, #64
	bge	.Lneon_b2f_copy_64
	cmp	r12, #32
	bge	.Lneon_b2f_copy_32
	cmp	r12, #8
	bge	.Lneon_b2f_copy_8
	cmp	r12, #4
	bge	.Lneon_b2f_copy_4
	b	.Lneon_b2f_copy_1
.Lneon_b2f_copy_64:
	sub	r1, r1, #64	/* Predecrement */
	sub	r0, r0, #64
	movs	r12, r2, lsr #6
	cmp	r12, #PLDTHRESH
	ble	.Lneon_b2f_copy_64_loop_nopld
	sub	r12, #PLDOFFS
	pld	[r1, #-(PLDOFFS-5)*PLDSIZE]
	pld	[r1, #-(PLDOFFS-4)*PLDSIZE]
	pld	[r1, #-(PLDOFFS-3)*PLDSIZE]
	pld	[r1, #-(PLDOFFS-2)*PLDSIZE]
	pld	[r1, #-(PLDOFFS-1)*PLDSIZE]
	.balignl 64, NOP_OPCODE, 4*2
.Lneon_b2f_copy_64_loop_outer:
	pld	[r1, #-(PLDOFFS)*PLDSIZE]
	vld1.32	{q0, q1}, [r1]!
	vld1.32	{q2, q3}, [r1]
	subs	r12, r12, #1
	vst1.32	{q0, q1}, [r0]!
	sub	r1, r1, #96	/* Post-fixup and predecrement */
	vst1.32	{q2, q3}, [r0]
	sub	r0, r0, #96
	bne	.Lneon_b2f_copy_64_loop_outer
	mov	r12, #PLDOFFS
	.balignl 64, NOP_OPCODE, 4*2
.Lneon_b2f_copy_64_loop_nopld:
	vld1.32	{q8, q9}, [r1]!
	vld1.32	{q10, q11}, [r1]
	subs	r12, r12, #1
	vst1.32	{q8, q9}, [r0]!
	sub	r1, r1, #96	/* Post-fixup and predecrement */
	vst1.32	{q10, q11}, [r0]
	sub	r0, r0, #96
	bne	.Lneon_b2f_copy_64_loop_nopld
	ands	r2, r2, #0x3f
	beq	.Lneon_memmove_done
	add	r1, r1, #64	/* Post-fixup */
	add	r0, r0, #64
	cmp	r2, #32
	blt	.Lneon_b2f_copy_finish
.Lneon_b2f_copy_32:
	mov	r12, r2, lsr #5
.Lneon_b2f_copy_32_loop:
	sub	r1, r1, #32	/* Predecrement */
	sub	r0, r0, #32
	vld1.32	{q0,q1}, [r1]
	subs	r12, r12, #1
	vst1.32	{q0,q1}, [r0]
	bne	.Lneon_b2f_copy_32_loop
	ands	r2, r2, #0x1f
	beq	.Lneon_memmove_done
.Lneon_b2f_copy_finish:
.Lneon_b2f_copy_8:
	movs	r12, r2, lsr #0x3
	beq	.Lneon_b2f_copy_4
	.balignl 64, NOP_OPCODE, 4*2
.Lneon_b2f_copy_8_loop:
	sub	r1, r1, #8	/* Predecrement */
	sub	r0, r0, #8
	vld1.32	{d0}, [r1]
	subs	r12, r12, #1
	vst1.32	{d0}, [r0]
	bne	.Lneon_b2f_copy_8_loop
	ands	r2, r2, #0x7
	beq	.Lneon_memmove_done
.Lneon_b2f_copy_4:
	movs	r12, r2, lsr #0x2
	beq	.Lneon_b2f_copy_1
.Lneon_b2f_copy_4_loop:
	ldr	r3, [r1, #-4]!
	subs	r12, r12, #1
	str	r3, [r0, #-4]!
	bne	.Lneon_b2f_copy_4_loop
	ands	r2, r2, #0x3
.Lneon_b2f_copy_1:
	cmp	r2, #0
	beq	.Lneon_memmove_done
	.balignl 64, NOP_OPCODE, 4*2
.Lneon_b2f_copy_1_loop:
	ldrb	r12, [r1, #-1]!
	subs	r2, r2, #1
	strb	r12, [r0, #-1]!
	bne	.Lneon_b2f_copy_1_loop

.Lneon_memmove_done:
	pop	{r0}
	bx	lr

	.end