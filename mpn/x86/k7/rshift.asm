# AMD K7 mpn_rshift -- mpn right shift.
#
# K7: 1.21 cycles/limb (at 16 limbs/loop).
#
# Future: The simple loop can probably benefit from MMX too.


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
# setups and finish-ups are done to arrange this for the loop.  To
# understand these bits, draw some pictures to see four cases at the start
# and another four at the end.  Within the loop mm1 or mm2 is a carry,
# meaning the previous two limbs from src.  At the end it's mm2 and because
# the size was decremented the last dst is yet to be written.

ifdef(`PIC',`
deflit(UNROLL_THRESHOLD, 12)
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

dnl  overlaps shift once it's been fetched
define(VAR_RETVAL,`PARAM_SHIFT')

	.text
	ALIGN(32)

PROLOGUE(mpn_rshift)
deflit(`FRAME',0)

	movl	PARAM_SIZE, %eax
	movl	PARAM_SHIFT, %ecx
	subl	$SAVE_SIZE, %esp
deflit(`FRAME',SAVE_SIZE)

	movl	%esi, SAVE_ESI
	movl	PARAM_SRC, %esi

	movl	%edi, SAVE_EDI
	movl	PARAM_DST, %edi
	decl	%eax

	movl	(%esi), %edx		# src low limb
	jnz	L(more_than_one_limb)

	shrdl	%cl, %edx, %eax		# eax was decremented to zero

 	shrl	%cl, %edx
	movl	SAVE_ESI, %esi

	movl	%edx, (%edi)		# store low limb
	movl	SAVE_EDI, %edi
	addl	$SAVE_SIZE, %esp

	ret


# -----------------------------------------------------
L(more_than_one_limb):
	# eax	size-1
	# ebx
	# ecx	shift
	# edx	src low limb
	# esi	src
	# edi	dst
	# ebp

	movl	%ebx, SAVE_EBX
	cmp	$UNROLL_THRESHOLD, %eax

	jae	L(unroll)

	movl	%eax, %ebx
	xorl	%eax, %eax

	shrdl	%cl, %edx, %eax

	leal	4(%esi,%ebx,4), %esi
	leal	(%edi,%ebx,4), %edi
	negl	%ebx

	movl	%eax, VAR_RETVAL


L(simple):
	# eax	result
	# ebx	loop counter (negative)
	# ecx	shift
	# edx	carry
	# esi	src
	# edi	dst
	# ebp

	movl	%edx, %eax
	movl	(%esi,%ebx,4), %edx

	shrdl	%cl, %edx, %eax

	movl	%eax, (%edi,%ebx,4)
	incl	%ebx

	jnz	L(simple)


	shrl	%cl, %edx
	movl	SAVE_ESI, %esi
	movl	SAVE_EBX, %ebx

	movl	%edx, (%edi)
	movl	SAVE_EDI, %edi

	movl	VAR_RETVAL, %eax
	addl	$SAVE_SIZE, %esp

	ret


#--------------------------------------------------------------------------
	# aligning here saves a couple of cycles
	ALIGN(16)
L(unroll):
	# eax	size-1
	# ebx	(saved)
	# ecx	shift
	# edx	src low limb
	# esi	src
	# edi	dst
	# ebp

	xorl	%ebx, %ebx
	movd	PARAM_SHIFT, %mm6  # directpath, whereas reg->mmx is vectorpath

	shrdl	%cl, %edx, %ebx

	testl	$4, %esi
	movl	%ebx, VAR_RETVAL

	jz	L(start_src_aligned)


	# src isn't aligned, process one limb to make it so

	movq	(%esi), %mm0		# src low two limbs
	addl	$4, %esi
	movl	%eax, PARAM_SIZE	# size-1

	addl	$4, %edi
	decl	%eax			# size-2 is new size-1

	psrlq	%mm6, %mm0
	movl	%edi, PARAM_DST		# new dst

	movd	%mm0, -4(%edi)
L(start_src_aligned):


	movq	(%esi), %mm1		# src low two limbs
	decl	%eax			# size-2, two last limbs handled at end
	testl	$4, %edi

	jz	L(start_dst_aligned)


	# dst isn't aligned, step edi to make it so, and pretend the shift
	# is 32 bits extra.  Low limb of dst handled here separately.

	movq	%mm1, %mm0
	psrlq	%mm6, %mm1
	addl	$32, %ecx		# shift+32

	movd	%mm1, (%edi)
	movq	%mm0, %mm1
	addl	$4, %edi		# new dst

	movd	%ecx, %mm6
L(start_dst_aligned):

	movq	%mm1, %mm2	# copy of src low two limbs
	negl	%ecx
	andl	$~1, %eax	# round size down to even

	movl	%eax, %ebx
	negl	%eax
	addl	$64, %ecx

	andl	$UNROLL_MASK, %eax
	decl	%ebx

	shll	%eax

ifdef(`PIC',`
	call	L(pic_calc)
L(here):
',`
	leal	L(entry) (%eax,%eax,4), %edx
	negl	%eax
')
	shrl	$UNROLL_LOG2, %ebx	# loop counter

	movd	%ecx, %mm7	# lshift = 64-rshift

	leal	ifelse(UNROLL_BYTES,256,128+) 8(%esi,%eax,2), %esi
	leal	ifelse(UNROLL_BYTES,256,128) (%edi,%eax,2), %edi
	jmp	*%edx


ifdef(`PIC',`
L(pic_calc):
	leal	L(entry)-L(here) (%eax,%eax,4), %edx
	addl	(%esp), %edx
	negl	%eax

	ret
')


#--------------------------------------------------------------------------
	ALIGN(64)
L(top):
	# eax	
	# esi	src
	# ecx	shift
	# edx	was computed jump	
	# ebx	loop counter
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
	deflit(`disp2', eval(disp1 + 8))
	deflit(`disp3', eval(disp2 + 8))

 	movq	disp0(%esi), %mm0
 	psrlq	%mm6, %mm2

 	movq	%mm0, %mm1
 	psllq	%mm7, %mm0

 	por	%mm2, %mm0
 	movq	%mm0, disp0(%edi)


 	movq	disp1(%esi), %mm0
 	psrlq	%mm6, %mm1

 	movq	%mm0, %mm2
 	psllq	%mm7, %mm0

 	por	%mm1, %mm0
 	movq	%mm0, disp1(%edi)
')

	addl	$UNROLL_BYTES, %esi
	addl	$UNROLL_BYTES, %edi
	decl	%ebx

	jns	L(top)


deflit(`disp0', ifelse(UNROLL_BYTES,256,-128))
deflit(`disp1', eval(disp0-0 + 8))

	testb	$1, PARAM_SIZE
	psrlq	%mm6, %mm2
	movl	VAR_RETVAL, %eax

	jz	L(end_even)

	
	# size is odd, one extra limb on src
	
	movd	disp0(%esi), %mm0
	movq	%mm0, %mm1

	psllq	%mm7, %mm0
	testb	$4, PARAM_DST

 	por	%mm2, %mm0

	movq	%mm0, disp0(%edi)
	jnz	L(done)

	# dst was aligned, must store one extra limb

	psrlq	%mm6, %mm1
	movd	%mm1, disp1(%edi)
L(done):

	movl	SAVE_ESI, %esi
	movl	SAVE_EBX, %ebx

	movl	SAVE_EDI, %edi
	addl	$SAVE_SIZE, %esp
	femms

	ret


L(end_even):
	# size even

	testb	$4, PARAM_DST
	jnz	L(end_even_unaligned)

	# dst was aligned, must store two limbs
	movq	%mm2, disp0(%edi)

L(end_even_unaligned):
	# dst was unaligned, must store one limb
	movd	%mm2, disp0(%edi)
	movl	SAVE_ESI, %esi

	movl	SAVE_EBX, %ebx
	movl	SAVE_EDI, %edi
	addl	$SAVE_SIZE, %esp

	femms

	ret

EPILOGUE()
