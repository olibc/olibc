@ Copyright (c) 2012, The Linux Foundation. All rights reserved.
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
@
@ Additional notices preserved for attributions purposes only.
@
@ ====================================================
@ Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
@
@ Developed at SunSoft, a Sun Microsystems, Inc. business.
@ Permission to use, copy, modify, and distribute this
@ software is freely granted, provided that this notice
@ is preserved.
@ ====================================================
@
@ ====================================================
@ Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
@
@ Developed at SunPro, a Sun Microsystems, Inc. business.
@ Permission to use, copy, modify, and distribute this
@ software is freely granted, provided that this notice
@ is preserved.
@ ====================================================

#include <machine/cpu-features.h>
#include <machine/asm.h>

#define vmov_f64 fconstd

ENTRY(sin)
    push            {r4, r6, r7, lr}
    vmov            d0, r0, r1
    mov             r2, r0
    mov             r3, r1
    movw            r1, #0x21fb
    movt            r1, #0x3fe9
    mov             r4, r3
    bic             r3, r3, #0x80000000
    sub             sp, sp, #48
    cmp             r3, r1
    bgt             .Lxgtpio4
    cmp             r3, #0x3e400000
    bge             .Lxnottiny
    vcvt.s32.f64    s15, d0
    vmov            r3, s15
    cmp             r3, #0
    bne             .Lxnottiny
.Lleave_sin:
    vmov            r0, r1, d0
    add             sp, sp, #48
    pop             {r4, r6, r7, pc}
.Lxgtpio4:
    movw            r2, #0xffff
    movt            r2, #0x7fef
    cmp             r3, r2
    bgt             .LxisNaN
    movw            r0, #0xd97b
    movt            r0, #0x4002
    cmp             r3, r0
    movw            r2, #0x21fb
    bgt             .Lxge3pio4
    cmp             r4, #0
    movt            r2, #0x3ff9
    ble             .Lsmallxisnegative
    vldr            d16, .Lpio2_1
    cmp             r3, r2
    vsub.f64        d16, d0, d16
    beq             .Lxnearpio2
    vldr            d17, .Lpio2_1t
.Lfinalizesmallxremainder:
    vsub.f64        d0, d16, d17
    vsub.f64        d16, d16, d0
    vstr            d0, [sp, #8]
    vsub.f64        d1, d16, d17
    vstr            d1, [sp, #16]
.Lnmod3is1:
    bl              __kernel_cos
    b               .Lleave_sin
.Lxnottiny:
    vmov.i64        d1, #0
    mov             r0, #0
    bl              __kernel_sin
    b               .Lleave_sin
.LxisNaN:
    vsub.f64        d0, d0, d0
    b               .Lleave_sin
.Lxge3pio4:
    movt            r2, #0x4139
    cmp             r3, r2
    bgt             .Lxgigantic
    vmov_f64        d3, #0x60
    vldr            d2, .Linvpio2
    vldr            d18, .Lpio2_1
    vabs.f64        d16, d0
    vmla.f64        d3, d16, d2
    vcvt.s32.f64    s3, d3
    vcvt.f64.s32    d17, s3
    vmov            r0, s3
    cmp             r0, #31
    vmls.f64        d16, d17, d18
    vldr            d18, .Lpio2_1t
    vmul.f64        d18, d17, d18
    bgt             .Lcomputeremainder
    ldr             r2, .Lnpio2_hw_ptr
    sub             lr, r0, #1
.LPICnpio2_hw0:
    add             r12, pc, r2
    ldr             r1, [r12, lr, lsl #2]
    cmp             r3, r1
    beq             .Lcomputeremainder
.Lfinishthirditeration:
    vsub.f64        d0, d16, d18
    vstr            d0, [sp, #8]
.Lfinishcomputingremainder:
    vsub.f64        d16, d16, d0
    cmp             r4, #0
    vsub.f64        d1, d16, d18
    vstr            d1, [sp, #16]
    blt             .Lhandlenegativex
.Lselectregion:
    and             r0, r0, #3
    cmp             r0, #1
    beq             .Lnmod3is1
    cmp             r0, #2
    beq             .Lnmod3is2
    cmp             r0, #0
    bne             .Lnmod3is0
    mov             r0, #1
    bl              __kernel_sin
    b               .Lleave_sin
.Lxgigantic:
    asr             r2, r3, #20
    vmov            r6, r7, d0
    sub             r2, r2, #1040
    mov             r0, r6
    sub             r2, r2, #6
    vldr            d16, .Ltwo24
    sub             r1, r3, r2, lsl #20
    vmov            d18, r0, r1
    vcvt.s32.f64    s15, d18
    add             r1, sp, #48
    mov             r3, #3
    vcvt.f64.s32    d17, s15
    vsub.f64        d18, d18, d17
    vstr            d17, [sp, #24]
    vmul.f64        d18, d18, d16
    vcvt.s32.f64    s15, d18
    vcvt.f64.s32    d17, s15
    vsub.f64        d18, d18, d17
    vstr            d17, [sp, #32]
    vmul.f64        d16, d18, d16
    fcmpzd          d16
    vstmdb          r1!, {d16}
    vmrs            APSR_nzcv, fpscr
    bne             .Lprocessnonzeroterm
.Lskipzeroterms:
    vldmdb          r1!, {d16}
    sub             r3, r3, #1
    fcmpzd          d16
    vmrs            APSR_nzcv, fpscr
    beq             .Lskipzeroterms
.Lprocessnonzeroterm:
    ldr             r12, .Ltwo_over_pi_ptr
    add             r0, sp, #24
    add             r1, sp, #8
.LPICtwo_over_pi0:
    add             lr, pc, r12
    mov             r12, #2
    str             lr, [sp, #4]
    str             r12, [sp]
    bl              __kernel_rem_pio2
    cmp             r4, #0
    vldr            d0, [sp, #8]
    blt             .Lhandlenegativexalso
    vldr            d1, [sp, #16]
    b               .Lselectregion
.Lxnearpio2:
    vldr            d17, .Lpio2_2
    vsub.f64        d16, d16, d17
    vldr            d17, .Lpio2_2t
    b               .Lfinalizesmallxremainder
.Lsmallxisnegative:
    vldr            d1, .Lpio2_1
    cmp             r3, r2
    vadd.f64        d16, d0, d1
    beq             .Lxnearnegpio2
    vldr            d17, .Lpio2_1t
.Lfinalizesmallnegxremainder:
    vadd.f64        d0, d16, d17
    vsub.f64        d16, d16, d0
    vstr            d0, [sp, #8]
    vadd.f64        d1, d16, d17
    vstr            d1, [sp, #16]
.Lnmod3is0:
    bl              __kernel_cos
    vneg.f64        d0, d0
    b               .Lleave_sin
.Lnmod3is2:
    mov             r0, #1
    bl              __kernel_sin
    vneg.f64        d0, d0
    b               .Lleave_sin
.Lcomputeremainder:
    vsub.f64        d0, d16, d18
    asr             r1, r3, #20
    vmov            r2, r3, d0
    ubfx            r3, r3, #20, #11
    rsb             r3, r3, r1
    vstr            d0, [sp, #8]
    cmp             r3, #16
    ble             .Lfinishcomputingremainder
    vldr            d18, .Lpio2_2
    vmul.f64        d20, d17, d18
    vsub.f64        d19, d16, d20
    vsub.f64        d16, d16, d19
    vsub.f64        d18, d16, d20
    vldr            d16, .Lpio2_2t
    vnmls.f64       d18, d17, d16
    vsub.f64        d0, d19, d18
    vmov            r2, r3, d0
    ubfx            r3, r3, #20, #11
    rsb             r1, r3, r1
    vstr            d0, [sp, #8]
    cmp             r1, #49
    ble             .Lfinishseconditeration
    vldr            d5, .Lpio2_3
    vmul.f64        d20, d17, d5
    vsub.f64        d16, d19, d20
    vsub.f64        d4, d19, d16
    vldr            d19, .Lpio2_3t
    vsub.f64        d18, d4, d20
    vnmls.f64       d18, d17, d19
    b               .Lfinishthirditeration
.Lhandlenegativex:
    vneg.f64        d0, d0
    rsb             r0, r0, #0
    vneg.f64        d1, d1
    vstr            d0, [sp, #8]
    vstr            d1, [sp, #16]
    b               .Lselectregion
.Lfinishseconditeration:
    vmov            d16, d19
    b               .Lfinishcomputingremainder
.Lxnearnegpio2:
    vldr            d0, .Lpio2_2
    vldr            d17, .Lpio2_2t
    vadd.f64        d16, d16, d0
    b               .Lfinalizesmallnegxremainder
.Lhandlenegativexalso:
    vldr            d6, [sp, #16]
    vneg.f64        d0, d0
    rsb             r0, r0, #0
    vneg.f64        d1, d6
    vstr            d0, [sp, #8]
    vstr            d1, [sp, #16]
    b               .Lselectregion

.align 3
.Lpio2_1:
    .word           0x54400000, 0x3ff921fb
.Lpio2_1t:
    .word           0x1a626331, 0x3dd0b461
.Linvpio2:
    .word           0x6dc9c883, 0x3fe45f30
.Ltwo24:
    .word           0x00000000, 0x41700000
.Lpio2_2:
    .word           0x1a600000, 0x3dd0b461
.Lpio2_2t:
    .word           0x2e037073, 0x3ba3198a
.Lpio2_3:
    .word           0x2e000000, 0x3ba3198a
.Lpio2_3t:
    .word           0x252049c1, 0x397b839a
.Lnpio2_hw_ptr:
    .word           .Lnpio2_hw-(.LPICnpio2_hw0+8)
.Ltwo_over_pi_ptr:
    .word           .Ltwo_over_pi-(.LPICtwo_over_pi0+8)
END(sin)

    .section        .rodata.npio2_hw,"a",%progbits
    .align          2
.Lnpio2_hw = . + 0
    .type           npio2_hw, %object
    .size           npio2_hw, 128
npio2_hw:
    .word           0x3ff921fb
    .word           0x400921fb
    .word           0x4012d97c
    .word           0x401921fb
    .word           0x401f6a7a
    .word           0x4022d97c
    .word           0x4025fdbb
    .word           0x402921fb
    .word           0x402c463a
    .word           0x402f6a7a
    .word           0x4031475c
    .word           0x4032d97c
    .word           0x40346b9c
    .word           0x4035fdbb
    .word           0x40378fdb
    .word           0x403921fb
    .word           0x403ab41b
    .word           0x403c463a
    .word           0x403dd85a
    .word           0x403f6a7a
    .word           0x40407e4c
    .word           0x4041475c
    .word           0x4042106c
    .word           0x4042d97c
    .word           0x4043a28c
    .word           0x40446b9c
    .word           0x404534ac
    .word           0x4045fdbb
    .word           0x4046c6cb
    .word           0x40478fdb
    .word           0x404858eb
    .word           0x404921fb

    .section        .rodata.two_over_pi,"a",%progbits
    .align          2
.Ltwo_over_pi = . + 0
    .type           two_over_pi, %object
    .size           two_over_pi, 264
two_over_pi:
    .word           0x00a2f983
    .word           0x006e4e44
    .word           0x001529fc
    .word           0x002757d1
    .word           0x00f534dd
    .word           0x00c0db62
    .word           0x0095993c
    .word           0x00439041
    .word           0x00fe5163
    .word           0x00abdebb
    .word           0x00c561b7
    .word           0x00246e3a
    .word           0x00424dd2
    .word           0x00e00649
    .word           0x002eea09
    .word           0x00d1921c
    .word           0x00fe1deb
    .word           0x001cb129
    .word           0x00a73ee8
    .word           0x008235f5
    .word           0x002ebb44
    .word           0x0084e99c
    .word           0x007026b4
    .word           0x005f7e41
    .word           0x003991d6
    .word           0x00398353
    .word           0x0039f49c
    .word           0x00845f8b
    .word           0x00bdf928
    .word           0x003b1ff8
    .word           0x0097ffde
    .word           0x0005980f
    .word           0x00ef2f11
    .word           0x008b5a0a
    .word           0x006d1f6d
    .word           0x00367ecf
    .word           0x0027cb09
    .word           0x00b74f46
    .word           0x003f669e
    .word           0x005fea2d
    .word           0x007527ba
    .word           0x00c7ebe5
    .word           0x00f17b3d
    .word           0x000739f7
    .word           0x008a5292
    .word           0x00ea6bfb
    .word           0x005fb11f
    .word           0x008d5d08
    .word           0x00560330
    .word           0x0046fc7b
    .word           0x006babf0
    .word           0x00cfbc20
    .word           0x009af436
    .word           0x001da9e3
    .word           0x0091615e
    .word           0x00e61b08
    .word           0x00659985
    .word           0x005f14a0
    .word           0x0068408d
    .word           0x00ffd880
    .word           0x004d7327
    .word           0x00310606
    .word           0x001556ca
    .word           0x0073a8c9
    .word           0x0060e27b
    .word           0x00c08c6b
