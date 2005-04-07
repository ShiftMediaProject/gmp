dnl  AMD64 logops.

dnl  Copyright 2004, 2005 Free Software Foundation, Inc.

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
dnl  along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
dnl  the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
dnl  MA 02111-1307, USA.

include(`../config.m4')


C		    cycles/limb
C Hammer:		1.5
C Prescott/Nocona:	3-3.5

C TODO
C  * Clean up VARIANT_3 feed-in code.

ifdef(`OPERATION_and_n',`
  define(`func',`mpn_and_n')
  define(`VARIANT_1')
  define(`LOGOP',`andq')')
ifdef(`OPERATION_andn_n',`
  define(`func',`mpn_andn_n')
  define(`VARIANT_2')
  define(`LOGOP',`andq')')
ifdef(`OPERATION_nand_n',`
  define(`func',`mpn_nand_n')
  define(`VARIANT_3')
  define(`LOGOP',`andq')')
ifdef(`OPERATION_ior_n',`
  define(`func',`mpn_ior_n')
  define(`VARIANT_1')
  define(`LOGOP',`orq')')
ifdef(`OPERATION_iorn_n',`
  define(`func',`mpn_iorn_n')
  define(`VARIANT_2')
  define(`LOGOP',`orq')')
ifdef(`OPERATION_nior_n',`
  define(`func',`mpn_nior_n')
  define(`VARIANT_3')
  define(`LOGOP',`orq')')
ifdef(`OPERATION_xor_n',`
  define(`func',`mpn_xor_n')
  define(`VARIANT_1')
  define(`LOGOP',`xorq')')
ifdef(`OPERATION_xnor_n',`
  define(`func',`mpn_xnor_n')
  define(`VARIANT_3')
  define(`LOGOP',`xorq')')


MULFUNC_PROLOGUE(mpn_and_n mpn_andn_n mpn_nand_n mpn_ior_n mpn_iorn_n mpn_nior_n mpn_xor_n mpn_xnor_n)

C INPUT PARAMETERS
define(`rp',`%rdi')
define(`up',`%rsi')
define(`vp',`%rdx')
define(`n',`%rcx')


ASM_START()

ifdef(`VARIANT_1',`
	TEXT
	ALIGN(32)
PROLOGUE(func)
	movl	%ecx, %eax
	leaq	(vp,n,8), vp
	leaq	(up,n,8), up
	leaq	(rp,n,8), rp
	negq	n
	andl	$3, %eax
	je	.Lb00
	cmpl	$1, %eax
	je	.Lb01
	cmpl	$2, %eax
	je	.Lb10

.Lb11:	movq	(vp,n,8), %r8
	LOGOP	(up,n,8), %r8
	movq	%r8, (rp,n,8)
	incq	n
.Lb10:	movq	(vp,n,8), %r8
	movq	8(vp,n,8), %r9
	LOGOP	(up,n,8), %r8
	LOGOP	8(up,n,8), %r9
	movq	%r8, (rp,n,8)
	movq	%r9, 8(rp,n,8)
	addq	$2, n
	jnc	.Loop
	ret
.Lb01:	movq	(vp,n,8), %r8
	LOGOP	(up,n,8), %r8
	movq	%r8, (rp,n,8)
	addq	$1, n
	jnc	.Loop
	ret

	.byte	0,0,0,0,0,0,0,0,0,0,0,0
.Lb00:

.Loop:	movq	(vp,n,8), %r8
	movq	8(vp,n,8), %r9
	LOGOP	(up,n,8), %r8
	LOGOP	8(up,n,8), %r9
	nop
	movq	%r8, (rp,n,8)
	movq	%r9, 8(rp,n,8)
	movq	16(vp,n,8), %r8
	movq	24(vp,n,8), %r9
	LOGOP	16(up,n,8), %r8
	LOGOP	24(up,n,8), %r9
	movq	%r8, 16(rp,n,8)
	movq	%r9, 24(rp,n,8)
	addq	$4, n
	jnc	.Loop
	ret
EPILOGUE()
')

ifdef(`VARIANT_2',`
	TEXT
	ALIGN(32)
PROLOGUE(func)
	movl	%ecx, %eax
	leaq	(vp,n,8), vp
	leaq	(up,n,8), up
	leaq	(rp,n,8), rp
	negq	n
	andl	$3, %eax
	je	.Lb00
	cmpl	$1, %eax
	je	.Lb01
	cmpl	$2, %eax
	je	.Lb10

.Lb11:	movq	(vp,n,8), %r8
	notq	%r8
	LOGOP	(up,n,8), %r8
	movq	%r8, (rp,n,8)
	incq	n
.Lb10:	movq	(vp,n,8), %r8
	movq	8(vp,n,8), %r9
	notq	%r8
	notq	%r9
	LOGOP	(up,n,8), %r8
	LOGOP	8(up,n,8), %r9
	movq	%r8, (rp,n,8)
	movq	%r9, 8(rp,n,8)
	addq	$2, n
	jnc	.Loop
	ret
.Lb01:	movq	(vp,n,8), %r8
	notq	%r8
	LOGOP	(up,n,8), %r8
	movq	%r8, (rp,n,8)
	addq	$1, n
	jnc	.Loop
	ret
.Lb00:

.Loop:	movq	(vp,n,8), %r8
	movq	8(vp,n,8), %r9
	notq	%r8
	notq	%r9
	LOGOP	(up,n,8), %r8
	LOGOP	8(up,n,8), %r9
	movq	%r8, (rp,n,8)
	movq	%r9, 8(rp,n,8)
	movq	16(vp,n,8), %r8
	movq	24(vp,n,8), %r9
	notq	%r8
	notq	%r9
	LOGOP	16(up,n,8), %r8
	LOGOP	24(up,n,8), %r9
	movq	%r8, 16(rp,n,8)
	movq	%r9, 24(rp,n,8)
	addq	$4, n
	jnc	.Loop
	ret
EPILOGUE()
')

ifdef(`VARIANT_3',`
	TEXT
	ALIGN(32)
PROLOGUE(func)
	movl	%ecx, %eax
	leaq	-16(up,n,8), up
	leaq	(vp,n,8), vp
	leaq	-16(rp,n,8), rp
	negq	n
	andl	$3, %eax
	je	.Lb00
	cmpl	$1, %eax
	je	.Lb01
	cmpl	$2, %eax
	je	.Lb10

.Lb11:	movq	(vp,n,8), %r10
	LOGOP	16(up,n,8), %r10
	notq	%r10
	movq	%r10, 16(rp,n,8)
	addq	$3, n
	jmp	.Lent

.Lb00:	movq	(vp,n,8), %r10
	movq	8(vp,n,8), %r11
	LOGOP	16(up,n,8), %r10
	LOGOP	24(up,n,8), %r11
	notq	%r10
	notq	%r11
	movq	%r10, 16(rp,n,8)
	movq	%r11, 24(rp,n,8)
	addq	$4, n
	jmp	.Lent


.Lb01:	movq	(vp,n,8), %r10
	LOGOP	16(up,n,8), %r10
	notq	%r10
	movq	%r10, 16(rp,n,8)
	cmpq	$-1, %rcx
	je	.Lret
	movq	8(vp,n,8), %r10
	movq	16(vp,n,8), %r11
	LOGOP	24(up,n,8), %r10
	LOGOP	32(up,n,8), %r11
	notq	%r10
	notq	%r11
	movq	%r10, 24(rp,n,8)
	movq	%r11, 32(rp,n,8)

	addq	$5, n
	jmp	.Lent

.Lb10:	addq	$2, n
.Lent:	movq	-16(vp,n,8), %r8
	movq	-8(vp,n,8), %r9
	jc	.Lend

	.byte	0x66, 0x66, 0x90, 0x66, 0x66, 0x90
.Loop:	LOGOP	(up,n,8), %r8
	LOGOP	8(up,n,8), %r9
	notq	%r8
	notq	%r9
	movq	%r8, (rp,n,8)
	movq	%r9, 8(rp,n,8)
	movq	(vp,n,8), %r8
	movq	8(vp,n,8), %r9
	LOGOP	16(up,n,8), %r8
	LOGOP	24(up,n,8), %r9
	notq	%r8
	notq	%r9
	movq	%r8, 16(rp,n,8)
	movq	%r9, 24(rp,n,8)
	movq	16(vp,n,8), %r8
	movq	24(vp,n,8), %r9
	addq	$4, n
	jnc	.Loop

.Lend:	LOGOP	(up,n,8), %r8
	LOGOP	8(up,n,8), %r9
	notq	%r8
	notq	%r9
	movq	%r8, (rp,n,8)
	movq	%r9, 8(rp,n,8)
.Lret:	ret
EPILOGUE()
')
