# x86 mpn_copyi -- copy limb vector, incrementing.


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


# void mpn_copyi (mp_ptr dst, mp_srcptr src, mp_size_t size);
#
# Copy src,size to dst,size, working from low to high addresses.
#
# The code here is very generic and can be expected to be reasonable on all
# the x86 family.
#
# P5 - 1.0 cycles/limb.
#
# P6 - 0.75 cycles/limb.  An MMX based copy was tried, but was found to be
#      slower than a rep movs in all cases.  The fastest MMX found was 0.8
#      cycles/limb (when fully aligned).  A rep movs seems to have a startup
#      time of about 15 cycles, but doing something special for small sizes
#      could lead to a branch misprediction that would destroy any saving.
#      For now a plain rep movs seems ok for P6.

defframe(PARAM_SIZE,12)
defframe(PARAM_SRC, 8)
defframe(PARAM_DST, 4)
deflit(`FRAME',0)

	.text
	ALIGN(32)

	# eax	saved esi
	# ebx
	# ecx	counter
	# edx	saved edi
	# esi	src
	# edi	dst
	# ebp

PROLOGUE(mpn_copyi)

	movl	PARAM_SIZE, %ecx
	movl	%esi, %eax

	movl	PARAM_SRC, %esi
	movl	%edi, %edx

	movl	PARAM_DST, %edi

	cld	# better safe than sorry, see mpn/x86/README.family

	rep
	movsl

	movl	%eax, %esi
	movl	%edx, %edi

	ret

EPILOGUE()
