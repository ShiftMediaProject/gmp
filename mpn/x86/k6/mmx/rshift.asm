# AMD K6 mpn_rshift -- mpn right shift.
#
# K6: 1.75 cycles/limb


# Copyright (C) 2000 Free Software Foundation, Inc.
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


# mp_limb_t mpn_rshift (mp_ptr dst, mp_srcptr src, mp_size_t size,
#                       unsigned shift);
#
# Shift src,size right by shift many bits and store the result in dst,size.
# Zeros are shifted in at the left.  Return the bits shifted out at the
# right.

defframe(PARAM_SHIFT,16)
defframe(PARAM_SIZE, 12)
defframe(PARAM_SRC,  8)
defframe(PARAM_DST,  4)
deflit(`FRAME',0)

dnl  Minimum 9, because the unrolled loop can't handle less.
dnl  Doesn't depend on femms_available_p because there's the same
dnl  emms_or_femms in both the simple and unrolled.

deflit(UNROLL_THRESHOLD, 9)

	.text
	ALIGN(32)

PROLOGUE(mpn_rshift)
	# The 1 limb case can be done without the push %ebx, but it's then
	# still the same speed.  The push is left as a free helping hand for
	# the two_or_more code.

	movl	PARAM_SIZE, %eax
	pushl	%ebx
deflit(`FRAME',4)

	movl	PARAM_SRC, %ebx
	decl	%eax

	movl	PARAM_SHIFT, %ecx
	jnz	L(two_or_more)

	movl	(%ebx), %edx		# src limb
	movl	PARAM_DST, %ebx

	shrdl(	%cl, %edx, %eax)	# return value

 	shrl	%cl, %edx

	movl	%edx, (%ebx)		# dst limb
	popl	%ebx

	ret


#------------------------------------------------------------------------------
	ALIGN(16)	# avoid offset 0x1f
L(two_or_more):
	# eax	size-1
	# ebx	src
	# ecx	shift
	# edx

	movl	(%ebx), %edx	# src low limb
	negl	%ecx

	addl	$32, %ecx
	movd	PARAM_SHIFT, %mm6

	shll	%cl, %edx
	cmpl	$UNROLL_THRESHOLD-1, %eax

	jae	L(unroll)


	# eax	size-1
	# ebx	src
	# ecx	32-shift
	# edx	retval
	#
	# mm6	shift

	movl	PARAM_DST, %ecx
	leal	(%ebx,%eax,4), %ebx

	leal	-4(%ecx,%eax,4), %ecx
	negl	%eax

	# This loop runs at about 3 cycles/limb, which is the amount of
	# decoding, this is despite every second access being unaligned.

L(simple):
	# eax	counter (negative)
	# ebx	src, pointing at last limb
	# ecx	dst, pointing at last limb
	# edx	retval
	#
	# mm0	scratch
	# mm6	shift

	movq	(%ebx,%eax,4), %mm0
	incl	%eax

 	psrlq	%mm6, %mm0

	movd	%mm0, (%ecx,%eax,4)
	jnz	L(simple)


	movq	%mm0, (%ecx)
	movl	%edx, %eax

	popl	%ebx

	emms_or_femms
	ret


#------------------------------------------------------------------------------
# The strange offsets used on src and dst are due to the following,
# - needing no displacement (%ebx,%eax,4) and (%edx,%eax,4) first in the loop
# - needing the loop running %eax upwards from negative values and wanting
#   to stop when %eax goes positive
# - wanting to end up with %eax between 0 and 3 so as to be able to test for
#   0-3 extras with test $2 and test $1

L(unroll):
	# eax	size-1
	# ebx	src
	# ecx	32-shift
	# edx	retval
	#
	# mm6	shift

	addl	$32, %ecx
	subl	$7, %eax		# size-8

	movd	%ecx, %mm7
	movl	PARAM_DST, %ecx

	movq	(%ebx), %mm2		# src low qword
	leal	4(%ebx,%eax,4), %ebx	# src end - 28

	testb	$4, %cl
	leal	-12(%ecx,%eax,4), %ecx	# dst end - 44

	notl	%eax			# -(size-7)
	jz	L(dst_aligned)

	psrlq	%mm6, %mm2
	incl	%eax

	movd	%mm2, 12(%ecx,%eax,4)
	movq	(%ebx,%eax,4), %mm2	# new src low qword
L(dst_aligned):

	movq	8(%ebx,%eax,4), %mm0	# src second lowest qword


	# This loop is the important bit, the rest is just support for it.
	# Four src limbs are held at the start, and four more will be read.
	# Four dst limbs will be written.
	#
	# The magic ingredients for speed here are
	#
	# - aligning the code to 32 bytes
	# - fitting the first 10 instructions into 32 bytes (the first fetch
	#   and store must have no displacements)
	# - the instruction scheduling shown


	# Offset 0x88 here, so use a jump to get to L(top) in one cycle.
	# This guards against executing through a bunch of nop's if a dumb
	# assembler doesn't generate multi-byte do-nothing instructions when
	# aligning.

	jmp	L(top)

	ALIGN(32)
L(top):
	# eax	limb counter, negative
	# ebx	src end - 28
	# ecx	dst end - 44
	# ecx	retval
	#
	# mm0	src next qword
	# mm1	scratch
	# mm2	src prev qword
	# mm6	shift
	# mm7	64-shift

	psrlq	%mm6, %mm2
	addl	$4, %eax

	movq	%mm0, %mm1
	psllq	%mm7, %mm0

	por	%mm0, %mm2
	movq	(%ebx,%eax,4), %mm0

	psrlq	%mm6, %mm1
	movq	%mm2, (%ecx,%eax,4)

	movq	%mm0, %mm2
	psllq	%mm7, %mm0

	por	%mm0, %mm1
	movq	8(%ebx,%eax,4), %mm0

	movq	%mm1, 8(%ecx,%eax,4)
	ja	L(top)		# jump if no carry and not zero



	# Now have the four limbs in mm2 (low) and mm0 (high), and %eax is 0
	# to 3 representing respectively 3 to 0 further limbs.


	testb	$2, %al
	jnz	L(finish_nottwo)

	# Two or three extra limbs: rshift mm2, OR it with lshifted mm0, mm0
	# becomes new mm2 and a new mm0 is loaded.

	psrlq	%mm6, %mm2
	movq	%mm0, %mm1

	psllq	%mm7, %mm0
	addl	$2, %eax

	por	%mm0, %mm2
	movq	8(%ebx,%eax,4), %mm0

	movq	%mm2, 8(%ecx,%eax,4)
	movq	%mm1, %mm2
L(finish_nottwo):


	testb	$1, %al
	psrlq	%mm6, %mm2

	movq	%mm0, %mm1
	psllq	%mm7, %mm0

	por	%mm0, %mm2
	psrlq	%mm6, %mm1

	movq	%mm2, 16(%ecx,%eax,4)
	jnz	L(finish_even)


	# one further extra limb to process

	movd	28-4(%ebx), %mm0	# src[size-1], most significant limb
	popl	%ebx

	movq	%mm0, %mm2
	psllq	%mm7, %mm0

	por	%mm0, %mm1
	psrlq	%mm6, %mm2

	movq	%mm1, 44-12(%ecx)	# dst[size-3,size-2]
	movd	%mm2, 44-4(%ecx)	# dst[size-1]

	movl	%edx, %eax		# retval

	emms_or_femms
	ret


L(finish_even):
	# no further extra limbs

	movq	%mm1, 44-8(%ecx)	# dst[size-2,size-1]
	movl	%edx, %eax		# retval

	popl	%ebx

	emms_or_femms
	ret

EPILOGUE()
