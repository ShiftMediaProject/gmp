# AMD K7 mpn_copyd -- copy limb vector, decrementing.
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


# void mpn_copyd (mp_ptr dst, mp_srcptr src, mp_size_t size);
#
# The various comments in mpn/x86/k7/copyi.asm apply here too.

defframe(PARAM_SIZE,12)
defframe(PARAM_SRC, 8)
defframe(PARAM_DST, 4)
deflit(`FRAME',0)

dnl  parameter space reused
define(SAVE_EBX,`PARAM_SIZE')
define(SAVE_ESI,`PARAM_SRC')

dnl  minimum 5 since the unrolled code can't handle less than 5
deflit(UNROLL_THRESHOLD, 5)

	.text
	ALIGN(32)
PROLOGUE(mpn_copyd)

	movl	PARAM_SIZE, %ecx
	movl	%ebx, SAVE_EBX

	movl	PARAM_SRC, %eax
	movl	PARAM_DST, %edx

	cmpl	$UNROLL_THRESHOLD, %ecx
	jae	L(unroll)

	orl	%ecx, %ecx
	jz	L(simple_done)

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

L(simple_done):
	movl	SAVE_EBX, %ebx
	ret


L(unroll):
	movl	%esi, SAVE_ESI
	leal	(%eax,%ecx,4), %ebx
	leal	(%edx,%ecx,4), %esi

	andl	%esi, %ebx
	movl	SAVE_ESI, %esi
	subl	$4, %ecx		# size-4

	testl	$4, %ebx   # testl to pad code closer to 16 bytes for L(top)
	jz	L(aligned)

	# both src and dst unaligned, process one limb to align them
	movl	12(%eax,%ecx,4), %ebx
	movl	%ebx, 12(%edx,%ecx,4)
	decl	%ecx
L(aligned):


	ALIGN(16)
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
	jns	L(top)


	# now %ecx is -4 to -1 representing respectively 0 to 3 limbs remaining

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
	movl	SAVE_EBX, %ebx
	emms
	ret


EPILOGUE()
