dnl  Itanium-2 mpn_hamdist -- mpn hamming distance.

dnl  Copyright 2003 Free Software Foundation, Inc.
dnl
dnl  This file is part of the GNU MP Library.
dnl
dnl  The GNU MP Library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Lesser General Public License as
dnl  published by the Free Software Foundation; either version 2.1 of the
dnl  License, or (at your option) any later version.
dnl
dnl  The GNU MP Library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Lesser General Public License for more details.
dnl
dnl  You should have received a copy of the GNU Lesser General Public
dnl  License along with the GNU MP Library; see the file COPYING.LIB.  If
dnl  not, write to the Free Software Foundation, Inc., 59 Temple Place -
dnl  Suite 330, Boston, MA 02111-1307, USA.

include(`../config.m4')


C            cycles/limb
C itanium-2:    1.0           from L1 cache
C               1.875-2.3125  from L2, depending on operand alignments


C mpn_hamdist (mp_srcptr xp, mp_srcptr yp, mp_size_t size)
C
C The loads are scheduled for an L2 hit of 5 cycles.  But unfortunately we
C don't address the fact that L2 is not true dual ported and requires
C different address bits 7:4.  Even when those are forced to be different
C (eg. tune/speed.c run with -x2), we only seem to get 1.875, so perhaps
C there's more to it than that.  The docs say L2 can sustain reads of
C 4x8bytes per cycle, so presumably throughput isn't the limitation.
C
C The ar.ec epilogue count is deliberately shortened and the last three
C steps done explicitly at the end.  These are just adds and can be nicely
C paired with the register restores.
C
C 32-byte alignment on the loop is essential for the claimed speed, since we
C need 2 bundles fetched from there in 1 cycle.
C
C Enhancements:
C
C It'd be possible to reduce the loop count and do the first few iterations
C (ie. loads) in parallel with the initial register setups.  ar.lc would be
C size-N (or 0 if size<N, and ar.ec reduced in that case too).  pr would get
C min(size,N) bits set.  The gain would be at most 5 cycles, since that's
C the time now until the first load.  Perhaps not worth the trouble.
C

ASM_START()
		.explicit

PROLOGUE(mpn_hamdist)

		C r32	xp
		C r33	yp
		C r34	size

define(xp_param, r32)
define(yp_param, r33)
define(siz,      r34)

define(save_pfs, r14)
define(save_lc,  r15)
define(save_pr,  r16)
define(xp,       r17)
define(yp,       r18)

	.prologue
ifdef(`HAVE_ABI_32',
`.mmi;		addp4	xp_param = 0, xp_param	C M
		addp4	yp_param = 0, yp_param	C M
		sxt4	siz = siz		C I
		;;
')

.mmi;		mov	xp = xp_param		C M0
		mov	yp = yp_param		C M1
	.save	ar.lc, save_lc
		mov	save_lc = ar.lc		C I0
.mmi;	.save	ar.pfs, save_pfs
		alloc	save_pfs = ar.pfs, 2,22,0,24	C M2
		add	siz = -1, siz		C M3  size-1
		;;

		mov	r8 = 0			C M0  total
		mov	ar.lc = siz		C I0  size-2

	.save	pr, save_pr
		mov	save_pr = pr		C I0
	.body

		mov	ar.ec = 9		C I0

		mov	pr.rot = 1<<16		C I0  p16,p17 true
		;;

		C Stages:
		C
		C p16 r32	load from x
		C p17 r33	.
		C p18 r34	.
		C p19 r35	.
		C p20 r36	.
		C p21 r37	xor x,y
		C p22 r38	.
		C p23 r39	.
		C p24 r40	popcount xor
		C p25 r41	.
		C p26 r42	.
		C p27 r43	add to total
		C
		C     r44	load from y
		C     r45	.
		C     r46	.
		C     r47	.
		C     r48	.
		C     r49	xor above
		C
		C total 18 regs

		.align	32
.Ltop:
.mmi;	(p16)	ld8	r32 = [xp], 8		C M0
	(p16)	ld8	r44 = [yp], 8		C M1
	(p24)	popcnt	r40 = r40		C I0
.mib;	(p21)	xor	r37 = r37, r49		C M2
	(p27)	add	r8 = r8, r43		C I1
		br.cexit.spnt.few.clr .Ldone	C B0
		;;

.mmi;	(p16)	ld8	r32 = [xp], 8		C M0
	(p16)	ld8	r44 = [yp], 8		C M1
	(p24)	popcnt	r40 = r40		C I0
.mib;	(p21)	xor	r37 = r37, r49		C M2
	(p27)	add	r8 = r8, r43		C I1
		br.ctop.sptk.few.clr .Ltop	C B0
		;;

.Ldone:
		ASSERT(p25)			C last limb of source data

.mmi;	(p27)	add	r8 = r8, r43		C M0
		mov	ar.lc = save_lc		C I0
		;;

.mmi;	(p26)	add	r8 = r8, r42		C M0
		mov	pr = save_pr, 0xfffffffffffffffe C I0
		;;

		add	r8 = r8, r41		C M0
		mov	ar.pfs = save_pfs	C I0
		br.ret.sptk.many b0

EPILOGUE()
ASM_END()
