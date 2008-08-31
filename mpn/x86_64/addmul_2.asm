dnl  AMD64 mpn_addmul_2 -- Multiply an n-limb vector with a 2-limb and add the
dnl  result to a third limb vector.

dnl  Copyright 2007 Free Software Foundation, Inc.

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
C K8,K9:	 2.86
C K10:		 2.86
C P4:		13.65	FIXME now it seems to magically have dropped to 15 c/l.
C P6-15:	 4.67

C TODO
C  1. Try scheduling mul last in loop, just like we do in addmul_1.
C  2. Try more loop variants, play with this variant more.  We reached the
C     current speed without much effort, surely it is not ultimate.

C INPUT PARAMETERS
define(`rp',	`%rdi')
define(`up',	`%rsi')
define(`n',	`%rdx')
define(`vp',	`%rcx')

ASM_START()
	TEXT
	ALIGN(16)
PROLOGUE(mpn_addmul_2)

	push	%rbx
	push	%rbp
	push	%r12

define(`vl', `%r9')
define(`vh', `%r10')
	mov  (vp), vl
	mov  8(vp), vh

	mov	n, %r11
define(`n',	`%r11')

	lea	(up,n,8), up
	lea	(rp,n,8), rp
	neg	n

	xor	%r8, %r8
	xor	%ebx, %ebx
	xor	%ecx, %ecx
	xor	%rbp, %rbp

	mov	(up,n,8), %r12
	mov	%r12, %rax
	add	$3, n
	jns	L(end)			C <= 4 iterations

	ALIGN(32)
L(oop):	mul	vl
	add	%rax, %rbx
	mov	%r12, %rax
	adc	%rdx, %rcx
	adc	$0, %ebp
	mul	vh
	add	%rbx, -24(rp,n,8)
	mov	-16(up,n,8), %r12
	mov	%r8d, %ebx
	adc	%rax, %rcx
	mov	%r12, %rax
	adc	%rdx, %rbp

	mul	vl
	add	%rax, %rcx
	mov	%r12, %rax
	adc	%rdx, %rbp
	adc	$0, %ebx
	mul	vh
	add	%rcx, -16(rp,n,8)
	mov	-8(up,n,8), %r12
	mov	%r8d, %ecx
	adc	%rax, %rbp
	mov	%r12, %rax
	adc	%rdx, %rbx

	mul	vl
	add	%rax, %rbp
	mov	%r12, %rax
	adc	%rdx, %rbx
	adc	$0, %ecx
	mul	vh
	add	%rbp, -8(rp,n,8)
	mov	(up,n,8), %r12
	mov	%r8d, %ebp
	adc	%rax, %rbx
	mov	%r12, %rax
	adc	%rdx, %rcx

	add	$3, n
	jns	L(end)

	mul	vl
	add	%rax, %rbx
	mov	%r12, %rax
	adc	%rdx, %rcx
	adc	$0, %ebp
	mul	vh
	add	%rbx, -24(rp,n,8)
	mov	-16(up,n,8), %r12
	mov	%r8d, %ebx
	adc	%rax, %rcx
	mov	%r12, %rax
	adc	%rdx, %rbp

	mul	vl
	add	%rax, %rcx
	mov	%r12, %rax
	adc	%rdx, %rbp
	adc	$0, %ebx
	mul	vh
	add	%rcx, -16(rp,n,8)
	mov	-8(up,n,8), %r12
	mov	%r8d, %ecx
	adc	%rax, %rbp
	mov	%r12, %rax
	adc	%rdx, %rbx

	mul	vl
	add	%rax, %rbp
	mov	%r12, %rax
	adc	%rdx, %rbx
	adc	$0, %ecx
	mul	vh
	add	%rbp, -8(rp,n,8)
	mov	(up,n,8), %r12
	mov	%r8d, %ebp
	adc	%rax, %rbx
	mov	%r12, %rax
	adc	%rdx, %rcx

	add	$3, n
	js	L(oop)

L(end):	jne	L(n3)
	mul	vl
	add	%rax, %rbx
	mov	%r12, %rax
	adc	%rdx, %rcx
	adc	$0, %ebp
	mul	vh
	add	%rbx, -24(rp)
	mov	-16(up), %r12
	mov	%r8d, %ebx
	adc	%rax, %rcx
	mov	%r12, %rax
	adc	%rdx, %rbp
	mul	vl
	add	%rax, %rcx
	mov	%r12, %rax
	adc	%rdx, %rbp
	adc	$0, %ebx
	mul	vh
	add	%rcx, -16(rp)
	mov	-8(up), %r12
	mov	%r8d, %ecx
	adc	%rax, %rbp
	mov	%r12, %rax
	adc	%rdx, %rbx
	mul	vl
	add	%rax, %rbp
	mov	%r12, %rax
	adc	%rdx, %rbx
	adc	$0, %ecx
	mul	vh
	add	%rbp, -8(rp)
	adc	%rax, %rbx
	adc	%rdx, %rcx
	mov	%rbx, (rp)
	mov	%rcx, %rax
	jmp	L(ret)

L(n3):	cmp	$1, n
	jne	L(n2)
	mul	vl
	add	%rax, %rbx
	mov	%r12, %rax
	adc	%rdx, %rcx
	adc	$0, %ebp
	mul	vh
	add	%rbx, -16(rp)
	mov	-8(up), %r12
	mov	%r8d, %ebx
	adc	%rax, %rcx
	mov	%r12, %rax
	adc	%rdx, %rbp
	mul	vl
	add	%rax, %rcx
	mov	%r12, %rax
	adc	%rdx, %rbp
	adc	$0, %ebx
	mul	vh
	add	%rcx, -8(rp)
	adc	%rax, %rbp
	adc	%rdx, %rbx
	mov	%rbp, (rp)
	mov	%rbx, %rax
	jmp	L(ret)

L(n2):	mul	vl
	add	%rax, %rbx
	mov	%r12, %rax
	adc	%rdx, %rcx
	adc	$0, %ebp
	mul	vh
	add	%rbx, -8(rp)
	adc	%rax, %rcx
	adc	%rdx, %rbp
	mov	%rcx, (rp)
	mov	%rbp, %rax

L(ret):	pop	%r12
	pop	%rbp
	pop	%rbx
	ret

EPILOGUE()
