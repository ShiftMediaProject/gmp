# AMD K6 mpn_mul_1 -- mpn by limb multiply.
#
# K6: 6.4 cycles/limb in the loop (at 16 limbs/loop), PIC adds 4 cycles at
# the start of the unrolled loop.


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


dnl  Unrolling to 8 pushes the code size for the loop just just over a 32
dnl  byte boundary, so perhaps extra code fetching explains the poorer speed
dnl  on this unrolling.  Unrolling to 16 gets the loop code nicely in a 256
dnl  byte block, which might explain why it's as good as 32 or 64.
dnl
dnl  K6 UNROLL_COUNT cycles/limb
dnl           4          6.8
dnl           8          7.5
dnl          16          6.4
dnl          32          6.4
dnl          64          6.4
dnl
dnl  Cf. simple loop is 8.0.
dnl  Maximum unrolling possible with the current code is 64.

deflit(UNROLL_COUNT, 16)


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
#
# In the unrolled loop it's 6 cycles/limb, with instruction decoding being
# the limiting factor.
#
# The jacdl0() technique used in mpn_add/submul_1 doesn't help in the
# unrolled loop here, using it comes out a touch slower than adcl.

ifdef(`PIC',`
deflit(UNROLL_THRESHOLD, 11)
',`
deflit(UNROLL_THRESHOLD, 9)
')

defframe(PARAM_CARRY,     20)
defframe(PARAM_MULTIPLIER,16)
defframe(PARAM_SIZE,      12)
defframe(PARAM_SRC,       8)
defframe(PARAM_DST,       4)

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
deflit(`FRAME',8)

	movl	PARAM_SRC, %ebx
	push	%edi
deflit(`FRAME',12)

	movl	PARAM_DST, %edi
	pushl	%ebp
deflit(`FRAME',16)

	movl	PARAM_MULTIPLIER, %ebp
	cmp	$UNROLL_THRESHOLD, %ecx

	jae	L(unroll)


	# this is offset 0x22, which is close enough to aligned
L(simple):
	# eax	scratch
	# ebx	src
	# ecx	counter
	# edx	scratch
	# esi	carry
	# edi	dst
	# ebp	multiplier

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


#-----------------------------------------------------------------------------
	ALIGN(16)
L(unroll):
	movl	%ecx, %eax
	negl	%ecx

	andl	$UNROLL_MASK, %ecx
	movl	$0, %edx

	subl	%ecx, %edx
	shll	$4, %ecx

	leal	ifelse(UNROLL_BYTES,256,128) (%ebx,%edx,4), %ebx
	decl	%eax

	shrl	$UNROLL_LOG2, %eax
	leal	ifelse(UNROLL_BYTES,256,128) (%edi,%edx,4), %edi

	# 15 code bytes/limb
ifdef(`PIC',`
	call	L(add_eip_to_edx)
L(here):
	leal	L(entry)-L(here) (%edx,%ecx), %edx
',`
	leal	L(entry) (%edx,%ecx), %edx
')
	mov	%eax, %ecx
	
	jmp	*%edx


ifdef(`PIC',`
L(add_eip_to_edx):
	addl	(%esp), %edx
	ret
')


#----------------------------------------------------------------------------
# need 32 byte alignment here to get the claimed speed
	ALIGN(32)
L(top):
	# eax	scratch
	# ebx	src
	# ecx	loop counter
	# edx	scratch
	# esi	carry
	# edi	dst
	# ebp	multiplier
	#
	# 15 code bytes/limb

	leal	UNROLL_BYTES(%edi), %edi

L(entry):
forloop(i, 0, UNROLL_COUNT-1, `
	deflit(`disp', eval(i*4 ifelse(UNROLL_BYTES,256,-128)))

	movl	disp(%ebx), %eax
	mull	%ebp
	addl	%esi, %eax
	movl	%edx, %esi
	adcl	$0, %esi
	movl	%eax, disp(%edi)
')

	decl	%ecx
	leal	UNROLL_BYTES(%ebx), %ebx

	jns	L(top)


	popl	%ebp
	popl	%edi

	popl	%ebx
	movl	%esi, %eax

	popl	%esi
	ret

EPILOGUE()
