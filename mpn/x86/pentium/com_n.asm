dnl  Intel P5 mpn_com_n -- mpn bitwise ones-complement.
dnl 
dnl  P5: 2.0 cycles/limb


dnl  Copyright (C) 2000 Free Software Foundation, Inc.
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


C void mpn_com_n (mp_ptr dst, mp_srcptr src, mp_size_t size);
C
C Nothing complicated here, just some care to avoid an AGI in the loop.
C Further unrolling could approach 1.5 c/l if desired.

defframe(PARAM_SIZE,12)
defframe(PARAM_SRC, 8)
defframe(PARAM_DST, 4)

	TEXT
	ALIGN(8)
PROLOGUE(mpn_com_n)
deflit(`FRAME',0)

	movl	PARAM_SRC, %eax
	movl	PARAM_SIZE, %ecx

	shrl	%ecx
	jnz	L(two_or_more)

	movl	(%eax), %ecx
	movl	PARAM_DST, %edx		C risk of data cache bank clash here

	notl	%ecx

	movl	%ecx, (%edx)

	ret


L(two_or_more):
	pushl	%ebx	FRAME_pushl()
	pushl	%esi	FRAME_pushl()

	movl	PARAM_DST, %edx
	jnc	L(entry)

	movl	(%eax,%ecx,8), %ebx
	movl	-8(%eax,%ecx,8), %esi

	notl	%ebx

	movl	%ebx, (%edx,%ecx,8)
	movl	-4(%eax,%ecx,8), %ebx	C risk of data cache bank clash here

	decl	%ecx
	jz	L(done)


L(top):
	C eax	src
	C ebx	data
	C ecx	counter, limb pairs, decrementing
	C edx	dst
	C esi	data
	C edi
	C ebp

	xorl	$-1, %ebx
	xorl	$-1, %esi

	movl	%ebx, 4(%edx,%ecx,8)
	movl	%esi, (%edx,%ecx,8)

L(entry):
	movl	-4(%eax,%ecx,8), %ebx
	movl	-8(%eax,%ecx,8), %esi

	decl	%ecx
	jnz	L(top)


L(done):
	xorl	$-1, %ebx
	xorl	$-1, %esi

	movl	%ebx, 4(%edx,%ecx,4)
	movl	%esi, (%edx,%ecx,4)
	
	popl	%esi
	popl	%ebx

	ret

EPILOGUE()
