dnl  AMD64 mpn_copyd -- copy limb vector, decrementing.

dnl  Copyright 2003 Free Software Foundation, Inc.

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


C         cycles/limb
C Hammer:     1


C INPUT PARAMETERS
C rp	rdi
C up	rsi
C n	rdx

define(`rp',`%rdi')
define(`up',`%rsi')
define(`n',`%rdx')

ASM_START()
	TEXT
	ALIGN(16)
PROLOGUE(mpn_copyd)
	leaq	-8(up,n,8), up
	leaq	(rp,n,8), rp
	subq	$4, n
	jc	.Lend
	ALIGN(16)
.Loop:	movq	(up), %r8
	movq	-8(up), %r9
	leaq	-32(rp), rp
	movq	-16(up), %r10
	movq	-24(up), %r11
	leaq	-32(up), up
	movq	%r8, 24(rp)
	movq	%r9, 16(rp)
	subq	$4, n
	movq	%r10, 8(rp)
	movq	%r11, (rp)
	jnc	.Loop

.Lend:	shrl	$1, %edx		C edx = lowart(n)
	jnc	1f
	movq	(up), %r8
	movq	%r8, -8(rp)
	leaq	-8(rp), rp
	leaq	-8(up), up
1:	shrl	$1, %edx		C edx = lowart(n)
	jnc	1f
	movq	(up), %r8
	movq	-8(up), %r9
	movq	%r8, -8(rp)
	movq	%r9, -16(rp)
1:	ret				C				1
EPILOGUE()
