# AMD K7 mpn_rshift -- mpn right shift.
#
# K7: 1.21 cycles/limb (at 16 limbs/loop).


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


dnl  K7: UNROLL_COUNT cycles/limb
dnl           4           1.51
dnl           8           1.26
dnl          16           1.21
dnl          32           1.2
dnl  Maximum possible with the current code is 64.

deflit(UNROLL_COUNT, 16)


# mp_limb_t mpn_rshift (mp_ptr dst, mp_srcptr src, mp_size_t size,
#                       unsigned shift);
#
# Shift src,size right by shift many bits and store the result in dst,size.
# Zeros are shifted in at the left.  The bits shifted out at the right are
# the return value.
#
# This code uses 64-bit MMX operations, which makes it possible to handle
# two limbs at a time, for a theoretical 1.0 cycles/limb.  Plain integer
# code, on the other hand, suffers from shrd being a vector path decode and
# running at 3 cycles back-to-back.
#
# Full speed depends on source and destination being aligned, and some hairy
# setups and finish-ups are done to arrange this for the loop.

ifdef(`PIC',`
deflit(UNROLL_THRESHOLD, 10)
',`
deflit(UNROLL_THRESHOLD, 10)
')

defframe(PARAM_SHIFT,16)
defframe(PARAM_SIZE, 12)
defframe(PARAM_SRC,  8)
defframe(PARAM_DST,  4)

defframe(SAVE_EDI, -4)
defframe(SAVE_ESI, -8)
defframe(SAVE_EBX, -12)
deflit(SAVE_SIZE, 12)

	.text
	ALIGN(32)

PROLOGUE(mpn_rshift)
deflit(`FRAME',0)

	movl	PARAM_SIZE, %eax
	movl	PARAM_SRC, %edx
	subl	$SAVE_SIZE, %esp
deflit(`FRAME',SAVE_SIZE)

	movl	PARAM_SHIFT, %ecx
	movl	%edi, SAVE_EDI

	movl	PARAM_DST, %edi
	decl	%eax
	jnz	L(more_than_one_limb)

	movl	(%edx), %edx		# src limb

	shrdl(	%cl, %edx, %eax)	# eax was decremented to zero

 	shrl	%cl, %edx

	movl	%edx, (%edi)		# dst limb
	movl	SAVE_EDI, %edi
	addl	$SAVE_SIZE, %esp

	ret


#------------------------------------------------------------------------------
L(more_than_one_limb):
	# eax	size-1
	# ebx
	# ecx	shift
	# edx	src
	# esi
	# edi	dst
	# ebp

	movd	PARAM_SHIFT, %mm6	# rshift
	movd	(%edx), %mm5		# src low limb
	cmp	$UNROLL_THRESHOLD-1, %eax

	jae	L(unroll)
	leal	(%edx,%eax,4), %edx	# &src[size-1]
	leal	-4(%edi,%eax,4), %edi	# &dst[size-2]

	movd	(%edx), %mm4		# src high limb
	negl	%eax


L(simple_top):
	# eax	loop counter, limbs, negative
	# ebx
	# ecx	shift
	# edx	carry
	# edx	&src[size-1]
	# edi	&dst[size-2]
	# ebp
	#
	# mm0	scratch
	# mm4	src high limb
	# mm5	src low limb
	# mm6	shift

	movq	(%edx,%eax,4), %mm0
	incl	%eax

 	psrlq	%mm6, %mm0

	movd	%mm0, (%edi,%eax,4)
	jnz	L(simple_top)


	psllq	$32, %mm5
 	psrlq	%mm6, %mm4

	psrlq	%mm6, %mm5
	movd	%mm4, 4(%edi)		# dst high limb

	movd	%mm5, %eax		# return value

	movl	SAVE_EDI, %edi
	addl	$SAVE_SIZE, %esp
	emms

	ret


#--------------------------------------------------------------------------
	ALIGN(16)
L(unroll):
	# eax	size-1
	# ebx
	# ecx	shift
	# edx	src
	# esi
	# edi	dst
	# ebp
	#
	# mm5	src low limb
	# mm6	rshift

	testb	$4, %dl
	movl	%esi, SAVE_ESI
	movl	%ebx, SAVE_EBX

	psllq	$32, %mm5
	jz	L(start_src_aligned)


	# src isn't aligned, process low limb separately (marked xxx) and
	# step src and dst by one limb, making src aligned.
	#
	# source                  edx
	# --+-------+-------+-------+
	#           |          xxx  |
	# --+-------+-------+-------+
	#         4mod8   0mod8   4mod8
	#
	#         dest            edi
	#         --+-------+-------+
	#           |       |  xxx  |  
	#         --+-------+-------+

	movq	(%edx), %mm0		# src low two limbs
	addl	$4, %edx
	movl	%eax, PARAM_SIZE	# size-1

	addl	$4, %edi
	decl	%eax			# size-2 is new size-1

	psrlq	%mm6, %mm0
	movl	%edi, PARAM_DST		# new dst

	movd	%mm0, -4(%edi)
L(start_src_aligned):


	movq	(%edx), %mm1		# src low two limbs
	decl	%eax			# size-2, two last limbs handled at end
	testl	$4, %edi

	psrlq	%mm6, %mm5
	jz	L(start_dst_aligned)


	# dst isn't aligned, add 4 to make it so, and pretend the shift is
	# 32 bits extra.  Low limb of dst (marked xxx) handled here separately.
	#
	#          source          edx
	#          --+-------+-------+
	#            |      mm1      |
	#          --+-------+-------+
	#                  4mod8   0mod8
	#
	#  dest                    edi
	#  --+-------+-------+-------+
	#                    |  xxx  |        
	#  --+-------+-------+-------+
	#          4mod8   0mod8   4mod8

	movq	%mm1, %mm0
	psrlq	%mm6, %mm1
	addl	$32, %ecx		# shift+32

	movd	%mm1, (%edi)
	movq	%mm0, %mm1
	addl	$4, %edi		# new dst

	movd	%ecx, %mm6
L(start_dst_aligned):


	movq	%mm1, %mm2		# copy of src low two limbs
	negl	%ecx
	andl	$~1, %eax		# round size down to even

	movl	%eax, %ebx
	negl	%eax
	addl	$64, %ecx

	andl	$UNROLL_MASK, %eax
	decl	%ebx

	shll	%eax

	movd	%ecx, %mm7		# lshift = 64-rshift

ifdef(`PIC',`
	call	L(pic_calc)
L(here):
',`
	leal	L(entry) (%eax,%eax,4), %esi
	negl	%eax
')
	shrl	$UNROLL_LOG2, %ebx	# loop counter

	leal	ifelse(UNROLL_BYTES,256,128+) 8(%edx,%eax,2), %edx
	leal	ifelse(UNROLL_BYTES,256,128) (%edi,%eax,2), %edi
	movl	PARAM_SIZE, %eax	# for use at end

	jmp	*%esi


ifdef(`PIC',`
L(pic_calc):
	# See README.family about old gas bugs
	leal	(%eax,%eax,4), %esi
	addl	$L(entry)-L(here), %esi
	addl	(%esp), %esi
	negl	%eax

	ret
')


#------------------------------------------------------------------------------
	ALIGN(64)
L(top):
	# eax	size, for use at end
	# ebx	loop counter
	# ecx	lshift
	# edx	src
	# esi	was computed jump
	# edi	dst
	# ebp
	#
	# mm0	scratch
	# mm1	\ carry (alternating)
	# mm2	/
	# mm6	rshift
	# mm7	lshift
	#
	# 10 code bytes/limb
	#
	# The two chunks differ in whether mm1 or mm2 hold the carry.
	# The computed jump puts the initial carry in both mm1 and mm2.
	
L(entry):
deflit(CHUNK_COUNT, 4)
forloop(i, 0, UNROLL_COUNT/CHUNK_COUNT-1, `
	deflit(`disp0', eval(i*CHUNK_COUNT*4 ifelse(UNROLL_BYTES,256,-128)))
	deflit(`disp1', eval(disp0 + 8))

 	movq	disp0(%edx), %mm0
 	psrlq	%mm6, %mm2

 	movq	%mm0, %mm1
 	psllq	%mm7, %mm0

 	por	%mm2, %mm0
 	movq	%mm0, disp0(%edi)


 	movq	disp1(%edx), %mm0
 	psrlq	%mm6, %mm1

 	movq	%mm0, %mm2
 	psllq	%mm7, %mm0

 	por	%mm1, %mm0
 	movq	%mm0, disp1(%edi)
')

	addl	$UNROLL_BYTES, %edx
	addl	$UNROLL_BYTES, %edi
	decl	%ebx

	jns	L(top)


deflit(`disp0', ifelse(UNROLL_BYTES,256,-128))
deflit(`disp1', eval(disp0-0 + 8))

	testb	$1, %al
	psrlq	%mm6, %mm2	# wanted rshifted in all cases below
	movl	SAVE_ESI, %esi

	movd	%mm5, %eax		# return value

	movl	SAVE_EBX, %ebx
	jz	L(end_even)

	
	# Size odd, destination was aligned.
	#
	# source
	#       edx
	# +-------+---------------+--
	# |       |      mm2      |
	# +-------+---------------+--
	#
	# dest                  edi
	# +-------+---------------+---------------+--
	# |       |               |    written    |
	# +-------+---------------+---------------+--
	#
	# mm6 = shift
	# mm7 = ecx = 64-shift


	# Size odd, destination was unaligned.
	#
	# source
	#       edx
	# +-------+---------------+--
	# |       |      mm2      |
	# +-------+---------------+--
	#
	# dest          edi
	# +---------------+---------------+--
	# |               |    written    |
	# +---------------+---------------+--
	#
	# mm6 = shift+32
	# mm7 = ecx = 64-(shift+32)


	# In both cases there's one extra limb of src to fetch and combine
	# with mm2 to make a qword to store, and in the aligned case there's
	# a further extra limb of dst to be formed.


	movd	disp0(%edx), %mm0
	movq	%mm0, %mm1

	psllq	%mm7, %mm0
	testb	$32, %cl

 	por	%mm2, %mm0
	psrlq	%mm6, %mm1

	movq	%mm0, disp0(%edi)
	jz	L(finish_odd_unaligned)

	movd	%mm1, disp1(%edi)
L(finish_odd_unaligned):

	movl	SAVE_EDI, %edi
	addl	$SAVE_SIZE, %esp
	emms

	ret


L(end_even):

	# Size even, destination was aligned.
	#
	# source
	# +---------------+--
	# |      mm2      |
	# +---------------+--
	#
	# dest          edi
	# +---------------+---------------+--
	# |               |      mm3      |
	# +---------------+---------------+--
	#
	# mm6 = shift
	# mm7 = ecx = 64-shift


	# Size even, destination was unaligned.
	#
	# source
	# +---------------+--
	# |      mm2      |
	# +---------------+--
	#
	# dest  edi
	# +-------+---------------+--
	# |       |      mm3      |
	# +-------+---------------+--
	#
	# mm6 = shift+32
	# mm7 = 64-(shift+32)


	# The movd for the unaligned case is the same data as the movq for
	# the aligned case, it's just a choice between whether one or two
	# limbs should be written.


	testb	$32, %cl
	movd	%mm2, disp0(%edi)

	jz	L(end_even_unaligned)

	movq	%mm2, disp0(%edi)
L(end_even_unaligned):

	movl	SAVE_EDI, %edi
	addl	$SAVE_SIZE, %esp
	emms

	ret

EPILOGUE()
