dnl  Itanium-2 mpn_gcd_1 -- mpn by 1 gcd.

dnl  Copyright 2002, 2003 Free Software Foundation, Inc.
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


// itanium2: 6.3 cycles/bitpair (1x1 gcd)


// mpn_gcd_1 (mp_srcptr xp, mp_size_t xsize, mp_limb_t y);
//
// The entry sequence is designed to expect xsize>1 and hence a modexact
// call.  This ought to be more common than a 1x1 operation.  Our critical
// path is thus stripping factors of 2 from y, calling modexact, then
// stripping factors of 2 from the x remainder returned.
//
// The common factors of 2 between x and y must be determined using the
// original x, not the remainder from the modexact.  This is done with
// x_orig which is xp[0].  There's plenty of time to do this while the rest
// of the modexact etc is happening.
//
// It's possible xp[0] is zero.  In this case the trailing zeros calculation
// popc((x-1)&~x) gives 63, and that's clearly no less than what y will
// have, making min(x_twos,y_twos) == y_twos.
//
// The main loop consists of transforming x,y to abs(x-y),min(x,y), and then
// stripping factors of 2 from abs(x-y).  Those factors of two are
// determined from just y-x, without the abs(), since there's the same
// number of trailing zeros on n or -n in twos complement.  That makes the
// dependent chain
//
//	cycles
//	  1    sub     x-y and x-y-1
//	  3    andcm   (x-y-1)&~(x-y)
//	  2    popcnt  trailing zeros
//	  3    shr.u   strip abs(x-y)
//	 ---
//        9
//
// The selection of x-y versus y-x for abs(x-y), and the selection of the
// minimum of x and y, is done in parallel with the above.
//
// The algorithm takes about 0.68 iterations per bit (two N bit operands) on
// average, hence the final 6.3 cycles/bitpair.
//
// The loop is not as fast as one might hope, since there's extra latency
// from andcm going across to the `multimedia' popcnt, and vice versa from
// multimedia shr.u back to the integer sub.
//
// The loop branch is .sptk.clr since we usually expect a good number of
// iterations, and the iterations are data dependent so it's unlikely past
// results will predict anything much about the future.
//
// Not done:
//
// An alternate algorithm which didn't strip all twos, but instead applied
// tbit and predicated extr on x, and then y, was attempted.  The loop was 6
// cycles, but the algorithm is an average 1.25 iterations per bitpair for a
// total 7.25 c/bp, which is slower than the current approach.
//
// Alternatives:
//
// Perhaps we could do something tricky by extracting a few high bits and a
// few low bits from the operands, and looking up a table which would give a
// set of predicates to control some shifts or subtracts or whatever.  That
// could knock off multiple bits per iteration.
//
// The right shifts are a bit of a bottleneck (shr at 2 or 3 cycles, or extr
// only going down I0), perhaps it'd be possible to shift left instead,
// using add.  That would mean keeping track of the lowest not-yet-zeroed
// bit, using some sort of mask.
//

ASM_START()
	.explicit

	.text
	.align	32
PROLOGUE(mpn_gcd_1)

		// r32	xp
		// r33	xsize
		// r34	y

define(x,           r8)
define(xp_orig,     r32)
define(xsize,       r33)
define(y,           r34)  define(inputs, 3)
define(save_rp,     r35)
define(save_pfs,    r36)
define(x_orig,      r37)
define(x_orig_one,  r38)
define(y_twos,      r39)  define(locals, 5)
define(out_xp,      r40)
define(out_xsize,   r41)
define(out_divisor, r42)
define(out_carry,   r43)  define(outputs, 4)

	.prologue
{ .mmi;
ifdef(`HAVE_ABI_32',
`		addp4	r9 = 0, xp_orig   define(xp,r9)',	// M0
`					  define(xp,xp_orig)')
	.save ar.pfs, save_pfs
		alloc	save_pfs = ar.pfs, inputs, locals, outputs, 0 // M2
	.save rp, save_rp
		mov	save_rp = b0		// I0
}{	.body
		add	r10 = -1, y		// M3  y-1
}		;;

{ .mmi;		ld8	x = [xp]		// M0  x = xp[0] if no modexact
		ld8	x_orig = [xp]		// M1  orig x for common twos
		cmp.ne	p6,p0 = 1, xsize	// I0
}{ .mmi;	andcm	y_twos = r10, y		// M2  (y-1)&~y
		mov	out_xp = xp_orig	// M3
		mov	out_xsize = xsize	// I1
}		;;

		mov	out_carry = 0
		
		//

		popcnt	y_twos = y_twos		// I0  y twos
		;;

		//

{ .mmi;		add	x_orig_one = -1, x_orig	// M0  orig x-1
		shr.u	out_divisor = y, y_twos	// I0  y without twos
}{		shr.u	y = y, y_twos		// I1  y without twos
	(p6)	br.call.sptk.many b0 = mpn_modexact_1c_odd  // if xsize>1
}		;;

		// modexact can leave x==0
{ .mmi;		cmp.eq	p6,p0 = 0, x		// M0  if {xp,xsize} % y == 0
		andcm	x_orig = x_orig_one, x_orig	// M1  orig (x-1)&~x
		add	r9 = -1, x		// I0  x-1
}		;;

{ .mmi;		andcm	r9 = r9, x		// M0  (x-1)&~x
		mov	b0 = save_rp		// I0
}		;;

		//

		popcnt	x_orig = x_orig		// I0  orig x twos

		popcnt	r9 = r9			// I0  x twos
		;;

		//

{		cmp.lt	p7,p0 = x_orig, y_twos	// M0  orig x_twos < y_twos
		shr.u	x = x, r9		// I0  x odd
}		;;

{	(p7)	mov	y_twos = x_orig		// M0  common twos
		add	r10 = -1, y		// I0  y-1
	(p6)	br.dpnt.few .Ldone_y		// B0  x%y==0 then result y
}		;;

		//


		// No noticable difference in speed for the loop aligned to
		// 32 or just 16.
.Ltop3:
		// r8	x
		// r10  y-1
		// r34	y
		// r38	common twos, for use at end

{  .mmi;	cmp.gtu	p8,p9 = x, y	// M0  x>y
		cmp.ne	p10,p0 = x, y	// M1  x==y
		sub	r9 = y, x	// I0  d = y - x
}{ .mmi;	sub	r10 = r10, x	// M2  d-1 = y - x - 1
}		;;

{ .mmi;	.pred.rel "mutex", p8, p9
	(p8)	sub	x = x, y	// M0  x>y  use x=x-y, y unchanged
	(p9)	mov	y = x		// M1  y>=x use y=x
	(p9)	mov	x = r9		// I0  y>=x use x=y-x
}{ .mmi;	andcm	r9 = r10, r9	// M2  (d-1)&~d
		;;

		add	r10 = -1, y	// M0  new y-1
		popcnt	r9 = r9		// I0  twos on x-y
}		;;

{		shr.u	x = x, r9	// I0   new x without twos
	(p10)	br.sptk.few.clr .Ltop3
}		;;



		// result is y
.Ldone_y:
		shl	r8 = y, y_twos		// I   common factors of 2
		;;
		mov	ar.pfs = save_pfs	// I0
		br.ret.sptk.many b0

EPILOGUE()
