# AMD K7 mpn_mul_1 -- mpn by limb multiply.
#
# K7: 3.4 cycles/limb (at 16 limbs/loop).


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
dnl           8           3.9
dnl          16           3.4
dnl          32           3.4
dnl          64           3.35
dnl  Maximum possible with the current code is 64.

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
# the low limb of the destination.
#
# Variations on the unrolled loop have been tried, with the current
# registers or with the counter on the stack to free up ecx.  The current
# code is the fastest found.
#
# An interesting effect is that removing the stores "movl %ebx, disp0(%edi)"
# from the unrolled loop actually slows it down to 5.0 cycles/limb.  Code
# with this change can be tested on sizes of the form UNROLL_COUNT*n+1
# without having to change the computed jump.  There's obviously something
# fishy going on, perhaps with what execution units the mul needs.

defframe(PARAM_CARRY,     20)
defframe(PARAM_MULTIPLIER,16)
defframe(PARAM_SIZE,      12)
defframe(PARAM_SRC,       8)
defframe(PARAM_DST,       4)

defframe(SAVE_EBP, -4)
defframe(SAVE_EDI, -8)
defframe(SAVE_ESI, -12)
defframe(SAVE_EBX, -16)
deflit(STACK_SPACE, 16)

dnl  Must have UNROLL_THRESHOLD >= 2, since the unrolled loop can't handle 1.
ifdef(`PIC',`
deflit(UNROLL_THRESHOLD, 7)
',`
deflit(UNROLL_THRESHOLD, 5)
')

	.text
	ALIGN(32)
PROLOGUE(mpn_mul_1c)
deflit(`FRAME',0)
	movl	PARAM_CARRY, %edx
	jmp	LF(mpn_mul_1,start_nc)
EPILOGUE()


PROLOGUE(mpn_mul_1)
deflit(`FRAME',0)
	xorl	%edx, %edx	# initial carry
L(start_nc):
	movl	PARAM_SIZE, %ecx
	subl	$STACK_SPACE, %esp
deflit(`FRAME', STACK_SPACE)

	movl	%edi, SAVE_EDI
	movl	%ebx, SAVE_EBX
	movl	%edx, %ebx

	movl	%esi, SAVE_ESI
	movl	PARAM_SRC, %esi
	cmpl	$UNROLL_THRESHOLD, %ecx

	movl	PARAM_DST, %edi
	movl	%ebp, SAVE_EBP
	jae	L(unroll)

	leal	(%esi,%ecx,4), %esi
	leal	(%edi,%ecx,4), %edi
	negl	%ecx

	movl	PARAM_MULTIPLIER, %ebp

L(simple):
	# eax	scratch
	# ebx	carry
	# ecx	counter (negative)
	# edx	scratch
	# esi	src
	# edi	dst
	# ebp	multiplier

	movl	(%esi,%ecx,4), %eax

	mull	%ebp

	addl	%ebx, %eax
	movl	%eax, (%edi,%ecx,4)
	movl	$0, %ebx

	adcl	%edx, %ebx
	incl	%ecx
	jnz	L(simple)

	movl	%ebx, %eax
	movl	SAVE_EBX, %ebx
	movl	SAVE_ESI, %esi

	movl	SAVE_EDI, %edi
	movl	SAVE_EBP, %ebp
	addl	$STACK_SPACE, %esp

	ret


# -----------------------------------------------------------------------------
# The mov to load the next source limb is done well ahead of the mul, this
# is necessary for full speed.  It leads to one limb handled separately
# after the loop.
#
# When unrolling to 32 or more, an offset of +4 is used on the src pointer,
# to avoid having an 0x80 displacement in the code for the last limb in the
# unrolled loop.  This is for a fair comparison between 16 and 32 unrolling.

ifelse(eval(UNROLL_COUNT >= 32),1,`
deflit(SRC_OFFSET,4)
',`
deflit(SRC_OFFSET,)
')

	# this is offset 0x62, so close enough to aligned
L(unroll):
	# eax
	# ebx	initial carry
	# ecx	size
	# edx
	# esi	src
	# edi	dst
	# ebp
deflit(`FRAME', STACK_SPACE)

	leal	-1(%ecx), %edx	# one limb handled at end
	leal	-2(%ecx), %ecx	# and ecx is one less than edx
	movl	%ebp, SAVE_EBP

	negl	%edx
	shrl	$UNROLL_LOG2, %ecx	# unrolled loop counter
	movl	(%esi), %eax		# src low limb

	andl	$UNROLL_MASK, %edx
	movl	PARAM_DST, %edi

	movl	%edx, %ebp
	shll	$4, %edx

	# 17 code bytes per limb
ifdef(`PIC',`
	call	L(add_eip_to_edx)
L(here):
',`
	leal	L(entry) (%edx,%ebp), %edx
')
	negl	%ebp

	leal	ifelse(UNROLL_BYTES,256,128+) SRC_OFFSET(%esi,%ebp,4), %esi
	leal	ifelse(UNROLL_BYTES,256,128) (%edi,%ebp,4), %edi
	movl	PARAM_MULTIPLIER, %ebp

	jmp	*%edx


ifdef(`PIC',`
L(add_eip_to_edx):
	# See README.family about old gas bugs
	leal	(%edx,%ebp), %edx
	addl	$L(entry)-L(here), %edx
	addl	(%esp), %edx
	ret
')


# ----------------------------------------------------------------------------
	ALIGN(32)
L(top):
	# eax	next src limb
	# ebx	carry
	# ecx	counter
	# edx	scratch
	# esi	src+4
	# edi	dst
	# ebp	multiplier
	#
	# 17 code bytes per limb processed

L(entry):
forloop(i, 0, UNROLL_COUNT-1, `
	deflit(`disp_dst', eval(i*4 ifelse(UNROLL_BYTES,256,-128)))
	deflit(`disp_src', eval(disp_dst + 4-(SRC_OFFSET-0)))
 
	mull	%ebp
	
	addl	%eax, %ebx
Zdisp(	movl,	disp_src,(%esi), %eax)
Zdisp(	movl,	%ebx, disp_dst,(%edi))

	movl	$0, %ebx
	adcl	%edx, %ebx
')

	decl	%ecx

	leal	UNROLL_BYTES(%esi), %esi
	leal	UNROLL_BYTES(%edi), %edi
	jns	L(top)


deflit(`disp0', ifelse(UNROLL_BYTES,256,-128))

	mull	%ebp
	
	addl	%eax, %ebx
	movl	$0, %eax
	movl	SAVE_ESI, %esi

	movl	%ebx, disp0(%edi)
	movl	SAVE_EBX, %ebx
	movl	SAVE_EDI, %edi

	adcl	%edx, %eax
	movl	SAVE_EBP, %ebp
	addl	$STACK_SPACE, %esp

	ret
	
EPILOGUE()
