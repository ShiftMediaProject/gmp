# AMD K6 mpn_com_n -- mpn bitwise one's complement.
#  
#    alignment dst/src, A=0mod8 N=4mod8
#       A/A   A/N   N/A   N/N
# K6    1.0   1.18  1.18  1.18  cycles/limb


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


# void mpn_com_n (mp_ptr dst, mp_srcptr src, mp_size_t size);
#
# Take the bitwise ones-complement of src,size and write it to dst,size.

defframe(PARAM_SIZE,12)
defframe(PARAM_SRC, 8)
defframe(PARAM_DST, 4)

	.text
	ALIGN(32)
PROLOGUE(mpn_com_n)
deflit(`FRAME',0)

	movl	PARAM_SIZE, %ecx
	movl	PARAM_SRC, %eax
	movl	PARAM_DST, %edx
	shrl	%ecx
	jnz	L(two_or_more)

	movl	(%eax), %eax
	notl	%eax
	movl	%eax, (%edx)
	ret


L(two_or_more):
	pushl	%ebx
FRAME_pushl()
	movl	%ecx, %ebx

	pcmpeqd	%mm7, %mm7	# all ones


	ALIGN(16)
L(top):
	# eax	src
	# ebx	floor(size/2)
	# ecx	counter
	# edx	dst
	# esi
	# edi
	# ebp

	movq	(%eax,%ecx,8), %mm0
	pxor	%mm7, %mm0
	movq	%mm0, (%edx,%ecx,8)
	loop	L(top)


	jnc	L(no_extra)
	movl	(%eax,%ebx,8), %eax
	notl	%eax
	movl	%eax, (%edx,%ebx,8)
L(no_extra):

	popl	%ebx
	emms_or_femms
	ret

EPILOGUE()
