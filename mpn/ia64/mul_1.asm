dnl  IA-64 mpn_mul_1 -- Multiply a limb vector with a limb and store the result
dnl  in a second limb vector.

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
C Itanium:    2.5
C Itanium 2:  2.0

C Further optimization ideas:
C  * Put the prologue and epilogue insns in proper, well-issuing bundles.
C  * Put the initial ldf8 right at function entry, and additional (predicated)
C    ldf8 in the prologue.  This would shave off many overhead cycles.
C  * Unroll the inner loop more, to save branch overhead on Itanium 1.  Could
C    get closer to 2 c/l that way.

ASM_START()
PROLOGUE(mpn_mul_1)
	.prologue
	.save	ar.pfs, r21
					C    in loc out rot
		alloc		r21 = ar.pfs, 4, 12, 0, 16
	.save	ar.lc, r2
		mov		r2 = ar.lc
		mov		r20 = ar.ec
	.save	pr, r22
		mov		r22 = pr
	.body
ifdef(`HAVE_ABI_32',
`		addp4		r14 = 0, r32
		addp4		r15 = 0, r33
		addp4		r34 = -1, r34
',
`		mov		r14 = r32
		mov		r15 = r33
		adds		r19 = -1, r34		C n - 1
')
		;;
  { .mib;	setf.sig	f6 = r35
		mov		ar.lc = r19
		nop.b		0
} { .mib;	mov		r44 = 0
		mov		r45 = 0
		nop.b		0
} { .mib;	nop.m		0
		mov		ar.ec = 13
		nop.b		0
} { .mib;	cmp.ne		p6, p7 = r0, r0
		mov		pr.rot = 1<<16
		nop.b		0
} { .mib;	mov		r46 = 0
		mov		r47 = 0
		nop.b		0
		;;
}

C Rotating register use:
C f32...f37 ldf8 data in progress
C f38...f41 xma.l results
C f42...f46 xma.hu results
C r32...r35 getf.sig low product results
C r36...r40 getf.sig high product results

							C			insn	fed by
		.align	32				C			name	insn
.Loop:
		.pred.rel "mutex",p6,p7
  { .mfi; (p25)	getf.sig	r36 = f46		C			i0	i4	hi
	  (p21)	xma.l		f38 = f37, f6, f0	C 11 15 19 23 27	i1	i18
	   (p6)	cmp.leu		p8, p9 = r23, r40	C			i2
} { .mfi; (p28)	st8		[r14] =	r23, 8		C			i3
	  (p21)	xma.hu		f42 = f37, f6, f0	C 11 15 19 23 27	i4	i18
	   (p7)	cmp.ltu		p8, p9 = r23, r40	C			i5
		;;
}
		.pred.rel "mutex",p8,p9
  { .mib; (p24)	getf.sig	r32 = f41		C			i6	i11	lo
	   (p8)	add		r23 = r35, r39, 1	C			i7
		nop.b		0			C
} { .mib; (p16)	ldf8		f32 = [r15], 8		C  0  4  8 12 16 20	i8	-
	   (p9)	add		r23 = r35, r39		C			i9
		br.cexit.dpnt.few	.Lend_odd
		;;
}
.Loopm:
		.pred.rel "mutex",p8,p9
  { .mfi; (p25)	getf.sig	r36 = f46		C			i10	i14	hi
	  (p21)	xma.l		f38 = f37, f6, f0	C  9 13 17 21 24	i11	i8
	   (p8)	cmp.leu		p6, p7 = r23, r40	C			i12
} { .mfi; (p28)	st8		[r14] = r23, 8		C			i13
	  (p21)	xma.hu		f42 = f37, f6, f0	C  9 13 17 21 24	i14	i8
	   (p9)	cmp.ltu		p6, p7 = r23, r40	C			i15
		;;
}
		.pred.rel "mutex",p6,p7
  { .mib; (p24)	getf.sig	r32 = f41		C			i16	i1	lo
	   (p6)	add		r23 = r35, r39, 1	C			i17
		nop.b		0			C
} { .mib; (p16)	ldf8		f32 = [r15], 8		C  2  6 10 14 18	i18	-
	   (p7)	add		r23 = r35, r39		C			i19
		br.ctop.dptk.few	.Loop
		;;
}
.Lend_even:
	.pred.rel "mutex",p6,p7
	   (p6)	adds		r8 = 1, r40
	   (p7)	mov		r8 = r40
		mov		pr = r22, 0x1fffe
		mov		ar.lc = r2
		mov		ar.ec = r20
		mov		ar.pfs = r21
		;;
		br.ret.sptk.many b0
.Lend_odd:
	.pred.rel "mutex",p8,p9
	   (p8)	adds		r8 = 1, r40
	   (p9)	mov		r8 = r40
		mov		pr = r22, 0x1fffe
		mov		ar.lc = r2
		mov		ar.ec = r20
		mov		ar.pfs = r21
		;;
		br.ret.sptk.many b0
EPILOGUE(mpn_mul_1)
ASM_END()
