dnl  AMD64 mpn_mod_1s_4p

dnl  Contributed to the GNU project by Torbjorn Granlund.

dnl  Copyright 2009 Free Software Foundation, Inc.

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
C K8,K9:	 3.0
C K10:		 3.0
C P4:		14.5
C P6-15 (Core2): 5.0
C P6-28 (Atom):	25.0

ASM_START()
	TEXT
	ALIGN(16)
PROLOGUE(mpn_mod_1s_4p)
	push	%r15
	push	%r13
	push	%r12
	push	%rbp
	push	%rbx
	
	mov	%rdx, -16(%rsp)
	mov	%rcx, %r15
	mov	16(%rcx), %r11
	mov	24(%rcx), %rbx
	mov	32(%rcx), %rbp
	mov	40(%rcx), %r13
	mov	48(%rcx), %r12
	mov	%esi, %edx
	and	$3, %edx
	je	L(b0)
	cmp	$2, %edx
	jc	L(b1)
	je	L(b2)

L(b3):	lea	-24(%rdi,%rsi,8), %rdi
	mov	8(%rdi), %rax
	mul	%r11
	mov	%rax, %r9
	mov	%rdx, %r8
	add	(%rdi), %r9
	adc	$0, %r8
	mov	16(%rdi), %rax
	mul	%rbx
	jmp	L(m0)

	ALIGN(8)
L(b0):	lea	-32(%rdi,%rsi,8), %rdi
	mov	8(%rdi), %rax
	mul	%r11
	mov	%rax, %r9
	mov	%rdx, %r8
	add	(%rdi), %r9
	adc	$0, %r8
	mov	16(%rdi), %rax
	mul	%rbx
	add	%rax, %r9
	adc	%rdx, %r8
	mov	24(%rdi), %rax
	mul	%rbp
	jmp	L(m0)

	ALIGN(8)
L(b1):	lea	-8(%rdi,%rsi,8), %rdi
	xor	%r8d, %r8d
	mov	(%rdi), %r9
	jmp	L(m1)

	ALIGN(8)
L(b2):	lea	-16(%rdi,%rsi,8), %rdi
	mov	8(%rdi), %rax
	mul	%r11
	mov	%rax, %r9
	mov	%rdx, %r8
	add	(%rdi), %r9
	adc	$0, %r8
	jmp	L(m1)

	ALIGN(16)
L(top):	mov	-24(%rdi), %rax
	mov	-32(%rdi), %r10
	mul	%r11
	add	%rax, %r10
	mov	-16(%rdi), %rax
	mov	%rdx, %rcx
	adc	$0, %rcx
	mul	%rbx
	add	%rax, %r10
	mov	-8(%rdi), %rax
	adc	%rdx, %rcx
	sub	$32, %rdi
	mul	%rbp
	add	%rax, %r10
	mov	%r9, %rax
	adc	%rdx, %rcx
	mul	%r13
	add	%rax, %r10
	mov	%r8, %rax
	adc	%rdx, %rcx
	mul	%r12
	mov	%r10, %r9
	mov	%rcx, %r8
L(m0):	add	%rax, %r9
	adc	%rdx, %r8
L(m1):	sub	$4, %rsi
	ja	L(top)

L(end):	mov	8(%r15), %esi
	mov	%r8, %rax
	mul	%r11
	mov	%rax, %r8
	add	%r9, %r8
	adc	$0, %rdx
	mov	$64, %ecx
	sub	%esi, %ecx
	mov	%r8, %rdi
	shr	%cl, %rdi
	mov	%esi, %ecx
	sal	%cl, %rdx
	or	%rdx, %rdi
	mov	%rdi, %rax
	mulq	(%r15)
	mov	%rax, %r9
	mov	%r8, %rax
	sal	%cl, %rax
	inc	%rdi
	mov	%r9, %rcx
	add	%rax, %rcx
	adc	%rdi, %rdx
	imul	-16(%rsp), %rdx
	sub	%rdx, %rax
	mov	-16(%rsp), %rbx
	lea	(%rax,%rbx), %rdx
	cmp	%rax, %rcx
	cmovb	%rdx, %rax
	mov	%rax, %rdx
	sub	%rbx, %rdx
	cmp	%rbx, %rax
	cmovae	%rdx, %rax
	mov	%esi, %ecx
	shr	%cl, %rax
	pop	%rbx
	pop	%rbp
	pop	%r12
	pop	%r13
	pop	%r15
	ret
EPILOGUE()

	ALIGN(16)
PROLOGUE(mpn_mod_1s_4p_cps)
	mov	%rbx, -40(%rsp)
	mov	%rbp, -32(%rsp)
	mov	%r12, -24(%rsp)
	mov	%r13, -16(%rsp)
	mov	%r14, -8(%rsp)
	sub	$40, %rsp
	mov	%rdi, %rbx
	bsr	%rsi, %rax
	mov	%eax, %r12d
	xor	$63, %r12d
	mov	%rsi, %rbp
	mov	%r12d, %ecx
	sal	%cl, %rbp
	mov	%rbp, %rdi
	call	mpn_invert_limb
	mov	%rax, %r14
	mov	$64, %ecx
	sub	%r12d, %ecx
	mov	%rax, %rsi
	shr	%cl, %rsi
	mov	$1, %edx
	mov	%r12d, %ecx
	sal	%cl, %rdx
	mov	%rsi, %rcx
	or	%rdx, %rcx
	mov	%rbp, %rdx
	neg	%rdx
	mov	%rcx, %r13
	imul	%rdx, %r13
	mov	%r13, %rax
	mul	%r14
	lea	1(%rdx,%r13), %rdx
	neg	%rdx
	mov	%rdx, %r9
	imul	%rbp, %r9
	lea	(%r9,%rbp), %rdx
	cmp	%r9, %rax
	cmovb	%rdx, %r9
	mov	%r9, %rax
	mul	%r14
	lea	1(%rdx,%r9), %rdx
	neg	%rdx
	mov	%rdx, %r8
	imul	%rbp, %r8
	lea	(%r8,%rbp), %rdx
	cmp	%r8, %rax
	cmovb	%rdx, %r8
	mov	%r8, %rax
	mul	%r14
	lea	1(%rdx,%r8), %rdx
	neg	%rdx
	mov	%rdx, %r11
	imul	%rbp, %r11
	lea	(%r11,%rbp), %rsi
	cmp	%r11, %rax
	cmovb	%rsi, %r11
	mov	%r11, %rax
	mul	%r14
	lea	1(%rdx,%r11), %rdi
	neg	%rdi
	mov	%rdi, %r10
	imul	%rbp, %r10
	lea	(%r10,%rbp), %rdi
	cmp	%r10, %rax
	cmovb	%rdi, %r10
	mov	%r14, (%rbx)
	movslq	%r12d, %rax
	mov	%rax, 8(%rbx)
	mov	%r13, %rax
	mov	%r12d, %ecx
	shr	%cl, %rax
	mov	%rax, 16(%rbx)
	mov	%r9, %rax
	shr	%cl, %rax
	mov	%rax, 24(%rbx)
	mov	%r8, %rax
	shr	%cl, %rax
	mov	%rax, 32(%rbx)
	mov	%r11, %rdx
	shr	%cl, %rdx
	mov	%rdx, 40(%rbx)
	mov	%r10, %rax
	shr	%cl, %rax
	mov	%rax, 48(%rbx)
	mov	(%rsp), %rbx
	mov	8(%rsp), %rbp
	mov	16(%rsp), %r12
	mov	24(%rsp), %r13
	mov	32(%rsp), %r14
	add	$40, %rsp
	ret
EPILOGUE()
