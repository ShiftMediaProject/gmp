# Intel P5 mpn_sqr_basecase -- square an mpn number.
#
# P5: approx 8 cycles per crossproduct, or 15.5 cycles per triangular
# product at around 20x20 limbs.


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
# lot of function call overheads are avoided, especially when the size is
# small.

defframe(PARAM_SIZE,12)
defframe(PARAM_SRC, 8)
defframe(PARAM_DST, 4)

	.text
	ALIGN(8)
PROLOGUE(mpn_sqr_basecase)
deflit(`FRAME',0)

	movl	PARAM_SIZE, %edx
	movl	PARAM_SRC, %eax

	cmpl	$2, %edx
	movl	PARAM_DST, %ecx

	je	L(two_limbs)

	movl	(%eax), %eax
	ja	L(three_or_more)

#------------------------------------------------------------------------------
# one limb only
	# eax	src
	# ebx
	# ecx	dst
	# edx

	mull	%eax

	movl	%eax, (%ecx)
	movl	%edx, 4(%ecx)

	ret

#------------------------------------------------------------------------------
	ALIGN(8)
L(two_limbs):
	# eax	src
	# ebx
	# ecx	dst
	# edx	size

	pushl	%ebp
	pushl	%edi

	pushl	%esi
	pushl	%ebx

	movl	%eax, %ebx
	movl	(%eax), %eax

	mull	%eax		# src[0]^2

	movl	%eax, (%ecx)	# dst[0]
	movl	%edx, %esi	# dst[1]

	movl	4(%ebx), %eax

	mull	%eax		# src[1]^2

	movl	%eax, %edi	# dst[2]
	movl	%edx, %ebp	# dst[3]

	movl	(%ebx), %eax

	mull	4(%ebx)		# src[0]*src[1]

	addl	%eax, %esi
	popl	%ebx

	adcl	%edx, %edi

	adcl	$0, %ebp
	addl	%esi, %eax

	adcl	%edi, %edx
	movl	%eax, 4(%ecx)

	adcl	$0, %ebp
	popl	%esi

	movl	%edx, 8(%ecx)
	movl	%ebp, 12(%ecx)

	popl	%edi
	popl	%ebp

	ret


#------------------------------------------------------------------------------
	ALIGN(8)
L(three_or_more):
	# eax	src low limb
	# ebx
	# ecx	dst
	# edx	size

	cmpl	$4, %edx
	pushl	%ebx
deflit(`FRAME',4)

	movl	PARAM_SRC, %ebx
	jae	L(four_or_more)


#------------------------------------------------------------------------------
# three limbs
	# eax	src low limb
	# ebx	src
	# ecx	dst
	# edx	size

	pushl	%ebp
	pushl	%edi

	mull	%eax		# src[0] ^ 2

	movl	%eax, (%ecx)
	movl	%edx, 4(%ecx)

	movl	4(%ebx), %eax
	xorl	%ebp, %ebp

	mull	%eax		# src[1] ^ 2

	movl	%eax, 8(%ecx)
	movl	%edx, 12(%ecx)

	movl	8(%ebx), %eax
	pushl	%esi		# risk of cache bank clash

	mull	%eax		# src[2] ^ 2

	movl	%eax, 16(%ecx)
	movl	%edx, 20(%ecx)

	movl	(%ebx), %eax

	mull	4(%ebx)		# src[0] * src[1]

	movl	%eax, %esi
	movl	%edx, %edi

	movl	(%ebx), %eax

	mull	8(%ebx)		# src[0] * src[2]

	addl	%eax, %edi
	movl	%edx, %ebp

	adcl	$0, %ebp
	movl	4(%ebx), %eax

	mull	8(%ebx)		# src[1] * src[2]

	xorl	%ebx, %ebx
	addl	%eax, %ebp

	# eax
	# ebx	zero, will be dst[5]
	# ecx	dst
	# edx	dst[4]
	# esi	dst[1]
	# edi	dst[2]
	# ebp	dst[3]

	adcl	$0, %edx
	addl	%esi, %esi

	adcl	%edi, %edi

	adcl	%ebp, %ebp

	adcl	%edx, %edx
	movl	4(%ecx), %eax

	adcl	$0, %ebx
	addl	%esi, %eax

	movl	%eax, 4(%ecx)
	movl	8(%ecx), %eax

	adcl	%edi, %eax
	movl	12(%ecx), %esi

	adcl	%ebp, %esi
	movl	16(%ecx), %edi

	movl	%eax, 8(%ecx)
	movl	%esi, 12(%ecx)

	adcl	%edx, %edi
	popl	%esi

	movl	20(%ecx), %eax
	movl	%edi, 16(%ecx)

	popl	%edi
	popl	%ebp

	adcl	%ebx, %eax	# no carry out of this
	popl	%ebx

	movl	%eax, 20(%ecx)

	ret


#------------------------------------------------------------------------------
	ALIGN(8)
L(four_or_more):
	# eax	src low limb
	# ebx	src
	# ecx	dst
	# edx	size
	# esi
	# edi
	# ebp
	#
	# First multiply src[0]*src[1..size-1] and store at dst[1..size].

deflit(`FRAME',4)

	pushl	%edi
FRAME_pushl()
	pushl	%esi
FRAME_pushl()

	pushl	%ebp
FRAME_pushl()
	leal	(%ecx,%edx,4), %edi	# dst end of this mul1

	leal	(%ebx,%edx,4), %esi	# src end
	movl	%ebx, %ebp		# src

	negl	%edx			# -size
	xorl	%ebx, %ebx		# clear carry limb and carry flag

	leal	1(%edx), %ecx		# -(size-1)

L(mul1):
	# eax	scratch
	# ebx	carry
	# ecx	counter, negative
	# edx	scratch
	# esi	&src[size]
	# edi	&dst[size]
	# ebp	src

	adcl	$0, %ebx
	movl	(%esi,%ecx,4), %eax

	mull	(%ebp)

	addl	%eax, %ebx

	movl	%ebx, (%edi,%ecx,4)
	incl	%ecx

	movl	%edx, %ebx
	jnz	L(mul1)


	# Add products src[n]*src[n+1..size-1] at dst[2*n-1...], for
	# n=1..size-2.
	#
	# The last two products, which are the end corner of the product
	# triangle, are handled separately to save looping overhead.  These
	# are src[size-3]*src[size-2,size-1] and src[size-2]*src[size-1].
	# If size is 4 then it's only these that need to be done.
	#
	# In the outer loop %esi is a constant, and %edi just advances by 1
	# limb each time.  The size of the operation decreases by 1 limb
	# each time.

	# eax
	# ebx	carry (needing carry flag added)
	# ecx
	# edx
	# esi	&src[size]
	# edi	&dst[size]
	# ebp

	adcl	$0, %ebx
	movl	PARAM_SIZE, %edx

	movl	%ebx, (%edi)
	subl	$4, %edx

	negl	%edx
	jz	L(corner)


L(outer):
	# ebx	previous carry limb to store
	# edx	outer loop counter (negative)
	# esi	&src[size]
	# edi	dst, pointing at stored carry limb of previous loop

	pushl	%edx			# new outer loop counter
	leal	-2(%edx), %ecx

	movl	%ebx, (%edi)
	addl	$4, %edi

	addl	$4, %ebp
	xorl	%ebx, %ebx		# initial carry limb, clear carry flag

L(inner):
	# eax	scratch
	# ebx	carry (needing carry flag added)
	# ecx	counter, negative
	# edx	scratch
	# esi	&src[size]
	# edi	dst end of this addmul
	# ebp	&src[j]

	adcl	$0, %ebx
	movl	(%esi,%ecx,4), %eax

	mull	(%ebp)

	addl	%ebx, %eax
	movl	(%edi,%ecx,4), %ebx

	adcl	$0, %edx
	addl	%eax, %ebx

	movl	%ebx, (%edi,%ecx,4)
	incl	%ecx

	movl	%edx, %ebx
	jnz	L(inner)


	adcl	$0, %ebx
	popl	%edx		# outer loop counter

	incl	%edx
	jnz	L(outer)


	movl	%ebx, (%edi)

L(corner):
	# esi	&src[size]
	# edi	&dst[2*size-4]

	movl	-8(%esi), %eax
	movl	-4(%edi), %ebx		# risk of data cache bank clash here

	mull	-12(%esi)		# src[size-2]*src[size-3]

	addl	%eax, %ebx
	movl	%edx, %ecx

	adcl	$0, %ecx
	movl	-4(%esi), %eax

	mull	-12(%esi)		# src[size-1]*src[size-3]

	addl	%ecx, %eax
	movl	(%edi), %ecx

	adcl	$0, %edx
	movl	%ebx, -4(%edi)

	addl	%eax, %ecx
	movl	%edx, %ebx

	adcl	$0, %ebx
	movl	-4(%esi), %eax

	mull	-8(%esi)		# src[size-1]*src[size-2]

	movl	%ecx, 0(%edi)
	addl	%eax, %ebx

	adcl	$0, %edx
	movl	PARAM_SIZE, %eax

	negl	%eax
	movl	%ebx, 4(%edi)

	addl	$1, %eax		# -(size-1) and clear carry
	movl	%edx, 8(%edi)


#------------------------------------------------------------------------------
# Left shift of dst[1..2*size-2], high bit shifted out becomes dst[2*size-1].

L(lshift):
	# eax	counter, negative
	# ebx	next limb
	# ecx
	# edx
	# esi
	# edi	&dst[2*size-4]
	# ebp

	movl	12(%edi,%eax,8), %ebx

	rcll	%ebx
	movl	16(%edi,%eax,8), %ecx
	
	rcll	%ecx
	movl	%ebx, 12(%edi,%eax,8)

	movl	%ecx, 16(%edi,%eax,8)
	incl	%eax

	jnz	L(lshift)


	adcl	%eax, %eax		# high bit out
	movl	PARAM_SRC, %esi

	movl	PARAM_SIZE, %ecx	# risk of cache bank clash
	movl	%eax, 12(%edi)		# dst most significant limb


#------------------------------------------------------------------------------
# Now add in the squares on the diagonal, namely src[0]^2, src[1]^2, ...,
# src[size-1]^2.  dst[0] hasn't yet been set at all yet, and just gets the
# low limb of src[0]^2.

	movl	(%esi), %eax		# src[0]
	leal	(%esi,%ecx,4), %esi	# src end

	negl	%ecx

	mull	%eax

	movl	%eax, 16(%edi,%ecx,8)	# dst[0]
	movl	%edx, %ebx

	addl	$1, %ecx		# size-1 and clear carry

L(diag):
	# eax	scratch (low product)
	# ebx	carry limb
	# ecx	counter, negative
	# edx	scratch (high product)
	# esi	&src[size]
	# edi	&dst[2*size-4]
	# ebp	scratch (fetched dst limbs)

	movl	(%esi,%ecx,4), %eax
	adcl	$0, %ebx

	mull	%eax

	movl	16-4(%edi,%ecx,8), %ebp

	addl	%ebp, %ebx
	movl	16(%edi,%ecx,8), %ebp

	adcl	%eax, %ebp
	movl	%ebx, 16-4(%edi,%ecx,8)

	movl	%ebp, 16(%edi,%ecx,8)
	incl	%ecx

	movl	%edx, %ebx
	jnz	L(diag)


	adcl	$0, %edx
	movl	16-4(%edi), %eax	# dst most significant limb

	addl	%eax, %edx
	popl	%ebp

	movl	%edx, 16-4(%edi)
	popl	%esi		# risk of cache bank clash

	popl	%edi
	popl	%ebx

	ret

EPILOGUE()
