dnl  AMD64 mpn_redc_1 -- Montgomery reduction with a one-limb modular inverse.

dnl  Copyright 2004 Free Software Foundation, Inc.
dnl
dnl  This file is part of the GNU MP Library.
dnl
dnl  The GNU MP Library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Lesser General Public License as
dnl  published by the Free Software Foundation; either version 3 of the
dnl  License, or (at your option) any later version.
dnl
dnl  The GNU MP Library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Lesser General Public License for more details.
dnl
dnl  You should have received a copy of the GNU Lesser General Public License
dnl  along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.

include(`../config.m4')


C K8: 3.25 cycles/limb (4.0 cycles/limb at 40/20 limbs).

C TODO
C  * Do feed-in like in mul_basecase.asm, i.e., jump into mid-loop.
C  * Generate new limb-multiplier earlier, in one outer loop iteration earlier.
C  * Don't reserve rdi (aka rp) over inner loops.
C  * See mul_basecase.asm for additional items.

C INPUT PARAMETERS
define(`rp',	`%rdi')
define(`up',	`%rsi')
define(`param_mp',`%rdx')
define(`n',	`%rcx')
define(`invm',	`%r8')

ifdef(`PIC',
  `define(`CALL',`call	$1@PLT')',
  `define(`CALL',`call	$1')')

define(`mp',`%rbx')

ASM_START()
PROLOGUE(mpn_redc_1)
	pushq	%rbp
	pushq	%rbx
	pushq	%r12
	pushq	n
	leaq	(param_mp,n,8), mp	C mp += n
	leaq	(up,n,8), up		C up += n
	movq	n, %rbp
	xorl	%r10d, %r10d		C keep r10 zero
	negq	%rbp			C rbp = -n
	movl	%ecx, %eax
	andl	$3, %eax
	je	.Lb00
	cmpl	$1, %eax
	je	.Lb01
	cmpl	$2, %eax
	je	.Lb10
	nop

.Lb11:	movq	(up,%rbp,8), %r12	C load up[0]
	imulq	invm, %r12
	leaq	6(%rbp), %r11		C inner-loop count

	movq	(mp,%rbp,8), %rax
	mulq	%r12
	addq	(up,%rbp,8), %rax
	adcq	%r10, %rdx
	movq	%rdx, %r9
	movq	-40(mp,%r11,8), %rax
	mulq	%r12
	addq	-40(up,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -40(up,%r11,8)	C next iteration up[0]
	adcq	%rdx, %r9
	movq	-32(mp,%r11,8), %rax
	mulq	%r12
	addq	-32(up,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -32(up,%r11,8)
	adcq	%rdx, %r9

	testq	%r11, %r11
	jns	.Lc11			C FIXME: Special exit for un=vn=1?

	ALIGN(8)
.Li11:	movq	-24(mp,%r11,8), %rax
	mulq	%r12
	addq	-24(up,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%rax, -24(up,%r11,8)
	movq	%r10, %r9
	adcq	%rdx, %r9
	movq	-16(mp,%r11,8), %rax
	mulq	%r12
	addq	-16(up,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -16(up,%r11,8)
	adcq	%rdx, %r9
	movq	-8(mp,%r11,8), %rax
	mulq	%r12
	addq	-8(up,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -8(up,%r11,8)
	adcq	%rdx, %r9
	movq	(mp,%r11,8), %rax
	mulq	%r12
	addq	(up,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, (up,%r11,8)
	adcq	%rdx, %r9
	addq	$4, %r11
	jae	.Li11

.Lc11:	movq	%r9, (up,%rbp,8)
	addq	$8, up
	decq	n
	jne	.Lb11
	jmp	.Lcommon


	ALIGN(8)
.Lb10:	movq	(up,%rbp,8), %r12	C load up[0]
	imulq	invm, %r12
	leaq	5(%rbp), %r11		C inner-loop count

	movq	(mp,%rbp,8), %rax
	mulq	%r12
	addq	(up,%rbp,8), %rax
	adcq	%r10, %rdx
	movq	%rdx, %r9
	movq	-32(mp,%r11,8), %rax
	mulq	%r12
	addq	-32(up,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -32(up,%r11,8)	C next iteration up[0]
	adcq	%rdx, %r9

	testq	%r11, %r11
	jns	.Lc10			C FIXME: Special exit for un=vn=1?

	ALIGN(8)
.Li10:	movq	-24(mp,%r11,8), %rax
	mulq	%r12
	addq	-24(up,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%rax, -24(up,%r11,8)
	movq	%r10, %r9
	adcq	%rdx, %r9
	movq	-16(mp,%r11,8), %rax
	mulq	%r12
	addq	-16(up,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -16(up,%r11,8)
	adcq	%rdx, %r9
	movq	-8(mp,%r11,8), %rax
	mulq	%r12
	addq	-8(up,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -8(up,%r11,8)
	adcq	%rdx, %r9
	movq	(mp,%r11,8), %rax
	mulq	%r12
	addq	(up,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, (up,%r11,8)
	adcq	%rdx, %r9
	addq	$4, %r11
	jae	.Li10

.Lc10:	movq	%r9, (up,%rbp,8)
	addq	$8, up
	decq	n
	jne	.Lb10
	jmp	.Lcommon


	ALIGN(8)
.Lb00:	movq	(up,%rbp,8), %r12	C load up[0]
	imulq	invm, %r12
	leaq	3(%rbp), %r11		C inner-loop count
	xorl	%r9d, %r9d		C clear carry limb

	ALIGN(8)
.Li00:	movq	-24(mp,%r11,8), %rax
	mulq	%r12
	addq	-24(up,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%rax, -24(up,%r11,8)
	movq	%r10, %r9
	adcq	%rdx, %r9
	movq	-16(mp,%r11,8), %rax
	mulq	%r12
	addq	-16(up,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -16(up,%r11,8)
	adcq	%rdx, %r9
	movq	-8(mp,%r11,8), %rax
	mulq	%r12
	addq	-8(up,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -8(up,%r11,8)
	adcq	%rdx, %r9
	movq	(mp,%r11,8), %rax
	mulq	%r12
	addq	(up,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, (up,%r11,8)
	adcq	%rdx, %r9
	addq	$4, %r11
	jae	.Li00

.Lc00:	movq	%r9, (up,%rbp,8)
	addq	$8, up
	decq	n
	jne	.Lb00
	jmp	.Lcommon


	ALIGN(8)
.Lb01:	movq	(up,%rbp,8), %r12	C load up[0]
	imulq	invm, %r12
	leaq	4(%rbp), %r11		C inner-loop count

	movq	(mp,%rbp,8), %rax
	mulq	%r12
	addq	(up,%rbp,8), %rax
	adcq	%r10, %rdx
	movq	%rdx, %r9

	testq	%r11, %r11
	js	.Li01

	addq	(up), %rdx
	jnc	1f
	subq	-8(mp), %rdx
1:	movq	%rdx, (rp)
	jmp	.Lret

	ALIGN(8)
.Li01:	movq	-24(mp,%r11,8), %rax
	mulq	%r12
	addq	-24(up,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%rax, -24(up,%r11,8)
	movq	%r10, %r9
	adcq	%rdx, %r9
	movq	-16(mp,%r11,8), %rax
	mulq	%r12
	addq	-16(up,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -16(up,%r11,8)
	adcq	%rdx, %r9
	movq	-8(mp,%r11,8), %rax
	mulq	%r12
	addq	-8(up,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, -8(up,%r11,8)
	adcq	%rdx, %r9
	movq	(mp,%r11,8), %rax
	mulq	%r12
	addq	(up,%r11,8), %rax
	adcq	%r10, %rdx
	addq	%r9, %rax
	movq	%r10, %r9
	movq	%rax, (up,%r11,8)
	adcq	%rdx, %r9
	addq	$4, %r11
	jae	.Li01

.Lc01:	movq	%r9, (up,%rbp,8)
	addq	$8, up
	decq	n
	jne	.Lb01

.Lcommon:
	leaq	(mp,%rbp,8), mp		C restore entry mp

C   cy = mpn_add_n (rp, up, up - n, n);
C		    rdi rsi  rdx    rcx
	leaq	(up,%rbp,8), up		C up -= n
	leaq	(up,%rbp,8), %rdx	C rdx = up - n [up entry value]
	movq	rp, %rbp		C preserve rp over first call
	movq	(%rsp), %rcx		C pass entry n
	movq	rp, %rdi
	CALL(`	mpn_add_n')
	testl	%eax, %eax
	jz	.Lret

C     mpn_sub_n (rp, rp, mp, n);
C		 rdi rsi rdx rcx
	movq	%rbp, %rdi
	movq	%rbp, %rsi
	movq	mp, %rdx
	movq	(%rsp), %rcx		C pass entry n
	CALL(`	mpn_sub_n')

.Lret:	popq	n			C just increment rsp
	popq	%r12
	popq	%rbx
	popq	%rbp
	ret
EPILOGUE()
