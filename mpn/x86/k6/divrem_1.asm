# AMD K6 mpn_divrem_1 -- mpn by limb division with fraction.
#
# K6: 20 cycles/limb.


# Copyright (C) 1999-2000 Free Software Foundation, Inc.
#
# This file is part of the GNU MP Library.
#
# The GNU MP Library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Library General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# The GNU MP Library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
# License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


include(`../config.m4')


# mp_limb_t mpn_divrem_1 (mp_ptr dst, mp_size_t xsize,
#                         mp_srcptr src, mp_size_t size, mp_limb_t divisor);
#
# Divide src,size by divisor and store the quotient in dst+xsize,size.
# Extend the division to fractional quotient limbs in dst,xsize.  Return the
# remainder.  xsize can be 0, but size must be at least 1.
#
# A test is done to see if the high limb is less the the divisor, and if so
# then one less div is done.  A div is 20 cycles, so assuming high<divisor
# about half the time, then this test saves 10 cycles on average.
#  	
# Essentially this code is the same as mpn/generic/divrem_1.c and the
# division based part of mpn/generic/divmod_1.c, but register usage and
# instruction scheduling is better than the compiler will probably give.
#
# Back-to-back div instructions run at 20 cycles, the same as the loop here,
# so it seems there's nothing to gain by unrolling.

defframe(PARAM_DIVISOR,20)
defframe(PARAM_SIZE,   16)
defframe(PARAM_SRC,    12)
defframe(PARAM_XSIZE,  8)
defframe(PARAM_DST,    4)

	.text
	ALIGN(32)

PROLOGUE(mpn_divrem_1)
deflit(`FRAME',0)

	movl	PARAM_SIZE, %ecx
	pushl	%ebx
FRAME_pushl()
	
	movl	PARAM_SRC, %ebx
	pushl	%esi
FRAME_pushl()

	movl	PARAM_DIVISOR, %esi
	pushl	%edi
FRAME_pushl()

	movl	-4(%ebx,%ecx,4), %eax	# src high limb
	pushl	%ebp
FRAME_pushl()

	movl	PARAM_DST, %edi
	xorl	%edx, %edx

	movl	PARAM_XSIZE, %ebp
	cmpl	%esi, %eax

	leal	-4(%edi,%ebp,4), %edi	# dst one limb below integer part
	jae	L(integer_entry)


	# high<divisor, so high of dst is zero, and avoid one div

	movl	$0, (%edi,%ecx,4)
	decl	%ecx

	movl	%eax, %edx
	jz	L(fraction)


L(integer_top):
	# eax	scratch (quotient)
	# ebx	src
	# ecx	counter
	# edx	scratch (remainder)
	# esi	divisor
	# edi	dst+4*xsize-4
	# ebp

	movl	-4(%ebx,%ecx,4), %eax
L(integer_entry):
	divl	%esi

	movl	%eax, (%edi,%ecx,4)
	loop	L(integer_top)


L(fraction):
	orl	%ebp, %ecx
	jz	L(done)

	movl	PARAM_DST, %edi


L(fraction_top):
	# eax	scratch (quotient)
	# ebx
	# ecx	counter
	# edx	scratch (remainder)
	# esi	divisor
	# edi	dst
	# ebp

	xorl	%eax, %eax

	divl	%esi

	movl	%eax, -4(%edi,%ecx,4)
	loop	L(fraction_top)


L(done):
	popl	%ebp
	popl	%edi
	popl	%esi
	popl	%ebx
	movl	%edx, %eax
	ret

EPILOGUE()
