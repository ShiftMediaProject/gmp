dnl  Intel Pentium-4 mpn_addmul_1 -- Multiply a limb vector with a limb and add
dnl  the result to a second limb vector.

dnl  Copyright 2001, 2002 Free Software Foundation, Inc.
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


C P4: 6 cycles/limb, unstable timing, at least on early Pentium4 silicon
C     (stepping 10).


C mp_limb_t mpn_addmul_1 (mp_ptr dst, mp_srcptr src, mp_size_t size,
C                         mp_limb_t multiplier);
C mp_limb_t mpn_addmul_1c (mp_ptr dst, mp_srcptr src, mp_size_t size,
C                          mp_limb_t multiplier, mp_limb_t carry);
C
C Only the carry limb propagation is on the dependent chain, but some other
C Pentium4 pipeline magic brings down performance to 6 cycles/l from the
C ideal 4 cycles/l.

defframe(PARAM_CARRY,     20)
defframe(PARAM_MULTIPLIER,16)
defframe(PARAM_SIZE,      12)
defframe(PARAM_SRC,       8)
defframe(PARAM_DST,       4)

	TEXT
	ALIGN(16)
PROLOGUE(mpn_addmul_1c)
deflit(`FRAME',0)
	movd	PARAM_CARRY, %mm0
	jmp	L(start_1c)
EPILOGUE()

PROLOGUE(mpn_addmul_1)
deflit(`FRAME',0)
	pxor	%mm0, %mm0
L(start_1c):
	movl	PARAM_SRC, %eax
	movl	PARAM_SIZE, %ecx
	movl	PARAM_DST, %edx
	movd	PARAM_MULTIPLIER, %mm7

	C eax	src, incrementing
	C ebx
	C ecx	loop counter, decrementing
	C edx	dst, incrementing
	C
	C mm0	carry, low 32-bits
	C mm7	multiplier

L(loop):
	movd	(%eax), %mm1		C src
	leal	4(%eax), %eax
	movd	(%edx),%mm2		C dst
	pmuludq	%mm7, %mm1
	paddq	%mm1, %mm2		C prod
	paddq	%mm2, %mm0		C carry
	subl	$1, %ecx
	movd	%mm0, (%edx)		C result
	psrlq	$32, %mm0
	leal	4(%edx), %edx
	jnz	L(loop)

	movd	%mm0, %eax
	emms
	ret

EPILOGUE()
