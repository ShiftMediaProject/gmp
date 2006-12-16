dnl  Intel P6-15 mpn_add_n/mpn_sub_n -- mpn add or subtract.

dnl  Copyright 2006 Free Software Foundation, Inc.

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


C         cycles/limb
C K8:     2.3		not good
C P6-15	  2.1

C INPUT PARAMETERS
define(`rp',	`%rdi')
define(`up',	`%rsi')
define(`vp',	`%rdx')
define(`n',	`%rcx')
define(`cy',	`%r8')

ifdef(`OPERATION_add_n', `
	define(ADCSBB,	      adc)
	define(func,	      mpn_add_n)
	define(func_nc,	      mpn_add_nc)')
ifdef(`OPERATION_sub_n', `
	define(ADCSBB,	      sbb)
	define(func,	      mpn_sub_n)
	define(func_nc,	      mpn_sub_nc)')

MULFUNC_PROLOGUE(mpn_add_n mpn_add_nc mpn_sub_n mpn_sub_nc)

ASM_START()

	TEXT
	ALIGN(16)

PROLOGUE(func_nc)
	jmp	L(start)

PROLOGUE(func)
	xor	%r8, %r8
L(start):
	mov	(%rsi), %r10
	mov	(%rdx), %r11

	lea	-8(%rsi,%rcx,8), %rsi
	lea	-8(%rdx,%rcx,8), %rdx
	lea	-16(%rdi,%rcx,8), %rdi
	mov	%ecx, %eax
	neg	%rcx
	and	$3, %eax
	je	L(b00)
	add	%rax, %rcx		C clear low rcx bits for jrcxz
	cmp	$2, %eax
	jl	L(b01)
	je	L(b10)

L(b11):	shr	%r8			C set cy
	jmp	L(e11)

L(b00):	shr	%r8			C set cy
	mov	%r10, %r8
	mov	%r11, %r9
	lea	4(%rcx), %rcx
	jmp	L(e00)

L(b01):	shr	%r8			C set cy
	jmp	L(e01)

L(b10):	shr	%r8			C set cy
	mov	%r10, %r8
	mov	%r11, %r9
	jmp	L(e10)

L(end):	adc	%r11, %r10
	mov	%r10, 8(%rdi)
	mov	%ecx, %eax		C clear eax, ecx contains 0
	adc	%eax, %eax
	ret

	ALIGN(16)
L(top):
	mov	-24(%rsi,%rcx,8), %r8
	mov	-24(%rdx,%rcx,8), %r9
	ADCSBB	%r11, %r10
	mov	%r10, -24(%rdi,%rcx,8)
L(e00):
	mov	-16(%rsi,%rcx,8), %r10
	mov	-16(%rdx,%rcx,8), %r11
	ADCSBB	%r9, %r8
	mov	%r8, -16(%rdi,%rcx,8)
L(e11):
	mov	-8(%rsi,%rcx,8), %r8
	mov	-8(%rdx,%rcx,8), %r9
	ADCSBB	%r11, %r10
	mov	%r10, -8(%rdi,%rcx,8)
L(e10):
	mov	(%rsi,%rcx,8), %r10
	mov	(%rdx,%rcx,8), %r11
	ADCSBB	%r9, %r8
	mov	%r8, (%rdi,%rcx,8)
L(e01):
	jrcxz	L(end)
	lea	4(%rcx), %rcx
	jmp	L(top)

EPILOGUE()
