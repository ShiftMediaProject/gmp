dnl  AMD64 mpn_com_n.

dnl  Copyright 2004, 2005, 2006 Free Software Foundation, Inc.

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


C		    cycles/limb
C Hammer:		1.3
C Prescott/Nocona:	2.78

C INPUT PARAMETERS
define(`rp',`%rdi')
define(`up',`%rsi')
define(`n',`%rdx')


ASM_START()
	TEXT
	ALIGN(32)
PROLOGUE(mpn_com_n)
	movq	(up), %r8
	movl	%edx, %eax
	leaq	(up,n,8), up
	leaq	(rp,n,8), rp
	negq	n
	andl	$3, %eax
	je	.Lb00
	cmpl	$2, %eax
	jc	.Lb01
	je	.Lb10

.Lb11:	notq	%r8
	movq	%r8, (rp,n,8)
	decq	n
	jmp	.Le11
.Lb10:	addq	$-2, n
	jmp	.Le10
	.byte	0x90,0x90,0x90,0x90,0x90,0x90
.Lb01:	notq	%r8
	movq	%r8, (rp,n,8)
	incq	n
	jz	.Lret

.Loop:	movq	(up,n,8), %r8
.Lb00:	movq	8(up,n,8), %r9
	notq	%r8
	notq	%r9
	movq	%r8, (rp,n,8)
	movq	%r9, 8(rp,n,8)
.Le11:	movq	16(up,n,8), %r8
.Le10:	movq	24(up,n,8), %r9
	notq	%r8
	notq	%r9
	movq	%r8, 16(rp,n,8)
	movq	%r9, 24(rp,n,8)
	addq	$4, n
	jnc	.Loop
.Lret:	ret
EPILOGUE()
