dnl  x86-64 mpn_add_n/mpn_sub_n optimized for Pentium 4.

dnl  Copyright 2007 Free Software Foundation, Inc.

dnl  This file is part of the GNU MP Library.

dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU Lesser General Public License as published
dnl  by the Free Software Foundation; either version 2.1 of the License, or (at
dnl  your option) any later version.

dnl  The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
dnl  License for more details.

dnl  You should have received a copy of the GNU Lesser General Public License
dnl  along with the GNU MP Library; see the file COPYING.LIB.  If not, write
dnl  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
dnl  Boston, MA 02110-1301, USA.

include(`../config.m4')


C		    cycles/limb
C K8:			2.8
C Prescott/Nocona:	4.0
C P6-15:		2.55

C INPUT PARAMETERS
define(`rp',	`%rdi')
define(`up',	`%rsi')
define(`vp',	`%rdx')
define(`n',	`%rcx')
define(`cy',	`%r8')

ifdef(`OPERATION_add_n', `
	define(ADDSUB,	      add)
	define(func,	      mpn_add_n)
	define(func_nc,	      mpn_add_nc)')
ifdef(`OPERATION_sub_n', `
	define(ADDSUB,	      sub)
	define(func,	      mpn_sub_n)
	define(func_nc,	      mpn_sub_nc)')

MULFUNC_PROLOGUE(mpn_add_n mpn_add_nc mpn_sub_n mpn_sub_nc)

ASM_START()

	TEXT
	ALIGN(16)

PROLOGUE(func_nc)
	jmp	.Lent
EPILOGUE()

PROLOGUE(func)
	xor	%r8, %r8
.Lent:
	push	%rbx
	push	%r12
	push	%r13
	push	%r14
	push	%r15

	mov	%ecx, %eax
	and	$3, %eax
	jne	.Ln00		C n = 0, 4, 8, ...
	mov	%r8, %rbx
	mov	(up), %r8
	mov	(vp), %r9
	mov	8(up), %r10
	ADDSUB	%r9, %r8
	mov	8(vp), %r11
	setc	%al
	lea	-16(rp), rp
	sub	$4, n
	jmp	.LL00

.Ln00:	cmp	$2, %eax
	jnc	.Ln01		C n = 1, 5, 9, ...
	mov	(up), %r14
	mov	(vp), %r15
	mov	%r8, %rax
	xor	%ebx, %ebx
	sub	$4, n
	jnc	.Lgt1
	ADDSUB	%r15, %r14
	setc	%bl
	ADDSUB	%rax, %r14
	adc	$0, %ebx
	mov	%r14, (rp)
	jmp	.Lret1
.Lgt1:	mov	8(up), %r8
	ADDSUB	%r15, %r14
	mov	8(vp), %r9
	setc	%bl
	lea	-8(rp), rp
	lea	8(up), up
	lea	8(vp), vp
	jmp	.LL01

.Ln01:	jne	.Ln10		C n = 2, 6, 10, ...
	mov	(up), %r12
	mov	(vp), %r13
	mov	%r8, %rbx
	mov	8(up), %r14
	ADDSUB	%r13, %r12
	mov	8(vp), %r15
	setc	%al
	ADDSUB	%rbx, %r12
	jc	.Lc3a
.Lrc3a:	lea	16(up), up
	lea	16(vp), vp
	sub	$4, n
	jnc	.Loop
	jmp	.Lend

.Ln10:	mov	(up), %r10	C n = 3, 7, 11, ...
	mov	(vp), %r11
	mov	%r8, %rax
	xor	%ebx, %ebx
	mov	8(up), %r12
	ADDSUB	%r11, %r10
	mov	8(vp), %r13
	setc	%bl
	lea	-24(rp), rp
	lea	-8(up), up
	lea	-8(vp), vp
	jmp	.LL11

.Lc3a:	mov	$1, %al
	jmp	.Lrc3a
.Lc0:	mov	$1, %bl
	jmp	.Lrc0
.Lc1:	mov	$1, %al
	jmp	.Lrc1
.Lc2:	mov	$1, %bl
	jmp	.Lrc2
.Lc3:	mov	$1, %al
	jmp	.Lrc3

	ALIGN(16)
.Loop:	mov	(up), %r8	C not on critical path
	ADDSUB	%r15, %r14	C not on critical path
	mov	(vp), %r9	C not on critical path
	setc	%bl		C save carry out
	mov	%r12, (rp)
.LL01:	ADDSUB	%rax, %r14	C apply previous carry out
	jc	.Lc0		C jump if ripple
.Lrc0:	mov	8(up), %r10
	ADDSUB	%r9, %r8
	mov	8(vp), %r11
	setc	%al
	mov	%r14, 8(rp)
.LL00:	ADDSUB	%rbx, %r8
	jc	.Lc1
.Lrc1:	mov	16(up), %r12
	ADDSUB	%r11, %r10
	mov	16(vp), %r13
	setc	%bl
	mov	%r8, 16(rp)
.LL11:	ADDSUB	%rax, %r10
	jc	.Lc2
.Lrc2:	mov	24(up), %r14
	ADDSUB	%r13, %r12
	lea	32(up), up
	mov	24(vp), %r15
	lea	32(vp), vp
	setc	%al
	mov	%r10, 24(rp)
.LL10:	ADDSUB	%rbx, %r12
	jc	.Lc3
.Lrc3:	lea	32(rp), rp
	sub	$4, n
	jnc	.Loop

.Lend:	ADDSUB	%r15, %r14
	setc	%bl
	mov	%r12, (rp)
	ADDSUB	%rax, %r14
	jc	.Lc0c
.Lrc0c:	mov	%r14, 8(rp)

.Lret1:	mov	%ebx, %eax
	pop	%r15
	pop	%r14
	pop	%r13
	pop	%r12
	pop	%rbx
	ret

.Lc0c:	mov	$1, %bl
	jmp	.Lrc0c

EPILOGUE()
