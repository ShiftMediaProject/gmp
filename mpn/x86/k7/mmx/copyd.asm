# AMD K7 mpn_copyd -- copy limb vector, decrementing.
#
#    alignment dst/src, A=0mod8 N=4mod8
#       A/A   A/N   N/A   N/N
# K7    0.75  1.0   1.0   0.75


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


# void mpn_copyd (mp_ptr dst, mp_srcptr src, mp_size_t size);
#
# The various comments in mpn/x86/k7/copyi.asm apply here too.


defframe(PARAM_SIZE,12)
defframe(PARAM_SRC, 8)
defframe(PARAM_DST, 4)

dnl  must be at least 5 since the unrolled code can't handle less than 5
deflit(UNROLL_THRESHOLD, 5)

	.text
	ALIGN(32)
PROLOGUE(mpn_copyd)
deflit(`FRAME',0)

	movl	PARAM_SIZE, %ecx
	pushl	%ebx
FRAME_pushl()

	movl	PARAM_SRC, %eax
	movl	PARAM_DST, %edx
	cmpl	$UNROLL_THRESHOLD, %ecx

	jae	L(unroll)

L(simple):
	# eax	src
	# ebx	scratch
	# ecx	counter
	# edx	dst
	#
	# this loop is 2 cycles/limb

	movl	-4(%eax,%ecx,4), %ebx
	movl	%ebx, -4(%edx,%ecx,4)
	decl	%ecx
	jnz	L(simple)

	popl	%ebx
	ret


L(unroll):
	pushl	%esi
	leal	(%eax,%ecx,4), %ebx
	leal	(%edx,%ecx,4), %esi

	andl	%esi, %ebx
	popl	%esi
	subl	$4, %ecx		# size-4

	testb	$4, %bl
	jz	L(aligned)

	# both src and dst unaligned, process one limb to align them
	movl	12(%eax,%ecx,4), %ebx
	movl	%ebx, 12(%edx,%ecx,4)
	decl	%ecx
L(aligned):


L(top):
	# eax	src
	# ebx
	# ecx	counter, limbs
	# edx	dst

	movq	8(%eax,%ecx,4), %mm0
	movq	(%eax,%ecx,4), %mm1
	subl	$4, %ecx
	movq	%mm0, 16+8(%edx,%ecx,4)
	movq	%mm1, 16(%edx,%ecx,4)
	jns	L(top)		# jump no carry and not zero


	# now %ecx is -1 to -4 representing respectively 3 to 0 limbs remaining

	test	$2, %cl
	jz	L(finish_not_two)

	movq	8(%eax,%ecx,4), %mm0
	movq	%mm0, 8(%edx,%ecx,4)
L(finish_not_two):

	test	$1, %cl
	jz	L(done)

	movl	(%eax), %ebx
	movl	%ebx, (%edx)

L(done):
	popl	%ebx
	femms
	ret


EPILOGUE()
