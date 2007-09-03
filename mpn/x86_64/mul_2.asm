dnl  AMD64 mpn_mul_2 -- Multiply an n-limb vector with a 2-limb and store the
dnl  result in a third limb vector.

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

C	     cycles/limb
C K8:		 2.86
C P4:		12.25
C P6-15:	 4.18

C INPUT PARAMETERS
define(`rp',	`%rdi')
define(`up',	`%rsi')
define(`n',	`%rdx')
define(`vp',	`%rcx')

	TEXT
	ALIGN(16)
ASM_START()
PROLOGUE(mpn_mul_2)

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
	jns	.Lend			C <= 4 iterations

	ALIGN(32)
.Loop:	mul	vl
	add	%rax, %rbx
	mov	%r12, %rax
	adc	%rdx, %rcx
	adc	$0, %ebp
	mul	vh
	mov	%rbx, -24(rp,n,8)
	mov	-16(up,n,8), %r12
	mov	%r8d, %ebx
	add	%rax, %rcx
	mov	%r12, %rax
	adc	%rdx, %rbp

	mul	vl
	add	%rax, %rcx
	mov	%r12, %rax
	adc	%rdx, %rbp
	adc	$0, %ebx
	mul	vh
	mov	%rcx, -16(rp,n,8)
	mov	-8(up,n,8), %r12
	mov	%r8d, %ecx
	add	%rax, %rbp
	mov	%r12, %rax
	adc	%rdx, %rbx

	mul	vl
	add	%rax, %rbp
	mov	%r12, %rax
	adc	%rdx, %rbx
	adc	$0, %ecx
	mul	vh
	mov	%rbp, -8(rp,n,8)
	mov	(up,n,8), %r12
	mov	%r8d, %ebp
	add	%rax, %rbx
	mov	%r12, %rax
	adc	%rdx, %rcx

	add	$3, n
	jns	.Lend

	mul	vl
	add	%rax, %rbx
	mov	%r12, %rax
	adc	%rdx, %rcx
	adc	$0, %ebp
	mul	vh
	mov	%rbx, -24(rp,n,8)
	mov	-16(up,n,8), %r12
	mov	%r8d, %ebx
	add	%rax, %rcx
	mov	%r12, %rax
	adc	%rdx, %rbp

	mul	vl
	add	%rax, %rcx
	mov	%r12, %rax
	adc	%rdx, %rbp
	adc	$0, %ebx
	mul	vh
	mov	%rcx, -16(rp,n,8)
	mov	-8(up,n,8), %r12
	mov	%r8d, %ecx
	add	%rax, %rbp
	mov	%r12, %rax
	adc	%rdx, %rbx

	mul	vl
	add	%rax, %rbp
	mov	%r12, %rax
	adc	%rdx, %rbx
	adc	$0, %ecx
	mul	vh
	mov	%rbp, -8(rp,n,8)
	mov	(up,n,8), %r12
	mov	%r8d, %ebp
	add	%rax, %rbx
	mov	%r12, %rax
	adc	%rdx, %rcx

	add	$3, n
	js	.Loop

.Lend:	jne	.Ln3
	mul	vl
	add	%rax, %rbx
	mov	%r12, %rax
	adc	%rdx, %rcx
	adc	$0, %ebp
	mul	vh
	mov	%rbx, -24(rp)
	mov	-16(up), %r12
	mov	%r8d, %ebx
	add	%rax, %rcx
	mov	%r12, %rax
	adc	%rdx, %rbp
	mul	vl
	add	%rax, %rcx
	mov	%r12, %rax
	adc	%rdx, %rbp
	adc	$0, %ebx
	mul	vh
	mov	%rcx, -16(rp)
	mov	-8(up), %r12
	mov	%r8d, %ecx
	add	%rax, %rbp
	mov	%r12, %rax
	adc	%rdx, %rbx
	mul	vl
	add	%rax, %rbp
	mov	%r12, %rax
	adc	%rdx, %rbx
	adc	$0, %ecx
	mul	vh
	mov	%rbp, -8(rp)
	add	%rax, %rbx
	adc	%rdx, %rcx
	mov	%rbx, (rp)
	mov	%rcx, %rax
	jmp	.Lret

.Ln3:	cmp	$1, n
	jne	.Ln2
	mul	vl
	add	%rax, %rbx
	mov	%r12, %rax
	adc	%rdx, %rcx
	adc	$0, %ebp
	mul	vh
	mov	%rbx, -16(rp)
	mov	-8(up), %r12
	mov	%r8d, %ebx
	add	%rax, %rcx
	mov	%r12, %rax
	adc	%rdx, %rbp
	mul	vl
	add	%rax, %rcx
	mov	%r12, %rax
	adc	%rdx, %rbp
	adc	$0, %ebx
	mul	vh
	mov	%rcx, -8(rp)
	add	%rax, %rbp
	adc	%rdx, %rbx
	mov	%rbp, (rp)
	mov	%rbx, %rax
	jmp	.Lret

.Ln2:	mul	vl
	add	%rax, %rbx
	mov	%r12, %rax
	adc	%rdx, %rcx
	adc	$0, %ebp
	mul	vh
	mov	%rbx, -8(rp)
	add	%rax, %rcx
	adc	%rdx, %rbp
	mov	%rcx, (rp)
	mov	%rbp, %rax

.Lret:	pop	%r12
	pop	%rbp
	pop	%rbx
	ret

EPILOGUE()
