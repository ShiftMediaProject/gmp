# mpn_com_n -- mpn bitwise one's complement.
#  
#     alignment dst/src, A=0mod8 N=4mod8
#        A/A   A/N   N/A   N/N
# K6     1.0  1.135  1.18  1.26  cycles/limb


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
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


include(`../config.m4')


# void mpn_com_n (mp_ptr dst, mp_srcptr src, mp_size_t size);
#
# Bitwise ones-complement of src,size, storing result in dst,size.

defframe(PARAM_SIZE,12)
defframe(PARAM_SRC, 8)
defframe(PARAM_DST, 4)

	.text
	ALIGN(32)
PROLOGUE(mpn_com_n)
deflit(`FRAME',0)

	movl	PARAM_SIZE, %ecx
	pushl	%edi
FRAME_pushl()

	movl	PARAM_DST, %edi
	pushl	%ebx
FRAME_pushl()

	movl	PARAM_SRC, %ebx
	cmpl	$1, %ecx

	movl	%ecx, %eax
	jne	L(two_or_more)


	movl	(%ebx), %eax
	popl	%ebx

	notl	%eax

	movl	%eax, (%edi)
	popl	%edi

	ret


L(two_or_more):
	shrl	$2, %ecx
	jnz	L(four_or_more)

	movl	(%ebx), %ecx
	testb	$1, %al
	movl	4(%ebx), %edx
	notl	%ecx
	notl	%edx
	movl	%ecx, (%edi)
	movl	%edx, 4(%edi)

	jz	L(two_only)
	movl	8(%ebx), %eax
	notl	%eax
	movl	%eax, 8(%edi)
L(two_only):

	popl	%ebx
	popl	%edi

	ret


L(four_or_more):
	xorl	%edx, %edx
	pcmpeqd	%mm7, %mm7	# all ones
	jmp	L(entry)

	ALIGN(16)
L(top):
	# eax	size
	# ebx	src end
	# ecx	counter
	# edx	offset
	# esi
	# edi	dst end
	# ebp

	movq	%mm1, -8(%edi,%edx)
L(entry):
	movq	(%ebx,%edx), %mm0

	movq	8(%ebx,%edx), %mm1
	addl	$16, %edx

	pxor	%mm7, %mm0
	movq	%mm0, -16(%edi,%edx)

	pxor	%mm7, %mm1
	loop	L(top)


	testb	$2, %al
	movq	%mm1, -8(%edi,%edx)

	jz	L(no_extra_2)
	movq	(%ebx,%edx), %mm0
	addl	$8, %edx
	pxor	%mm7, %mm0
	movq	%mm0, -8(%edi,%edx)
L(no_extra_2):

	testb	$1, %al
	jz	L(no_extra_1)
	movl	(%ebx,%edx), %eax
	notl	%eax
	movl	%eax, (%edi,%edx)
L(no_extra_1):

	popl	%ebx
	popl	%edi

	emms_or_femms

	ret

EPILOGUE()
