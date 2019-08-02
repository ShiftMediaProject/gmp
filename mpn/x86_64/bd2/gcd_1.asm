dnl  AMD64 mpn_gcd_1 optimised for AMD BD2-BD4, Zen.

dnl  Based on the K7 gcd_1.asm, by Kevin Ryde.  Rehacked for AMD64 by Torbjorn
dnl  Granlund.

dnl  Copyright 2000-2002, 2005, 2009, 2011, 2012, 2017, 2019 Free Software
dnl  Foundation, Inc.

dnl  This file is part of the GNU MP Library.
dnl
dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of either:
dnl
dnl    * the GNU Lesser General Public License as published by the Free
dnl      Software Foundation; either version 3 of the License, or (at your
dnl      option) any later version.
dnl
dnl  or
dnl
dnl    * the GNU General Public License as published by the Free Software
dnl      Foundation; either version 2 of the License, or (at your option) any
dnl      later version.
dnl
dnl  or both in parallel, as here.
dnl
dnl  The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
dnl  for more details.
dnl
dnl  You should have received copies of the GNU General Public License and the
dnl  GNU Lesser General Public License along with the GNU MP Library.  If not,
dnl  see https://www.gnu.org/licenses/.

include(`../config.m4')


C	     cycles/bit (approx)
C AMD K8,K9	 ?
C AMD K10	 ?
C AMD bd1	 ?
C AMD bd2	 ?
C AMD bd3	 ?
C AMD bd4	 3.65
C AMD bt1	 ?
C AMD bt2	 ?
C AMD zn1	 3.5
C AMD zn2	 3.8
C Intel P4	 ?
C Intel core2	 ?
C Intel NHM	 ?
C Intel SBR	 ?
C Intel IBR	 ?
C Intel HWL	 ?
C Intel BWL	 ?
C Intel SKL	 ?
C Intel atom	 ?
C Intel SLM	 ?
C Intel GLM	 ?
C Intel GLM+	 ?
C VIA nano	 ?
C Numbers measured with: speed -CD -s16-64 -t48 mpn_gcd_1

C TODO
C  * Optimise inner-loop for specific CPUs.
C  * Use DIV for 1-by-1 reductions, at least for some CPUs.

C Threshold of when to call bmod when U is one limb.  Should be about
C (time_in_cycles(bmod_1,1) + call_overhead) / (cycles/bit).
define(`BMOD_THRES_LOG2', 6)

C INPUT PARAMETERS
define(`up',    `%rdi')
define(`n',     `%rsi')
define(`v0',    `%rdx')

ABI_SUPPORT(DOS64)
ABI_SUPPORT(STD64)

IFDOS(`define(`STACK_ALLOC', 40)')
IFSTD(`define(`STACK_ALLOC', 8)')

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
	FUNC_ENTRY(3)
	mov	(up), %rax		C U low limb
	or	v0, %rax		C x | y
	bsf	%rax, %rax		C min(ctz(u0),ctz(v0))

	bsf	v0, %rcx
	shr	R8(%rcx), v0

	push	%rax			C preserve common twos over call

	cmp	$1, n
	jnz	L(reduce_nby1)

C Both U and V are single limbs, reduce with bmod if u0 >> v0.
	mov	(up), %r8
	mov	%r8, %rax
	shr	$BMOD_THRES_LOG2, %r8
	cmp	%r8, v0
	ja	L(reduced)

L(bmod):
	push	v0			C preserve v0 argument over call
	sub	$STACK_ALLOC, %rsp	C maintain ABI required rsp alignment
IFDOS(`	mov	%rdx, %r8	')
IFDOS(`	mov	%rsi, %rdx	')
IFDOS(`	mov	%rdi, %rcx	')
	ASSERT(nz, `test $15, %rsp')
	CALL(	mpn_modexact_1_odd)

L(called):
	add	$STACK_ALLOC, %rsp
	pop	v0

L(reduced):
	bsf	%rax, %rcx
C	test	%rax, %rax		C FIXME: does this lower latency?
	jnz	L(mid)
	jmp	L(end)

L(reduce_nby1):
	cmp	$BMOD_1_TO_MOD_1_THRESHOLD, n
	jl	L(bmod)

	push	v0			C preserve v0 argument over call
	sub	$STACK_ALLOC, %rsp	C maintain ABI required rsp alignment
IFDOS(`	mov	%rdx, %r8	')
IFDOS(`	mov	%rsi, %rdx	')
IFDOS(`	mov	%rdi, %rcx	')
	ASSERT(nz, `test $15, %rsp')
	CALL(	mpn_mod_1)
	jmp	L(called)

	ALIGN(16)			C              K10 BD1 BD2 ZEN CNR NHM SBR
L(top):	cmovc	%r10, %rax		C if x-y < 0   0,3 0,3 0,3 0,3 0,6 0,5 0,5
	cmovc	%r9, v0			C use x,y-x    0,3 0,3 0,3 0,3 2,8 1,7 1,7
L(mid):	shr	R8(%rcx), %rax		C              1,7 1,6 1,5 1,4 2,8 2,8 2,8
	mov	v0, %r10		C              1   1   1   1   4   3   3
	sub	%rax, %r10		C              2   2   2   1   5   4   4
	rep;bsf	%r10, %rcx		C tzcnt!       3   3   3   2   6   5   5
	mov	%rax, %r9		C              2   2   2   2   3   3   4
	sub	v0, %rax		C              2   2   2   2   4   3   4
	jnz	L(top)			C

L(end):	pop	%rcx			C common twos
	mov	v0, %rax
	shl	R8(%rcx), %rax
	FUNC_EXIT()
	ret
EPILOGUE()
