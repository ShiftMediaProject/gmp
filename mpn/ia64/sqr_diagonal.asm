dnl  IA-64 mpn_sqr_diagonal.  Helper for sqr_basecase.

dnl  Copyright 2001, 2002 Free Software Foundation, Inc.

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
C sp = r33
C n = r34

C         cycles/limb
C Itanium:    3
C Itanium 2:  2


ASM_START()
PROLOGUE(mpn_sqr_diagonal)
	.prologue
	.save	ar.lc, r2
		mov		r2 = ar.lc
		mov		r14 = ar.ec
	.save	pr, r15
		mov		r15 = pr
	.body
ifdef(`HAVE_ABI_32',
`		addp4	r32 = 0, r32
		addp4	r33 = 0, r33
		zxt4	r34 = r34
		;;
')
		adds		r19 = -1, r34	C decr n
		adds		r18 = 8, r32	C rp for high limb
		;;
		mov		ar.lc = r19
		mov		ar.ec = 7
		mov		pr.rot = 1<<16
		;;
.Loop:
	  (p16)	ldf8		f32 = [r33], 8
	  (p19)	xma.l		f36 = f35, f35, f0
	  (p22)	stf8		[r32] = f39, 16
	  (p19)	xma.hu		f40 = f35, f35, f0
	  (p22)	stf8		[r18] = f43, 16
		br.ctop.dptk.few	.Loop
		;;

		mov		pr = r15, 0x1ffff
		mov		ar.lc = r2
		mov		ar.ec = r14
		;;
		br.ret.sptk.many rp
EPILOGUE(mpn_sqr_diagonal)
ASM_END()
