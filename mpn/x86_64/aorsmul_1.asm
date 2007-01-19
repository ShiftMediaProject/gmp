dnl  AMD64 mpn_addmul_1 and mpn_submul_1.

dnl  Copyright 2003, 2004, 2005, 2007 Free Software Foundation, Inc.

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
C K8:		 3.0    (unlucky alignment can lead to 3.17)
C P4:		15.5-17 (fluctuating due to w/c buffer problems)
C P6-15:	 5.17

C INPUT PARAMETERS
define(`rp',	`%rdi')
define(`up',	`%rsi')
define(`n',	`%rdx')
define(`vl',	`%rcx')

ifdef(`OPERATION_addmul_1',`
      define(`ADDSUB',        `add')
      define(`func',  `mpn_addmul_1')

')
ifdef(`OPERATION_submul_1',`
      define(`ADDSUB',        `sub')
      define(`func',  `mpn_submul_1')
')

MULFUNC_PROLOGUE(mpn_addmul_1 mpn_submul_1)

	TEXT
	ALIGN(16)
ASM_START()
PROLOGUE(func)
	mov	n   , %r11		C move away n from rdx, mul uses it
define(`n',	`%r11')
	mov	(up), %rax		C load first limbs early...
	mul	vl			C ...and start first multiply

	cmp	$1, n			C need special handling for n = 1
	je	.Ln1

	lea	-8(up,n,8), up		C point at end of area
	lea	(rp,n,8), rp		C point at end of area
	neg	n

	mov	%rax, %r8
	mov	16(up,n,8), %rax
	mov	%rdx, %r9
	mul	vl

	add	$5, n
	jns	.Lend			C jump for n = 2, 3, 4, 5
	jmp	.Loop
.Ln1:	ADDSUB	%rax, (rp)
	adc	$0, %rdx
	mov	%rdx, %rax
	ret

	ALIGN(16)
.Loop:	ADDSUB	%r8 , -40(rp,n,8)
	mov	$0  , %r8
	adc	%rax, %r9
	mov	-16(up,n,8), %rax
	adc	%rdx, %r8

	mul	vl
	ADDSUB	%r9 , -32(rp,n,8)
	mov	$0  , %r9
	adc	%rax, %r8
	nop
	mov	-8(up,n,8), %rax
	adc	%rdx, %r9

	mul	vl
	ADDSUB	%r8 , -24(rp,n,8)
	mov	$0  , %r8
	adc	%rax, %r9
	mov	(up,n,8), %rax
	adc	%rdx, %r8

	mul	vl
	ADDSUB	%r9 , -16(rp,n,8)
	mov	$0  , %r9
	adc	%rax, %r8
	mov	8(up,n,8), %rax
	nop
	adc	%rdx, %r9
	mul	vl

	add	$4, n
	js	.Loop

.Lend:	cmp	$3, n
	je	.Lb11

	ADDSUB	%r8 , -40(rp,n,8)
	mov	$0  , %r8
	adc	%rax, %r9
	mov	-16(up,n,8), %rax
	adc	%rdx, %r8
	mul	vl
	cmp	$2, n
	je	.Lb00

	ADDSUB	%r9 , -32(rp,n,8)
	mov	$0  , %r9
	adc	%rax, %r8
	mov	-8(up,n,8), %rax
	adc	%rdx, %r9
	mul	vl
	cmp	$1, n
	je	.Lb01

	ADDSUB	%r8 , -24(rp,n,8)
	mov	$0  , %r8
	adc	%rax, %r9
	mov	(up,n,8), %rax
	adc	%rdx, %r8
	mul	vl
	ADDSUB	%r9 , -16(rp,n,8)
	adc	%rax, %r8
	mov	$0  , %eax
	adc	%rdx, %rax
	ADDSUB	%r8 , -8(rp,n,8)
	adc	$0  , %rax
	ret

.Lb01:	ADDSUB	%r8 , -24(rp,n,8)
	adc	%rax, %r9
	mov	$0  , %eax
	adc	%rdx, %rax
	ADDSUB	%r9 , -16(rp,n,8)
	adc	$0  , %rax
	ret

.Lb00:	ADDSUB	%r9 , -32(rp,n,8)
	adc	%rax, %r8
	mov	$0  , %eax
	adc	%rdx, %rax
	ADDSUB	%r8 , -24(rp,n,8)
	adc	$0  , %rax
	ret

.Lb11:	ADDSUB	%r8 , -40(rp,n,8)
	adc	%rax, %r9
	mov	$0  , %eax
	adc	%rdx, %rax
	ADDSUB	%r9 , -32(rp,n,8)
	adc	$0  , %rax
	ret

EPILOGUE()
