/*
* Copyright (c) 2026. All rights reserved
*
* written by KonataBSD
*
* This code is licensed under the GNU GPL v2 or later
*/

.equ STACK_SIZE_SHIFT, 14

.section ".text.rom", "ax"
.p2align 12
.globl _start
.type _start, %function
_start:

mov x0, #0
sev

// 4K boundary check
adr x0, _start
ands x0, x0, #0xFFF
b.eq 2f

b .Lwait

2:
mrs x0, mpidr_el1
ands x0, x0, #0xF
cbz x0, cpu0_init

.Lsecondary_park:

ldr x0, =0xFFFCFF00
mov x1, #1
str x1, [x0]
dsb st
sev

msr spsel, x0

ldr x0, =0xFFFFFFF0 // ill fix later so cpu0 wouldnt overwrite other core stack
mrs x1, mpidr_el1
and x1, x1, #0xF
lsl x2, x1, #STACK_SIZE_SHIFT

sub x0, x0, x2
bic x0, x0, #0xF
mov sp, x0
isb

1: wfe
dmb sy
ldr w0, =0xFFFCFF00 // scratchpad
cbz x0, secondary_init
b 1b

cpu0_init:

mov x0, #1
msr spsel, x0

ldr x0, =0xFFFFFFF8
mov sp, x0
isb

msr daifset, #0xF

ldr x0, =__bss_start
ldr x1, =__bss_end


.Lloop_bss:

cmp x0, x1
b.ge .Lloop_bss_done
str xzr, [x0], #8

b .Lloop_bss

.Lloop_bss_done:

tlbi alle3is
ic ialluis
dsb sy

mov x0, #(1 << 1) // SA
bic x0, x0, #(1 << 0) // M
bic x0, x0, #(1 << 2) // C
orr x0, x0, #(1 << 12) // I
bic x0, x0, #(1 << 25) // EE
orr x0, x0, #(1 << 22) // EIS
orr x0, x0, #(1 << 11) // EOS
msr sctlr_el3, x0
isb

msr daifclr, #0x8

mrs x0, midr_el1
ubfx x1, x0, #4, #12
ubfx x2, x0, #24, #8
cmp x1, #0xD03
b.eq .Lis_a53 // if a53
b .Lwait // if not a53

.Lis_a53:
cmp x2, #0x41
b.eq 1f
b .Lwait

1:
msr cptr_el3, xzr

mov x0, #(1 << 22)
msr s3_1_c11_c0_2, x0
mov x0, #(1 << 6)
msr s3_1_c15_c2_1, x0
isb

b main_boot

.Lwait: wfi
b .Lwait

secondary_init:
wfe
b .
