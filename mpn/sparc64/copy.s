! SPARC v9 __mpn_copy -- Copy a limb vector.

! Copyright (C) 1999 Free Software Foundation, Inc.

! This file is part of the GNU MP Library.

! The GNU MP Library is free software; you can redistribute it and/or modify
! it under the terms of the GNU Library General Public License as published by
! the Free Software Foundation; either version 2 of the License, or (at your
! option) any later version.

! The GNU MP Library is distributed in the hope that it will be useful, but
! WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
! or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
! License for more details.

! You should have received a copy of the GNU Library General Public License
! along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
! the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
! MA 02111-1307, USA.


! INPUT PARAMETERS
! rptr	%o0
! sptr	%o1
! n	%o2

.section	".text"
	.register	%g2,#scratch
	.register	%g3,#scratch
	.align	4
	.global	__mpn_copy
	.type	__mpn_copy,#function
	.proc	04
__mpn_copy:
	add	%o2,-8,%o2
	brlz,pn	%o2,.Lskip
	nop

.Loop:	ldx	[%o1+0],%g1
	ldx	[%o1+8],%g2
	ldx	[%o1+16],%g3
	ldx	[%o1+24],%g4
	ldx	[%o1+32],%g5
	ldx	[%o1+40],%o3
	ldx	[%o1+48],%o4
	ldx	[%o1+56],%o5
	add	%o1,64,%o1
	stx	%g1,[%o0+0]
	stx	%g2,[%o0+8]
	stx	%g3,[%o0+16]
	stx	%g4,[%o0+24]
	stx	%g5,[%o0+32]
	stx	%o3,[%o0+40]
	stx	%o4,[%o0+48]
	stx	%o5,[%o0+56]
	add	%o2,-8,%o2
	brgez,pt	%o2,.Loop
	add	%o0,64,%o0

.Lskip:	add	%o2,8,%o2
	brz,pt	%o2,.Lend
	nop

.Loop2:	ldx	[%o1],%g1
	add	%o1,8,%o1
	add	%o2,-1,%o2
	stx	%g1,[%o0]
	add	%o0,8,%o0
	brgz,pt	%o2,.Loop2
	nop

.Lend:	retl
	nop
.LLfe1:
	.size	 __mpn_copy,.LLfe1-__mpn_copy
