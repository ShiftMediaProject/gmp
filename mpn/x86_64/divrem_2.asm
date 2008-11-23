dnl  x86-64 mpn_divrem_2 -- Divide an mpn number by a normalized 2-limb number.

dnl  Copyright 2007, 2008 Free Software Foundation, Inc.

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


C		norm	frac
C K8		20	20
C P4		73	73
C P6-15		37	37

C TODO
C  * Perhaps compute the inverse without relying on divq?  Could either use
C    Newton's method and mulq, or perhaps the faster fdiv.
C  * The loop has not been carefully tuned, nor analysed for critical path
C    length.  It seems that 20 c/l is a bit long, compared to the 13 c/l for
C    mpn_divrem_1.


C INPUT PARAMETERS
define(`qp',		`%rdi')
define(`fn',		`%rsi')
define(`up_param',	`%rdx')
define(`un_param',	`%rcx')
define(`d',		`%r8')

define(`di',		`%r9')


C rax rbx rcx rdx rsi rdi rbp r8  r9  r10 r11 r12 r13 r14 r15
C         cnt         qp      d   di

ASM_START()
	TEXT
	ALIGN(16)
PROLOGUE(mpn_divrem_2)

	push	%r15
	lea	(%rdx,%rcx,8), %rax
	push	%r14
	push	%r13
	mov	%rsi, %r13
	push	%r12
	lea	-24(%rax), %r12
	push	%rbp
	mov	%rdi, %rbp
	push	%rbx
	mov	8(%r8), %r11
	mov	-8(%rax), %r9
	mov	(%r8), %r8
	mov	-16(%rax), %r10
	xor	R32(%r15), R32(%r15)
	cmp	%r9, %r11
	ja	L(2)
	setb	%dl
	cmp	%r10, %r8
	setbe	%al
	or	%al, %dl
	jne	L(23)
L(2):
	lea	-3(%rcx,%r13), %rbx
	test	%rbx, %rbx
	js	L(6)
	mov	%r11, %rdx
	mov	$-1, %rax
	not	%rdx
	div	%r11
	mov	%r11, %rdx
	mov	%rax, %rdi
	imul	%rax, %rdx
	mov	%rdx, -8(%rsp)
	mul	%r8
	mov	-8(%rsp), %rax
	mov	%rdx, %rcx
	mov	$-1, %rdx
	add	%r8, %rax
	adc	$0, %rdx
	add	%rcx, %rax
	adc	$0, %rdx
	js	L(8)
L(18):
	dec	%rdi
	sub	%r11, %rax
	sbb	$0, %rdx
	jns	L(18)
L(8):

C rax rbx rcx rdx rsi rdi rbp r8 r9 r10 r11 r12 r13 r14 r15
C n2      un      n1  di  qp  d0        d1  up  fn      msl
C     n2  un     -d1      n1     di XX              XX

ifdef(`NEW',`
	lea	(%rbp,%rbx,8), %r14
	mov	%rbx, %rcx		C un
	mov	%r9, %rbx
	mov	%rdi, %r9		C di
	mov	%r10, %rbp
	mov	%r11, %rsi
	neg	%rsi			C -d1
	ALIGN(16)
L(loop):
	mov	%r9, %rax		C di
	mul	%rbx
	add	%rbp, %rax
	mov	%rax, %r10		C q0
	adc	%rbx, %rdx
	mov	%rdx, %rdi		C q
	imul	%rsi, %rdx
	mov	%r8, %rax
	lea	(%rdx, %rbp), %rbx	C n1 -= ...
	mul	%rdi
	xor	R32(%rbp), R32(%rbp)
	cmp	%r13, %rcx
	jl	L(19)
	mov	(%r12), %rbp
	sub	$8, %r12
L(19):	sub	%r8, %rbp
	sbb	%r11, %rbx
	sub	%rax, %rbp
	sbb	%rdx, %rbx
	inc	%rdi
	xor	R32(%rdx), R32(%rdx)
	cmp	%r10, %rbx
	mov	%r8, %rax		C d1
	adc	$-1, %rdx		C mask
	add	%rdx, %rdi		C q--
	and	%rdx, %rax		C d0 or 0
	and	%r11, %rdx		C d1 or 0
	add	%rax, %rbp
	adc	%rdx, %rbx
	cmp	%r11, %rbx
	jae	L(fix)
L(bck):	mov	%rdi, (%r14)
	sub	$8, %r14
	dec	%rcx
	jns	L(loop)
',`
	lea	(%rbp,%rbx,8), %rbp
	mov	%rbx, %rcx
	mov	%r9, %rax
	mov	%r10, %rsi
	ALIGN(16)
L(loop):
	mov	%rax, %r14
	mul	%rdi
	mov	%r11, %r9
	add	%rsi, %rax
	mov	%rax, %rbx		# q0
	adc	%r14, %rdx		# q
	lea	1(%rdx), %r10
	mov	%rdx, %rax
	imul	%rdx, %r9
	sub	%r9, %rsi
	xor	R32(%r9), R32(%r9)
	mul	%r8
	cmp	%rcx, %r13
	jg	L(13)
	mov	(%r12), %r9
	sub	$8, %r12
L(13):	sub	%r8, %r9
	sbb	%r11, %rsi
	sub	%rax, %r9
	sbb	%rdx, %rsi
	cmp	%rbx, %rsi
	sbb	%rax, %rax
	not	%rax
	add	%rax, %r10
	mov	%r8, %rbx
	and	%rax, %rbx
	and	%r11, %rax
	add	%rbx, %r9
	adc	%rsi, %rax
	cmp	%rax, %r11
	jbe	L(24)
L(14):	mov	%r10, (%rbp)
	sub	$8, %rbp
	mov	%r9, %rsi
	dec	%rcx
	jns	L(loop)
')
L(6):
	pop	%rbx
	pop	%rbp
	pop	%r12
	pop	%r13
	pop	%r14
	mov	%r15, %rax
	pop	%r15
	ret
L(23):
	inc	R32(%r15)
	sub	%r8, %r10
	sbb	%r11, %r9
	jmp	L(2)

ifdef(`NEW',`
L(fix):	seta	%dl
	cmp	%r8, %rbp
	setae	%al
	orb	%dl, %al
	je	L(bck)
	inc	%rdi
	sub	%r8, %rbp
	sbb	%r11, %rbx
	jmp	L(bck)
',`
L(24):
	jb	L(88)
	cmp	%r8, %r9
	jb	L(14)
L(88):	inc	%r10
	sub	%r8, %r9
	sbb	%r11, %rax
	jmp	L(14)
')
EPILOGUE()
