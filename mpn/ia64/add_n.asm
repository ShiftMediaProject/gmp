dnl  IA-64 mpn_add_n -- Add two limb vectors of the same length > 0 and
dnl  store sum in a third limb vector.

dnl  Copyright (C) 2000 Free Software Foundation, Inc.

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

C INPUT PARAMETERS
C rp = r32
C s1p = r33
C s2p = r34
C n = r35

ASM_START()
PROLOGUE(mpn_add_n)
	.prologue
	.save	ar.lc, r2
		mov	r2 = ar.lc
	.body
		add	r35 = -1, r35;;
		mov	ar.lc = r35
		cmp.ne	p8, p9 = r0, r0
.Loop:
		ld8	r15 = [r33], 8
		ld8	r17 = [r34], 8;;
	(p8)	add	r16 = r15, r17, 1
	(p9)	add	r16 = r15, r17;;
	(p8)	cmp.leu	p8, p9 = r16, r17
	(p9)	cmp.ltu	p8, p9 = r16, r17
		st8	[r32] = r16, 8
		br.cloop.dptk .Loop
	;;
	(p8)	mov	r8 = 1
	(p9)	mov	r8 = 0
		mov	ar.lc = r2
		br.ret.sptk.many b0
EPILOGUE(mpn_add_n)
ASM_END()
