dnl  IA-64 mpn_sub_n -- Subtract two limb vectors of the same length > 0 and
dnl  store difference in a third limb vector.

dnl  Copyright (C) 2001, 2002 Free Software Foundation, Inc.

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
C vp = r34
C n = r35

C         cycles/limb
C Itanium:    3.0
C Itanium 2:  2.0

ASM_START()
PROLOGUE(mpn_sub_n)
	.prologue
	.save	ar.lc, r2
		mov	r2 = ar.lc
	.body
ifdef(`HAVE_ABI_32',
`		addp4		r32 = 0, r32
		addp4		r33 = 0, r33
		addp4		r34 = 0, r34
		zxt4		r35 = r35
		;;
')
		and	r14 = 3, r35
		cmp.ge	p14, p15 = 4, r35
		add	r35 = -4, r35
		;;
		cmp.eq	p8, p9 = 1, r14
		cmp.eq	p10, p11 = 2, r14
		cmp.eq	p12, p13 = 3, r14
		;;
	  (p8)	br.dptk	.Lb01
	  (p10)	br.dptk	.Lb10
	  (p12)	br.dptk	.Lb11

.Lb00:	C  n = 4, 8, 12, ...
		ld8	r16 = [r34], 8
		ld8	r20 = [r33], 8
		shr	r15 = r35, 2
		;;
		ld8	r17 = [r34], 8
		ld8	r21 = [r33], 8
		mov	ar.lc = r15
		;;
		ld8	r18 = [r34], 8
		ld8	r22 = [r33], 8
		;;
		ld8	r19 = [r34], 8
		ld8	r23 = [r33], 8
		cmp.ne	p6, p7 = r0, r0
	  (p14)	br.sptk	.Ls00
		;;
  { .mii;	sub	r24 = r20, r16
		;;
} { .mib;	cmp.ltu	p8, p9 = r20, r24
} { .mmb;	ld8	r16 = [r34], 8
		ld8	r20 = [r33], 8
		br.cloop.dptk	.Li00
		;;
}

.Lb01:	C  n = 1, 5, 9, 13, ...
		ld8	r19 = [r34], 8
		ld8	r23 = [r33], 8
		shr	r15 = r35, 2
		cmp.ne	p8, p9 = r0, r0
	  (p14)	br.sptk	.Ls01
		;;
		ld8	r16 = [r34], 8
		ld8	r20 = [r33], 8
		mov	ar.lc = r15
		;;
		ld8	r17 = [r34], 8
		ld8	r21 = [r33], 8
		;;
		ld8	r18 = [r34], 8
		ld8	r22 = [r33], 8
		;;
  { .mii;	sub	r24 = r23, r19
		;;
} { .mib;	cmp.ltu	p6, p7 = r23, r24
} { .mmb;	ld8	r19 = [r34], 8
		ld8	r23 = [r33], 8
		br.cloop.dptk	.Li01
		;;
}
		br.sptk	.Lend

.Lb10:	C  n = 2, 6, 10, 14, ...
		ld8	r18 = [r34], 8
		ld8	r22 = [r33], 8
		shr	r15 = r35, 2
		;;
		ld8	r19 = [r34], 8
		ld8	r23 = [r33], 8
		mov	ar.lc = r15
		cmp.ne	p6, p7 = r0, r0
	  (p14)	br.sptk	.Ls10
		;;
		ld8	r16 = [r34], 8
		ld8	r20 = [r33], 8
		;;
		ld8	r17 = [r34], 8
		ld8	r21 = [r33], 8
		;;
  { .mii;	sub	r24 = r22, r18
		;;
} { .mib;	cmp.ltu	p8, p9 = r22, r24
} { .mmb;	ld8	r18 = [r34], 8
		ld8	r22 = [r33], 8
		br.sptk	.Li10
		;;
}

.Lb11:	C  n = 3, 7, 11, 15, ...
		ld8	r17 = [r34], 8
		ld8	r21 = [r33], 8
		shr	r15 = r35, 2
		;;
		ld8	r18 = [r34], 8
		ld8	r22 = [r33], 8
		mov	ar.lc = r15
		;;
		ld8	r19 = [r34], 8
		ld8	r23 = [r33], 8
		cmp.ne	p8, p9 = r0, r0
	  (p14)	br.sptk	.Ls11
		;;
		ld8	r16 = [r34], 8
		ld8	r20 = [r33], 8
		;;
  { .mii;	sub	r24 = r21, r17
		;;
} { .mib;	cmp.ltu	p6, p7 = r21, r24
} { .mmb;	ld8	r17 = [r34], 8
		ld8	r21 = [r33], 8
		br.sptk	.Li11
		;;
}

		.align	32
.Loop:	C *** MAIN LOOP ***
.Li01:
	.pred.rel "mutex", p6, p7
  { .mii;	st8	[r32] = r24, 8
	   (p6)	sub	r24 = r20, r16, 1
	   (p7)	sub	r24 = r20, r16
		;;
}
	.pred.rel "mutex", p6, p7
  { .mib;  (p6)	cmp.leu	p8, p9 = r20, r24
	   (p7)	cmp.ltu	p8, p9 = r20, r24
} { .mmb;	ld8	r16 = [r34], 8
		ld8	r20 = [r33], 8
		;;
}
.Li00:
	.pred.rel "mutex", p8, p9
  { .mii;	st8	[r32] = r24, 8
	   (p8)	sub	r24 = r21, r17, 1
	   (p9)	sub	r24 = r21, r17
		;;
}
	.pred.rel "mutex", p8, p9
  { .mib;  (p8)	cmp.leu	p6, p7 = r21, r24
	   (p9)	cmp.ltu	p6, p7 = r21, r24
} { .mmb;	ld8	r17 = [r34], 8
		ld8	r21 = [r33], 8
		;;
}
.Li11:
	.pred.rel "mutex", p6, p7
  { .mii;	st8	[r32] = r24, 8
	   (p6)	sub	r24 = r22, r18, 1
	   (p7)	sub	r24 = r22, r18
		;;
}
	.pred.rel "mutex", p6, p7
  { .mib;  (p6)	cmp.leu	p8, p9 = r22, r24
	   (p7)	cmp.ltu	p8, p9 = r22, r24
} { .mmb;	ld8	r18 = [r34], 8
		ld8	r22 = [r33], 8
		;;
}
.Li10:
	.pred.rel "mutex", p8, p9
  { .mii;	st8	[r32] = r24, 8
	   (p8)	sub	r24 = r23, r19, 1
	   (p9)	sub	r24 = r23, r19
		;;
}
	.pred.rel "mutex", p8, p9
  { .mib;  (p8)	cmp.leu	p6, p7 = r23, r24
	   (p9)	cmp.ltu	p6, p7 = r23, r24
} { .mmb;	ld8	r19 = [r34], 8
		ld8	r23 = [r33], 8
		br.cloop.dptk .Loop
		;;
}
.Lend:

.Ls00:
	.pred.rel "mutex", p6, p7
  { .mii; (p15)	st8	[r32] = r24, 8
	   (p6)	sub	r24 = r20, r16, 1
	   (p7)	sub	r24 = r20, r16
} { .mfb;	cmp.ne	p14, p15 = r0, r0
		;;
}
	.pred.rel "mutex", p6, p7
  { .mii;  (p6)	cmp.leu	p8, p9 = r20, r24
	   (p7)	cmp.ltu	p8, p9 = r20, r24
		;;
}
.Ls11:
	.pred.rel "mutex", p8, p9
  { .mii; (p15)	st8	[r32] = r24, 8
	   (p8)	sub	r24 = r21, r17, 1
	   (p9)	sub	r24 = r21, r17
} { .mfb;	cmp.ne	p14, p15 = r0, r0
		;;
}
	.pred.rel "mutex", p8, p9
  { .mib;  (p8)	cmp.leu	p6, p7 = r21, r24
	   (p9)	cmp.ltu	p6, p7 = r21, r24
		;;
}
.Ls10:
	.pred.rel "mutex", p6, p7
  { .mii; (p15)	st8	[r32] = r24, 8
	   (p6)	sub	r24 = r22, r18, 1
	   (p7)	sub	r24 = r22, r18
} { .mfb;	cmp.ne	p14, p15 = r0, r0
		;;
}
	.pred.rel "mutex", p6, p7
  { .mib;  (p6)	cmp.leu	p8, p9 = r22, r24
	   (p7)	cmp.ltu	p8, p9 = r22, r24
		;;
}
.Ls01:
	.pred.rel "mutex", p8, p9
  { .mii; (p15)	st8	[r32] = r24, 8
	   (p8)	sub	r24 = r23, r19, 1
	   (p9)	sub	r24 = r23, r19
		;;
}
	.pred.rel "mutex", p8, p9
  { .mib;  (p8)	cmp.leu	p6, p7 = r23, r24
	   (p9)	cmp.ltu	p6, p7 = r23, r24
		;;
}
	.pred.rel "mutex", p6, p7
		st8	[r32] = r24
	   (p6)	add	r8 = 1, r0
	   (p7)	add	r8 = r0, r0
		mov.i	ar.lc = r2
		br.ret.sptk.many rp
EPILOGUE(mpn_sub_n)
ASM_END()
