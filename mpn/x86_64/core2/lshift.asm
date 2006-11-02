dnl  x86 mpn_lshift -- mpn left shift.

dnl  Copyright 1992, 1994, 1996, 1999, 2000, 2001, 2002, 2006 Free Software
dnl  Foundation, Inc.
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
dnl  not, write to the Free Software Foundation, Inc., 51 Franklin Street,
dnl  Fifth Floor, Boston, MA 02110-1301, USA.

include(`../config.m4')


C	    cycles/limb
C K8:		4.5
C P4:	       16
C P6-15:	1.75

C This code was created from the generic x86 code, with minor improvements.
C With more unrolling, it should be possible to approach 1 cycle/limb on P6-15.


C INPUT PARAMETERS
C rp    rdi
C up    rsi
C n     rdx
C cnt   rcx

ASM_START()
	TEXT
	ALIGN(8)
PROLOGUE(mpn_lshift)

	sub	$8, %rsi			C adjust src

	mov	(%rsi,%rdx,8), %r8	C read most significant limb
	xorl	%eax, %eax
	shld	%cl, %r8, %rax		C compute carry limb
	dec	%rdx
	jz	L(end)
	testb	$1, %dl
	jz	L(1)			C enter loop in the middle
	inc	%rdx
	jmp	L(mid)

L(1):	mov	%r8, %r9

	ALIGN(8)
L(top):	mov	(%rsi,%rdx,8), %r8	C load next lower limb
	shld	%cl, %r8, %r9		C compute result limb
	mov	%r9, (%rdi,%rdx,8)	C store it
L(mid):	mov	-8(%rsi,%rdx,8), %r9
	shld	%cl, %r9, %r8
	mov	%r8, -8(%rdi,%rdx,8)
	sub	$2, %rdx
	jnz	L(top)

	shl	%cl, %r9		C compute least significant limb
	mov	%r9, (%rdi)		C store it
	ret

L(end):	shl	%cl, %r8		C compute least significant limb
	mov	%r8, (%rdi)		C store it
	ret

EPILOGUE()
