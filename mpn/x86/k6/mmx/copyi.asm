# AMD K6 mpn_copyi -- copy limb vector, incrementing.
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


# void mpn_copyi (mp_ptr dst, mp_srcptr src, mp_size_t size);
#
# Copy src,size to dst,size, processing limbs from low to high addresses.
#
# The MMX loop is faster than a rep movs when src and dst are both 0mod8.
# With one 0mod8 and one 4mod8 it's 1.056 c/l and the rep movs at 1.0 c/l is
# used instead.
#
#         mod8
#	src  dst
#	 0    0	   both aligned, use mmx
#	 0    4    unaligned, use rep movs
#	 4    0    unaligned, use rep movs
#	 4    4    do one movs, then both aligned, use mmx
#
# The MMX on aligned data is 0.5 c/l, plus loop overhead of 2 cycles/loop,
# which is 0.0625 c/l at 32 limbs/loop.
#
# Addressing modes like disp(%esi,%ecx,4) aren't used.  They'd make it
# possible to avoid incrementing %esi and %edi in the loop and hence get
# loop overhead down to 1 cycle, but they come out slower (about 0.7 c/l),
# possibly because they're 5 code bytes rather than 4 and so don't fall
# nicely on 16 byte boundaries.
#
# A pattern of two movq loads and two movq stores (or four and four) was
# tried, but found to be the same speed as just one of each.


defframe(PARAM_SIZE,12)
defframe(PARAM_SRC, 8)
defframe(PARAM_DST, 4)
deflit(`FRAME',0)

	.text
	ALIGN(32)

PROLOGUE(mpn_copyi)
	movl	PARAM_SIZE, %ecx
	movl	%esi, %eax

	movl	PARAM_SRC, %esi
	movl	%edi, %edx

	cld

	movl	PARAM_DST, %edi
	cmpl	$UNROLL_COUNT, %ecx

	ja	L(unroll)

L(simple):
	rep
	movsl

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
	jz	L(already_aligned)

	decl	%ecx

	movsl
L(already_aligned):


ifelse(UNROLL_BYTES,256,`
	# for testing speed at 64 limbs/loop unrolling
	addl	$128, %esi
	addl	$128, %edi
')

	# avoid executing through a bunch of nops if the assembler doesn't
	# generate multi-byte do-nothings
	jmp	L(top)

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
	# Zdisp gets 0(%esi) left that way to avoid vector decode, and with
	# 0(%edi) keeps code aligned to 16 byte boundaries.

deflit(CHUNK_COUNT, 2)
forloop(`i', 0, UNROLL_COUNT/CHUNK_COUNT-1, `
	deflit(`disp', eval(i*CHUNK_COUNT*4 ifelse(UNROLL_BYTES,256,-128)))
Zdisp(	movq,	disp,(%esi), %mm0)
Zdisp(	movq,	%mm0, disp,(%edi))
')

	addl	$UNROLL_BYTES, %esi
	subl	$UNROLL_COUNT, %ecx

	leal	UNROLL_BYTES(%edi), %edi
	jns	L(top)


	# now %ecx is -UNROLL_COUNT to -1 representing repectively 0 to
	# UNROLL_COUNT-1 limbs remaining

	testb	$eval(UNROLL_COUNT/2), %cl

	leal	UNROLL_COUNT(%ecx), %ecx
	jz	L(not_half)

	# at an unroll count of 32 this block of code is 16 cycles faster than
	# the rep movs, less 3 or 4 to test whether to do it

forloop(`i', 0, UNROLL_COUNT/CHUNK_COUNT/2-1, `
	deflit(`disp', eval(i*CHUNK_COUNT*4 ifelse(UNROLL_BYTES,256,-128)))
	movq	disp(%esi), %mm0
	movq	%mm0, disp(%edi)
')
	addl	$eval(UNROLL_BYTES/2), %esi
	addl	$eval(UNROLL_BYTES/2), %edi

	subl	$eval(UNROLL_COUNT/2), %ecx
L(not_half):


ifelse(UNROLL_BYTES,256,`
	# for testing speed at 64 limbs/loop unrolling
	subl	$128, %esi
	subl	$128, %edi
')

	rep
	movsl

	movl	%eax, %esi
	movl	%edx, %edi

	emms_or_femms
	ret

EPILOGUE()
