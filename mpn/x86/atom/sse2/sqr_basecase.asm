dnl  x86 mpn_sqr_basecase -- square an mpn number, optimised for atom.

dnl  Contributed to the GNU project by Torbjorn Granlund and Marco Bodrato.
dnl
dnl  Copyright 2011 Free Software Foundation, Inc.
dnl
dnl  This file is part of the GNU MP Library.
dnl
dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU Lesser General Public License as published
dnl  by the Free Software Foundation; either version 3 of the License, or (at
dnl  your option) any later version.
dnl
dnl  The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
dnl  License for more details.
dnl
dnl  You should have received a copy of the GNU Lesser General Public License
dnl  along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.

include(`../config.m4')

C TODO
C  * Check if 'jmp N(%esp)' is well-predicted enough to allow us to combine the
C    4 large loops into one; we could use it for the outer loop branch.
C  * Optimise code outside of inner loops.
C  * Combine rp and up updates in outer loop to save a bunch of lea insns.
C  * Write combined addmul_1 feed-in a wind-down code, and use when iterating
C    outer each loop.  ("Overlapping software pipelining")
C  * Postpone push of ebx until we know n > 1.  Perhaps use caller-saves regs
C    for inlined mul_1, allowing us to postpone all pushes.
C  * Perhaps write special code for n < M, for some small M.
C  * Replace inlined addmul_1 with smaller code from aorsmul_1.asm, or perhaps
C    with even less pipelined code.
C  * Fix function header code.
C  * We run the outer loop too long, until we perform a 1-limb by 1-limb
C    multiply.  The main problem with this is that the decreasing inner loop
C    trip counts will cause poor exit branch prediction; this hurts short loops
C    VERY much.

C void mpn_sqr_basecase (mp_ptr wp,
C                        mp_srcptr xp, mp_size_t xn);

define(`rp',  `%edi')
define(`up',  `%esi')
define(`n',   `%ecx')

define(`un',  `%ebp')

	TEXT
	ALIGN(16)
PROLOGUE(mpn_sqr_basecase)
	push	%edi
	push	%esi
	push	%ebx
	push	%ebp
	mov	20(%esp), rp
	mov	24(%esp), up
	mov	28(%esp), n

	lea	4(rp), rp	C write triangular product starting at rp[1]
	lea	-1(n), %eax
	neg	n
	movd	(up), %mm7
	movd	4(up), %mm0
	lea	4(up), up
	pmuludq	%mm7, %mm0
	pxor	%mm6, %mm6
	lea	1(n), un	C decr ABSOLUTE value
	lea	1(n), n		C decr ABSOLUTE value

	and	$3, %eax
	jz	L(of0)
	cmp	$2, %eax
	jc	L(of1)
	jz	L(of2)

C ================================================================
	jmp	L(m3)
	ALIGN(16)
L(lm3):	movd	-4(up), %mm0
	pmuludq	%mm7, %mm0
	psrlq	$32, %mm6
	lea	16(rp), rp
	paddq	%mm0, %mm6
	movd	(up), %mm0
	pmuludq	%mm7, %mm0
	movd	%mm6, -4(rp)
	psrlq	$32, %mm6
L(m3):	paddq	%mm0, %mm6
	movd	4(up), %mm0
	pmuludq	%mm7, %mm0
	movd	%mm6, (rp)
	psrlq	$32, %mm6
	paddq	%mm0, %mm6
	movd	8(up), %mm0
	pmuludq	%mm7, %mm0
	movd	%mm6, 4(rp)
	psrlq	$32, %mm6
	paddq	%mm0, %mm6
	add	$4, un
	movd	%mm6, 8(rp)
	lea	16(up), up
	js	L(lm3)

	psrlq	$32, %mm6
	movd	%mm6, 12(rp)

	inc	n
C	jz	L(done)
  lea	-12(up), up
  lea	4(rp), rp
	jmp	L(ol2)

C ================================================================
L(of0):	test	n, n
	jz	L(one)
	jmp	L(xx0)
	ALIGN(16)
L(lm0):	movd	(up), %mm0
	pmuludq	%mm7, %mm0
	psrlq	$32, %mm6
	lea	16(rp), rp
L(xx0):	paddq	%mm0, %mm6
	movd	4(up), %mm0
	pmuludq	%mm7, %mm0
	movd	%mm6, (rp)
	psrlq	$32, %mm6
	paddq	%mm0, %mm6
	movd	8(up), %mm0
	pmuludq	%mm7, %mm0
	movd	%mm6, 4(rp)
	psrlq	$32, %mm6
	paddq	%mm0, %mm6
	movd	12(up), %mm0
	pmuludq	%mm7, %mm0
	movd	%mm6, 8(rp)
	psrlq	$32, %mm6
	paddq	%mm0, %mm6
	add	$4, un
	movd	%mm6, 12(rp)
	lea	16(up), up
	js	L(lm0)

	psrlq	$32, %mm6
	movd	%mm6, 16(rp)

	inc	n
C	jz	L(done)
  lea	-8(up), up
  lea	8(rp), rp
	jmp	L(ol3)

C ================================================================
	ALIGN(16)
L(lm1):	movd	-12(up), %mm0
	pmuludq	%mm7, %mm0
	psrlq	$32, %mm6
	lea	16(rp), rp
	paddq	%mm0, %mm6
	movd	-8(up), %mm0
	pmuludq	%mm7, %mm0
	movd	%mm6, -12(rp)
	psrlq	$32, %mm6
	paddq	%mm0, %mm6
	movd	-4(up), %mm0
	pmuludq	%mm7, %mm0
	movd	%mm6, -8(rp)
	psrlq	$32, %mm6
	paddq	%mm0, %mm6
	movd	(up), %mm0
	pmuludq	%mm7, %mm0
	movd	%mm6, -4(rp)
	psrlq	$32, %mm6
L(of1):	paddq	%mm0, %mm6
	add	$4, un
	movd	%mm6, (rp)
	lea	16(up), up
	js	L(lm1)

	psrlq	$32, %mm6
	movd	%mm6, 4(rp)

	inc	n
	jz	L(done)
  lea	-20(up), up
  lea	-4(rp), rp
	jmp	L(ol0)

C ================================================================
	ALIGN(16)
L(lm2):	movd	-8(up), %mm0
	pmuludq	%mm7, %mm0
	psrlq	$32, %mm6
	lea	16(rp), rp
	paddq	%mm0, %mm6
	movd	-4(up), %mm0
	pmuludq	%mm7, %mm0
	movd	%mm6, -8(rp)
	psrlq	$32, %mm6
	paddq	%mm0, %mm6
	movd	(up), %mm0
	pmuludq	%mm7, %mm0
	movd	%mm6, -4(rp)
	psrlq	$32, %mm6
L(of2):	paddq	%mm0, %mm6
	movd	4(up), %mm0
	pmuludq	%mm7, %mm0
	movd	%mm6, (rp)
	psrlq	$32, %mm6
	paddq	%mm0, %mm6
	add	$4, un
	movd	%mm6, 4(rp)
	lea	16(up), up
	js	L(lm2)

	psrlq	$32, %mm6
	movd	%mm6, 8(rp)

	inc	n
C	jz	L(done)
  lea	-16(up), up
C  lea	(rp), rp
C	jmp	L(ol1)

C ================================================================

L(ol1):	lea	4(up,n,4), up
	movd	(up), %mm7	C read next U invariant limb
	lea	8(rp,n,4), rp
	mov	n, un

	movd	4(up), %mm1
	pmuludq	%mm7, %mm1
	sar	$2, un
	movd	%mm1, %ebx
	inc	un
	jz	L(eq1)

	movd	8(up), %mm0
	pmuludq	%mm7, %mm0
	xor	%edx, %edx	C zero edx and CF
	jmp	L(a1)
L(eq1):
	psrlq	$32, %mm1
	movd	%mm1, %eax
	add	%ebx, 4(rp)
	adc	un, %eax
	mov	%eax, 8(rp)
	jmp	L(cj1)

L(la1):	adc	$0, %edx
	add	%ebx, 12(rp)
	movd	%mm0, %eax
	pmuludq	%mm7, %mm1
	lea	16(rp), rp
	psrlq	$32, %mm0
	adc	%edx, %eax
	movd	%mm0, %edx
	movd	%mm1, %ebx
	movd	8(up), %mm0
	pmuludq	%mm7, %mm0
	adc	$0, %edx
	add	%eax, (rp)
L(a1):	psrlq	$32, %mm1
	adc	%edx, %ebx
	movd	%mm1, %edx
	movd	%mm0, %eax
	movd	12(up), %mm1
	pmuludq	%mm7, %mm1
	adc	$0, %edx
	add	%ebx, 4(rp)
	psrlq	$32, %mm0
	adc	%edx, %eax
	movd	%mm0, %edx
	movd	%mm1, %ebx
	lea	16(up), up
	movd	(up), %mm0
	adc	$0, %edx
	add	%eax, 8(rp)
	psrlq	$32, %mm1
	adc	%edx, %ebx
	movd	%mm1, %edx
	pmuludq	%mm7, %mm0
	inc	un
	movd	4(up), %mm1
	jnz	L(la1)

	adc	un, %edx	C un is zero here
	add	%ebx, 12(rp)
	movd	%mm0, %eax
	pmuludq	%mm7, %mm1
	lea	16(rp), rp
	psrlq	$32, %mm0
	adc	%edx, %eax
	movd	%mm0, %edx
	movd	%mm1, %ebx
	adc	un, %edx
	add	%eax, (rp)
	psrlq	$32, %mm1
	adc	%edx, %ebx
	movd	%mm1, %eax
	adc	un, %eax
	add	%ebx, 4(rp)
	adc	un, %eax
	mov	%eax, 8(rp)
L(cj1):
	inc	n
	jz	L(done)

C ================================================================

L(ol0):	lea	(up,n,4), up
	movd	4(up), %mm7	C read next U invariant limb
	lea	4(rp,n,4), rp
	mov	n, un

	movd	8(up), %mm0
	pmuludq	%mm7, %mm0
	sar	$2, un
	movd	12(up), %mm1
	movd	%mm0, %eax
	pmuludq	%mm7, %mm1
	xor	%edx, %edx	C zero edx and CF
	jmp	L(a0)

L(la0):	adc	$0, %edx
	add	%ebx, 12(rp)
	movd	%mm0, %eax
	pmuludq	%mm7, %mm1
	lea	16(rp), rp
	psrlq	$32, %mm0
	adc	%edx, %eax
	movd	%mm0, %edx
	movd	%mm1, %ebx
	movd	8(up), %mm0
	pmuludq	%mm7, %mm0
	adc	$0, %edx
	add	%eax, (rp)
	psrlq	$32, %mm1
	adc	%edx, %ebx
	movd	%mm1, %edx
	movd	%mm0, %eax
	movd	12(up), %mm1
	pmuludq	%mm7, %mm1
	adc	$0, %edx
	add	%ebx, 4(rp)
L(a0):	psrlq	$32, %mm0
	adc	%edx, %eax
	movd	%mm0, %edx
	movd	%mm1, %ebx
	lea	16(up), up
	movd	(up), %mm0
	adc	$0, %edx
	add	%eax, 8(rp)
	psrlq	$32, %mm1
	adc	%edx, %ebx
	movd	%mm1, %edx
	pmuludq	%mm7, %mm0
	inc	un
	movd	4(up), %mm1
	jnz	L(la0)

	adc	un, %edx	C un is zero here
	add	%ebx, 12(rp)
	movd	%mm0, %eax
	pmuludq	%mm7, %mm1
	lea	16(rp), rp
	psrlq	$32, %mm0
	adc	%edx, %eax
	movd	%mm0, %edx
	movd	%mm1, %ebx
	adc	un, %edx
	add	%eax, (rp)
	psrlq	$32, %mm1
	adc	%edx, %ebx
	movd	%mm1, %eax
	adc	un, %eax
	add	%ebx, 4(rp)
	adc	un, %eax
	mov	%eax, 8(rp)

	inc	n

C ================================================================

L(ol3):	lea	12(up,n,4), up
	movd	-8(up), %mm7	C read next U invariant limb
	lea	(rp,n,4), rp	C put rp back
	mov	n, un

	movd	-4(up), %mm1
	pmuludq	%mm7, %mm1
	sar	$2, un
	movd	%mm1, %ebx
	movd	(up), %mm0
	xor	%edx, %edx	C zero edx and CF
	jmp	L(a3)

L(la3):	adc	$0, %edx
	add	%ebx, 12(rp)
	movd	%mm0, %eax
	pmuludq	%mm7, %mm1
	lea	16(rp), rp
	psrlq	$32, %mm0
	adc	%edx, %eax
	movd	%mm0, %edx
	movd	%mm1, %ebx
	movd	8(up), %mm0
	pmuludq	%mm7, %mm0
	adc	$0, %edx
	add	%eax, (rp)
	psrlq	$32, %mm1
	adc	%edx, %ebx
	movd	%mm1, %edx
	movd	%mm0, %eax
	movd	12(up), %mm1
	pmuludq	%mm7, %mm1
	adc	$0, %edx
	add	%ebx, 4(rp)
	psrlq	$32, %mm0
	adc	%edx, %eax
	movd	%mm0, %edx
	movd	%mm1, %ebx
	lea	16(up), up
	movd	(up), %mm0
	adc	$0, %edx
	add	%eax, 8(rp)
L(a3):	psrlq	$32, %mm1
	adc	%edx, %ebx
	movd	%mm1, %edx
	pmuludq	%mm7, %mm0
	inc	un
	movd	4(up), %mm1
	jnz	L(la3)

	adc	un, %edx	C un is zero here
	add	%ebx, 12(rp)
	movd	%mm0, %eax
	pmuludq	%mm7, %mm1
	lea	16(rp), rp
	psrlq	$32, %mm0
	adc	%edx, %eax
	movd	%mm0, %edx
	movd	%mm1, %ebx
	adc	un, %edx
	add	%eax, (rp)
	psrlq	$32, %mm1
	adc	%edx, %ebx
	movd	%mm1, %eax
	adc	un, %eax
	add	%ebx, 4(rp)
	adc	un, %eax
	mov	%eax, 8(rp)

	inc	n

C ================================================================

L(ol2):	lea	8(up,n,4), up
	movd	-4(up), %mm7	C read next U invariant limb
	lea	12(rp,n,4), rp
	mov	n, un

	movd	(up), %mm0
	pmuludq	%mm7, %mm0
	xor	%edx, %edx
	sar	$2, un
	movd	4(up), %mm1
	test	un, un		C clear carry
	movd	%mm0, %eax
	pmuludq	%mm7, %mm1
	inc	un
	jnz	L(a2)
	jmp	L(wd2)

L(la2):	adc	$0, %edx
	add	%ebx, 12(rp)
	movd	%mm0, %eax
	pmuludq	%mm7, %mm1
	lea	16(rp), rp
L(a2):	psrlq	$32, %mm0
	adc	%edx, %eax
	movd	%mm0, %edx
	movd	%mm1, %ebx
	movd	8(up), %mm0
	pmuludq	%mm7, %mm0
	adc	$0, %edx
	add	%eax, (rp)
	psrlq	$32, %mm1
	adc	%edx, %ebx
	movd	%mm1, %edx
	movd	%mm0, %eax
	movd	12(up), %mm1
	pmuludq	%mm7, %mm1
	adc	$0, %edx
	add	%ebx, 4(rp)
	psrlq	$32, %mm0
	adc	%edx, %eax
	movd	%mm0, %edx
	movd	%mm1, %ebx
	lea	16(up), up
	movd	(up), %mm0
	adc	$0, %edx
	add	%eax, 8(rp)
	psrlq	$32, %mm1
	adc	%edx, %ebx
	movd	%mm1, %edx
	pmuludq	%mm7, %mm0
	inc	un
	movd	4(up), %mm1
	jnz	L(la2)

	adc	un, %edx	C un is zero here
	add	%ebx, 12(rp)
	movd	%mm0, %eax
	pmuludq	%mm7, %mm1
	lea	16(rp), rp
L(wd2):	psrlq	$32, %mm0
	adc	%edx, %eax
	movd	%mm0, %edx
	movd	%mm1, %ebx
	adc	un, %edx
	add	%eax, (rp)
	psrlq	$32, %mm1
	adc	%edx, %ebx
	movd	%mm1, %eax
	adc	un, %eax
	add	%ebx, 4(rp)
	adc	un, %eax
	mov	%eax, 8(rp)

	inc	n
	jmp	L(ol1)

C ================================================================

L(done):
	mov	24(%esp), up
	mov	28(%esp), %eax

	movd	(up), %mm0
	xor	%ebp, %ebp
	pmuludq	%mm0, %mm0
	mov	20(%esp), rp
	shr	%eax
	movd	%mm0, (rp)
	psrlq	$32, %mm0
	mov	%eax, 28(%esp)
	jc	L(odd)

	movd	%mm0, %ecx
	movd	4(up), %mm0
	lea	-4(rp), rp
	pmuludq	%mm0, %mm0
	add	8(rp), %ecx
	movd	%mm0, %edx
	adc	12(rp), %edx
	rcr	%ebp
	jmp	L(ent)

L(odd):	clc			C clear carry  FIXME: use test/and
	lea	4(rp), rp
C	jz	L(end)
	lea	4(up), up

C	ALIGN(16)		C alignment seems irrelevant
L(top):	movd	(up), %mm1
	adc	%ebp, %ebp
	movd	%mm0, %eax
	pmuludq	%mm1, %mm1
	movd	4(up), %mm0
	adc	(rp), %eax
	movd	%mm1, %ebx
	pmuludq	%mm0, %mm0
	psrlq	$32, %mm1
	adc	4(rp), %ebx
	movd	%mm1, %ecx
	movd	%mm0, %edx
	adc	8(rp), %ecx
	adc	12(rp), %edx
	rcr	%ebp		C FIXME: isn't this awfully slow on atom???
	adc	%eax, (rp)
	adc	%ebx, 4(rp)
L(ent):	lea	8(up), up
	adc	%ecx, 8(rp)
	psrlq	$32, %mm0
	adc	%edx, 12(rp)
	decl	28(%esp)
	lea	16(rp), rp
	jnz	L(top)

L(end):	adc	%ebp, %ebp
	movd	%mm0, %eax
	adc	%ebp, %eax
	mov	%eax, (rp)

L(rtn):	emms
	pop	%ebp
	pop	%ebx
	pop	%esi
	pop	%edi
	ret

L(one):	movd	-4(up), %mm0
	pmuludq	%mm0, %mm0
	movq	%mm0, -4(rp)
	jmp	L(rtn)
EPILOGUE()
