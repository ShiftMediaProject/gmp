# x86 mpn_divrem_1 -- mpn by limb division extending to fractional quotient.
#
#       cycles/limb
# K7        42
# K6        20
# P6        40
# P5        44
# 486   approx 43 maybe


# Copyright (C) 1999, 2000 Free Software Foundation, Inc.
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
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
# MA 02111-1307, USA.


include(`../config.m4')


# mp_limb_t mpn_divrem_1 (mp_ptr dst, mp_size_t xsize,
#                         mp_srcptr src, mp_size_t size, mp_limb_t divisor);
# mp_limb_t mpn_divrem_1c (mp_ptr dst, mp_size_t xsize,
#                          mp_srcptr src, mp_size_t size, mp_limb_t divisor);
#
# Divide src,size by divisor and store the quotient in dst+xsize,size.
# Extend the division to fractional quotient limbs in dst,xsize.  Return the
# remainder.  Either or both xsize and size can be 0.
#
# mpn_divrem_1c takes a carry parameter which is an initial high limb,
# effectively one extra limb at the top of src,size.  Must have
# carry<divisor.
#
#
# Essentially the code is the same as the division based part of
# mpn/generic/divrem_1.c, but has the following advantages.
#
# - If gcc isn't being used then divrem_1.c will get the generic C
#   udiv_qrnnd() and be rather slow.
#
# - On K6, using the loop instruction is a 10% speedup, but gcc doesn't
#   generate that instruction (as of 2.95 at least).
#
# A test is done to see if the high limb is less the the divisor, and if so
# one less div is done.  A div is between 20 and 40 cycles on the various
# x86s, so assuming high<divisor about half the time, then this test saves
# half that amount.  The branch misprediction penalty on each chip is less
# than half a div.
#  	
#
# K6: Back-to-back div instructions run at 20 cycles, the same as the loop
#     here, so it seems there's nothing to gain by rearranging the loop.
#     Pairing the mov and loop instructions was found to gain nothing.  (The
#     same is true of the mpn/x86/mod_1.asm loop.)
#
#     With a "decl/jnz" rather than a "loop" this code runs at 22 cycles.
#     The loop_or_decljnz macro is an easy way to get a 10% speedup.
#
# P5: Moving the load down to pair with the store might save 1 cycle, but
#     that doesn't seem worth bothering with, since it'd be only a 2.2%
#     saving.
#
# K7: New code doing a multiply by inverse is in progress and promises to
#     run at about 25 cycles.  The code here is just something reasonable to
#     use for now.
#
# P6: Multiply by inverse is going to be looked at for this too.

defframe(PARAM_CARRY,  24)
defframe(PARAM_DIVISOR,20)
defframe(PARAM_SIZE,   16)
defframe(PARAM_SRC,    12)
defframe(PARAM_XSIZE,  8)
defframe(PARAM_DST,    4)

	.text
	ALIGN(16)

PROLOGUE(mpn_divrem_1c)
deflit(`FRAME',0)

	movl	PARAM_SIZE, %ecx
	pushl	%edi
FRAME_pushl()
	
	movl	PARAM_SRC, %edi
	pushl	%esi
FRAME_pushl()

	movl	PARAM_DIVISOR, %esi
	pushl	%ebx
FRAME_pushl()

	movl	PARAM_DST, %ebx
	pushl	%ebp
FRAME_pushl()

	movl	PARAM_XSIZE, %ebp
	orl	%ecx, %ecx

	movl	PARAM_CARRY, %edx
	jz	LF(mpn_divrem_1,fraction)

	leal	-4(%ebx,%ebp,4), %ebx	# dst one limb below integer part
	jmp	LF(mpn_divrem_1,integer_top)

EPILOGUE()


PROLOGUE(mpn_divrem_1)
deflit(`FRAME',0)

	movl	PARAM_SIZE, %ecx
	pushl	%edi
FRAME_pushl()
	
	movl	PARAM_SRC, %edi
	pushl	%esi
FRAME_pushl()

	movl	PARAM_DIVISOR, %esi
	orl	%ecx,%ecx

	jz	L(size_zero)
	pushl	%ebx
FRAME_pushl()

	movl	-4(%edi,%ecx,4), %eax	# src high limb
	xorl	%edx, %edx

	movl	PARAM_DST, %ebx
	pushl	%ebp
FRAME_pushl()

	movl	PARAM_XSIZE, %ebp
	cmpl	%esi, %eax

	leal	-4(%ebx,%ebp,4), %ebx	# dst one limb below integer part
	jae	L(integer_entry)


	# high<divisor, so high of dst is zero, and avoid one div

	movl	%edx, (%ebx,%ecx,4)
	decl	%ecx

	movl	%eax, %edx
	jz	L(fraction)


L(integer_top):
	# eax	scratch (quotient)
	# ebx	dst+4*xsize-4
	# ecx	counter
	# edx	scratch (remainder)
	# esi	divisor
	# edi	src
	# ebp	xsize

	movl	-4(%edi,%ecx,4), %eax
L(integer_entry):
	divl	%esi

	movl	%eax, (%ebx,%ecx,4)
	loop_or_decljnz	L(integer_top)


L(fraction):
	orl	%ebp, %ecx
	jz	L(done)

	movl	PARAM_DST, %ebx


L(fraction_top):
	# eax	scratch (quotient)
	# ebx	dst
	# ecx	counter
	# edx	scratch (remainder)
	# esi	divisor
	# edi
	# ebp

	xorl	%eax, %eax

	divl	%esi

	movl	%eax, -4(%ebx,%ecx,4)
	loop_or_decljnz	L(fraction_top)


L(done):
	popl	%ebp
	movl	%edx, %eax
	popl	%ebx
	popl	%esi
	popl	%edi
	ret


L(size_zero):
deflit(`FRAME',8)
	movl	PARAM_XSIZE, %ecx
	xorl	%eax, %eax

	movl	PARAM_DST, %edi

	cld	# better safe than sorry, see mpn/x86/README.family

	rep
	stosl

	popl	%esi
	popl	%edi
	ret
EPILOGUE()
