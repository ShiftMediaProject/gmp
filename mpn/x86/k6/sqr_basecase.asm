# AMD K6 mpn_sqr_basecase -- square an mpn number.
#
# K6: approx 6.5 cycles per crossproduct, or 12 cycles per triangular product.
#
# Future: Some unrolling will be needed to help sizes 10 to 20.


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


# void mpn_sqr_basecase (mp_ptr dst, mp_srcptr src, mp_size_t size);
#
# Calculate src,size squared, storing the result in dst,2*size.
#
# The algorithm is basically the same as mpn/generic/sqr_basecase.c, but a
# lot of function call overheads are avoided, especially when the given size
# is smallish.

defframe(PARAM_SIZE,12)
defframe(PARAM_SRC, 8)
defframe(PARAM_DST, 4)

	.text
	ALIGN(32)
PROLOGUE(mpn_sqr_basecase)
deflit(`FRAME',0)

	movl	PARAM_SIZE, %ecx

	movl	PARAM_SRC, %eax

	cmpl	$2, %ecx
	movl	PARAM_DST, %edx

	je	L(two_limbs)
	ja	L(three_or_more)


#------------------------------------------------------------------------------
# one limb only
	# eax	src
	# ecx	size
	# edx	dst

	movl	(%eax), %eax
	movl	%edx, %ecx

	mull	%eax

	movl	%eax, (%ecx)
	movl	%edx, 4(%ecx)
	ret

#------------------------------------------------------------------------------
L(two_limbs):
	# eax	src
	# ecx	size
	# edx	dst

	pushl	%ebx
	movl	%eax, %ebx	# src

	movl	(%ebx), %eax
	movl	%edx, %ecx	# dst

	mull	%eax

	movl	%eax, (%ecx)
	movl	4(%ebx), %eax

	movl	%edx, 4(%ecx)

	mull	%eax

	movl	%eax, 8(%ecx)
	movl	(%ebx), %eax

	movl	%edx, 12(%ecx)
	movl	4(%ebx), %edx

	mull	%edx

	addl	%eax, 4(%ecx)

	adcl	%edx, 8(%ecx)
	adcl	$0, 12(%ecx)

	popl	%ebx
	addl	%eax, 4(%ecx)

	adcl	%edx, 8(%ecx)
	adcl	$0, 12(%ecx)

	ret


#------------------------------------------------------------------------------
L(three_or_more):
	cmpl	$4, %ecx
	jae	L(four_or_more)


#------------------------------------------------------------------------------
# three limbs
	# eax	src
	# ecx	size
	# edx	dst

	pushl	%ebx
	movl	%eax, %ebx	# src

	movl	(%ebx), %eax
	movl	%edx, %ecx	# dst

	mull	%eax		# src[0] ^ 2

	movl	%eax, (%ecx)
	movl	4(%ebx), %eax

	movl	%edx, 4(%ecx)

	pushl	%esi

	mull	%eax		# src[1] ^ 2

	movl	%eax, 8(%ecx)
	movl	8(%ebx), %eax

	movl	%edx, 12(%ecx)

	pushl	%edi

	mull	%eax		# src[2] ^ 2

	movl	%eax, 16(%ecx)
	movl	(%ebx), %eax

	movl	%edx, 20(%ecx)
	movl	4(%ebx), %edx

	mull	%edx		# src[0] * src[1]

	movl	%eax, %esi
	movl	(%ebx), %eax

	movl	%edx, %edi
	movl	8(%ebx), %edx

	pushl	%ebp
	xorl	%ebp, %ebp

	mull	%edx		# src[0] * src[2]

	addl	%eax, %edi
	movl	4(%ebx), %eax

	adcl	$0, %edx

	movl	%edx, %ebp
	movl	8(%ebx), %edx

	mull	%edx		# src[1] * src[2]

	addl	%eax, %ebp

	adcl	$0, %edx


	# eax	will be dst[5]
	# ebx	src
	# ecx	dst
	# edx	dst[4]
	# esi	dst[1]
	# edi	dst[2]
	# ebp	dst[3]

	xorl	%eax, %eax
	addl	%esi, %esi
	adcl	%edi, %edi
	adcl	%ebp, %ebp
	adcl	%edx, %edx
	adcl	$0, %eax

	addl	%esi, 4(%ecx)
	adcl	%edi, 8(%ecx)
	adcl	%ebp, 12(%ecx)
	adcl	%edx, 16(%ecx)
	adcl	%eax, 20(%ecx)
	# no carry here

	popl	%ebp
	popl	%edi
	popl	%esi
	popl	%ebx
	ret


#------------------------------------------------------------------------------
L(four_or_more):
	# eax	src
	# ebx
	# ecx	size
	# edx	dst
	# esi
	# edi
	# ebp

defframe(SAVE_EBX,-4)
defframe(SAVE_ESI,-8)
defframe(SAVE_EDI,-12)
defframe(SAVE_EBP,-16)
defframe(VAR_COUNTER,-20)
deflit(STACK_SPACE,20)

	subl	$STACK_SPACE, %esp
deflit(`FRAME',STACK_SPACE)
#	cmpl	$UNROLL_THRESHOLD, %ecx
	
	movl	%edi, SAVE_EDI
	leal	4(%edx), %edi

	movl	%ebx, SAVE_EBX
	leal	4(%eax), %ebx

	movl	%esi, SAVE_ESI
	movl	$0, %esi

	movl	%ebp, SAVE_EBP
#	jae	L(unroll)


#------------------------------------------------------------------------------
# simple looping
#
# First multiply src[0]*src[1..size-1] and store at dst[1..size].
# Further products are added in rather than stored.
 
	# eax
	# ebx	src+4
	# ecx	size
	# edx
	# esi
	# edi	dst
	# ebp

	movl	(%eax), %ebp	# multiplier
	decl	%ecx

L(simple_mul):
	# eax	scratch
	# ebx	src ptr
	# ecx	counter
	# edx	scratch
	# esi	carry
	# edi	dst ptr
	# ebp	multiplier

	movl	(%ebx), %eax
	addl	$4, %ebx

	mull	%ebp

	addl	%esi, %eax
	movl	$0, %esi

	adcl	%edx, %esi

	movl	%eax, (%edi)
	addl	$4, %edi

	loop	L(simple_mul)


#------------------------------------------------------------------------------
# Add products src[n]*src[n+1..size-1] at dst[2*n-1...], for n=1..size-2.
#
# The last two products, which are the bottom right corner of the product
# triangle, are left to the end.  These are src[size-3]*src[size-2,size-1]
# and src[size-2]*src[size-1].  If size is 4 then it's only these corner
# cases that need to be done.
#
# The outer loop has some trickery at the top of the loop to adjust ebx and
# edi ready for the next pass, using the n from VAR_COUNTER to know how much
# to decrease them by.

	# eax
	# ebx	src ptr [size]
	# ecx
	# edx
	# esi	carry
	# edi	dst ptr [size-1]
	# ebp

	movl	PARAM_SIZE, %ecx
	movl	%esi, (%edi)

	subl	$4, %ecx
	jz	L(simple_corner)

	movl	PARAM_SRC, %ebx
	movl	PARAM_DST, %edi

	movl	4(%ebx), %ebp	# src[1] multiplier
	addl	$8, %ebx	# start multiplying from src[2]

	addl	$12, %edi	# dst[3] first place to add to
	movl	%ecx, %edx	# size-4
	
	addl	$2, %ecx	# size-2 loop counter
	negl	%edx		# -(size-4)

	movl	%edx, VAR_COUNTER
	jmp	L(simple_outer_entry)


	ALIGN(16)
L(simple_outer_top):	
	# ebx	src, pointing just after last limb
	# edx	outer loop counter (negative)
	# edi	dst, pointing at stored carry limb of previous loop

	movl	%edx, VAR_COUNTER	# new value
	leal	-8(%ebx,%edx,4), %ebx

	leal	-2(%edx), %ecx
	movl	-4(%ebx), %ebp		# multiplier

	negl	%ecx
	leal	-4(%edi,%edx,4), %edi

L(simple_outer_entry):
	xorl	%esi, %esi		# initial carry


L(simple_inner):
	# eax	scratch
	# ebx	src, progressing
	# ecx	counter
	# edx	scratch
	# esi	carry
	# edi	dst, progressing
	# ebp	multiplier

	movl	(%ebx), %eax
	addl	$4, %ebx

	mull	%ebp

	addl	%esi, %eax
	movl	$0, %esi

	adcl	$0, %edx
	addl	%eax, (%edi)
	adcl	%edx, %esi

	addl	$4, %edi
	loop	L(simple_inner)


	movl	VAR_COUNTER, %edx
	movl	%esi, (%edi)

	incl	%edx
	jnz	L(simple_outer_top)


L(simple_corner):
	# ebx	src just past end
	# edi	dst at 2*size-5

	movl	-12(%ebx), %ebp
	movl	-8(%ebx), %eax
	movl	%eax, %ecx

	mull	%ebp

	addl	%eax, -4(%edi)
	adcl	$0, %edx

	movl	-4(%ebx), %eax
	movl	%edx, %esi
	movl	%eax, %ebx

	mull	%ebp

	addl	%esi, %eax
	adcl	$0, %edx

	addl	%eax, (%edi)
	adcl	$0, %edx

	movl	%edx, %esi
	movl	%ebx, %eax

	mull	%ecx

	addl	%esi, %eax
	movl	%eax, 4(%edi)

	adcl	$0, %edx

	movl	%edx, 8(%edi)
	

#------------------------------------------------------------------------------
# Left shift of dst[1..2*size-2], high bit shifted out becomes dst[2*size-1].


	movl	PARAM_SIZE, %eax
	xorl	%ecx, %ecx		# clear carry

	movl	PARAM_DST, %edi

	leal	(%edi,%eax,8), %edi
	notl	%eax			# -size-1, preserve carry

	leal	2(%eax), %eax		# -(size-1)

L(simple_shift):
	# eax	counter, negative
	# ebx
	# ecx
	# edx
	# esi
	# edi	dst, pointing just after last limb
	# ebp

	rcll	-4(%edi,%eax,8)
	rcll	(%edi,%eax,8)
	incl	%eax
	jnz	L(simple_shift)

	setc	%al

	movl	PARAM_SRC, %ebx
	movl	%eax, -4(%edi)	# dst most significant limb

	movl	PARAM_SIZE, %ecx

#------------------------------------------------------------------------------
# Now add in the squares on the diagonal, namely src[0]^2, src[1]^2, ...,
# src[size-1]^2.  dst[0] hasn't yet been set at all yet, and just gets the
# low limb of src[0]^2.

	movl	(%ebx), %eax	# src[0]

	mull	%eax

	leal	(%ebx,%ecx,4), %ebx	# src point just after last limb
	negl	%ecx

	movl	%eax, (%edi,%ecx,8)	# dst[0]
	incl	%ecx

L(simple_diag):
	# eax	scratch
	# ebx	src just after last limb
	# ecx	counter, negative
	# edx	carry
	# esi	scratch
	# edi	dst just after last limb
	# ebp

	movl	(%ebx,%ecx,4), %eax
	movl	%edx, %esi

	mull	%eax

	addl	%esi, -4(%edi,%ecx,8)
	adcl	%eax, (%edi,%ecx,8)
	adcl	$0, %edx

	incl	%ecx
	jnz	L(simple_diag)

	movl	SAVE_EBX, %ebx
	movl	SAVE_ESI, %esi

	addl	%edx, -4(%edi)	# dst most significant limb

	movl	SAVE_EDI, %edi
	movl	SAVE_EBP, %ebp
	addl	$FRAME, %esp
	ret

EPILOGUE()
