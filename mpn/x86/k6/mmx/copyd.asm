# AMD K6 mpn_copyd -- copy limb vector, decrementing.
#
# K6: 0.56 or 1.0 cycles/limb (at 32 limbs/loop), depending on data
# alignment.


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


dnl  K6 aligned:
dnl  UNROLL_COUNT cycles/limb
dnl        8          0.75
dnl       16          0.625
dnl       32          0.5625
dnl       64          0.53
dnl  Maximum possible with the current code is 64, the minimum is 2.

deflit(UNROLL_COUNT, 32)


# void mpn_copyd (mp_ptr dst, mp_srcptr src, mp_size_t size);
#
# Copy src,size to dst,size, processing limbs from high to low addresses.
#
# The comments in copyi.asm apply here too.


defframe(PARAM_SIZE,12)
defframe(PARAM_SRC, 8)
defframe(PARAM_DST, 4)
deflit(`FRAME',0)

	.text
	ALIGN(32)

PROLOGUE(mpn_copyd)
	movl	PARAM_SIZE, %ecx
	movl	%esi, %eax

	movl	PARAM_SRC, %esi
	movl	%edi, %edx

	std

	movl	PARAM_DST, %edi
	cmpl	$UNROLL_COUNT, %ecx

	leal	-4(%esi,%ecx,4), %esi

	leal	-4(%edi,%ecx,4), %edi
	ja	L(unroll)

L(simple):
	rep
	movsl

	cld

	movl	%eax, %esi
	movl	%edx, %edi

	ret


L(unroll):
	# if src and dst are different alignments mod8, then use rep movs
	# if src and dst are both 4mod8 then process one limb to get 0mod8

	pushl	%ebx
	leal	(%esi,%edi), %ebx

	testb	$4, %bl
	popl	%ebx
	
	jnz	L(simple)
	testl	$4, %esi

	leal	-UNROLL_COUNT(%ecx), %ecx
	jnz	L(already_aligned)

	movsl

	decl	%ecx
L(already_aligned):


ifelse(UNROLL_BYTES,256,`
	# for testing speed at 64 limbs/loop unrolling
	subl	$128, %esi
	subl	$128, %edi
')

	# aligning to 16 here isn't enough, 32 is needed to get the speeds
	# claimed
	ALIGN(32)

L(top):
	# eax	saved esi
	# ebx
	# ecx	counter, limbs
	# edx	saved edi
	# esi	src, incrementing
	# edi	dst, incrementing
	# ebp
	#
	# `disp' is never 0, so don't need to force 0(%esi).

deflit(CHUNK_COUNT, 2)
forloop(`i', 0, UNROLL_COUNT/CHUNK_COUNT-1, `
	deflit(`disp', eval(-4-i*CHUNK_COUNT*4 ifelse(UNROLL_BYTES,256,+128)))
	movq	disp(%esi), %mm0
	movq	%mm0, disp(%edi)
')

	leal	-UNROLL_BYTES(%esi), %esi
	subl	$UNROLL_COUNT, %ecx

	leal	-UNROLL_BYTES(%edi), %edi
	jns	L(top)


	# now %ecx is -UNROLL_COUNT to -1 representing repectively 0 to
	# UNROLL_COUNT-1 limbs remaining

	testb	$eval(UNROLL_COUNT/2), %cl

	leal	UNROLL_COUNT(%ecx), %ecx
	jz	L(not_half)

	# at an unroll count of 32 this block of code is 16 cycles faster than
	# the rep movs, less 3 or 4 to test whether to do it

forloop(`i', 0, UNROLL_COUNT/CHUNK_COUNT/2-1, `
	deflit(`disp', eval(-4-i*CHUNK_COUNT*4 ifelse(UNROLL_BYTES,256,+128)))
	movq	disp(%esi), %mm0
	movq	%mm0, disp(%edi)
')
	subl	$eval(UNROLL_BYTES/2), %esi
	subl	$eval(UNROLL_BYTES/2), %edi

	subl	$eval(UNROLL_COUNT/2), %ecx
L(not_half):


ifelse(UNROLL_BYTES,256,`
	# for testing speed at 64 limbs/loop unrolling
	addl	$128, %esi
	addl	$128, %edi
')

	rep
	movsl

	cld

	movl	%eax, %esi
	movl	%edx, %edi

	emms_or_femms
	ret

EPILOGUE()
