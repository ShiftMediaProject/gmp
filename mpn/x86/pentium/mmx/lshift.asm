# Intel P5 mpn_lshift -- mpn left shift.
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


# mp_limb_t mpn_lshift (mp_ptr dst, mp_srcptr src, mp_size_t size,
#                       unsigned shift);
#
# Shift src,size left by shift many bits and store the result in dst,size.
# Zeros are shifted in at the right.  Return the bits shifted out at the
# left.
#
# The comments in mpn_rshift apply here too.

defframe(PARAM_SHIFT,16)
defframe(PARAM_SIZE, 12)
defframe(PARAM_SRC,  8)
defframe(PARAM_DST,  4)
deflit(`FRAME',0)

dnl  minimum 5, because the unrolled loop can't handle less
deflit(UNROLL_THRESHOLD, 5)

	.text
	ALIGN(8)

PROLOGUE(mpn_lshift)

	pushl	%ebx
	pushl	%edi
deflit(`FRAME',8)

	movl	PARAM_SIZE, %eax
	movl	PARAM_DST, %edx

	movl	PARAM_SRC, %ebx
	movl	PARAM_SHIFT, %ecx

	cmp	$UNROLL_THRESHOLD, %eax
	jae	L(unroll)

	movl	-4(%ebx,%eax,4), %edi	# src high limb
	decl	%eax

	jnz	L(simple)

	shldl(	%cl, %edi, %eax)	# eax was decremented to zero

 	shll	%cl, %edi

	movl	%edi, (%edx)		# dst low limb
	popl	%edi			# risk of data cache bank clash

	popl	%ebx

	ret


#------------------------------------------------------------------------------
L(simple):
	# eax	size-1
	# ebx	src
	# ecx	shift
	# edx	dst
	# esi
	# edi
	# ebp
deflit(`FRAME',8)

	movd	(%ebx,%eax,4), %mm5	# src high limb

 	movd	%ecx, %mm6		# lshift
	negl	%ecx

	psllq	%mm6, %mm5
	addl	$32, %ecx

	movd	%ecx, %mm7
	psrlq	$32, %mm5		# retval


L(simple_top):
	# eax	counter, limbs, negative
	# ebx	src
	# ecx
	# edx	dst
	# esi
	# edi
	#
	# mm0	scratch
	# mm5	return value
	# mm6	shift
	# mm7	32-shift

	movq	-4(%ebx,%eax,4), %mm0
	decl	%eax

 	psrlq	%mm7, %mm0

	#

	movd	%mm0, 4(%edx,%eax,4)
	jnz	L(simple_top)


	movd	(%ebx), %mm0

	movd	%mm5, %eax
 	psllq	%mm6, %mm0

	popl	%edi
	popl	%ebx

	movd	%mm0, (%edx)

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

	movd	-4(%ebx,%eax,4), %mm5	# src high limb
	leal	(%ebx,%eax,4), %edi

 	movd	%ecx, %mm6		# lshift
	andl	$4, %edi

	psllq	%mm6, %mm5
	jz	L(start_src_aligned)


	# src isn't aligned, process high limb separately (marked xxx) to
	# make it so.
	#
	#  source     -8(ebx,%eax,4)
	#                  |
	#  +-------+-------+-------+--
	#  |               |          
	#  +-------+-------+-------+--
	#        0mod8   4mod8   0mod8
	#
	#  dest
	#     -4(edx,%eax,4)
	#          |
	#  +-------+-------+--
	#  |  xxx  |       |  
	#  +-------+-------+--

	movq	-8(%ebx,%eax,4), %mm0	# unaligned load

	psllq	%mm6, %mm0
	decl	%eax

	psrlq	$32, %mm0

	#

	movd	%mm0, (%edx,%eax,4)
L(start_src_aligned):

	movq	-8(%ebx,%eax,4), %mm1	# src high qword
	leal	(%edx,%eax,4), %edi

	andl	$4, %edi
	psrlq	$32, %mm5		# return value

	movq	-16(%ebx,%eax,4), %mm3	# src second highest qword
	jz	L(start_dst_aligned)

	# dst isn't aligned, subtract 4 to make it so, and pretend the shift
	# is 32 bits extra.  High limb of dst (marked xxx) handled here
	# separately.
	#
	#  source     -8(ebx,%eax,4)
	#                  |
	#  +-------+-------+--
	#  |      mm1      |  
	#  +-------+-------+--
	#                0mod8   4mod8
	#
	#  dest
	#     -4(edx,%eax,4)
	#          |
	#  +-------+-------+-------+--
	#  |  xxx  |               |
	#  +-------+-------+-------+--
	#        0mod8   4mod8   0mod8

	movq	%mm1, %mm0
	addl	$32, %ecx		# new shift

	psllq	%mm6, %mm0

	movd	%ecx, %mm6
	psrlq	$32, %mm0

	# wasted cycle here waiting for %mm0

	movd	%mm0, -4(%edx,%eax,4)
	subl	$4, %edx
L(start_dst_aligned):


 	psllq	%mm6, %mm1
	negl	%ecx			# -shift

        addl    $64, %ecx		# 64-shift
 	movq	%mm3, %mm2

        movd    %ecx, %mm7
	subl	$8, %eax		# size-8

 	psrlq	%mm7, %mm3

 	por	%mm1, %mm3		# mm3 ready to store
	jc	L(finish)


	# The comments in mpn_rshift apply here too.

	ALIGN(8)
L(unroll_loop):
	# eax	counter, limbs
	# ebx	src
	# ecx
	# edx	dst
	# esi
	# edi
	#
	# mm0
	# mm1
	# mm2	src qword from 48(%ebx,%eax,4)
	# mm3	dst qword ready to store to 56(%edx,%eax,4)
	#
	# mm5	return value
	# mm6	lshift
	# mm7	rshift

 	movq	8(%ebx,%eax,4), %mm0
 	psllq	%mm6, %mm2

 	movq	%mm0, %mm1
 	psrlq	%mm7, %mm0

 	movq	%mm3, 24(%edx,%eax,4)	# prev
 	por	%mm2, %mm0

 	movq	(%ebx,%eax,4), %mm3	#
 	psllq	%mm6, %mm1		#

 	movq	%mm0, 16(%edx,%eax,4)
 	movq	%mm3, %mm2		#

 	psrlq	%mm7, %mm3		#
	subl	$4, %eax

 	por	%mm1, %mm3		#
	jnc	L(unroll_loop)



L(finish):
	# eax	-4 to -1 representing respectively 0 to 3 limbs remaining

	testb	$2, %al

	jz	L(finish_no_two)

 	movq	8(%ebx,%eax,4), %mm0
 	psllq	%mm6, %mm2

 	movq	%mm0, %mm1
 	psrlq	%mm7, %mm0

 	movq	%mm3, 24(%edx,%eax,4)	# prev
 	por	%mm2, %mm0

	movq	%mm1, %mm2
	movq	%mm0, %mm3

	subl	$2, %eax
L(finish_no_two):


	# eax	-4 or -3 representing respectively 0 or 1 limbs remaining
	#
	# mm2	src prev qword, from 48(%ebx,%eax,4)
	# mm3	dst qword, for 56(%edx,%eax,4)

	testb	$1, %al
	movd	%mm5, %eax	# retval

	popl	%edi
	jz	L(finish_zero)


	# One extra src limb, destination was aligned.
	#
	#                 source                  ebx
	#                 --+---------------+-------+
	#                   |      mm2      |       |
	#                 --+---------------+-------+
	#
	# dest         edx+12           edx+4     edx
	# --+---------------+---------------+-------+
	#   |      mm3      |               |       |
	# --+---------------+---------------+-------+
	#
	# mm6 = shift
	# mm7 = ecx = 64-shift


	# One extra src limb, destination was unaligned.
	#
	#                 source                  ebx
	#                 --+---------------+-------+
	#                   |      mm2      |       |
	#                 --+---------------+-------+
	#
	#         dest         edx+12           edx+4
	#         --+---------------+---------------+
	#           |      mm3      |               |
	#         --+---------------+---------------+
	#
	# mm6 = shift+32
	# mm7 = ecx = 64-(shift+32)


	# In both cases there's one extra limb of src to fetch and combine
	# with mm2 to make a qword at 4(%edx), and in the aligned case
	# there's an extra limb of dst to be formed from that extra src limb
	# left shifted.


        movd    (%ebx), %mm0
 	psllq	%mm6, %mm2

	movq	%mm3, 12(%edx)
	psllq	$32, %mm0

        movq    %mm0, %mm1
        psrlq   %mm7, %mm0

        por     %mm2, %mm0
        psllq   %mm6, %mm1

	movq    %mm0, 4(%edx)
	psrlq	$32, %mm1

        andl	$32, %ecx
	popl	%ebx

	jz	L(finish_one_unaligned)

	movd	%mm1, (%edx)
L(finish_one_unaligned):

	emms

        ret


L(finish_zero):

	# No extra src limbs, destination was aligned.
	#
	#                 source          ebx
	#                 --+---------------+
	#                   |      mm2      |
	#                 --+---------------+
	#
	# dest          edx+8             edx
	# --+---------------+---------------+
	#   |      mm3      |               |
	# --+---------------+---------------+
	#
	# mm6 = shift
	# mm7 = ecx = 64-shift


	# No extra src limbs, destination was unaligned.
	#
	#               source            ebx
	#                 --+---------------+
	#                   |      mm2      |
	#                 --+---------------+
	#
	#         dest          edx+8   edx+4
	#         --+---------------+-------+
	#           |      mm3      |       |
	#         --+---------------+-------+
	#
	# mm6 = shift+32
	# mm7 = ecx = 64-(shift+32)


	# The movd for the unaligned case writes the same data to 4(%edx)
	# that the movq does for the aligned case.


 	movq	%mm3, 8(%edx)
	andl	$32, %ecx

 	psllq	%mm6, %mm2
	jz	L(finish_zero_unaligned)

 	movq	%mm2, (%edx)
L(finish_zero_unaligned):

	psrlq	$32, %mm2
	popl	%ebx

	movd	%mm5, %eax	# retval

	movd	%mm2, 4(%edx)

	emms

	ret

EPILOGUE()
