# AMD K7 mpn_copyi -- copy limb vector, incrementing.
#
#    alignment dst/src, A=0mod8 N=4mod8
#       A/A   A/N   N/A   N/N
# K7    0.75  1.0   1.0   0.75


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


# void mpn_copyi (mp_ptr dst, mp_srcptr src, mp_size_t size);
#
# Copy src,size to dst,size.
#
# This code at 0.75 or 1.0 c/l is always faster than a plain rep movsl at
# 1.33 c/l.
#
# The K7 can do two loads, or two stores, or a load and a store, in one
# cycle, so if those are 64-bit operations then 0.5 c/l should be possible,
# however nothing under 0.7 c/l is known.
#
# If both source and destination are unaligned then one limb is processed at
# the start to make them aligned and so get 0.75 c/l, whereas if they'd been
# used unaligned it would be 1.5 c/l.

defframe(PARAM_SIZE,12)
defframe(PARAM_SRC, 8)
defframe(PARAM_DST, 4)

dnl  parameter space reused
define(SAVE_EBX,`PARAM_SIZE')

dnl  minimum 5 since the unrolled code can't handle less than 5
deflit(UNROLL_THRESHOLD, 5)

	.text
	ALIGN(32)
PROLOGUE(mpn_copyi)
deflit(`FRAME',0)

	movl	PARAM_SIZE, %ecx
	movl	%ebx, SAVE_EBX

	movl	PARAM_SRC, %eax
	movl	PARAM_DST, %edx

	cmpl	$UNROLL_THRESHOLD, %ecx
	jae	L(unroll)

	orl	%ecx, %ecx
	jz	L(simple_done)

L(simple):
	# eax	src, incrementing
	# ebx	scratch
	# ecx	counter
	# edx	dst, incrementing
	#
	# this loop is 2 cycles/limb

	movl	(%eax), %ebx
	movl	%ebx, (%edx)
	decl	%ecx
	leal	4(%eax), %eax
	leal	4(%edx), %edx
	jnz	L(simple)

L(simple_done):
	movl	SAVE_EBX, %ebx
	ret


L(unroll):
	movl	%eax, %ebx
	leal	-12(%eax,%ecx,4), %eax	# src end - 12
	subl	$3, %ecx		# size-3

	andl	%edx, %ebx
	leal	(%edx,%ecx,4), %edx	# dst end - 12
	negl	%ecx

	testl	$4, %ebx   # testl to pad code closer to 16 bytes for L(top)
	jz	L(aligned)

	# both src and dst unaligned, process one limb to align them
	movl	(%eax,%ecx,4), %ebx
	movl	%ebx, (%edx,%ecx,4)
	incl	%ecx
L(aligned):


	ALIGN(16)
L(top):
	# eax	src end - 12
	# ebx
	# ecx	counter, negative, limbs
	# edx	dst end - 12

	movq	(%eax,%ecx,4), %mm0
	movq	8(%eax,%ecx,4), %mm1
	addl	$4, %ecx
	movq	%mm0, -16(%edx,%ecx,4)
	movq	%mm1, -16+8(%edx,%ecx,4)
	ja	L(top)		# jump no carry and not zero


	# now %ecx is 0 to 3 representing respectively 3 to 0 limbs remaining

	test	$2, %cl
	jnz	L(finish_not_two)

	movq	(%eax,%ecx,4), %mm0
	movq	%mm0, (%edx,%ecx,4)
L(finish_not_two):

	test	$1, %cl
	jnz	L(done)

	movl	8(%eax), %ebx
	movl	%ebx, 8(%edx)

L(done):
	movl	SAVE_EBX, %ebx
	emms
	ret

EPILOGUE()
