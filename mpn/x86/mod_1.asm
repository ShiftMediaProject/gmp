# x86 mpn_mod_1 -- mpn by limb remainder.
#
#       cycles/limb
# K7        42
# K6        20
# P6        40
# P5        44
# 486   approx 42 maybe


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


# mp_limb_t mpn_mod_1 (mp_ptr dst, mp_srcptr src, mp_size_t size,
#                      mp_limb_t divisor);
# mp_limb_t mpn_mod_1c (mp_ptr dst, mp_srcptr src, mp_size_t size,
#                       mp_limb_t divisor, mp_limb_t carry);
#
# Divide src,size by divisor and return the remainder.  The quotient is
# discarded.
#
# See mpn/x86/divrem_1.asm for some comments.

defframe(PARAM_CARRY,  16)
defframe(PARAM_DIVISOR,12)
defframe(PARAM_SIZE,   8)
defframe(PARAM_SRC,    4)

	.text
	ALIGN(16)

PROLOGUE(mpn_mod_1c)
deflit(`FRAME',0)

	movl	PARAM_SIZE, %ecx
	pushl	%ebx
FRAME_pushl()

	movl	PARAM_SRC, %ebx
	pushl	%esi
FRAME_pushl()

	movl	PARAM_DIVISOR, %esi
	orl	%ecx, %ecx

	movl	PARAM_CARRY, %edx
	jnz	LF(mpn_mod_1,top)

	popl	%esi
	movl	%edx, %eax

	popl	%ebx
	
	ret

EPILOGUE()


PROLOGUE(mpn_mod_1)
deflit(`FRAME',0)

	movl	PARAM_SIZE, %ecx
	pushl	%ebx
FRAME_pushl()

	movl	PARAM_SRC, %ebx
	pushl	%esi
FRAME_pushl()

	movl	PARAM_DIVISOR, %esi
	orl	%ecx, %ecx

	movl	$0, %edx
	jz	L(done)

	movl	-4(%ebx,%ecx,4), %eax	# src high limb
	cmpl	%esi, %eax
	jae	L(entry)

	# high<divisor, so avoid one div

	decl	%ecx
	movl	%eax, %edx

	jz	L(done)

L(top):
	# eax	scratch (quotient)
	# ebx	src
	# ecx	counter
	# edx	carry (remainder)
	# esi	divisor
	# edi
	# ebp

	movl	-4(%ebx,%ecx,4), %eax
L(entry):
	divl	%esi

	loop_or_decljnz	L(top)

L(done):
	popl	%esi

	movl	%edx, %eax
	popl	%ebx

	ret

EPILOGUE()
