C SPARC v8 __mpn_submul_1 -- Multiply a limb vector with a limb and
C subtract the result from a second limb vector.

C Copyright (C) 1992, 1993, 1994 Free Software Foundation, Inc.

C This file is part of the GNU MP Library.

C The GNU MP Library is free software; you can redistribute it and/or modify
C it under the terms of the GNU Library General Public License as published by
C the Free Software Foundation; either version 2 of the License, or (at your
C option) any later version.

C The GNU MP Library is distributed in the hope that it will be useful, but
C WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
C or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
C License for more details.

C You should have received a copy of the GNU Library General Public License
C along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
C the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
C MA 02111-1307, USA.


C INPUT PARAMETERS
C res_ptr	o0
C s1_ptr	o1
C size		o2
C s2_limb	o3

include(`../config.m4')

ASM_START()
PROLOGUE(mpn_submul_1)
	sub	%g0,%o2,%o2		C negate ...
	sll	%o2,2,%o2		C ... and scale size
	sub	%o1,%o2,%o1		C o1 is offset s1_ptr
	sub	%o0,%o2,%g1		C g1 is offset res_ptr

	mov	0,%o0			C clear cy_limb

L(loop):
	ld	[%o1+%o2],%o4
	ld	[%g1+%o2],%g2
	umul	%o4,%o3,%o5
	rd	%y,%g3
	addcc	%o5,%o0,%o5
	addx	%g3,0,%o0
	subcc	%g2,%o5,%g2
	addx	%o0,0,%o0
	st	%g2,[%g1+%o2]

	addcc	%o2,4,%o2
	bne	L(loop)
	 nop

	retl
	 nop
EPILOGUE(mpn_submul_1)
