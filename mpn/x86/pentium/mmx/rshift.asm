# Intel P5 mpn_rshift -- mpn right shift.
#
# P5: 1.75 cycles/limb.


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
#
# It takes 6 mmx instructions to process 2 limbs, making 1.5 cycles/limb,
# and with a 4 limb loop and 1 cycle of loop overhead the total is 1.75 c/l.
#
# Full speed depends on source and destination being aligned.  Unaligned mmx
# loads and stores on P5 don't pair and have a 2 cycle penalty.  Some hairy
# setups and finish-ups are done to ensure alignment for the loop.
#
# MMX shifts work out a bit faster even for the simple loop.

defframe(PARAM_SHIFT,16)
defframe(PARAM_SIZE, 12)
defframe(PARAM_SRC,  8)
defframe(PARAM_DST,  4)
deflit(`FRAME',0)

dnl  Minimum 5, because the unrolled loop can't handle less.
deflit(UNROLL_THRESHOLD, 5)

	.text
	ALIGN(8)

PROLOGUE(mpn_rshift)

	pushl	%ebx
	pushl	%edi
deflit(`FRAME',8)

	movl	PARAM_SIZE, %eax
	movl	PARAM_DST, %edx

	movl	PARAM_SRC, %ebx
	movl	PARAM_SHIFT, %ecx

	cmp	$UNROLL_THRESHOLD, %eax
	jae	L(unroll)

	decl	%eax
	movl	(%ebx), %edi		# src low limb

	jnz	L(simple)

	shrdl(	%cl, %edi, %eax)	# eax was decremented to zero

 	shrl	%cl, %edi

	movl	%edi, (%edx)		# dst low limb
	popl	%edi			# risk of data cache bank clash

	popl	%ebx

	ret


#------------------------------------------------------------------------------
	ALIGN(8)
L(simple):
	# eax	size-1
	# ebx	src
	# ecx	shift
	# edx	dst
	# esi
	# edi
	# ebp
deflit(`FRAME',8)

	movd	(%ebx), %mm5		# src[0]
	leal	(%ebx,%eax,4), %ebx	# &src[size-1]

 	movd	%ecx, %mm6		# rshift
	leal	-4(%edx,%eax,4), %edx	# &dst[size-2]

	psllq	$32, %mm5
	negl	%eax


# This loop is 5 or 8 cycles, with every second load unaligned and a wasted
# cycle waiting for the mm0 result to be ready.  For comparison a shrdl is 4
# cycles and would be 8 in a simple loop.  Using mmx helps the return value
# and last limb calculations too.

L(simple_top):
	# eax	counter, limbs, negative
	# ebx	&src[size-1]
	# ecx	return value
	# edx	&dst[size-2]
	#
	# mm0	scratch
	# mm5	return value
	# mm6	shift

	movq	(%ebx,%eax,4), %mm0
	incl	%eax

 	psrlq	%mm6, %mm0

	movd	%mm0, (%edx,%eax,4)
	jnz	L(simple_top)


	movd	(%ebx), %mm0
	psrlq	%mm6, %mm5		# return value

 	psrlq	%mm6, %mm0
	popl	%edi

	movd	%mm5, %eax
	popl	%ebx

	movd	%mm0, 4(%edx)

	emms

	ret


#------------------------------------------------------------------------------
	ALIGN(8)
L(unroll):
	# eax	size
	# ebx	src
	# ecx	shift
	# edx	dst
	# esi
	# edi
	# ebp
deflit(`FRAME',8)

	movd	(%ebx), %mm5		# src[0]
	movl	$4, %edi

 	movd	%ecx, %mm6		# rshift
	testl	%edi, %ebx

	psllq	$32, %mm5
	jz	L(start_src_aligned)


	# src isn't aligned, process low limb separately (marked xxx) and
	# step src and dst by one limb, making src aligned.
	#
	# source                  ebx
	# --+-------+-------+-------+
	#           |          xxx  |
	# --+-------+-------+-------+
	#         4mod8   0mod8   4mod8
	#
	#         dest            edx
	#         --+-------+-------+
	#           |       |  xxx  |  
	#         --+-------+-------+

	movq	(%ebx), %mm0		# unaligned load

	psrlq	%mm6, %mm0
	addl	$4, %ebx

	decl	%eax

	movd	%mm0, (%edx)
	addl	$4, %edx
L(start_src_aligned):


	movq	(%ebx), %mm1
	testl	%edi, %edx

	psrlq	%mm6, %mm5		# retval
	jz	L(start_dst_aligned)

	# dst isn't aligned, add 4 to make it so, and pretend the shift is
	# 32 bits extra.  Low limb of dst (marked xxx) handled here
	# separately.
	#
	#          source          ebx
	#          --+-------+-------+
	#            |      mm1      |
	#          --+-------+-------+
	#                  4mod8   0mod8
	#
	#  dest                    edx
	#  --+-------+-------+-------+
	#                    |  xxx  |        
	#  --+-------+-------+-------+
	#          4mod8   0mod8   4mod8

	movq	%mm1, %mm0
	addl	$32, %ecx		# new shift

	psrlq	%mm6, %mm0

	movd	%ecx, %mm6

	movd	%mm0, (%edx)
	addl	$4, %edx
L(start_dst_aligned):


	movq	8(%ebx), %mm3
	negl	%ecx

 	movq	%mm3, %mm2		# mm2 src qword
        addl    $64, %ecx

        movd    %ecx, %mm7
 	psrlq	%mm6, %mm1

	leal	-12(%ebx,%eax,4), %ebx
	leal	-20(%edx,%eax,4), %edx

 	psllq	%mm7, %mm3
	subl	$7, %eax		# size-7

 	por	%mm1, %mm3		# mm3 ready to store
	negl	%eax			# -(size-7)

	jns	L(finish)


	# This loop is the important bit, the rest is just support.  Careful
	# instruction scheduling achieves the claimed 1.75 c/l.  The
	# relevant parts of the pairing rules are:
	#
	# - mmx loads and stores execute only in the U pipe
	# - only one mmx shift in a pair
	# - wait one cycle before storing an mmx register result
	# - the usual address generation interlock
	#
	# Two qword calculations are slightly interleaved.  The instructions
	# marked "#" belong to the second qword, and the "# prev" one is for
	# the second qword from the previous iteration.

	ALIGN(8)
L(unroll_loop):
	# eax	counter, limbs, negative
	# ebx	&src[size-12]
	# ecx
	# edx	&dst[size-12]
	# esi
	# edi
	#
	# mm0
	# mm1
	# mm2	src qword from -8(%ebx,%eax,4)
	# mm3	dst qword ready to store to -8(%edx,%eax,4)
	#
	# mm5	return value
	# mm6	rshift
	# mm7	lshift

 	movq	(%ebx,%eax,4), %mm0
 	psrlq	%mm6, %mm2

 	movq	%mm0, %mm1
 	psllq	%mm7, %mm0

 	movq	%mm3, -8(%edx,%eax,4)	# prev
 	por	%mm2, %mm0

 	movq	8(%ebx,%eax,4), %mm3	#
 	psrlq	%mm6, %mm1		#

 	movq	%mm0, (%edx,%eax,4)
 	movq	%mm3, %mm2		#

 	psllq	%mm7, %mm3		#
	addl	$4, %eax

 	por	%mm1, %mm3		#
	js	L(unroll_loop)


L(finish):
	# eax	0 to 3 representing respectively 3 to 0 limbs remaining

	testb	$2, %al

	jnz	L(finish_no_two)

 	movq	(%ebx,%eax,4), %mm0
 	psrlq	%mm6, %mm2

 	movq	%mm0, %mm1
 	psllq	%mm7, %mm0

 	movq	%mm3, -8(%edx,%eax,4)	# prev
 	por	%mm2, %mm0

	movq	%mm1, %mm2
	movq	%mm0, %mm3

	addl	$2, %eax
L(finish_no_two):


	# eax	2 or 3 representing respectively 1 or 0 limbs remaining
	#
	# mm2	src prev qword, from -8(%ebx,%eax,4)
	# mm3	dst qword, for -8(%edx,%eax,4)

	testb	$1, %al
	popl	%edi

	movd	%mm5, %eax	# retval
	jnz	L(finish_zero)


	# One extra limb, destination was aligned.
	#
	# source                ebx
	# +-------+---------------+--
	# |       |      mm2      |
	# +-------+---------------+--
	#
	# dest                                  edx
	# +-------+---------------+---------------+--
	# |       |               |      mm3      |
	# +-------+---------------+---------------+--
	#
	# mm6 = shift
	# mm7 = ecx = 64-shift


	# One extra limb, destination was unaligned.
	#
	# source                ebx
	# +-------+---------------+--
	# |       |      mm2      |
	# +-------+---------------+--
	#
	# dest                          edx
	# +---------------+---------------+--
	# |               |      mm3      |
	# +---------------+---------------+--
	#
	# mm6 = shift+32
	# mm7 = ecx = 64-(shift+32)


	# In both cases there's one extra limb of src to fetch and combine
	# with mm2 to make a qword at 8(%edx), and in the aligned case
	# there's a further extra limb of dst to be formed.


        movd    8(%ebx), %mm0
 	psrlq	%mm6, %mm2

        movq    %mm0, %mm1
        psllq   %mm7, %mm0

	movq	%mm3, (%edx)
        por     %mm2, %mm0

        psrlq   %mm6, %mm1
        andl	$32, %ecx

	popl	%ebx
	jz	L(finish_one_unaligned)

        # dst was aligned, must store one extra limb
	movd	%mm1, 16(%edx)
L(finish_one_unaligned):

	movq    %mm0, 8(%edx)

	emms

        ret


L(finish_zero):

	# No extra limbs, destination was aligned.
	#
	# source        ebx
	# +---------------+--
	# |      mm2      |
	# +---------------+--
	#
	# dest                        edx+4
	# +---------------+---------------+--
	# |               |      mm3      |
	# +---------------+---------------+--
	#
	# mm6 = shift
	# mm7 = ecx = 64-shift


	# No extra limbs, destination was unaligned.
	#
	# source        ebx
	# +---------------+--
	# |      mm2      |
	# +---------------+--
	#
	# dest                edx+4
	# +-------+---------------+--
	# |       |      mm3      |
	# +-------+---------------+--
	#
	# mm6 = shift+32
	# mm7 = 64-(shift+32)


	# The movd for the unaligned case is clearly the same data as the
	# movq for the aligned case, it's just a choice between whether one
	# or two limbs should be written.


 	movq	%mm3, 4(%edx)
 	psrlq	%mm6, %mm2

 	movd	%mm2, 12(%edx)
	andl	$32, %ecx

	popl	%ebx
	jz	L(finish_zero_unaligned)

 	movq	%mm2, 12(%edx)
L(finish_zero_unaligned):

	emms

	ret

EPILOGUE()
