# AMD K7 mpn_lshift -- mpn left shift.
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


# mp_limb_t mpn_lshift (mp_ptr dst, mp_srcptr src, mp_size_t size,
#                       unsigned shift);
#
# Shift src,size left by shift many bits and store the result in dst,size.
# Zeros are shifted in at the right.  The bits shifted out at the left are
# the return value.
#
# The comments in mpn_rshift apply here too.

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

PROLOGUE(mpn_lshift)
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

	movl	(%edx), %edx

	shldl(	%cl, %edx, %eax)	# eax was decremented to zero

 	shll	%cl, %edx

	movl	%edx, (%edi)
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

	movd	PARAM_SHIFT, %mm6
	movd	(%edx,%eax,4), %mm5	# src high limb
	cmp	$UNROLL_THRESHOLD-1, %eax

	jae	L(unroll)
	negl	%ecx
	movd	(%edx), %mm4		# src low limb

	addl	$32, %ecx

	movd	%ecx, %mm7

L(simple_top):
	# eax	loop counter, limbs
	# ebx
	# ecx
	# edx	src
	# esi
	# edi	dst
	# ebp
	#
	# mm0	scratch
	# mm4	src low limb
	# mm5	src high limb
	# mm6	shift
	# mm7	32-shift

	movq	-4(%edx,%eax,4), %mm0
	decl	%eax

 	psrlq	%mm7, %mm0

	movd	%mm0, 4(%edi,%eax,4)
	jnz	L(simple_top)


	psllq	%mm6, %mm5
 	psllq	%mm6, %mm4

	psrlq	$32, %mm5
	movd	%mm4, (%edi)		# dst low limb

	movd	%mm5, %eax		# return value

	movl	SAVE_EDI, %edi
	addl	$SAVE_SIZE, %esp
	emms

	ret


#--------------------------------------------------------------------------
	ALIGN(16)
L(unroll):
	# eax	size-1
	# ebx	(saved)
	# ecx	shift
	# edx	src
	# esi
	# edi	dst
	# ebp
	#
	# mm5	src high limb, for return value
	# mm6	lshift

	movl	%esi, SAVE_ESI
	movl	%ebx, SAVE_EBX
	leal	-4(%edx,%eax,4), %edx   # &src[size-2]

	testb	$4, %dl
	movq	(%edx), %mm1		# src high qword

	jz	L(start_src_aligned)


	# src isn't aligned, process high limb (marked xxx) separately to
	# make it so
	#
	#  source    -4(edx,%eax,4)
	#                  |
	#  +-------+-------+-------+--
	#  |  xxx          |
	#  +-------+-------+-------+--
	#        0mod8   4mod8   0mod8
	#
	#  dest      -4(edi,%eax,4)
	#                  |
	#  +-------+-------+--
	#  |  xxx  |       |  
	#  +-------+-------+--

	psllq	%mm6, %mm1
	subl	$4, %edx
	movl	%eax, PARAM_SIZE	# size-1

	psrlq	$32, %mm1
	decl	%eax			# size-2 is new size-1

	movd	%mm1, 4(%edi,%eax,4)
	movq	(%edx), %mm1		# new src high qword
L(start_src_aligned):


        leal    -4(%edi,%eax,4), %edi   # &dst[size-2]
	psllq	%mm6, %mm5

	testl	$4, %edi
	psrlq	$32, %mm5		# return value

	jz	L(start_dst_aligned)


	# dst isn't aligned, subtract 4 bytes to make it so, and pretend the
	# shift is 32 bits extra.  High limb of dst (marked xxx) handled
	# here separately.
	#
	#  source       %edx
	#  +-------+-------+--
	#  |      mm1      |  
	#  +-------+-------+--
	#                0mod8   4mod8
	#
	#  dest         %edi
	#  +-------+-------+-------+--
	#  |  xxx  |          
	#  +-------+-------+-------+--
	#        0mod8   4mod8   0mod8

	movq	%mm1, %mm0
	psllq	%mm6, %mm1
	addl	$32, %ecx		# shift+32

	psrlq	$32, %mm1

	movd	%mm1, 4(%edi)
	movq	%mm0, %mm1
	subl	$4, %edi

	movd	%ecx, %mm6		# new lshift
L(start_dst_aligned):

	decl	%eax			# size-2, two last limbs handled at end
	movq	%mm1, %mm2		# copy of src high qword
	negl	%ecx

	andl	$~1, %eax		# round size down to even
	addl	$64, %ecx

	movl	%eax, %ebx
	negl	%eax

	andl	$UNROLL_MASK, %eax
	decl	%ebx

	shll	%eax

	movd	%ecx, %mm7		# rshift = 64-lshift

ifdef(`PIC',`
	call	L(pic_calc)
L(here):
',`
	leal	L(entry) (%eax,%eax,4), %esi
')
	shrl	$UNROLL_LOG2, %ebx	# loop counter

	leal	ifelse(UNROLL_BYTES,256,128) -8(%edx,%eax,2), %edx
	leal	ifelse(UNROLL_BYTES,256,128) (%edi,%eax,2), %edi
	movl	PARAM_SIZE, %eax	# for use at end
	jmp	*%esi


ifdef(`PIC',`
L(pic_calc):
	# See README.family about old gas bugs
	leal	(%eax,%eax,4), %esi
	addl	$L(entry)-L(here), %esi
	addl	(%esp), %esi

	ret
')


#------------------------------------------------------------------------------
	ALIGN(32)
L(top):
	# eax	size (for use at end)
	# ebx	loop counter
	# ecx	rshift
	# edx	src
	# esi	computed jump
	# edi	dst
	# ebp
	#
	# mm0	scratch
	# mm1	\ carry (alternating, mm2 first)
	# mm2	/
	# mm6	lshift
	# mm7	rshift
	#
	# 10 code bytes/limb
	#
	# The two chunks differ in whether mm1 or mm2 hold the carry.
	# The computed jump puts the initial carry in both mm1 and mm2.
	
L(entry):
deflit(CHUNK_COUNT, 4)
forloop(i, 0, UNROLL_COUNT/CHUNK_COUNT-1, `
	deflit(`disp0', eval(-i*CHUNK_COUNT*4 ifelse(UNROLL_BYTES,256,-128)))
	deflit(`disp1', eval(disp0 - 8))

 	movq	disp0(%edx), %mm0
 	psllq	%mm6, %mm2

 	movq	%mm0, %mm1
 	psrlq	%mm7, %mm0

 	por	%mm2, %mm0
 	movq	%mm0, disp0(%edi)


 	movq	disp1(%edx), %mm0
 	psllq	%mm6, %mm1

 	movq	%mm0, %mm2
 	psrlq	%mm7, %mm0

 	por	%mm1, %mm0
 	movq	%mm0, disp1(%edi)
')

	subl	$UNROLL_BYTES, %edx
	subl	$UNROLL_BYTES, %edi
	decl	%ebx

	jns	L(top)



define(`disp', `m4_empty_if_zero(eval($1 ifelse(UNROLL_BYTES,256,-128)))')

L(end):
	testb	$1, %al
	movl	SAVE_EBX, %ebx
	psllq	%mm6, %mm2	# wanted left shifted in all cases below

	movd	%mm5, %eax

	movl	SAVE_ESI, %esi
	jz	L(end_even)


L(end_odd):

	# Size odd, destination was aligned.
	#
	#                 source        edx+8   edx+4
	#                 --+---------------+-------+
	#                   |      mm2      |       |
	#                 --+---------------+-------+
	#
	# dest                            edi
	# --+---------------+---------------+-------+
	#   |   written     |               |       |
	# --+---------------+---------------+-------+
	#
	# mm6 = shift
	# mm7 = ecx = 64-shift


	# Size odd, destination was unaligned.
	#
	#                 source        edx+8   edx+4
	#                 --+---------------+-------+
	#                   |      mm2      |       |
	#                 --+---------------+-------+
	#
	#         dest                            edi
	#         --+---------------+---------------+
	#           |   written     |               |
	#         --+---------------+---------------+
	#
	# mm6 = shift+32
	# mm7 = ecx = 64-(shift+32)


	# In both cases there's one extra limb of src to fetch and combine
	# with mm2 to make a qword at (%edi), and in the aligned case
	# there's an extra limb of dst to be formed from that extra src limb
	# left shifted.

	movd	disp(4) (%edx), %mm0
	testb	$32, %cl

	movq	%mm0, %mm1
	psllq	$32, %mm0

	psrlq	%mm7, %mm0
	psllq	%mm6, %mm1

	por	%mm2, %mm0

	movq	%mm0, disp(0) (%edi)
	jz	L(end_odd_unaligned)
	movd	%mm1, disp(-4) (%edi)
L(end_odd_unaligned):

	movl	SAVE_EDI, %edi
	addl	$SAVE_SIZE, %esp
	emms

	ret


L(end_even):

	# Size even, destination was aligned.
	#
	#                 source        edx+8
	#                 --+---------------+
	#                   |      mm2      |
	#                 --+---------------+
	#
	# dest                            edi
	# --+---------------+---------------+
	#   |   written     |               |
	# --+---------------+---------------+
	#
	# mm6 = shift
	# mm7 = ecx = 64-shift


	# Size even, destination was unaligned.
	#
	#               source          edx+8
	#                 --+---------------+
	#                   |      mm2      |
	#                 --+---------------+
	#
	#         dest                  edi+4
	#         --+---------------+-------+
	#           |    written    |       |
	#         --+---------------+-------+
	#
	# mm6 = shift+32
	# mm7 = ecx = 64-(shift+32)


	# The movq for the aligned case overwrites the movd for the
	# unaligned case.

	movq	%mm2, %mm0
	psrlq	$32, %mm2

	testb	$32, %cl
	movd	%mm2, disp(4) (%edi)

	jz	L(end_even_unaligned)
	movq	%mm0, disp(0) (%edi)
L(end_even_unaligned):

	movl	SAVE_EDI, %edi
	addl	$SAVE_SIZE, %esp
	emms

	ret

EPILOGUE()
