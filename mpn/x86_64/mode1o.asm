dnl  AMD64 mpn_modexact_1_odd -- exact division style remainder.

dnl  Copyright 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.
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


C Hammer: 10.0 cycles/limb


C mp_limb_t mpn_modexact_1_odd (mp_srcptr src, mp_size_t size,
C                               mp_limb_t divisor);
C mp_limb_t mpn_modexact_1c_odd (mp_srcptr src, mp_size_t size,
C                                mp_limb_t divisor, mp_limb_t carry);
C
C
C The dependent chain in the main loop is
C
C                            cycles
C	subq	%rdx, %rax	1
C	imulq	%r9, %rax	4
C	mulq	%r8		5
C			      ----
C       total		       10
C
C The movq load from src seems to need to be scheduled back before the jz to
C achieve this speed, out-of-order execution apparently can't completely
C hide the latency otherwise.
C
C The l=src[i]-cbit step is rotated back too, since that allows us to avoid
C it for the first iteration (where there's no cbit).
C
C The code alignment used (32-byte) for the loop also seems necessary.
C Without that the non-PIC case has adcq crossing the 0x60 offset,
C apparently making it run at 11 cycles instead of 10.
C
C Not done:
C
C divq for size==1 was measured at about 79 cycles, compared to the inverse
C at about 25 cycles (both including function call overheads), so that's not
C used.
C
C Enhancements:
C
C For PIC, the GOT fetch for modlimb_invert_table measures about 2 or 3
C cycles slower then the non-PIC direct reference.  That table will be in
C rodata or text, so could perhaps be accessed directly %rip relative.
C Would need to check what the ABI says about this though, and how the
C linker treats it.


	TEXT

	ALIGN(32)
PROLOGUE(mpn_modexact_1_odd)

	xorl	%ecx, %ecx

PROLOGUE(mpn_modexact_1c_odd)

	C rdi	src
	C rsi	size
	C rdx	divisor
	C rcx	carry

	movq	%rdx, %r8		C d
	shrq	%rdx			C d/2
ifdef(`PIC',`
	movq	modlimb_invert_table@GOTPCREL(%rip), %r10
')

	andq	$127, %rdx

ifdef(`PIC',`
	movzbq	(%r10,%rdx), %r10			C inv 8 bits
',`
	movzbq	modlimb_invert_table(%rdx), %r10	C inv 8 bits
')
	C

	movq	(%rdi), %rax		C src[0]
	leaq	(%rdi,%rsi,8), %rdi	C src end
	negq	%rsi			C -size

	leaq	(%r10,%r10), %r9	C 2*inv
	imulq	%r10, %r10		C inv*inv

	movq	%rcx, %rdx		C initial climb
	xorq	%rcx, %rcx		C initial cbit

	imulq	%r8, %r10		C inv*inv*d

	subq	%r10, %r9		C inv = 2*inv - inv*inv*d, 16 bits

	leaq	(%r9,%r9), %r10		C 2*inv
	imulq	%r9, %r9		C inv*inv

	imulq	%r8, %r9		C inv*inv*d

	subq	%r9, %r10		C inv = 2*inv - inv*inv*d, 32 bits

	leaq	(%r10,%r10), %r9	C 2*inv
	imulq	%r10, %r10		C inv*inv

	imulq	%r8, %r10		C inv*inv*d

	subq	%r10, %r9		C inv = 2*inv - inv*inv*d, 64 bits

	ASSERT(e,`	C d*inv == 1 mod 2^64
	movq	%r8, %r10
	imulq	%r9, %r10
	cmpq	$1, %r10')

	incq	%rsi
	jz	L(one)


	ALIGN(32)
L(top):
	C rax	l = src[i]-cbit
	C rcx	new cbit, 0 or 1
	C rdx	climb, high of last product
	C rsi	counter, limbs, negative
	C rdi	src end ptr
	C r8	divisor
	C r9	inverse

	subq	%rdx, %rax		C l = src[i]-cbit - climb

	adcq	$0, %rcx		C more cbit
	imulq	%r9, %rax		C q = l * inverse

	mulq	%r8			C climb = high (q * d)

	movq	(%rdi,%rsi,8), %rax	C src[i+1]
	subq	%rcx, %rax		C next l = src[i+1] - cbit
	movq	$0, %rcx
	setc	%cl			C new cbit

	incq	%rsi
	jnz	L(top)


L(one):
	subq	%rdx, %rax		C l = src[i]-cbit - climb

	adcq	$0, %rcx		C more cbit
	imulq	%r9, %rax		C q = l * inverse

	mulq	%r8			C climb = high (q * d)

	leaq	(%rcx,%rdx), %rax	C climb+cbit
	ret

EPILOGUE(mpn_modexact_1c_odd)
EPILOGUE(mpn_modexact_1_odd)
