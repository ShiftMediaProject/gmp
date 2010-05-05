dnl  AMD64 mpn_mod_1_1p

dnl  Contributed to the GNU project by Torbjorn Granlund.

dnl  Copyright 2009, 2010 Free Software Foundation, Inc.

dnl  This file is part of the GNU MP Library.

dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU Lesser General Public License as published
dnl  by the Free Software Foundation; either version 3 of the License, or (at
dnl  your option) any later version.

dnl  The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
dnl  License for more details.

dnl  You should have received a copy of the GNU Lesser General Public License
dnl  along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.

include(`../config.m4')

C	     cycles/limb
C AMD K8,K9	 7
C AMD K10	 7
C Intel P4	27
C Intel core2	14
C Intel corei	12.5
C Intel atom	37
C VIA nano	15

define(`B1modb', `%r10')
define(`B2modb', `%r11')
define(`ap',     `%rdi')
define(`n',      `%rsi')

ASM_START()
	TEXT
	ALIGN(16)
PROLOGUE(mpn_mod_1_1p)
	push	%rbp
	push	%rbx
	mov	%rdx, %rbp
	mov	%rcx, %rbx
	mov	16(%rcx), B1modb
	mov	24(%rcx), B2modb

C FIXME: See comment in generic/mod_1_1.c.
	mov	-8(ap,n,8), %rax
	mul	B1modb
	mov	-16(ap,n,8), %r9
	xor	R32(%r8), R32(%r8)
	add	%r9, %rax
	adc	%rdx, %r8

	sub	$3, n
	js	L(2)
	ALIGN(16)
L(top):	mul	B1modb			C 1  15
	xor	R32(%r9), R32(%r9)	C
	mov	(ap,n,8), %rcx	C
	add	%rax, %rcx		C 5  19
	mov	%r8, %rax		C 0  16
	adc	%rdx, %r9		C 6  20
	mul	B2modb			C 3  17
	add	%rcx, %rax		C 7  21
	nop
	adc	%rdx, %r9		C 8  22
	sub	$1, n			C
	js	L(end)			C

	mul	B1modb			C 8  22
	xor	R32(%r8), R32(%r8)	C
	mov	(ap,n,8), %rcx	C
	add	%rax, %rcx		C 12 26
	mov	%r9, %rax		C 9  23
	adc	%rdx, %r8		C 13 27
	mul	B2modb			C 10 24
	add	%rcx, %rax		C 14 28
	nop
	adc	%rdx, %r8		C 15 29
	sub	$1, n			C
	jns	L(top)			C

	jmp	L(2)
L(end):	mov	%r9, %r8
L(2):
	mov	%rax, %r9
	mov	(%rbx), %rsi
	mov	8(%rbx), R32(%rdi)
	test	R32(%rdi), R32(%rdi)
	je	L(4)
	mov	%r8, %rax
	mov	R32(%rdi), R32(%rcx)
	sal	%cl, %rax
	mov	%r9, %r8
	sal	%cl, %r9
	neg	R32(%rcx)
	shr	%cl, %r8
	or	%rax, %r8
L(4):
	mov	%r8, %rax
	sub	%rbp, %r8
	cmovc	%rax, %r8
	mov	%r8, %rax
	mul	%rsi
	mov	%rax, %rbx
	mov	%r9, %rsi
	lea	1(%r8), %rax
	add	%rsi, %rbx
	adc	%rax, %rdx
	mov	%rbp, %rax
	imul	%rdx, %rax
	sub	%rax, %rsi
	mov	%rsi, %rax
	lea	(%rsi,%rbp), %rdx
	cmp	%rsi, %rbx
	cmovb	%rdx, %rax
	mov	%rax, %rdx
	sub	%rbp, %rdx
	cmp	%rbp, %rax
	cmovae	%rdx, %rax
	mov	R32(%rdi), R32(%rcx)
	shr	%cl, %rax
	pop	%rbx
	pop	%rbp
	ret
EPILOGUE()

	ALIGN(16)
PROLOGUE(mpn_mod_1_1p_cps)
	mov	%rbx, -24(%rsp)
	mov	%rbp, -16(%rsp)
	mov	%rsi, %rbp
	bsr	%rsi,%rax
	mov	%eax, %ebx
	mov	%r12, -8(%rsp)
	sub	$24, %rsp
	xor	$63, %ebx
	mov	%rdi, %r12
	mov	%ebx, %ecx
	sal	%cl, %rbp
	mov	%rbp, %rdi
	CALL(	mpn_invert_limb)
	mov	%rbp, %r8
	mov	%rax, %r9
	mov	%rax, (%r12)
	neg	%r8
	test	%ebx, %ebx
	je	L(c0)
	xor	%ecx, %ecx
	mov	%rax, %rdx
	mov	$1, %eax
	sub	%ebx, %ecx
	shr	%cl, %rdx
	mov	%ebx, %ecx
	sal	%cl, %rax
	or	%rax, %rdx
	imul	%rdx, %r8
L(c0):	mov	%r8, %rax
	mul	%r9
	lea	1(%rdx,%r8), %rdx
	mov	%rax, %rcx
	neg	%rdx
	imul	%rbp, %rdx
	lea	(%rdx,%rbp), %rax
	cmp	%rdx, %rcx
	mov	%ebx, %ecx
	mov	8(%rsp), %rbp
	cmovb	%rax, %rdx
	shr	%cl, %r8
	shr	%cl, %rdx
	mov	%rbx, 8(%r12)
	mov	%r8, 16(%r12)
	mov	%rdx, 24(%r12)
	mov	(%rsp), %rbx
	mov	16(%rsp), %r12
	add	$24, %rsp
	ret
EPILOGUE()
ASM_END()
