dnl  AMD64 mpn_lshift -- mpn left shift.

dnl  Copyright 2003, 2005 Free Software Foundation, Inc.
dnl
dnl  This file is part of the GNU MP Library.
dnl
dnl  The GNU MP Library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Lesser General Public License as
dnl  published by the Free Software Foundation; either version 2.1 of the
dnl  License, or (at your option) any later version.
dnl
dnl  The GNU MP Library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Lesser General Public License for more details.
dnl
dnl  You should have received a copy of the GNU Lesser General Public
dnl  License along with the GNU MP Library; see the file COPYING.LIB.  If
dnl  not, write to the Free Software Foundation, Inc., 51 Franklin Street,
dnl  Fifth Floor, Boston, MA 02110-1301, USA.

include(`../config.m4')


C		    cycles/limb
C Hammer:		2.375
C Prescott/Nocona:	3.5


C INPUT PARAMETERS
C rp	rdi
C up	rsi
C n	rdx
C cnt	rcx

define(`rp',`%rdi')
define(`up',`%rsi')
define(`n',`%rdx')

ASM_START()
	TEXT
	ALIGN(32)
PROLOGUE(mpn_lshift)
	mov	-8(up,n,8), %rax
	movd	%ecx, %mm4
	negl	%ecx			C put rsh count in cl
	and	$63, %ecx
	movd	%ecx, %mm5

	shrq	%cl, %rax		C function return value

	leal	1(n), %r8d
	andl	$3, %r8d
	je	.Lrolx			C jump for n = 3, 7, 11, ...

	decl	%r8d
	jne	.L1
C	n = 4, 8, 12, ...
	movq	-8(up,n,8), %mm2
	psllq	%mm4, %mm2
	movq	-16(up,n,8), %mm0
	psrlq	%mm5, %mm0
	por	%mm0, %mm2
	movq	%mm2, -8(rp,n,8)
	decq	n
	jmp	.Lroll

.L1:	decl	%r8d
	je	.L1x			C jump for n = 1, 5, 9, 13, ...
C	n = 2, 6, 10, 16, ...
	movq	-8(up,n,8), %mm2
	psllq	%mm4, %mm2
	movq	-16(up,n,8), %mm0
	psrlq	%mm5, %mm0
	por	%mm0, %mm2
	movq	%mm2, -8(rp,n,8)
	decq	n
.L1x:
	cmpq	$1, n
	je	.Last
	movq	-8(up,n,8), %mm2
	psllq	%mm4, %mm2
	movq	-16(up,n,8), %mm3
	psllq	%mm4, %mm3
	movq	-16(up,n,8), %mm0
	movq	-24(up,n,8), %mm1
	psrlq	%mm5, %mm0
	por	%mm0, %mm2
	psrlq	%mm5, %mm1
	por	%mm1, %mm3
	movq	%mm2, -8(rp,n,8)
	movq	%mm3, -16(rp,n,8)
	subq	$2, n

.Lroll:
.Lrolx:	movq	-8(up,n,8), %mm2
	psllq	%mm4, %mm2
	movq	-16(up,n,8), %mm3
	psllq	%mm4, %mm3

	subq	$4, n			C				      4
	jb	.Lend			C				      2
	ALIGN(32)
	nop
	nop
.Loop:
	C finish stuff from lsh block
	movq	16(up,n,8), %mm0
	movq	8(up,n,8), %mm1
	psrlq	%mm5, %mm0
	por	%mm0, %mm2
	psrlq	%mm5, %mm1
	por	%mm1, %mm3
	movq	%mm2, 24(rp,n,8)
	movq	%mm3, 16(rp,n,8)
	C start two new rsh
	movq	0(up,n,8), %mm0
	movq	-8(up,n,8), %mm1
	psrlq	%mm5, %mm0
	psrlq	%mm5, %mm1

	C finish stuff from rsh block
	movq	8(up,n,8), %mm2
	movq	0(up,n,8), %mm3
	psllq	%mm4, %mm2
	por	%mm2, %mm0
	psllq	%mm4, %mm3
	por	%mm3, %mm1
	movq	%mm0, 8(rp,n,8)
	movq	%mm1, 0(rp,n,8)
	C start two new lsh
	movq	-8(up,n,8), %mm2
	movq	-16(up,n,8), %mm3
	subq	$4, n
	psllq	%mm4, %mm2
	psllq	%mm4, %mm3

	jae	.Loop			C				      2
.Lend:
	movq	16(up,n,8), %mm0
	psrlq	%mm5, %mm0
	por	%mm0, %mm2
	movq	8(up,n,8), %mm1
	psrlq	%mm5, %mm1
	por	%mm1, %mm3
	movq	%mm2, 24(rp,n,8)
	movq	%mm3, 16(rp,n,8)

.Last:	movq	(up), %mm2
	psllq	%mm4, %mm2
	movq	%mm2, (rp)
	emms
	ret
EPILOGUE()
