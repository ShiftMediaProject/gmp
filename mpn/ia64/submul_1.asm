dnl  IA-64 mpn_submul_1 -- Multiply a limb vector with a limb and subtract the
dnl  result from a second limb vector.

dnl  Copyright 2000, 2001, 2002 Free Software Foundation, Inc.

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
C up = r33
C n = r34
C v = r35

C         cycles/limb
C Itanium:    4
C Itanium 2:  3.5

ASM_START()
PROLOGUE(mpn_submul_1)
	.prologue
	.save	ar.pfs, r21
		alloc		r21 = ar.pfs, 4, 4, 0, 8
	.save	ar.lc, r2
		mov		r2 = ar.lc
		mov		r20 = ar.ec
	.save	pr, r22
		mov		r22 = pr
	.body
ifdef(`HAVE_ABI_32',
`		addp4	r32 = 0, r32
		addp4	r33 = 0, r33
		zxt4	r34 = r34
')
		sub r35 = r0, r35			C negate v
		;;
  { .mib;	setf.sig	f6 = r35
		adds		r19 = -1, r34		C n - 1
		nop.b		0
} { .mib;	cmp.eq		p6, p7 = 0, r35
		mov		r18 = r32
		nop.b		0		;;
} { .mib;	mov		r16 = r32
		mov		ar.lc = r19
		nop.b		0
} { .mib;	mov		r17 = r33
		mov		ar.ec = 8
		nop.b		0
} { .mib;	mov		r19 = r33
		mov		pr.rot = 1<<16
		nop.b		0
} { .mib;	mov		r23 = 0			C clear "carry in"
		mov		r24 = 0			C clear "carry in"
	   (p6)	br.spnt		.Ldone			C multiplier == 0
		;;
}
		br.sptk		.Loop
		.align	32
.Loop:
  { .mfi; (p16)	ldf8		f32 = [r17], 8		C >0  3  6  9 12 15 18
	  (p19)	xma.l		f40 = f35, f6, f39	C  0  3  6 >9 12 15 18
	  (p23)	cmp.ltu		p6, p7 = r34, r23	C  0  3  6  9 12 15>18
} { .mfi; (p16)	ldf8		f36 = [r16], 8		C >0  3  6  9 12 15 18
	  (p19)	xma.hu		f44 = f35, f6, f39	C  0  3  6 >9 12 15 18
	  (p23)	sub		r14 = r34, r23	;;	C  0  3  6  9 12 15>18
} { .mib; (p21)	getf.sig	r32 = f42		C  1  4  7 10 13>16 19
	  (p23)	sub		r23 = r24, r38		C  1  4  7 10 13 16>19
		nop.b		0			C  1  4  7 10 13 16 19
} { .mib; (p22)	ld8		r24 = [r19], 8		C  1  4  7 10 13>16 19
		nop.i		0			C  1  4  7 10 13 16>19
		nop.b		0		;;	C  1  4  7 10 13 16 19
} { .mib; (p21)	getf.sig	r36 = f46		C  2  5  8 11 14>17 20
	   (p6)	add		r23 = 1, r23		C  2  5  8 11 14 17 20
		nop.b		0			C  2  5  8 11 14 17 20
} { .mib; (p23)	st8		[r18] = r14, 8		C  2  5  8 11 14 17>20
		nop.i		0			C  2  5  8 11 14 17 20
		br.ctop.sptk.few .Loop		;;	C  2  5  8 11 14 17 20
}
.Ldone:		mov		r8 = r23
		mov		pr = r22, 0x1fffe
		mov		ar.lc = r2
		mov		ar.ec = r20
		mov		ar.pfs = r21;;
		br.ret.sptk.many b0
EPILOGUE(mpn_submul_1)
ASM_END()
