# AMD K6 mpn_divmod_1 -- mpn by limb division.
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


# mp_limb_t mpn_divmod_1 (mp_ptr dst, mp_srcptr src, mp_size_t size,
#                         mp_limb_t divisor);
#
# Divide src,size by divisor and store the quotient in dst,size.
# Return the remainder.
#
# A test is done to see if the high limb is less the the divisor, and if so
# one less div is done.  A div is 20 cycles, so assuming high<divisor about
# half the time, then this test saves 10 cycles on average.
#  	
# Essentially this code is the same as the division based part of
# mpn/generic/divmod_1.c, but register usage and instruction scheduling is
# better than the compiler will probably give.
#
# Back-to-back div instructions run at 20 cycles, the same as the loop here,
# so it seems there's nothing to gain by rearranging the loop.

defframe(PARAM_DIVISOR,16)
defframe(PARAM_SIZE,   12)
defframe(PARAM_SRC,    8)
defframe(PARAM_DST,    4)

	.text
	ALIGN(32)

PROLOGUE(mpn_divmod_1)
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

	movl	-4(%ebx,%ecx,4), %eax	# high limb
	xorl	%edx, %edx

	movl	PARAM_DST, %edi
	cmpl	%esi, %eax

	jae	L(entry)


	# high<divisor, so high of dst is zero, and avoid one div

	movl	$0, -4(%edi,%ecx,4)
	decl	%ecx

	movl	%eax, %edx
	jz	L(done)


L(top):
	# eax	scratch (quotient)
	# ebx	src
	# ecx	counter
	# edx	scratch (remainder)
	# esi	divisor
	# edi	dst
	# ebp

	movl	-4(%ebx,%ecx,4), %eax
L(entry):
	divl	%esi

	movl	%eax, -4(%edi,%ecx,4)
	loop	L(top)

L(done):
	popl	%edi
	popl	%esi

	popl	%ebx
	movl	%edx, %eax

	ret

EPILOGUE()
