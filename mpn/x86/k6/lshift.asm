# AMD K6 mpn_lshift -- mpn left shift.
#
# K6: 1.8 cycles/limb (at 32 limbs/loop), PIC adds 4 cycles at the start.
#
# Future: A version with the same speed but less unrolling is in progress.


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


dnl  K6: UNROLL_COUNT cycles/limb
dnl           8           2.0
dnl          16           1.9
dnl          32           1.8
dnl          64           1.75
dnl  Maximum possible with the current code is 64.

deflit(UNROLL_COUNT, 32)


# mp_limb_t mpn_lshift (mp_ptr dst, mp_srcptr src, mp_size_t size,
#                       unsigned shift);
#
# Shift src,size left by shift many bits and store the result in dst,size.
# Zeros are shifted in at the right.  Return the bits shifted out at the
# left.
#
# This code uses MMX to handle two limbs at a time, and with the MMX
# instructions being short decodes, it approaches 1.5 cycles/limb.  Plain
# integer code, on the other hand, suffers from shrd being a 2 cycle vector
# decode, meaning at best 3 cycles/limb.
#
# In practice this code is 1.75 cycles/limb, or 7 cycles per 4 limb chunk in
# the loop.  It's not clear where the extra cycle is going.  No 1.5 c/l
# version has been found.
#
# Full speed depends on the destination being aligned to an 8 byte boundary,
# and one limb is separately processed at the start to arrange this if
# necessary.
#
# Aligning the source to an 8 byte boundary makes no difference, ie. 0mod8
# or 4mod8 are the same speed.
#
# Two limbs are left to the end to process since zeros are shifted in there,
# rather than src data.  Or if the size is odd three limbs are left since
# the unrolled loop only handles an even number of limbs.


dnl  The unrolled loop takes slightly longer when dst+4*size isn't a
dnl  multiple of 8, so check both aligned and unaligned destinations when
dnl  determining these thresholds.  Must be at least 4, since the unrolled
dnl  loop can't handle smaller.

ifdef(`PIC',`
deflit(UNROLL_THRESHOLD, 10)
',`
deflit(UNROLL_THRESHOLD, 8)
')

defframe(PARAM_SHIFT,16)
defframe(PARAM_SIZE, 12)
defframe(PARAM_SRC,  8)
defframe(PARAM_DST,  4)

dnl  retval held here after shift has been fetched
define(VAR_RETVAL,`PARAM_SHIFT')

	.text
	ALIGN(32)

PROLOGUE(mpn_lshift)
deflit(`FRAME',0)

	movl	PARAM_SIZE, %eax
	pushl	%ebx
deflit(`FRAME',4)

	movl	PARAM_SRC, %ebx
	decl	%eax

	movl	PARAM_SHIFT, %ecx
	jnz	L(more_than_one_limb)


	movl	(%ebx), %edx	# src limb
	movl	PARAM_DST, %ebx

	shldl	%cl, %edx, %eax	# eax was decremented to zero

 	shll	%cl, %edx

	movl	%edx, (%ebx)	# dst limb
	popl	%ebx

	ret


# -----------------------------------------------------
	ALIGN(16)
L(more_than_one_limb):
	# eax	size-1
	# ebx	src
	# ecx	shift
	# edx
	# esi
	# edi
	# ebp

	cmp	$UNROLL_THRESHOLD-1, %eax
	pushl	%edi
deflit(`FRAME',8)
	
	movl	(%ebx,%eax,4), %edx	# src high limb
	pushl	%esi
deflit(`FRAME',12)

	movl	$0, %esi
	jae	L(unroll)


	shldl	%cl, %edx, %esi

	movl	PARAM_DST, %edi
	movl	%esi, VAR_RETVAL


	# aligning to 16 here slows down by 1 cycle
L(simple):
	# eax	counter
	# ebx	src
	# ecx	shift
	# edx	carry
	# esi	scratch
	# edi	dst
	# ebp

	movl	%edx, %esi
	movl	-4(%ebx,%eax,4), %edx

	shldl	%cl, %edx, %esi

	movl	%esi, (%edi,%eax,4)
	decl	%eax

	jnz	L(simple)


	shll	%cl, %edx	# result low limb, zeros shifted in
	movl	VAR_RETVAL, %eax

	movl	%edx, (%edi)
	popl	%esi

	popl	%edi
	popl	%ebx

	ret


# -----------------------------------------------------
# aligning here saves a couple of cycles
#	ALIGN(16)
L(unroll):
	# eax	size-1
	# ebx	src
	# ecx	shift
	# edx	src high limb
	# esi	zero
	# edi	dst
	# ebp

deflit(`FRAME',12)

	movl	PARAM_DST, %edi
	movd	%ecx, %mm6		# lshift

	shldl	%cl, %edx, %esi

	leal	-4(%edi,%eax,4), %edi	# dst point at high qword
	movl	%esi, VAR_RETVAL

	testl	$4, %edi

	leal	-4(%ebx,%eax,4), %ebx	# src point at high qword
	jz	L(start_dst_aligned)


	# dst isn't aligned, process one limb to make it aligned
	movq	(%ebx), %mm0		# src high qword
	decl	%eax

	psllq	%mm6, %mm0
	subl	$4, %ebx

	psrlq	$32, %mm0
	subl	$4, %edi

	movd	%mm0, 8(%edi)
L(start_dst_aligned):


	movl	%eax, %edx	# size held for end
	decl	%eax		# size-2, two last limbs handled at end

	andl	$~1, %eax	# round size down to even
	movl	%eax, %esi

	movq	(%ebx), %mm1	# src high qword
	negl	%eax

	andl	$UNROLL_MASK, %eax
	negl	%ecx

	shll	%eax
	movq	%mm1, %mm2	# initial carry in both mm1 and mm2

	leal	ifelse(UNROLL_BYTES,256,-124) -8(%ebx,%eax,2), %ebx
	addl	$64, %ecx

	leal	ifelse(UNROLL_BYTES,256,-124) (%edi,%eax,2), %edi
	decl	%esi		# loop counter

ifdef(`PIC',`
	call	L(pic_calc)
L(here):
',`
	leal	L(entry) (%eax,%eax,4), %eax	# 10 bytes code per limb
')
	movd	%ecx, %mm7	# rshift = 64-lshift

	shrl	$UNROLL_LOG2, %esi
	jmp	*%eax


ifdef(`PIC',`
L(pic_calc):
	leal	L(entry)-L(here) (%eax,%eax,4), %eax
	addl	(%esp), %eax
	ret
')


# ---------------------------------------------------
	ALIGN(32)
L(top):
	# eax	was computed jump
	# ebx	src
	# ecx	shift
	# edx	size-1 (for use at end)
	# esi	loop counter
	# edi	dst
	# ebp
	#
	# mm0   scratch
	# mm1 \	previous src qword (alternately in mm1 or mm2)
	# mm2 /
	# mm6	lshift
	# mm7	rshift (64-lshift)
	#
	# 10 code bytes per limb processed
	#
	# The two chunks differ in whether mm1 or mm2 holds the carry.
	# The computed jump puts the initial carry in both mm1 and mm2.

L(entry):
deflit(CHUNK_COUNT, 4)
forloop(i, 0, UNROLL_COUNT/CHUNK_COUNT-1, `
deflit(`disp0', eval(-i*CHUNK_COUNT*4 ifelse(UNROLL_BYTES,256,+124)))
deflit(`disp1', eval(disp0 - 8))

 	movq	disp0(%ebx), %mm0
 	psllq	%mm6, %mm2

 	movq	%mm0, %mm1
 	psrlq	%mm7, %mm0

 	por	%mm2, %mm0
 	movq	%mm0, disp0(%edi)


 	movq	disp1(%ebx), %mm0
 	psllq	%mm6, %mm1

 	movq	%mm0, %mm2
 	psrlq	%mm7, %mm0

 	por	%mm1, %mm0
 	movq	%mm0, disp1(%edi)
')

	subl	$UNROLL_BYTES, %ebx
	subl	$UNROLL_BYTES, %edi
	
	decl	%esi
	jns	L(top)


deflit(`disp0', ifelse(UNROLL_BYTES,256,124))
deflit(`disp1', eval(disp0 - 8))

	testb	$1, %dl		# size-1
	movl	VAR_RETVAL, %eax

	psllq	%mm6, %mm2
	jnz	L(end_even)


L(end_odd):
	# size odd, three more limbs on dst

	# note load is first in case src==dst
	movq	eval(disp0+4)(%ebx), %mm0	# src low qword
	movq	%mm2, disp0(%edi)	# high dword of this

	psllq	%mm6, %mm0
	popl	%esi

	movq	%mm0, disp1+4(%edi)	# gives low two limbs
	popl	%edi

	popl	%ebx

	emms_or_femms

	ret


L(end_even):
	# size even, must store two limbs

	movq	%mm2, disp0(%edi)
	popl	%esi

	popl	%edi
	popl	%ebx

	emms_or_femms

	ret


EPILOGUE()
