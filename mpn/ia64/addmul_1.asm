dnl  IA-64 mpn_addmul_1 -- Multiply a limb vector with a limb and add
dnl  the result to a second limb vector.

dnl  Copyright (C) 2000, 2001 Free Software Foundation, Inc.

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
C n  = r34
C vl = r35

C         cycles/limb
C Itanium:    3.0
C Itanium 2:  2.0

C See README for general coding principles and why modulo scheduled loops
C aren't utilized.  This code is a compromise between speed and code size, in
C that the feed-in code is somewhat sub-optimal for n mod 4 != 0.  Doing it
C optimally would require duplicating most of the current feed-in code.

C It is possible to do mpn_addmul_1 in just 2 c/l also for Itanium 1, but that
C would require the use of ldpf8 for all loads.  Since that instruction require
C 16-byte alignment, two loop variants would be necessary.

ASM_START()
PROLOGUE(mpn_addmul_1)
	.prologue
	.save	ar.lc, r2
		mov		r2 = ar.lc
	.body
ifdef(`HAVE_ABI_32',
`		addp4		r32 = 0, r32
		addp4		r33 = 0, r33
		zxt4		r34 = r34
		;;
')
		ldf8		f32 = [r33], 8		C load up[0] early
		ldf8		f33 = [r32], 8		C load rp[0] early
		;;
		and		r14 = 3, r34		C n mod 4
		cmp.ltu		p15, p0 = 4, r34	C is n < 4?
		add		r34 = -4, r34		C
		;;
		shr		r15 = r34, 2		C [n / 4] - 1
		cmp.eq		p8, p0 = 1, r14		C
		cmp.eq		p10, p0 = 2, r14	C
		cmp.eq		p12, p0 = 3, r14	C
		;;
		mov		ar.lc = r15		C lc = [n / 4] - 1
		adds		r20 = -8, r32		C
		setf.sig	f6 = r35		C
		;;
	   (p8)	br.dptk		.Lb01			C branch for n = 1, 5, ...
	  (p10)	br.dptk		.Lb10			C branch for n = 2, 6, ...
	  (p12)	br.dptk		.Lb11			C branch for n = 3, 7, ...
		mov		r31 = 0			C fall thru for n = 4, 8, ...
		br.sptk		.Lb00

.Lb01:
		xma.l		f34 = f32, f6, f33	C
		xma.hu		f35 = f32, f6, f33	C
		;;
		stf8		[r20] = f34, 8		C store lowest limb
		getf.sig	r31 = f35		C
	  (p15)	br.sptk		.L_sw_pipeline
		;;
		mov		r8 = r31		C
		mov		ar.lc = r2		C
		br.ret.sptk.many rp
.Lb10:
		ldf8		f36 = [r33], 8		C
		ldf8		f37 = [r32], 8		C
		;;
		xma.l		f34 = f32, f6, f33	C
		xma.hu		f35 = f32, f6, f33	C
		;;
		xma.l		f38 = f36, f6, f37	C
		xma.hu		f39 = f36, f6, f37	C
		;;
		getf.sig	r28 = f35		C
		getf.sig	r25 = f38		C
		getf.sig	r31 = f39		C
		;;
		add		r14 = r28, r25		C
		;;
		cmp.ltu		p8, p9 = r14, r25	C
		stf8		[r20] = f34, 8		C store lowest limb
		;;
		st8		[r20] = r14, 8		C
	   (p8)	add		r31 = r31, r0, 1
	  (p15)	br.sptk		.L_sw_pipeline
		;;
		mov		r8 = r31		C
		mov		ar.lc = r2		C
		br.ret.sptk.many rp

.Lb11:
		ldf8		f36 = [r33], 8		C
		ldf8		f37 = [r32], 8		C
		;;
		ldf8		f40 = [r33], 8		C
		ldf8		f41 = [r32], 8		C
		;;
		xma.l		f34 = f32, f6, f33	C
		xma.hu		f35 = f32, f6, f33	C
		;;
		xma.l		f38 = f36, f6, f37	C
		xma.hu		f39 = f36, f6, f37	C
		;;
		xma.l		f42 = f40, f6, f41	C
		xma.hu		f43 = f40, f6, f41	C
		;;
		stf8		[r20] = f34, 8		C store lowest limb
		getf.sig	r28 = f35		C
		getf.sig	r25 = f38		C
		getf.sig	r29 = f39		C
		getf.sig	r26 = f42		C
		getf.sig	r31 = f43		C
		;;
		add		r14 = r28, r25		C
		;;
		cmp.ltu		p8, p9 = r14, r25	C
		st8		[r20] = r14, 8		C
		;;
		.pred.rel "mutex", p8, p9
	   (p8)	add		r16 = r29, r26, 1	C
	   (p9)	add		r16 = r29, r26		C
		;;
		.pred.rel "mutex", p8, p9
	   (p8)	cmp.leu		p6, p7 = r16, r26	C
	   (p9)	cmp.ltu		p6, p7 = r16, r26	C
		st8		[r20] = r16, 8		C
		;;
	   (p6)	add		r31 = r31, r0, 1	C
	  (p15)	br.sptk		.L_sw_pipeline
		;;
		mov		r8 = r31		C
		mov		ar.lc = r2		C
		br.ret.sptk.many rp

.L_sw_pipeline:
		ldf8		f32 = [r33], 8		C
		ldf8		f33 = [r32], 8		C
		;;
.Lb00:		ldf8		f36 = [r33], 8		C
		ldf8		f37 = [r32], 8		C
		;;
		ldf8		f40 = [r33], 8		C
		ldf8		f41 = [r32], 8		C
		;;
		ldf8		f44 = [r33], 8		C
		br.cloop.dptk	.Lph1			C taken means that n > 4
		;;					C else finish up and return
		xma.l		f34 = f32, f6, f33	C
		ldf8		f45 = [r32], 8		C
		xma.hu		f35 = f32, f6, f33	C
		;;
		xma.l		f38 = f36, f6, f37	C
		xma.hu		f39 = f36, f6, f37	C
		;;
		xma.l		f42 = f40, f6, f41	C
		xma.hu		f43 = f40, f6, f41	C
		;;
		getf.sig	r24 = f34		C
		;;
		getf.sig	r28 = f35		C
		xma.l		f46 = f44, f6, f45	C
		xma.hu		f47 = f44, f6, f45	C
		;;
		getf.sig	r25 = f38		C
		getf.sig	r29 = f39		C
		getf.sig	r26 = f42		C
		;;
		getf.sig	r30 = f43		C
		add		r16 = r31, r24		C
		;;
		cmp.ltu		p6, p7 = r16, r24	C
		getf.sig	r27 = f46		C
		st8		[r20] = r16, 8		C
		br.sptk		.Lcj2			C
		;;
.Lph1:		xma.l		f34 = f32, f6, f33	C
		ldf8		f45 = [r32], 8		C
		xma.hu		f35 = f32, f6, f33	C
		;;
		ldf8		f32 = [r33], 8		C
		;;
		xma.l		f38 = f36, f6, f37	C
		ldf8		f33 = [r32], 8		C
		xma.hu		f39 = f36, f6, f37	C
		;;
		ldf8		f36 = [r33], 8		C
		;;
		xma.l		f42 = f40, f6, f41	C
		ldf8		f37 = [r32], 8		C
		xma.hu		f43 = f40, f6, f41	C
		;;
		ldf8		f40 = [r33], 8		C
		getf.sig	r24 = f34		C
		;;
		getf.sig	r28 = f35		C
		xma.l		f46 = f44, f6, f45	C
		ldf8		f41 = [r32], 8		C
		xma.hu		f47 = f44, f6, f45	C
		;;
		ldf8		f44 = [r33], 8		C
		getf.sig	r25 = f38		C
		br.cloop.dptk	.Lph2			C taken means that n > 8
		;;					C else finish up and return
		getf.sig	r29 = f39		C
		xma.l		f34 = f32, f6, f33	C
		ldf8		f45 = [r32], 8		C
		xma.hu		f35 = f32, f6, f33	C
		;;
		getf.sig	r26 = f42		C
		;;
		getf.sig	r30 = f43		C
		xma.l		f38 = f36, f6, f37	C
		xma.hu		f39 = f36, f6, f37	C
		add		r16 = r31, r24		C
		;;
		cmp.ltu		p6, p7 = r16, r24	C
		getf.sig	r27 = f46		C
		st8		[r20] = r16, 8		C
		br.sptk		.Lcj1
		;;
.Lph2:		getf.sig	r29 = f39		C
		xma.l		f34 = f32, f6, f33	C
		ldf8		f45 = [r32], 8		C
		xma.hu		f35 = f32, f6, f33	C
		;;
		ldf8		f32 = [r33], 8		C
		getf.sig	r26 = f42		C
		;;
		getf.sig	r30 = f43		C
		xma.l		f38 = f36, f6, f37	C
		ldf8		f33 = [r32], 8		C
		xma.hu		f39 = f36, f6, f37	C
		add		r16 = r31, r24		C
		;;
		ldf8		f36 = [r33], 8		C
		cmp.ltu		p6, p7 = r16, r24	C
		getf.sig	r27 = f46		C
		st8		[r20] = r16, 8		C
		;;
		.pred.rel "mutex", p6, p7
		getf.sig	r31 = f47		C
		xma.l		f42 = f40, f6, f41	C
	   (p6)	add		r14 = r28, r25, 1	C
		ldf8		f37 = [r32], 8		C
		xma.hu		f43 = f40, f6, f41	C
	   (p7)	add		r14 = r28, r25		C
		;;
		.pred.rel "mutex", p6, p7
		ldf8		f40 = [r33], 8		C
	   (p6)	cmp.leu		p8, p9 = r14, r25	C
	   (p7)	cmp.ltu		p8, p9 = r14, r25	C
		getf.sig	r24 = f34		C
		st8		[r20] = r14, 8		C
		;;
		.pred.rel "mutex", p8, p9
		getf.sig	r28 = f35		C
		xma.l		f46 = f44, f6, f45	C
	   (p8)	add		r16 = r29, r26, 1	C
		ldf8		f41 = [r32], 8		C
		xma.hu		f47 = f44, f6, f45	C
	   (p9)	add		r16 = r29, r26		C
		;;
		.pred.rel "mutex", p8, p9
		ldf8		f44 = [r33], 8		C
	   (p8)	cmp.leu		p6, p7 = r16, r26	C
	   (p9)	cmp.ltu		p6, p7 = r16, r26	C
		getf.sig	r25 = f38		C
		st8		[r20] = r16, 8		C
		br.cloop.dptk	.Loop			C
		;;
		br.sptk.many	.Le0


	.align 32					C insn	fed	prod	cycle number
.Loop:	C *** MAIN LOOP START ***			C name 	by	part	itan1	itan2
		.pred.rel "mutex", p6, p7
  { .mfi;	getf.sig	r29 = f39		C i00	i16	hi	0	0
		xma.l		f34 = f32, f6, f33	C i01	i06,i15		0	0
	   (p6)	add		r14 = r30, r27, 1	C i02			0	0
} { .mfi;	ldf8		f45 = [r32], 8		C i03			0	0
		xma.hu		f35 = f32, f6, f33	C i04	i06,i15		0	0
	   (p7)	add		r14 = r30, r27	;;	C i05			0	0
}
		.pred.rel "mutex", p6, p7
  { .mii;	ldf8		f32 = [r33], 8		C i06			1	1
	   (p6)	cmp.leu		p8, p9 = r14, r27	C i07			1	1
	   (p7)	cmp.ltu		p8, p9 = r14, r27	C i08			1	1
} { .mmb;	getf.sig	r26 = f42		C i09	i25	lo	2	1
		st8		[r20] = r14, 8		C i10			2	1
		nop.b		0		;;	C i11			2	1
}
		.pred.rel "mutex", p8, p9
  { .mfi;	getf.sig	r30 = f43		C i12	i28	hi	3	2
		xma.l		f38 = f36, f6, f37	C i13	i18,i27		3	2
	   (p8)	add		r16 = r31, r24, 1	C i14			3	2
} { .mfi;	ldf8		f33 = [r32], 8		C i15			3	2
		xma.hu		f39 = f36, f6, f37	C i16	i18,i27		3	2
	   (p9)	add		r16 = r31, r24	;;	C i17			3	2
}
		.pred.rel "mutex", p8, p9
  { .mii;	ldf8		f36 = [r33], 8		C i18			4	3
	   (p8)	cmp.leu		p6, p7 = r16, r24	C i19			4	3
	   (p9)	cmp.ltu		p6, p7 = r16, r24	C i20			4	3
} { .mmb;	getf.sig	r27 = f46		C i21	i37	lo	5	3
		st8		[r20] = r16, 8		C i22			5	3
		nop.b		0		;;	C i23			5	3
}
		.pred.rel "mutex", p6, p7
  { .mfi;	getf.sig	r31 = f47		C i24	i40	hi	6	4
		xma.l		f42 = f40, f6, f41	C i25	i30,i39		6	4
	   (p6)	add		r14 = r28, r25, 1	C i26			6	4
} { .mfi;	ldf8		f37 = [r32], 8		C i27			6	4
		xma.hu		f43 = f40, f6, f41	C i28	i30,i39		6	4
	   (p7)	add		r14 = r28, r25	;;	C i29			6	4
}
		.pred.rel "mutex", p6, p7
  { .mii;	ldf8		f40 = [r33], 8		C i30			7	5
	   (p6)	cmp.leu		p8, p9 = r14, r25	C i31			7	5
	   (p7)	cmp.ltu		p8, p9 = r14, r25	C i32			7	5
} { .mmb;	getf.sig	r24 = f34		C i33	i01	lo	8	5
		st8		[r20] = r14, 8		C i34			8	5
		nop.b		0		;;	C i35			8	5
}
		.pred.rel "mutex", p8, p9
  { .mfi;	getf.sig	r28 = f35		C i36	i04	hi	9	6
		xma.l		f46 = f44, f6, f45	C i37	i42,i03		9	6
	   (p8)	add		r16 = r29, r26, 1	C i38			9	6
} { .mfi;	ldf8		f41 = [r32], 8		C i39			9	6
		xma.hu		f47 = f44, f6, f45	C i40	i42,i03		9	6
	   (p9)	add		r16 = r29, r26	;;	C i41			9	6
}
		.pred.rel "mutex", p8, p9
  { .mii;	ldf8		f44 = [r33], 8		C i42		       10	7
	   (p8)	cmp.leu		p6, p7 = r16, r26	C i43		       10	7
	   (p9)	cmp.ltu		p6, p7 = r16, r26	C i44		       10	7
} { .mmb;	getf.sig	r25 = f38		C i45	i13	lo     11	7
		st8		[r20] = r16, 8		C i46		       11	7
		br.cloop.dptk	.Loop		;;	C i47		       11	7
}	C *** MAIN LOOP END ***

.Le0:
		.pred.rel "mutex", p6, p7
		getf.sig	r29 = f39		C
		xma.l		f34 = f32, f6, f33	C
	   (p6)	add		r14 = r30, r27, 1	C
		ldf8		f45 = [r32], 8		C
		xma.hu		f35 = f32, f6, f33	C
	   (p7)	add		r14 = r30, r27		C
		;;
		.pred.rel "mutex", p6, p7
	   (p6)	cmp.leu		p8, p9 = r14, r27	C
	   (p7)	cmp.ltu		p8, p9 = r14, r27	C
		getf.sig	r26 = f42		C
		st8		[r20] = r14, 8		C
		;;
		.pred.rel "mutex", p8, p9
		getf.sig	r30 = f43		C
		xma.l		f38 = f36, f6, f37	C
	   (p8)	add		r16 = r31, r24, 1	C
		xma.hu		f39 = f36, f6, f37	C
	   (p9)	add		r16 = r31, r24		C
		;;
		.pred.rel "mutex", p8, p9
	   (p8)	cmp.leu		p6, p7 = r16, r24	C
	   (p9)	cmp.ltu		p6, p7 = r16, r24	C
		getf.sig	r27 = f46		C
		st8		[r20] = r16, 8		C
		;;
.Lcj1:
		.pred.rel "mutex", p6, p7
		getf.sig	r31 = f47		C
		xma.l		f42 = f40, f6, f41	C
	   (p6)	add		r14 = r28, r25, 1	C
		xma.hu		f43 = f40, f6, f41	C
	   (p7)	add		r14 = r28, r25		C
		;;
		.pred.rel "mutex", p6, p7
	   (p6)	cmp.leu		p8, p9 = r14, r25	C
	   (p7)	cmp.ltu		p8, p9 = r14, r25	C
		getf.sig	r24 = f34		C
		st8		[r20] = r14, 8		C
		;;
		.pred.rel "mutex", p8, p9
		getf.sig	r28 = f35		C
		xma.l		f46 = f44, f6, f45	C
	   (p8)	add		r16 = r29, r26, 1	C
		xma.hu		f47 = f44, f6, f45	C
	   (p9)	add		r16 = r29, r26		C
		;;
		.pred.rel "mutex", p8, p9
	   (p8)	cmp.leu		p6, p7 = r16, r26	C
	   (p9)	cmp.ltu		p6, p7 = r16, r26	C
		getf.sig	r25 = f38		C
		st8		[r20] = r16, 8		C
		;;
		.pred.rel "mutex", p6, p7
		getf.sig	r29 = f39		C
	   (p6)	add		r14 = r30, r27, 1	C
	   (p7)	add		r14 = r30, r27		C
		;;
		.pred.rel "mutex", p6, p7
	   (p6)	cmp.leu		p8, p9 = r14, r27	C
	   (p7)	cmp.ltu		p8, p9 = r14, r27	C
		getf.sig	r26 = f42		C
		st8		[r20] = r14, 8		C
		;;
		.pred.rel "mutex", p8, p9
		getf.sig	r30 = f43		C
	   (p8)	add		r16 = r31, r24, 1	C
	   (p9)	add		r16 = r31, r24		C
		;;
		.pred.rel "mutex", p8, p9
	   (p8)	cmp.leu		p6, p7 = r16, r24	C
	   (p9)	cmp.ltu		p6, p7 = r16, r24	C
		getf.sig	r27 = f46		C
		st8		[r20] = r16, 8		C
		;;
.Lcj2:
		.pred.rel "mutex", p6, p7
		getf.sig	r31 = f47		C
	   (p6)	add		r14 = r28, r25, 1	C
	   (p7)	add		r14 = r28, r25		C
		;;
		.pred.rel "mutex", p6, p7
	   (p6)	cmp.leu		p8, p9 = r14, r25	C
	   (p7)	cmp.ltu		p8, p9 = r14, r25	C
		st8		[r20] = r14, 8		C
		;;
		.pred.rel "mutex", p8, p9
	   (p8)	add		r16 = r29, r26, 1	C
	   (p9)	add		r16 = r29, r26		C
		;;
		.pred.rel "mutex", p8, p9
	   (p8)	cmp.leu		p6, p7 = r16, r26	C
	   (p9)	cmp.ltu		p6, p7 = r16, r26	C
		st8		[r20] = r16, 8		C
		;;
		.pred.rel "mutex", p6, p7
	   (p6)	add		r14 = r30, r27, 1	C
	   (p7)	add		r14 = r30, r27		C
		;;
		.pred.rel "mutex", p6, p7
	   (p6)	cmp.leu		p8, p9 = r14, r27	C
	   (p7)	cmp.ltu		p8, p9 = r14, r27	C
		st8		[r20] = r14, 8		C
		;;
		.pred.rel "mutex", p8, p9
	   (p8)	add		r8 = r31, r0, 1		C
	   (p9)	add		r8 = r31, r0		C
		mov		ar.lc = r2		C
		br.ret.sptk.many rp
EPILOGUE(mpn_addmul_1)
ASM_END()
