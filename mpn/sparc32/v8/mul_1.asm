C SPARC v8 __mpn_mul_1 -- Multiply a limb vector with a single limb and
C store the product in a second limb vector.

C Copyright (C) 1992, 1994, 1995 Free Software Foundation, Inc.

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
PROLOGUE(mpn_mul_1)
	sll	%o2,4,%g1
	and	%g1,(4-1)<<4,%g1
#if PIC
	mov	%o7,%g4		C Save return address register
	call	1f
	add	%o7,L(L)-1f,%g3
1:	mov	%g4,%o7		C Restore return address register
#else
	sethi	%hi(L(1)),%g3
	or	%g3,%lo(L(1)),%g3
#endif
	jmp	%g3+%g1
	ld	[%o1+0],%o4	C 1
L(1):
L(L00):	add	%o0,-4,%o0
	add	%o1,-4,%o1
	b	L(loop00)	C 4, 8, 12, ...
	orcc	%g0,%g0,%g2
L(L01):	b	L(loop01)	C 1, 5, 9, ...
	orcc	%g0,%g0,%g2
	nop
	nop
L(L10):	add	%o0,-12,%o0	C 2, 6, 10, ...
	add	%o1,4,%o1
	b	L(loop10)
	orcc	%g0,%g0,%g2
	nop
L(L11):	add	%o0,-8,%o0	C 3, 7, 11, ...
	add	%o1,-8,%o1
	b	L(loop11)
	orcc	%g0,%g0,%g2

L(loop):
	addcc	%g3,%g2,%g3	C 1
	ld	[%o1+4],%o4	C 2
	st	%g3,[%o0+0]	C 1
	rd	%y,%g2		C 1
L(loop00):
	umul	%o4,%o3,%g3	C 2
	addxcc	%g3,%g2,%g3	C 2
	ld	[%o1+8],%o4	C 3
	st	%g3,[%o0+4]	C 2
	rd	%y,%g2		C 2
L(loop11):
	umul	%o4,%o3,%g3	C 3
	addxcc	%g3,%g2,%g3	C 3
	ld	[%o1+12],%o4	C 4
	add	%o1,16,%o1
	st	%g3,[%o0+8]	C 3
	rd	%y,%g2		C 3
L(loop10):
	umul	%o4,%o3,%g3	C 4
	addxcc	%g3,%g2,%g3	C 4
	ld	[%o1+0],%o4	C 1
	st	%g3,[%o0+12]	C 4
	add	%o0,16,%o0
	rd	%y,%g2		C 4
	addx	%g0,%g2,%g2
L(loop01):
	addcc	%o2,-4,%o2
	bg	L(loop)
	umul	%o4,%o3,%g3	C 1

	addcc	%g3,%g2,%g3	C 4
	st	%g3,[%o0+0]	C 4
	rd	%y,%g2		C 4

	retl
	addx	%g0,%g2,%o0
EPILOGUE(mpn_mul_1)
