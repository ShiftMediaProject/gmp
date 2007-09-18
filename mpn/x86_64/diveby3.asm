dnl  AMD64 mpn_divexact_by3 -- mpn division by 3, expecting no remainder.

dnl  Copyright 2000, 2002, 2004, 2005 Free Software Foundation, Inc.
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


C	    cycles/limb
C K8:		 5
C P4:		20.5
C P6-15:	 8

C INPUT PARAMETERS
define(`rp',	`%rdi')
define(`up',	`%rsi')
define(`n',	`%rdx')
define(`cy',	`%rcx')

ASM_START()
PROLOGUE(mpn_divexact_by3c)
	push	%rbx
	movq	(up), %rax
	movq	$0x5555555555555555, %r10
	leaq	(%r10,%r10), %r11		C 0xAAAAAAAAAAAAAAAA
	mov	%rcx, %r8
	imulq	%r10, %r8
	leaq	1(%r10,%r10), %r9		C 0xAAAAAAAAAAAAAAAB
	leaq	(up,n,8), up
	leaq	-8(rp,n,8), rp
	negq	n

	imulq	%r9, %rax

	incq	n
	jz,pn	.Lendo

	.align 16
.Loop:	movq	(up,n,8), %rbx
	imulq	%r9, %rbx
	addq	%r8, %rax
	cmpq	%rcx, -8(up,n,8)		C highly unlikely carry!!!
	movq	%rax, (rp,n,8)
	jc,pn	.Lc0
.Lb0:	xor	%r8, %r8
	cmpq	%rax, %r10
	cmovc	%r10, %r8
	setc	%cl
	cmpq	%rax, %r11
	cmovc	%r11, %r8
	adcq	$0, %rcx

	incq	n
	jz,pn	.Lende

	movq	(up,n,8), %rax
	imulq	%r9, %rax
	addq	%r8, %rbx
	cmpq	%rcx, -8(up,n,8)
	movq	%rbx, (rp,n,8)
	jc,pn	.Lc1
.Lb1:	xor	%r8, %r8
	cmpq	%rbx, %r10
	cmovc	%r10, %r8
	setc	%cl
	cmpq	%rbx, %r11
	cmovc	%r11, %r8
	adcq	$0, %rcx

	incq	n
	jnz,pt	.Loop

.Lendo:	addq	%r8, %rax
	cmpq	%rcx, -8(up,n,8)
	movq	%rax, (rp,n,8)
	jc,pn	.Lc2
.Lb2:	xor	%r8, %r8
	cmpq	%rax, %r10
	cmovc	%r10, %r8
	setc	%cl
	cmpq	%rax, %r11
	cmovc	%r11, %r8
	adcq	$0, %rcx
	movq	%rcx, %rax
	pop	%rbx
	ret

.Lende:	addq	%r8, %rbx
	cmpq	%rcx, -8(up,n,8)
	movq	%rbx, (rp,n,8)
	jc,pn	.Lc3
.Lb3:	xor	%r8, %r8
	cmpq	%rbx, %r10
	cmovc	%r10, %r8
	setc	%cl
	cmpq	%rbx, %r11
	cmovc	%r11, %r8
	adcq	$0, %rcx
	movq	%rcx, %rax
	pop	%rbx
	ret

.Lc0:	incq	%rax
	jmp .Lb0
.Lc1:	incq	%rbx
	jmp .Lb1
.Lc2:	incq	%rax
	jmp .Lb2
.Lc3:	incq	%rbx
	jmp .Lb3
EPILOGUE()
