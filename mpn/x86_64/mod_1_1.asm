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

ASM_START()
	TEXT
	ALIGN(16)
PROLOGUE(mpn_mod_1_1p)
	push	%rbp
	push	%rbx
	mov	%rdx, %rbp
	mov	%rcx, %rbx
	mov	16(%rcx), %r10
	mov	24(%rcx), %r11

C FIXME: See comment in generic/mod_1_1.c.
	mov	-8(%rdi,%rsi,8), %rax
	mul	%r10
	mov	-16(%rdi,%rsi,8), %r9
	xor	R32(%r8), R32(%r8)
	add	%r9, %rax
	adc	%rdx, %r8

	sub	$3, %rsi
	js	L(2)
	ALIGN(16)
L(top):	mul	%r10			C 1  15
	xor	R32(%r9), R32(%r9)	C
	mov	(%rdi,%rsi,8), %rcx	C
	add	%rax, %rcx		C 5  19
	mov	%r8, %rax		C 0  16
	adc	%rdx, %r9		C 6  20
	mul	%r11			C 3  17
	add	%rcx, %rax		C 7  21
	nop
	adc	%rdx, %r9		C 8  22
	sub	$1, %rsi		C
	js	L(end)			C

	mul	%r10			C 8  22
	xor	R32(%r8), R32(%r8)	C
	mov	(%rdi,%rsi,8), %rcx	C
	add	%rax, %rcx		C 12 26
	mov	%r9, %rax		C 9  23
	adc	%rdx, %r8		C 13 27
	mul	%r11			C 10 24
	add	%rcx, %rax		C 14 28
	nop
	adc	%rdx, %r8		C 15 29
	sub	$1, %rsi		C
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
	mov	%r12, -8(%rsp)
	sub	$24, %rsp
	mov	%rdi, %r12
	bsr	%rsi, %rax
	mov	R32(%rax), R32(%rbx)
	xor	$63, R32(%rbx)
	mov	%rsi, %rbp
	mov	R32(%rbx), R32(%rcx)
	sal	%cl, %rbp
	mov	%rbp, %rdi
	CALL(	mpn_invert_limb)
	mov	%rax, %rdi
	test	R32(%rbx), R32(%rbx)
	jne	L(11)
	mov	%rbp, %rsi
	neg	%rsi
	jmp	L(12)
L(11):
	xor	R32(%rcx), R32(%rcx)
	sub	R32(%rbx), R32(%rcx)
	mov	%rax, %rsi
	shr	%cl, %rsi
	mov	$1, R32(%rax)
	mov	R32(%rbx), R32(%rcx)
	sal	%cl, %rax
	or	%rax, %rsi
	mov	%rbp, %rax
	neg	%rax
	imul	%rax, %rsi
L(12):
	mov	%rsi, %rax
	mul	%rdi
	mov	%rax, %rcx
	add	%rsi, %rdx
	not	%rdx
	imul	%rbp, %rdx
	lea	(%rdx,%rbp), %rax
	cmp	%rdx, %rcx
	cmovb	%rax, %rdx
	mov	%rdi, (%r12)
	mov	R32(%rbx), 8(%r12)
	mov	R32(%rbx), R32(%rcx)
	shr	%cl, %rsi
	mov	%rsi, 16(%r12)
	shr	%cl, %rdx
	mov	%rdx, 24(%r12)
	mov	(%rsp), %rbx
	mov	8(%rsp), %rbp
	mov	16(%rsp), %r12
	add	$24, %rsp
	ret
EPILOGUE()
ASM_END()
