dnl  x86 mpn_lshift -- left shift.
dnl 
dnl  Pentium4: 4.0 cycles/limb

dnl  Copyright 2001 Free Software Foundation, Inc.
dnl 
dnl  This file is part of the GNU MP Library.
dnl 
dnl  The GNU MP Library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Lesser General Public License as
dnl  published by the Free Software Foundation; either version 2.1 of the
dnl  License, or (at your option) any later version.
dnl 
dnl  The GNU MP Library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Lesser General Public License for more details.
dnl 
dnl  You should have received a copy of the GNU Lesser General Public
dnl  License along with the GNU MP Library; see the file COPYING.LIB.  If
dnl  not, write to the Free Software Foundation, Inc., 59 Temple Place -
dnl  Suite 330, Boston, MA 02111-1307, USA.


include(`../config.m4')


C mp_limb_t mpn_lshift (mp_ptr dst, mp_srcptr src, mp_size_t size,
C                       unsigned shift);

defframe(PARAM_SHIFT,16)
defframe(PARAM_SIZE, 12)
defframe(PARAM_SRC,  8)
defframe(PARAM_DST,  4)


	TEXT
	ALIGN(16)
PROLOGUE(mpn_lshift)
deflit(`FRAME',0)
	movl	$32, %eax
	movd	%eax, %mm6
	movl	PARAM_DST, %edx
	movl	PARAM_SRC, %eax
	movl	PARAM_SIZE, %ecx

	leal	-8(%eax,%ecx,4), %eax	C src end
	leal	-4(%edx,%ecx,4), %edx	C dst end

	movd	PARAM_SHIFT, %mm7
	psubd	%mm7, %mm6

	movd	4(%eax), %mm0
	movq	%mm0, %mm3
	psrld	%mm6, %mm3		C retval

	movq	%mm0, %mm1
	pslld	%mm7, %mm1

	subl	$1, %ecx
	jz	L(end)

L(loop):
	movd	(%eax), %mm0
	movd	4(%eax), %mm1
	psrld	%mm6, %mm0
	pslld	%mm7, %mm1
	por	%mm0, %mm1
	movd	%mm1, (%edx)

	leal	-4(%eax), %eax
	leal	-4(%edx), %edx
	subl	$1, %ecx
	jnz	L(loop)
L(end):
	movd	4(%eax), %mm1
	pslld	%mm7, %mm1
	movd	%mm1, (%edx)
	movd	%mm3, %eax
	emms
	ret

EPILOGUE()
