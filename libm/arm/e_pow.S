@ Copyright (c) 2012, Code Aurora Forum. All rights reserved.
@
@ Redistribution and use in source and binary forms, with or without
@ modification, are permitted provided that the following conditions are
@ met:
@     * Redistributions of source code must retain the above copyright
@       notice, this list of conditions and the following disclaimer.
@     * Redistributions in binary form must reproduce the above
@       copyright notice, this list of conditions and the following
@       disclaimer in the documentation and/or other materials provided
@       with the distribution.
@     * Neither the name of Code Aurora Forum, Inc. nor the names of its
@       contributors may be used to endorse or promote products derived
@       from this software without specific prior written permission.
@
@ THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
@ WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
@ MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
@ ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
@ BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
@ CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
@ SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
@ BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
@ WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
@ OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
@ IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <machine/cpu-features.h>
#include <machine/asm.h>

@ Values which exist the program lifetime:
#define HIGH_WORD_MASK      d31
#define EXPONENT_MASK       d30
#define int_1               d29
#define double_1            d28
@ sign and 2^int_n fixup:
#define expadjustment       d7
#define literals            r10
@ Values which exist within both polynomial implementations:
#define int_n               d2
#define int_n_low           s4
#define int_n_high          s5
#define double_n            d3
#define k1                  d27
#define k2                  d26
#define k3                  d25
#define k4                  d24
#define k5                  d23
#define k6                  d22
@ Values which cross the boundaries between polynomial implementations:
#define ss                  d16
#define ss2                 d17
#define ss4                 d18
#define Result              d0
#define Return_hw           r1
#define Return_lw           r0
#define ylg2x               d0
@ Intermediate values only needed sometimes:
@ initial (sorted in approximate order of availability for overwriting):
#define x_hw                r1
#define x_lw                r0
#define y_hw                r3
#define y_lw                r2
#define x                   d0
#define bp                  d4
#define y                   d1
#define ln2                 d5
@ log series:
#define u                   d19
#define v                   d20
#define lg2coeff            d21
#define bpa                 d5
#define bpb                 d3
#define lg2const            d6
#define xmantissa           r8
#define twoto1o5            r4
#define twoto3o5            r5
#define ix                  r6
#define iEXP_MASK           r7
@ exp input setup:
#define twoto1o8mask        d3
#define twoto1o4mask        d4
#define twoto1o2mask        d1
#define ylg2x_round_offset  d16
#define ylg2x_temp          d17
#define yn_temp             d18
#define yn_round_offset     d19
@ Careful, overwriting HIGH_WORD_MASK, reset it if you need it again ...
#define rounded_exponent    d31
@ exp series:
#define k7                  d21
#define k8                  d20
#define ss3                 d19
#define k0                  d20
#define twoto1o4            d6

@instructions that gas doesn't like to encode correctly:
#define vmov_f64            fconstd
#define vmov_f32            fconsts
#define vmovne_f64          fconstdne

ENTRY(pow_neon)
    vmov            x, x_lw, x_hw
    push            {r4, r5, r6, r7, r8, r9, r10, lr}

    @ pre-staged bp values
    vldr            bpa, .LbpA
    vldr            bpb, .LbpB
    @ load two fifths into constant term in case we need it due to offsets
    vldr            lg2const, .Ltwofifths

    @ bp is initially 1.0, may adjust later based on x value
    vmov_f64        bp,  #0x70

    @ extract the mantissa from x for scaled value comparisons
    lsl             xmantissa, x_hw, #12

    @ twoto1o5 = 2^(1/5) (input bracketing)
    movw            twoto1o5, #0x186c
    movt            twoto1o5, #0x2611
    @ twoto3o5 = 2^(3/5) (input bracketing)
    movw            twoto3o5, #0x003b
    movt            twoto3o5, #0x8406

    @ finish extracting xmantissa
    orr             xmantissa, xmantissa, x_lw, lsr #20

    @ begin preparing a mask for normalization
    vmov.i64        HIGH_WORD_MASK, #0xffffffff00000000

    @ double_1 = (double) 1.0
    vmov_f64        double_1, #0x70

    vmov            y, y_lw, y_hw

    cmp             xmantissa, twoto1o5

    vshl.i64        EXPONENT_MASK, HIGH_WORD_MASK, #20
    vshr.u64        int_1, HIGH_WORD_MASK, #63

    adr             literals, .LliteralTable

    movw            iEXP_MASK, #0xfff0
    movt            iEXP_MASK, #0x0000

    bic             ix, x_hw, iEXP_MASK

    @ if normalized x > 2^(1/5), bp = 1 + (2^(2/5)-1) = 2^(2/5)
    vaddhi.f64      bp, bp, bpa
    @ zero out lg2 constant term if don't offset our input
    vsubls.f64      lg2const, lg2const, lg2const

    @ will need ln2 for various things
    vldr            ln2, .Lln2

    cmp             xmantissa, twoto3o5
@@@@ X Value Normalization @@@@

    @ ss = abs(x) 2^(-1024)
    vbic.i64        ss, x, EXPONENT_MASK

    @ N = (floor(log2(x)) + 0x3ff) * 2^52
    vand.i64        int_n, x, EXPONENT_MASK

    @ if normalized x > 2^(3/5), bp = 2^(2/5) + (2^(4/5) - 2^(2/5) = 2^(4/5)
    vaddhi.f64      bp, bp, bpb
    vaddhi.f64      lg2const, lg2const, lg2const

    @ load log2 polynomial series constants
    vldm            literals!, {k4, k3, k2, k1}

    @ s = abs(x) 2^(-floor(log2(x))) (normalize abs(x) to around 1)
    vorr.i64        ss, ss, double_1

@@@@ 3/2 (Log(bp(1+s)/(1-s))) input computation (s = (x-bp)/(x+bp)) @@@@

    vsub.f64        u, ss, bp
    vadd.f64        v, ss, bp

    @ s = (x-1)/(x+1)
    vdiv.f64        ss, u, v

    @ load 2/(3log2) into lg2coeff
    vldr            lg2coeff, .Ltwooverthreeln2

    @ N = floor(log2(x)) * 2^52
    vsub.i64        int_n, int_n, double_1

@@@@ 3/2 (Log(bp(1+s)/(1-s))) polynomial series @@@@

    @ ss2 = ((x-dp)/(x+dp))^2
    vmul.f64        ss2, ss, ss
    @ ylg2x = 3.0
    vmov_f64        ylg2x, #8
    vmul.f64        ss4, ss2, ss2

    @ todo: useful later for two-way clamp
    vmul.f64        lg2coeff, lg2coeff, y

    @ N = floor(log2(x))
    vshr.s64        int_n, int_n, #52

    @ k3 = ss^2 * L4 + L3
    vmla.f64        k3, ss2, k4

    @ k1 = ss^2 * L2 + L1
    vmla.f64        k1, ss2, k2

    @ scale ss by 2/(3 ln 2)
    vmul.f64        lg2coeff, ss, lg2coeff

    @ ylg2x = 3.0 + s^2
    vadd.f64        ylg2x, ylg2x, ss2

    vcvt.f64.s32    double_n, int_n_low

    @ k1 = s^4 (s^2 L4 + L3) + s^2 L2 + L1
    vmla.f64        k1, ss4, k3

    @ add in constant term
    vadd.f64        double_n, lg2const

    @ ylg2x = 3.0 + s^2 + s^4 (s^4 (s^2 L4 + L3) + s^2 L2 + L1)
    vmla.f64        ylg2x, ss4, k1

    @ ylg2x = y 2 s / (3 ln(2)) (3.0 + s^2 + s^4 (s^4(s^2 L4 + L3) + s^2 L2 + L1)
    vmul.f64        ylg2x, lg2coeff, ylg2x

@@@@ Compute input to Exp(s) (s = y(n + log2(x)) - (floor(8 yn + 1)/8 + floor(8 ylog2(x) + 1)/8) @@@@@

    @ mask to extract bit 1 (2^-2 from our fixed-point representation)
    vshl.u64        twoto1o4mask, int_1, #1

    @ double_n = y * n
    vmul.f64        double_n, double_n, y

    @ Load 2^(1/4) for later computations
    vldr            twoto1o4, .Ltwoto1o4

    @ move unmodified y*lg2x into temp space
    vmov            ylg2x_temp, ylg2x
    @ move unmodified y*n into temp space
    vmov            yn_temp, double_n

    @ either add or subtract one based on the sign of double_n and ylg2x
    vshr.s64        ylg2x_round_offset, ylg2x, #62
    vshr.s64        yn_round_offset, double_n, #62

    @ load exp polynomial series constants
    vldm            literals!, {k8, k7, k6, k5, k4, k3, k2, k1}

    @ mask to extract bit 2 (2^-1 from our fixed-point representation)
    vshl.u64        twoto1o2mask, int_1, #2

    @ make rounding offsets either 1 or -1 instead of 0 or -2
    vorr.u64        ylg2x_round_offset, ylg2x_round_offset, int_1
    vorr.u64        yn_round_offset, yn_round_offset, int_1

    @ compute floor(8 y * n + 1)/8
    @ and floor(8 y (log2(x)) + 1)/8
    vcvt.s32.f64    ylg2x, ylg2x, #3

    vcvt.s32.f64    double_n, double_n, #3
    @ round up to the nearest 1/8th
    vadd.s32        ylg2x, ylg2x, ylg2x_round_offset
    vadd.s32        double_n, double_n, yn_round_offset

    @ clear out round-up bit for y log2(x)
    vbic.s32        ylg2x, ylg2x, int_1
    @ clear out round-up bit for yn
    vbic.s32        double_n, double_n, int_1
    @ add together the (fixed precision) rounded parts
    vadd.s64        rounded_exponent, double_n, ylg2x
    @ turn int_n into a double with value 2^int_n
    vshl.i64        int_n, rounded_exponent, #49
    @ compute masks for 2^(1/4) and 2^(1/2) fixups for fractional part of fixed-precision rounded values:
    vand.u64        twoto1o4mask, twoto1o4mask, rounded_exponent
    vand.u64        twoto1o2mask, twoto1o2mask, rounded_exponent

    @ convert back into floating point, double_n now holds (double) floor(8 y * n + 1)/8
    @                                   ylg2x now holds (double) floor(8 y * log2(x) + 1)/8
    vcvt.f64.s32    ylg2x, ylg2x, #3
    vcvt.f64.s32    double_n, double_n, #3

    @ put the 2 bit (0.5) through the roof of twoto1o2mask (make it 0x0 or 0xffffffffffffffff)
    vqshl.u64        twoto1o2mask, twoto1o2mask, #62
    @ put the 1 bit (0.25) through the roof of twoto1o4mask (make it 0x0 or 0xffffffffffffffff)
    vqshl.u64        twoto1o4mask, twoto1o4mask, #63

    @ center y*log2(x) fractional part between -0.125 and 0.125 by subtracting (double) floor(8 y * log2(x) + 1)/8
    vsub.f64        ylg2x_temp, ylg2x_temp, ylg2x
    @ center y*n fractional part between -0.125 and 0.125 by subtracting (double) floor(8 y * n + 1)/8
    vsub.f64        yn_temp, yn_temp, double_n

    @ Add fractional parts of yn and y log2(x) together
    vadd.f64        ss, ylg2x_temp, yn_temp

    @ Result = 1.0 (offset for exp(s) series)
    vmov_f64        Result, #0x70

    @ multiply fractional part of y * log2(x) by ln(2)
    vmul.f64        ss, ln2, ss

@@@@ 10th order polynomial series for Exp(s) @@@@

    @ ss2 = (ss)^2
    vmul.f64        ss2, ss, ss

    @ twoto1o2mask = twoto1o2mask & twoto1o4
    vand.u64        twoto1o2mask, twoto1o2mask, twoto1o4
    @ twoto1o2mask = twoto1o2mask & twoto1o4
    vand.u64        twoto1o4mask, twoto1o4mask, twoto1o4

    @ Result = 1.0 + ss
    vadd.f64        Result, Result, ss

    @ k7 = ss k8 + k7
    vmla.f64        k7, ss, k8

    @ ss4 = (ss*ss) * (ss*ss)
    vmul.f64        ss4, ss2, ss2

    @ twoto1o2mask = twoto1o2mask | (double) 1.0 - results in either 1.0 or 2^(1/4) in twoto1o2mask
    vorr.u64        twoto1o2mask, twoto1o2mask, double_1
    @ twoto1o2mask = twoto1o4mask | (double) 1.0 - results in either 1.0 or 2^(1/4) in twoto1o4mask
    vorr.u64        twoto1o4mask, twoto1o4mask, double_1

    @ TODO: should setup sign here, expadjustment = 1.0
    vmov_f64        expadjustment, #0x70

    @ ss3 = (ss*ss) * ss
    vmul.f64        ss3, ss2, ss

    @ k0 = 1/2 (first non-unity coefficient)
    vmov_f64        k0, #0x60

    @ Mask out non-exponent bits to make sure we have just 2^int_n
    vand.i64        int_n, int_n, EXPONENT_MASK

    @ square twoto1o2mask to get 1.0 or 2^(1/2)
    vmul.f64        twoto1o2mask, twoto1o2mask, twoto1o2mask
    @ multiply twoto2o4mask into the exponent output adjustment value
    vmul.f64        expadjustment, expadjustment, twoto1o4mask

    @ k5 = ss k6 + k5
    vmla.f64        k5, ss, k6

    @ k3 = ss k4 + k3
    vmla.f64        k3, ss, k4

    @ k1 = ss k2 + k1
    vmla.f64        k1, ss, k2

    @ multiply twoto1o2mask into exponent output adjustment value
    vmul.f64        expadjustment, expadjustment, twoto1o2mask

    @ k5 = ss^2 ( ss k8 + k7 ) + ss k6 + k5
    vmla.f64        k5, ss2, k7

    @ k1 = ss^2 ( ss k4 + k3 ) + ss k2 + k1
    vmla.f64        k1, ss2, k3

    @ Result = 1.0 + ss + 1/2 ss^2
    vmla.f64      Result, ss2, k0

    @ Adjust int_n so that it's a double precision value that can be multiplied by Result
    vadd.i64        expadjustment, int_n, expadjustment

    @ k1 = ss^4 ( ss^2 ( ss k8 + k7 ) + ss k6 + k5 ) + ss^2 ( ss k4 + k3 ) + ss k2 + k1
    vmla.f64        k1, ss4, k5

    @ Result = 1.0 + ss + 1/2 ss^2 + ss^3 ( ss^4 ( ss^2 ( ss k8 + k7 ) + ss k6 + k5 ) + ss^2 ( ss k4 + k3 ) + ss k2 + k1 )
    vmla.f64        Result, ss3, k1

    @ multiply by adjustment (sign*(rounding ? sqrt(2) : 1) * 2^int_n)
    vmul.f64        Result, expadjustment, Result

.LleavePow:
    @ return Result (FP)
    vmov            Return_lw, Return_hw, Result
.LleavePowDirect:
    @ leave directly returning whatever is in Return_lw and Return_hw
    pop             {r4, r5, r6, r7, r8, r9, r10, pc}

.align 6
.LliteralTable:
@ Least-sqares tuned constants for 11th order (log2((1+s)/(1-s)):
.LL4: @ ~3/11
    .long       0x53a79915, 0x3fd1b108
.LL3: @ ~1/3
    .long       0x9ca0567a, 0x3fd554fa
.LL2: @ ~3/7
    .long       0x1408e660, 0x3fdb6db7
.LL1: @ ~3/5
    .long       0x332D4313, 0x3fe33333

@ Least-squares tuned constants for 10th order exp(s):
.LE10: @ ~1/3628800
    .long       0x25c7ba0a, 0x3e92819b
.LE9: @ ~1/362880
    .long       0x9499b49c, 0x3ec72294
.LE8: @ ~1/40320
    .long       0xabb79d95, 0x3efa019f
.LE7: @ ~1/5040
    .long       0x8723aeaa, 0x3f2a019f
.LE6: @ ~1/720
    .long       0x16c76a94, 0x3f56c16c
.LE5: @ ~1/120
    .long       0x11185da8, 0x3f811111
.LE4: @ ~1/24
    .long       0x5555551c, 0x3fa55555
.LE3: @ ~1/6
    .long       0x555554db, 0x3fc55555

.LbpA: @ (2^(2/5) - 1)
    .long       0x4ee54db1, 0x3fd472d1

.LbpB: @ (2^(4/5) - 2^(2/5))
    .long       0x1c8a36cf, 0x3fdafb62

.Ltwofifths: @
    .long       0x9999999a, 0x3fd99999

.Ltwooverthreeln2:
    .long       0xDC3A03FD, 0x3FEEC709

.Lln2: @ ln(2)
    .long       0xFEFA39EF, 0x3FE62E42

.Ltwoto1o4: @ 2^1/4
    .long       0x0a31b715, 0x3ff306fe
END(pow)
