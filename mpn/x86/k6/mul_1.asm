# AMD K6 mpn_mul_1 -- mpn by limb multiply.
#
# K6: 6.25 cycles/limb.


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


# mp_limb_t mpn_mul_1 (mp_ptr dst, mp_srcptr src, mp_size_t size,
#                      mp_limb_t multiplier);
# mp_limb_t mpn_mul_1c (mp_ptr dst, mp_srcptr src, mp_size_t size,
#                       mp_limb_t multiplier, mp_limb_t carry);
#
# Multiply src,size by mult and store the result in dst,size.
# Return the carry limb from the top of the result.
#
# mpn_mul_1c() accepts an initial carry for the calculation, it's added into
# the low limb of the result.

defframe(PARAM_CARRY,     20)
defframe(PARAM_MULTIPLIER,16)
defframe(PARAM_SIZE,      12)
defframe(PARAM_SRC,       8)
defframe(PARAM_DST,       4)

dnl  minimum 5 because the unrolled code can't handle less
deflit(UNROLL_THRESHOLD, 5)

	.text
	ALIGN(32)

PROLOGUE(mpn_mul_1c)
	pushl	%esi
deflit(`FRAME',4)
	movl	PARAM_CARRY, %esi
	jmp	LF(mpn_mul_1,start_nc)
EPILOGUE()


PROLOGUE(mpn_mul_1)
	push	%esi
deflit(`FRAME',4)
	xorl	%esi, %esi	# initial carry

L(start_nc):
	mov	PARAM_SIZE, %ecx
	push	%ebx
FRAME_pushl()

	movl	PARAM_SRC, %ebx
	push	%edi
FRAME_pushl()

	movl	PARAM_DST, %edi
	pushl	%ebp
FRAME_pushl()

	cmpl	$UNROLL_THRESHOLD, %ecx
	movl	PARAM_MULTIPLIER, %ebp

	jae	L(unroll)


	# code offset 0x22 here, close enough to aligned
L(simple):
	# eax	scratch
	# ebx	src
	# ecx	counter
	# edx	scratch
	# esi	carry
	# edi	dst
	# ebp	multiplier
	#
	# this loop 8 cycles/limb

	movl	(%ebx), %eax
	addl	$4, %ebx

	mull	%ebp

	addl	%esi, %eax
	movl	$0, %esi

	adcl	%edx, %esi

	movl	%eax, (%edi)
	addl	$4, %edi

	loop	L(simple)


	popl	%ebp

	popl	%edi
	popl	%ebx

	movl	%esi, %eax
	popl	%esi

	ret


#------------------------------------------------------------------------------
# The code for each limb is 6 cycles, with instruction decoding being the
# limiting factor.  At 4 limbs/loop and 1 cycle/loop of overhead it's 6.25
# cycles/limb in total.
#
# The secret ingredient to get 6.25 is to start the loop with the mul and
# have the load/store pair at the end.  Rotating the load/store to the top
# is an 0.5 c/l slowdown.  (Some address generation effect probably.)
#
# The whole unrolled loop fits nicely in exactly 80 bytes.


	ALIGN(16)	# already aligned to 16 here actually
L(unroll):
	movl	(%ebx), %eax
	leal	-16(%ebx,%ecx,4), %ebx

	leal	-16(%edi,%ecx,4), %edi
	subl	$4, %ecx

	negl	%ecx


	ALIGN(16)	# one byte nop for this alignment
L(top):
	# eax	scratch
	# ebx	&src[size-4]
	# ecx	counter
	# edx	scratch
	# esi	carry
	# edi	&dst[size-4]
	# ebp	multiplier

	mull	%ebp

	addl	%esi, %eax
	movl	$0, %esi

	adcl	%edx, %esi

	movl	%eax, (%edi,%ecx,4)
	movl	4(%ebx,%ecx,4), %eax


	mull	%ebp

	addl	%esi, %eax
	movl	$0, %esi

	adcl	%edx, %esi

	movl	%eax, 4(%edi,%ecx,4)
	movl	8(%ebx,%ecx,4), %eax


	mull	%ebp

	addl	%esi, %eax
	movl	$0, %esi

	adcl	%edx, %esi

	movl	%eax, 8(%edi,%ecx,4)
	movl	12(%ebx,%ecx,4), %eax


	mull	%ebp

	addl	%esi, %eax
	movl	$0, %esi

	adcl	%edx, %esi

	movl	%eax, 12(%edi,%ecx,4)
	movl	16(%ebx,%ecx,4), %eax


	addl	$4, %ecx
	js	L(top)



	# eax	next src limb
	# ebx	&src[size-4]
	# ecx	0 to 3 representing respectively 4 to 1 further limbs
	# edx
	# esi	carry
	# edi	&dst[size-4]

	testb	$2, %cl
	jnz	L(finish_not_two)

	mull	%ebp

	addl	%esi, %eax
	movl	$0, %esi

	adcl	%edx, %esi

	movl	%eax, (%edi,%ecx,4)
	movl	4(%ebx,%ecx,4), %eax


	mull	%ebp

	addl	%esi, %eax
	movl	$0, %esi

	adcl	%edx, %esi

	movl	%eax, 4(%edi,%ecx,4)
	movl	8(%ebx,%ecx,4), %eax

	addl	$2, %ecx
L(finish_not_two):


	testb	$1, %cl
	jnz	L(finish_not_one)

	mull	%ebp

	addl	%esi, %eax
	movl	$0, %esi

	adcl	%edx, %esi

	movl	%eax, 8(%edi)
	movl	12(%ebx), %eax
L(finish_not_one):


	mull	%ebp

	addl	%esi, %eax
	popl	%ebp

	adcl	$0, %edx

	movl	%eax, 12(%edi)
	popl	%edi

	popl	%ebx
	movl	%edx, %eax

	popl	%esi

	ret

EPILOGUE()
