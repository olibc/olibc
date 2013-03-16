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
	 *   TARGET_USE_SCORPION_BIONIC_OPTIMIZATION := true
	 *   TARGET_USE_SCORPION_PLD_SET := true
	 *   TARGET_SCORPION_BIONIC_PLDOFFS := <pldoffset>
	 *   TARGET_SCORPION_BIONIC_PLDSIZE := <pldsize>
	 */
#ifndef PLDOFFS
#define PLDOFFS	(6)
#endif
#ifndef PLDSIZE
#define PLDSIZE	(128)	/* L2 cache line size */
#endif

	.code 32
	.align 5
	.global memmove
	.type memmove, %function

	.global bcopy
	.type bcopy, %function

bcopy:
	mov		r12, r0
	mov		r0, r1
	mov		r1, r12
memmove:
	push            {r0}

	/*
	 * The requirements for memmove state that the function should
	 * operate as if data were being copied from the source to a
	 * buffer, then to the destination.  This is to allow a user
	 * to copy data from a source and target that overlap.
	 *
	 * We can't just do byte copies front-to-back automatically, since
	 * there's a good chance we may have an overlap (why else would someone
	 * intentionally use memmove then?).
	 *
	 * We'll break this into two parts.  Front-to-back, or back-to-front
	 * copies.
	 */
.Lneon_memmove_cmf:
	cmp             r0, r1
	blt             .Lneon_front_to_back_copy
	bgt             .Lneon_back_to_front_copy
	b               .Lneon_memmove_done

	/* #############################################################
	 * Front to Back copy
	 */
.Lneon_front_to_back_copy:
	/*
	 * For small copies, just do a quick memcpy.  We can do this for
	 * front-to-back copies, aligned or unaligned, since we're only
	 * doing 1 byte at a time...
	 */
	cmp             r2, #4
	bgt             .Lneon_f2b_gt4
	cmp             r2, #0
.Lneon_f2b_smallcopy_loop:
	beq             .Lneon_memmove_done
	ldrb            r12, [r1], #1
	subs            r2, r2, #1
	strb            r12, [r0], #1
	b               .Lneon_f2b_smallcopy_loop
.Lneon_f2b_gt4:
	/* The window size is in r3. */
	sub             r3, r1, r0
	/* #############################################################
	 * Front to Back copy 
	 */
	/*
	 * Note that we can't just route based on the size in r2.  If that's
	 * larger than the overlap window in r3, we could potentially
	 * (and likely!) destroy data we're copying.
	 */
	cmp             r2, r3
	movle           r12, r2
	movgt           r12, r3
	cmp             r12, #256
	bge             .Lneon_f2b_copy_128
	cmp             r12, #64
	bge             .Lneon_f2b_copy_32
	cmp             r12, #16
	bge             .Lneon_f2b_copy_16
	cmp             r12, #8
	bge             .Lneon_f2b_copy_8
	cmp             r12, #4
	bge             .Lneon_f2b_copy_4
	b               .Lneon_f2b_copy_1
	nop
.Lneon_f2b_copy_128:
	mov             r12, r2, lsr #7
	cmp             r12, #PLDOFFS
	ble             .Lneon_f2b_copy_128_loop_nopld
	sub             r12, #PLDOFFS
	pld             [r1, #(PLDOFFS-1)*PLDSIZE]
.Lneon_f2b_copy_128_loop_outer:
	pld             [r1, #(PLDOFFS*PLDSIZE)]
	vld1.32         {q0,q1}, [r1]!
	vld1.32         {q2,q3}, [r1]!
	vld1.32         {q8,q9}, [r1]!
	vld1.32         {q10,q11}, [r1]!
	subs            r12, r12, #1
	vst1.32         {q0,q1}, [r0]!
	vst1.32         {q2,q3}, [r0]!
	vst1.32         {q8,q9}, [r0]!
	vst1.32         {q10,q11}, [r0]!
	bne             .Lneon_f2b_copy_128_loop_outer
	mov             r12, #PLDOFFS
.Lneon_f2b_copy_128_loop_nopld:
	vld1.32         {q0,q1}, [r1]!
	vld1.32         {q2,q3}, [r1]!
	vld1.32         {q8,q9}, [r1]!
	vld1.32         {q10,q11}, [r1]!
	subs            r12, r12, #1
	vst1.32         {q0,q1}, [r0]!
	vst1.32         {q2,q3}, [r0]!
	vst1.32         {q8,q9}, [r0]!
	vst1.32         {q10,q11}, [r0]!
	bne             .Lneon_f2b_copy_128_loop_nopld
	ands            r2, r2, #0x7f
	beq             .Lneon_memmove_done
	cmp             r2, #32
	bge             .Lneon_f2b_copy_32
	b               .Lneon_f2b_copy_finish
.Lneon_f2b_copy_32:
	mov             r12, r2, lsr #5
.Lneon_f2b_copy_32_loop:
	vld1.32         {q0,q1}, [r1]!
	subs            r12, r12, #1
	vst1.32         {q0,q1}, [r0]!
	bne             .Lneon_f2b_copy_32_loop
	ands            r2, r2, #0x1f
	beq             .Lneon_memmove_done
.Lneon_f2b_copy_finish:
.Lneon_f2b_copy_16:
	movs            r12, r2, lsr #4
	beq             .Lneon_f2b_copy_8
.Lneon_f2b_copy_16_loop:
	vld1.32         {q0}, [r1]!
	subs            r12, r12, #1
	vst1.32         {q0}, [r0]!
	bne             .Lneon_f2b_copy_16_loop
	ands            r2, r2, #0xf
	beq             .Lneon_memmove_done
.Lneon_f2b_copy_8:
	movs            r12, r2, lsr #3
	beq             .Lneon_f2b_copy_4
.Lneon_f2b_copy_8_loop:
	vld1.32         {d0}, [r1]!
	subs            r12, r12, #1
	vst1.32         {d0}, [r0]!
	bne             .Lneon_f2b_copy_8_loop
	ands            r2, r2, #0x7
	beq             .Lneon_memmove_done
.Lneon_f2b_copy_4:
	movs            r12, r2, lsr #2
	beq             .Lneon_f2b_copy_1
.Lneon_f2b_copy_4_loop:
	ldr             r3, [r1], #4
	subs            r12, r12, #1
	str             r3, [r0], #4
	bne             .Lneon_f2b_copy_4_loop
	ands            r2, r2, #0x3
	nop
.Lneon_f2b_copy_1:
	cmp             r2, #0
	beq             .Lneon_memmove_done
.Lneon_f2b_copy_1_loop:
	ldrb            r12, [r1], #1
	subs            r2, r2, #1
	strb            r12, [r0], #1
	bne             .Lneon_f2b_copy_1_loop
.Lneon_f2b_finish:
	b               .Lneon_memmove_done

	/* #############################################################
	 * Back to Front copy
	 */
.Lneon_back_to_front_copy:
	/*
	 * Here, we'll want to shift to the end of the buffers.  This
	 * actually points us one past where we need to go, but since
	 * we'll pre-decrement throughout, this will be fine.
	 */
	add             r0, r0, r2
	add             r1, r1, r2
	cmp             r2, #4
	bgt             .Lneon_b2f_gt4
	cmp             r2, #0
.Lneon_b2f_smallcopy_loop:
	beq             .Lneon_memmove_done
	ldrb            r12, [r1, #-1]!
	subs            r2, r2, #1
	strb            r12, [r0, #-1]!
	b               .Lneon_b2f_smallcopy_loop
.Lneon_b2f_gt4:
	/*
	 * The minimum of the overlap window size and the copy size
	 * is in r3.
	 */
	sub             r3, r0, r1
	/*
	 * #############################################################
	 * Back to Front copy -
	 */
	cmp             r2, r3
	movle           r12, r2
	movgt           r12, r3
	cmp             r12, #256
	bge             .Lneon_b2f_copy_128
	cmp             r12, #64
	bge             .Lneon_b2f_copy_32
	cmp             r12, #8
	bge             .Lneon_b2f_copy_8
	cmp             r12, #4
	bge             .Lneon_b2f_copy_4
	b               .Lneon_b2f_copy_1
	nop
.Lneon_b2f_copy_128:
	movs            r12, r2, lsr #7
	cmp             r12, #PLDOFFS
	ble             .Lneon_b2f_copy_128_loop_nopld
	sub             r12, #PLDOFFS
	pld             [r1, #-(PLDOFFS-1)*PLDSIZE]
.Lneon_b2f_copy_128_loop_outer:
	pld             [r1, #-(PLDOFFS*PLDSIZE)]
	sub             r1, r1, #128
	sub             r0, r0, #128
	vld1.32         {q0, q1}, [r1]!
	vld1.32         {q2, q3}, [r1]!
	vld1.32         {q8, q9}, [r1]!
	vld1.32         {q10, q11}, [r1]!
	subs            r12, r12, #1
	vst1.32         {q0, q1}, [r0]!
	vst1.32         {q2, q3}, [r0]!
	vst1.32         {q8, q9}, [r0]!
	vst1.32         {q10, q11}, [r0]!
	sub             r1, r1, #128
	sub             r0, r0, #128
	bne             .Lneon_b2f_copy_128_loop_outer
	mov             r12, #PLDOFFS
.Lneon_b2f_copy_128_loop_nopld:
	sub             r1, r1, #128
	sub             r0, r0, #128
	vld1.32         {q0, q1}, [r1]!
	vld1.32         {q2, q3}, [r1]!
	vld1.32         {q8, q9}, [r1]!
	vld1.32         {q10, q11}, [r1]!
	subs            r12, r12, #1
	vst1.32         {q0, q1}, [r0]!
	vst1.32         {q2, q3}, [r0]!
	vst1.32         {q8, q9}, [r0]!
	vst1.32         {q10, q11}, [r0]!
	sub             r1, r1, #128
	sub             r0, r0, #128
	bne             .Lneon_b2f_copy_128_loop_nopld
	ands            r2, r2, #0x7f
	beq             .Lneon_memmove_done
	cmp             r2, #32
	bge             .Lneon_b2f_copy_32
	b               .Lneon_b2f_copy_finish
.Lneon_b2f_copy_32:
	mov             r12, r2, lsr #5
.Lneon_b2f_copy_32_loop:
	sub             r1, r1, #32
	sub             r0, r0, #32
	vld1.32         {q0,q1}, [r1]
	subs            r12, r12, #1
	vst1.32         {q0,q1}, [r0]
	bne             .Lneon_b2f_copy_32_loop
	ands            r2, r2, #0x1f
	beq             .Lneon_memmove_done
.Lneon_b2f_copy_finish:
.Lneon_b2f_copy_8:
	movs            r12, r2, lsr #0x3
	beq             .Lneon_b2f_copy_4
.Lneon_b2f_copy_8_loop:
	sub             r1, r1, #8
	sub             r0, r0, #8
	vld1.32         {d0}, [r1]
	subs            r12, r12, #1
	vst1.32         {d0}, [r0]
	bne             .Lneon_b2f_copy_8_loop
	ands            r2, r2, #0x7
	beq             .Lneon_memmove_done
.Lneon_b2f_copy_4:
	movs            r12, r2, lsr #0x2
	beq             .Lneon_b2f_copy_1
.Lneon_b2f_copy_4_loop:
	ldr             r3, [r1, #-4]!
	subs            r12, r12, #1
	str             r3, [r0, #-4]!
	bne             .Lneon_b2f_copy_4_loop
	ands            r2, r2, #0x3
	nop
.Lneon_b2f_copy_1:
	cmp             r2, #0
	beq             .Lneon_memmove_done
.Lneon_b2f_copy_1_loop:
	ldrb            r12, [r1, #-1]!
	subs            r2, r2, #1
	strb            r12, [r0, #-1]!
	bne             .Lneon_b2f_copy_1_loop

.Lneon_memmove_done:
	pop             {r0}
	bx              lr

	.end