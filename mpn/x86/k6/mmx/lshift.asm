# AMD K6 mpn_lshift -- mpn left shift.
#
# K6: 1.75 cycles/limb


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


# mp_limb_t mpn_lshift (mp_ptr dst, mp_srcptr src, mp_size_t size,
#                       unsigned shift);
#
# Shift src,size left by shift many bits and store the result in dst,size.
# Zeros are shifted in at the right.  Return the bits shifted out at the
# left.

defframe(PARAM_SHIFT,16)
defframe(PARAM_SIZE, 12)
defframe(PARAM_SRC,  8)
defframe(PARAM_DST,  4)
deflit(`FRAME',0)

dnl  used after src has been fetched
define(VAR_RETVAL,`PARAM_SRC')

dnl  minimum 9, because unrolled loop can't handle less
deflit(UNROLL_THRESHOLD, 12)

	.text
	ALIGN(32)

PROLOGUE(mpn_lshift)
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

	shldl(	%cl, %edx, %eax)	# return value

 	shll	%cl, %edx

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

	movl	(%ebx,%eax,4), %edx	# src high limb
	negl	%ecx

	movd	PARAM_SHIFT, %mm6
	addl	$32, %ecx

	shrl	%cl, %edx
	cmpl	$UNROLL_THRESHOLD-1, %eax

	movl	%edx, VAR_RETVAL
	jae	L(unroll)


	# eax	size-1
	# ebx	src
	# ecx	32-shift
	# edx	retval
	#
	# mm6	shift

	movd	%ecx, %mm7
	movl	%eax, %ecx

	movl	PARAM_DST, %eax

L(simple):
	# ecx	dst
	# ebx	src
	# ecx	counter
	# edx	retval
	#
	# mm0	scratch
	# mm6	shift
	# mm7	32-shift

	movq	-4(%ebx,%ecx,4), %mm0
 	psrlq	%mm7, %mm0

	movd	%mm0, (%eax,%ecx,4)
	loop	L(simple)


	movd	(%ebx), %mm0
	popl	%ebx

 	psllq	%mm6, %mm0

	movd	%mm0, (%eax)
	movl	%edx, %eax

	emms_or_femms
	ret


#------------------------------------------------------------------------------
# The strange offsets used on src and dst are due to the following,
# - needing no displacement (%ebx,%eax,4) and (%edx,%eax,4) first in the loop
# - needing the loop running %eax downwards and wanting to stop when %eax
#   goes negative
# - wanting to end up with %eax set to -1 to -4 so as to be able to test for
#   0-3 extras with test $2 and test $1

L(unroll):
	# eax	size-1
	# ebx	src
	# ecx	32-shift
	# edx	retval (but instead VAR_RETVAL is used)
	#
	# mm6	shift

	addl	$32, %ecx
	movl	PARAM_DST, %edx

	movd	%ecx, %mm7
	subl	$7, %eax		# size-8

	leal	(%edx,%eax,4), %ecx	# alignment of dst
	addl	$40, %edx

	movq	32-8(%ebx,%eax,4), %mm2		# src high qword
	testb	$4, %cl

	jz	L(dst_aligned)
	psllq	%mm6, %mm2

	psrlq	$32, %mm2
	decl	%eax

	movd	%mm2, 32-40+4-4(%edx,%eax,4)	# dst high limb
	movq	32-8(%ebx,%eax,4), %mm2		# new src high qword
L(dst_aligned):

	movq	32-16(%ebx,%eax,4), %mm0	# src second highest qword
	addl	$24, %ebx



	# This loop is the important bit, the rest is just support for it.
	# Four src limbs are held at the start, and four more will be read.
	# Four dst limbs will be written.
	#
	# The magic ingredients for speed here are the same as in rshift,
	#
	# - aligning the code to 32 bytes
	# - fitting the first 10 instructions into 32 bytes (the first fetch
	#   and store must have no displacements)
	# - the instruction scheduling shown


	# Offset 0x95 here, so use a jump to get to L(top) in one cycle.
	# This guards against executing through a bunch of nop's if a dumb
	# assembler doesn't generate multi-byte do-nothing instructions when
	# aligning.

	jmp L(top)

	ALIGN(32)
L(top):
	# eax	limb counter
	# ebx	src + 24
	# ecx
	# edx	dst + 40
	#
	# mm0	src next qword
	# mm1	scratch
	# mm2	src prev qword
	# mm6	shift
	# mm7	64-shift

	psllq	%mm6, %mm2
	subl	$4, %eax

	movq	%mm0, %mm1
	psrlq	%mm7, %mm0

	por	%mm0, %mm2
	movq	(%ebx,%eax,4), %mm0

	psllq	%mm6, %mm1
	movq	%mm2, (%edx,%eax,4)

	movq	%mm0, %mm2
	psrlq	%mm7, %mm0

	por	%mm0, %mm1
	movq	-8(%ebx,%eax,4), %mm0

	movq	%mm1, -8(%edx,%eax,4)
	jnc	L(top)


	# Now have four limbs in mm2 (prev) and mm0 (next), plus eax mod 4.
	#
	# -16(%ebx) is the next source, and -16(%edx) is the next destination.
	# %eax is between -4 and -1, representing respectively 0 to 3 extra
	# limbs that must be read.


	testb	$2, %al
	jz	L(finish_nottwo)

	# Two more limbs: lshift mm2, OR it with rshifted mm0, mm0 becomes
	# new mm2 and a new mm0 is loaded.

	psllq	%mm6, %mm2
	movq	%mm0, %mm1

	psrlq	%mm7, %mm0
	subl	$2, %eax

	por	%mm0, %mm2
	movq	-8(%ebx,%eax,4), %mm0

	movq	%mm2, -8(%edx,%eax,4)
	movq	%mm1, %mm2
L(finish_nottwo):


	# lshift mm2, OR with rshifted mm0, mm1 becomes lshifted mm0

	testb	$1, %al
	psllq	%mm6, %mm2

	movq	%mm0, %mm1
	psrlq	%mm7, %mm0

	por	%mm0, %mm2
	psllq	%mm6, %mm1

	movq	%mm2, -16(%edx,%eax,4)
	jz	L(finish_even)


	# Size is odd, so mm1 and one extra limb to process.

	movd	-24(%ebx), %mm0		# src[0]
	popl	%ebx
deflit(`FRAME',0)

	movq	%mm0, %mm2
	psllq	$32, %mm0

	psrlq	%mm7, %mm0

	psllq	%mm6, %mm2
	por	%mm0, %mm1

	movq	%mm1, 4-40(%edx)	# dst[1,2]
	movd	%mm2, -40(%edx)		# dst[0]

	movl	VAR_RETVAL, %eax

	emms_or_femms
	ret


L(finish_even):
deflit(`FRAME',4)
	# Size is even, so only mm1 left to process.

	movq	%mm1, -40(%edx)		# dst[0,1]
	movl	VAR_RETVAL, %eax

	popl	%ebx
	emms_or_femms
	ret

EPILOGUE()
