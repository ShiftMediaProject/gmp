dnl  Intel Pentium-4 mpn_submul_1 -- subtract mpn multiple.
dnl 
dnl  P4: 10 cycles/limb

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


C mp_limb_t mpn_submul_1 (mp_ptr dst, mp_srcptr src, mp_size_t size,
C                         mp_limb_t mult);
C mp_limb_t mpn_submul_1c (mp_ptr dst, mp_srcptr src, mp_size_t size,
C                          mp_limb_t mult, mp_limb_t carry);
C
C The basic calculation here is d-s*m+c, where d is a destination limb, s is
C a source limb and c is a carry, being a 64-bit twos complement borrow.
C The low limb is stored back to the destination and the high limb becomes
C the new c'.  If this new c' is non-zero then it represents a borrow and
C should be extended to 64-bits with an 0xFFFFFFFF, or if it's zero then
C there's no borrow and it should be extended with 0.  This is achieved by
C calculating instead (b-1)*b+d-s*m+c, where b=2^32, by putting b-1 in the
C high of d.  This leaves (b-1)+c' mod b, which means b-1 if no borrow, or
C any other value for a borrow.  Subtracting b-1 produces the desired high
C limb of c', ie. 0 if no borrow, all 1s if there is a borrow.
C
C This code is not particularly good at 10 c/l.  The dependent chain is only
C 6 c/l and it's not clear what that isn't achieved.  In theory we should be
C able to go at 4 or 5 c/l, for instance by handling a carry bit and carry
C limb separately, though straightforward attempts have been no better than
C 10 c/l.

defframe(PARAM_CARRY,     20)
defframe(PARAM_MULTIPLIER,16)
defframe(PARAM_SIZE,      12)
defframe(PARAM_SRC,       8)
defframe(PARAM_DST,       4)

	TEXT
	ALIGN(16)

PROLOGUE(mpn_submul_1c)
deflit(`FRAME',0)
	movd	PARAM_CARRY, %mm1
	pxor	%mm0, %mm0
	psubq	%mm1, %mm0	C twos complement negative
	jmp	LF(mpn_submul_1,start_1c)
EPILOGUE()

PROLOGUE(mpn_submul_1)
deflit(`FRAME',0)
	pxor	%mm0, %mm0
L(start_1c):
	movl	PARAM_SRC, %eax
	pcmpeqd	%mm5, %mm5

	movd	PARAM_MULTIPLIER, %mm7
	pcmpeqd	%mm6, %mm6

	movl	PARAM_DST, %edx
	psrlq	$32, %mm5		C 0x00000000FFFFFFFF

	movl	PARAM_SIZE, %ecx
	psllq	$32, %mm6		C 0xFFFFFFFF00000000

L(top):
	C eax	src, incrementing
	C ebx
	C ecx	size, decrementing
	C edx	dst, incrementing
	C
	C mm0	carry, low 32-bits, twos comp neg of borrow
	C mm5	0x00000000FFFFFFFF
	C mm6	0xFFFFFFFF00000000
	C mm7	multiplier

	movd	(%eax), %mm1		C src
	addl	$4, %eax

	movd	(%edx), %mm2		C dst
	por	%mm6, %mm2		C add 0xFFFFFFFF to high

	pmuludq	%mm7, %mm1

	psubq	%mm1, %mm2		C prod

	paddq	%mm2, %mm0		C borrow

	movd	%mm0, (%edx)		C result
	addl	$4, %edx

	psrlq	$32, %mm0

	psubq	%mm5, %mm0		C sub 0xFFFFFFFF from low

	subl	$1, %ecx
	jnz	L(top)


	pxor	%mm1, %mm1
	psubq	%mm0, %mm1		C twos comp neg, to give borrow
	movd	%mm1, %eax
	emms
	ret

EPILOGUE()
