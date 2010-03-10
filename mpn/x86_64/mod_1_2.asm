dnl  AMD64 mpn_mod_1s_2p

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
C AMD K8,K9	 4
C AMD K10	 4
C Intel P4	19
C Intel core2	 8
C Intel corei	 6.5
C Intel atom	28
C VIA nano	 8

ASM_START()
	TEXT
	ALIGN(16)
PROLOGUE(mpn_mod_1s_2p)
	push	%r14
	test	$1, %sil
	mov	%rdx, %r14
	push	%r13
	mov	%rcx, %r13
	push	%r12
	push	%rbp
	push	%rbx
	mov	16(%rcx), %r10
	mov	24(%rcx), %rbx
	mov	32(%rcx), %rbp
	je	L(b0)
	dec	%rsi
	je	L(one)
	mov	-8(%rdi,%rsi,8), %rax
	mul	%r10
	mov	%rax, %r9
	mov	%rdx, %r8
	mov	(%rdi,%rsi,8), %rax
	add	-16(%rdi,%rsi,8), %r9
	adc	$0, %r8
	mul	%rbx
	add	%rax, %r9
	adc	%rdx, %r8
	jmp	.L11
L(b0):
	mov	-8(%rdi,%rsi,8), %rax
	mul	%r10
	mov	%rdx, %r8
	mov	%rax, %r9
	add	-16(%rdi,%rsi,8), %r9
	adc	$0, %r8
.L11:
	sub	$4, %rsi
	jb	L(ed2)
	lea	40(%rdi,%rsi,8), %rdi
	mov	-40(%rdi), %r11
	mov	-32(%rdi), %rax
	jmp	L(m0)

	ALIGN(16)
L(top):	mov	-24(%rdi), %r9
	add	%rax, %r11
	mov	-16(%rdi), %rax
	adc	%rdx, %r12
	mul	%r10
	add	%rax, %r9
	mov	%r11, %rax
	mov	%rdx, %r8
	adc	$0, %r8
	mul	%rbx
	add	%rax, %r9
	mov	%r12, %rax
	adc	%rdx, %r8
	mul	%rbp
	sub	$2, %rsi
	jb	L(ed1)
	mov	-40(%rdi), %r11
	add	%rax, %r9
	mov	-32(%rdi), %rax
	adc	%rdx, %r8
L(m0):	mul	%r10
	add	%rax, %r11
	mov	%r9, %rax
	mov	%rdx, %r12
	adc	$0, %r12
	mul	%rbx
	add	%rax, %r11
	lea	-32(%rdi), %rdi		C ap -= 4
	mov	%r8, %rax
	adc	%rdx, %r12
	mul	%rbp
	sub	$2, %rsi
	jae	L(top)

L(ed0):	mov	%r11, %r9
	mov	%r12, %r8
L(ed1):	add	%rax, %r9
	adc	%rdx, %r8
L(ed2):	mov	8(%r13), R32(%rdi)		C cnt
	mov	%r8, %rax
	mov	%r9, %r8
	mul	%r10
	add	%rax, %r8
	adc	$0, %rdx
L(1):	xor	R32(%rcx), R32(%rcx)
	mov	%r8, %r9
	sub	R32(%rdi), R32(%rcx)
	shr	R8(%rcx), %r9
	mov	R32(%rdi), R32(%rcx)
	sal	R8(%rcx), %rdx
	or	%rdx, %r9
	sal	R8(%rcx), %r8
	mov	%r9, %rax
	mulq	(%r13)
	mov	%rax, %rsi
	inc	%r9
	add	%r8, %rsi
	adc	%r9, %rdx
	imul	%r14, %rdx
	sub	%rdx, %r8
	lea	(%r8,%r14), %rax
	cmp	%r8, %rsi
	cmovb	%rax, %r8
	mov	%r8, %rax
	sub	%r14, %rax
	cmovb	%r8, %rax
	mov	R32(%rdi), R32(%rcx)
	shr	R8(%rcx), %rax
	pop	%rbx
	pop	%rbp
	pop	%r12
	pop	%r13
	pop	%r14
	ret
L(one):
	mov	(%rdi), %r8
	mov	8(%rcx), R32(%rdi)
	xor	%rdx, %rdx
	jmp	L(1)
EPILOGUE()

PROLOGUE(mpn_mod_1s_2p_cps)
	bsr	%rsi, %rax
	mov	%rbp, -16(%rsp)
	mov	R32(%rax), R32(%rbp)
	mov	%r12, -8(%rsp)
	xor	$63, R32(%rbp)
	mov	%rsi, %r12
	mov	%rbx, -24(%rsp)
	mov	R32(%rbp), R32(%rcx)
	mov	%rdi, %rbx
	sub	$24, %rsp
	sal	R8(%rcx), %r12
	mov	%r12, %rdi
	CALL(	mpn_invert_limb)
	mov	$64, R32(%rcx)
	mov	%rax, %r10
	mov	$1, R32(%rdx)
	sub	R32(%rbp), R32(%rcx)
	mov	%rax, %r11
	shr	R8(%rcx), %r10
	mov	R32(%rbp), R32(%rcx)
	mov	%r11, (%rbx)
	sal	R8(%rcx), %rdx
	or	%rdx, %r10
	mov	%r12, %rdx
	neg	%rdx
	imul	%rdx, %r10
	mov	%r10, %rax
	mul	%r11
	lea	1(%rdx,%r10), %r9
	neg	%r9
	imul	%r12, %r9
	lea	(%r9,%r12), %rsi
	cmp	%r9, %rax
	cmovb	%rsi, %r9
	mov	%r9, %rax
	mul	%r11
	lea	1(%rdx,%r9), %r8
	neg	%r8
	imul	%r12, %r8
	lea	(%r8,%r12), %rdi
	cmp	%r8, %rax
	movslq	R32(%rbp), %rax
	mov	%rax, 8(%rbx)
	mov	8(%rsp), %rbp
	cmovb	%rdi, %r8
	shr	R8(%rcx), %r10
	shr	R8(%rcx), %r9
	shr	R8(%rcx), %r8
	mov	%r10, 16(%rbx)
	mov	%r9, 24(%rbx)
	mov	%r8, 32(%rbx)
	mov	16(%rsp), %r12
	mov	(%rsp), %rbx
	add	$24, %rsp
	ret
EPILOGUE()
