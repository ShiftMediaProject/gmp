# AMD K6 mpn_rshift -- mpn right shift.
#
# K6: 1.8 cycles/limb (at 32 limbs/loop), PIC adds 5 cycles at the start.
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


dnl  K6: 8 gives 2.0 cycles/limb, 16 gives 1.9, 32 gives 1.8, 64 gives 1.75.
dnl  Maximum possible with the current code is 64.

deflit(UNROLL_COUNT, 32)


# mp_limb_t mpn_rshift (mp_ptr dst, mp_srcptr src, mp_size_t size,
#                       unsigned shift);
#
# Shift src,size right by shift many bits and store the result in dst,size.
# Zeros are shifted in at the left.  Return the bits shifted out at the
# right.
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
# necessary.  Aligning the source to an 8 byte boundary makes no difference,
# ie. 0mod8 or 4mod8 are the same speed.
#
# Two limbs are left to the end to process since zeros are shifted in there,
# rather than src data.  Or if the size is odd three limbs are left since
# the unrolled loop only handles an even number of limbs.


dnl  The unrolled loop takes slightly longer when the destination isn't
dnl  aligned, so check both aligned and unaligned destinations when
dnl  determining these thresholds.  Must be at least 4, since the unrolled
dnl  loop can't handle smaller.

ifdef(`PIC',`
deflit(UNROLL_THRESHOLD, 9)
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

PROLOGUE(mpn_rshift)
deflit(`FRAME',0)

	# This can be done without the push %ebx, but it's then still the
	# same speed.  The push is left as a free helping hand for the
	# more_than_one_limb code.

	movl	PARAM_SIZE, %eax
	pushl	%ebx
deflit(`FRAME',4)

	movl	PARAM_SRC, %ebx
	decl	%eax

	movl	PARAM_SHIFT, %ecx
	jnz	L(more_than_one_limb)

	movl	(%ebx), %edx		# src limb
	movl	PARAM_DST, %ebx

	shrdl	%cl, %edx, %eax		# return value

 	shrl	%cl, %edx

	movl	%edx, (%ebx)		# dst limb
	popl	%ebx

	ret


# -----------------------------------------------------
	ALIGN(16)	# otherwise is offset 0x1f
L(more_than_one_limb):
	# eax	size-1
	# ebx	src
	# ecx	shift
	# edx
	# esi
	# edi
	# ebp

	cmp	$eval(UNROLL_THRESHOLD-1), %eax
	pushl	%edi
deflit(`FRAME',8)

	movl	(%ebx), %edx	# src low limb
	pushl	%esi
deflit(`FRAME',12)

	movl	$0, %esi
	jae	L(unroll)

	shrdl	%cl, %edx, %esi

	movl	PARAM_DST, %edi
	leal	4(%ebx,%eax,4), %ebx

	movl	%esi, VAR_RETVAL
	leal	(%edi,%eax,4), %edi

	negl	%eax

	# this is offset 0x42, which is close enough to aligned
L(simple):
	# eax	loop counter, negative
	# ebx	src
	# ecx	shift
	# edx	carry
	# esi	scratch
	# edi	dst
	# ebp

	movl	%edx, %esi
	movl	(%ebx,%eax,4), %edx

	shrdl	%cl, %edx, %esi

	movl	%esi, (%edi,%eax,4)
	incl	%eax

	jnz	L(simple)


	shrl	%cl, %edx	# dst high limb, zeros shifted in
	movl	VAR_RETVAL, %eax

	movl	%edx, (%edi)
	popl	%esi

	popl	%edi
	popl	%ebx

	ret


# -----------------------------------------------------
	# aligning here saves a couple of cycles
	ALIGN(16)
L(unroll):
	# eax	size-1
	# ebx	src
	# ecx	shift
	# edx	src low limb
	# esi	zero
	# edi	dst
	# ebp

deflit(`FRAME',12)

	movl	PARAM_DST, %edi
	movd	%ecx, %mm6		# rshift

	shrdl	%cl, %edx, %esi

	testl	$4, %edi

	movl	%esi, VAR_RETVAL
	jz	L(start_dst_aligned)


	# dst isn't aligned, process one limb to make it aligned
	movq	(%ebx), %mm0

	movl	%eax, PARAM_SIZE	# new size is size-1
	addl	$4, %ebx

	decl	%eax			# new size-1 is size-2
	psrlq	%mm6, %mm0

	movd	%mm0, (%edi)
	addl	$4, %edi
L(start_dst_aligned):


	decl	%eax		# two last limbs done at end (eax was size-1)
	movq	(%ebx), %mm1

	andl	$~1, %eax	# round size down to even
	movl	%eax, %esi

	negl	%eax
	movq	%mm1, %mm2	# initial carry in both mm1 and mm2

	andl	$UNROLL_MASK, %eax
	negl	%ecx

	addl	%eax, %eax
	addl	$64, %ecx

	decl	%esi		# loop counter
ifdef(`PIC',`
	call	L(pic_calc)
L(here):
',`
	leal	L(entry) (%eax,%eax,4), %edx
')

	movd	%ecx, %mm7	# lshift = 64-rshift
	negl	%eax

	leal	ifelse(UNROLL_BYTES,256,128+) 8(%ebx,%eax,2), %ebx
	shrl	$UNROLL_LOG2, %esi

	leal	ifelse(UNROLL_BYTES,256,128) (%edi,%eax,2), %edi
	movl	PARAM_SIZE, %eax

	jmp	*%edx


# ---------------------------------------------------
	ALIGN(32)
L(top):
	# eax	size (for use at end)
	# ebx	src
	# ecx	shift
	# edx	was computed jump	
	# esi	loop counter
	# edi	dst
	# ebp
	#
	# mm0   scratch
	# mm1 \	previous src qword (alternately in mm1 or mm2)
	# mm2 /
	# mm6	rshift
	# mm7	lshift (64-rshift)
	#
	# 10 code bytes per limb processed
	#
	# The two chunks differ in whether mm1 or mm2 hold the carry.
	# The computed jump puts the initial carry in both mm1 and mm2.

L(entry):
deflit(CHUNK_COUNT, 4)
forloop(i, 0, UNROLL_COUNT/CHUNK_COUNT-1, `
deflit(`disp0', eval(i*CHUNK_COUNT*4 ifelse(UNROLL_BYTES,256,-128)))
deflit(`disp1', eval(disp0 + 8))

 	movq	disp0(%ebx), %mm0
 	psrlq	%mm6, %mm2

 	movq	%mm0, %mm1
 	psllq	%mm7, %mm0

 	por	%mm2, %mm0
 	movq	%mm0, disp0(%edi)


 	movq	disp1(%ebx), %mm0
 	psrlq	%mm6, %mm1

 	movq	%mm0, %mm2
 	psllq	%mm7, %mm0

 	por	%mm1, %mm0
 	movq	%mm0, disp1(%edi)
')

	addl	$UNROLL_BYTES, %ebx
	addl	$UNROLL_BYTES, %edi
	
	decl	%esi
	jns	L(top)


deflit(`disp0', ifelse(UNROLL_BYTES,256,-128))
deflit(`disp1', eval(disp0-0 + 8))

	testb	$1, %al			# size
	psrlq	%mm6, %mm2

	jz	L(end_even)


L(end_odd):
	# size is odd, three more limbs on dst
	
	movd	disp0(%ebx), %mm0
	movl	VAR_RETVAL, %eax

	movq	%mm0, %mm1
	psllq	%mm7, %mm0

	psrlq	%mm6, %mm1
 	por	%mm2, %mm0

	movd	%mm1, disp1(%edi)
	popl	%esi

	movq	%mm0, disp0(%edi)
	popl	%edi

	popl	%ebx

	emms_or_femms

	ret


L(end_even):
	# size is even, must store two limbs
	movq	%mm2, disp0(%edi)

	popl	%esi

#movl	$0, disp0(%edi)
	popl	%edi

	popl	%ebx
deflit(`FRAME',0)
	movl	VAR_RETVAL, %eax

	emms_or_femms

	ret


ifdef(`PIC',`
L(pic_calc):
	leal	L(entry)-L(here) (%eax,%eax,4), %edx
	addl	(%esp), %edx
	ret
')

EPILOGUE()
