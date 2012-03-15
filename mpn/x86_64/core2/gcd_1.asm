dnl  AMD64 mpn_gcd_1 optimised for Intel C2, NHM, SBR and AMD K10, BD.

dnl  Based on the K7 gcd_1.asm, by Kevin Ryde.  Rehacked for AMD64 by Torbjorn
dnl  Granlund.

dnl  Copyright 2000, 2001, 2002, 2005, 2009, 2011, 2012 Free Software
dnl  Foundation, Inc.

dnl  This file is part of the GNU MP Library.

dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU Lesser General Public License as published
dnl  by the Free Software Foundation; either version 3 of the License, or (at
dnl  your option) any later version.

dnl  The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
dnl  License for more details.

dnl  You should have received a copy of the GNU Lesser General Public License
dnl  along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.

include(`../config.m4')


C	     cycles/bit (approx)
C AMD K8,K9	 9.79
C AMD K10	 5.34
C AMD bd1	 ?
C AMD bobcat	11.3
C Intel P4	20.8
C Intel core2	 5.09
C Intel NHM	 6.27
C Intel SBR	 5.30
C Intel atom	19.6
C VIA nano	 6.75
C Numbers measured with: speed -c -s64 mpn_gcd_1

C TODO
C  * Optimise inner-loop for specific CPUs.

C INPUT PARAMETERS
define(`up',    `%rdi')
define(`n',     `%rsi')
define(`v0',    `%rdx')

ABI_SUPPORT(DOS64)
ABI_SUPPORT(STD64)

C Undo some configure cleverness.
C The problem is that C only defines the '1c' variant, and that configure
C therefore considers modexact_1c to be the base function.  It then adds a
C special fat rule for mpn_modexact_1_odd, messing up things when a cpudep
C gcd_1 exists without a corresponding cpudep mode1o.
ifdef(`WANT_FAT_BINARY', `
  define(`mpn_modexact_1_odd', `MPN_PREFIX`modexact_1_odd_x86_64'')')


ASM_START()
	TEXT
	ALIGN(16)
PROLOGUE(mpn_gcd_1)
	DOS64_ENTRY(3)
	mov	(%rdi), %r8		C src low limb
	or	%rdx, %r8		C x | y
	bsf	%r8, %r8		C common twos

	bsf	%rdx, %rcx
	shr	R8(%rcx), %rdx

	push	%r8			C preserve common twos over call
	push	%rdx			C preserve v0 argument over call
	sub	$8, %rsp		C maintain ABI required rsp alignment

IFDOS(`	mov	%rdx, %r8	')
IFDOS(`	mov	%rsi, %rdx	')
IFDOS(`	mov	%rdi, %rcx	')
	cmp	$BMOD_1_TO_MOD_1_THRESHOLD, %rsi
	jl	L(bmod)
	CALL(	mpn_mod_1)
	jmp	L(reduced)
L(bmod):
	CALL(	mpn_modexact_1_odd)
L(reduced):

	add	$8, %rsp
	pop	%rdx
	pop	%r8

	bsf	%rax, %rcx
	test	%rax, %rax
	jnz	L(mid)
	jmp	L(end)

C FIXME: 1st sub to rdx would shorten path

	ALIGN(16)		C               K10   BD    C2    NHM   SBR
L(top):	cmovc	%r10, %rax	C if x-y < 0    0,3   0,3   0,6   0,5   0,5
	cmovc	%r9, %rdx	C use x,y-x     0,3   0,3   2,8   1,7   1,7
L(mid):	shr	R8(%rcx), %rax	C               1,7   1,6   2,8   2,8   2,8
	mov	%rdx, %r10	C               1     1     4     3     3
	sub	%rax, %r10	C               2     2     5     4     4
	bsf	%r10, %rcx	C               3     3     6     5     5
	mov	%rax, %r9	C               2     2     3     3     4
	sub	%rdx, %rax	C               2     2     4     3     4
	jnz	L(top)		C

L(end):	mov	%rdx, %rax
	mov	%r8, %rcx
	shl	R8(%rcx), %rax
	DOS64_EXIT()
	ret
EPILOGUE()
