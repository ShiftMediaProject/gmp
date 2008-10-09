dnl  AMD64 mpn_mul_basecase.

dnl  Copyright 2008 Free Software Foundation, Inc.

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

C	     cycles/limb
C K8,K9:	 2.375
C K10:		 ?
C P4:		 ?
C P6-15:	 4.45

C The inner loops of this code are the result of running a code generation and
C permutation tool suite written by David Harvey and Torbjorn Granlund.

C TODO
C  * Finish.

C INPUT PARAMETERS
define(`rp',      `%rdi')
define(`up',      `%rsi')
define(`un_param',`%rdx')
define(`vp_param',`%rcx')
define(`vn',      `%r8')

define(`v0', `%r12')
define(`v1', `%r9')

define(`w0', `%rbx')
define(`w1', `%rcx')
define(`w2', `%rbp')
define(`w3', `%r10')

define(`n',  `%r11')
define(`outer_addr', `%r14')
define(`un',  `%r13')
define(`vp',  `%r15')

ASM_START()
	TEXT
	ALIGN(16)
PROLOGUE(mpn_mul_basecase)
	push	%rbx
	push	%rbp
	push	%r12
	push	%r13
	push	%r14
	push	%r15

	lea	(rp,un_param,8), rp
	lea	(up,un_param,8), up
	mov	un_param, un
	mov	vp_param, vp

	test	$1, R8(vn)
	jz	L(mul_2)

C ===========================================================
C     mul_1 for vp[0] if vn is odd

L(mul_1):
	mov	$0, R32(n)
	sub	un, n

	mov	(up,n,8), %rax
	mov	(vp), v0
	mul	v0

	mov	R32(un), R32(w0)
	and	$3, R32(w0)
	jz	L(mul_1_prologue_0)
	cmp	$2, R32(w0)
	jc	L(mul_1_prologue_1)
	jz	L(mul_1_prologue_2)
	jmp	L(mul_1_prologue_3)

L(mul_1_prologue_0):
	mov	%rax, w2
	mov	%rdx, w3
C	mov	$0, R32(w0)		C FIXME: isn't already w0 == 0?
	lea	0(%rip), outer_addr	C FIXME: combine lea/add
1:	add	$(L(addmul_outer_0) - 1b), outer_addr
	jmp	L(mul_1_entry_0)

L(mul_1_prologue_1):
	cmp	$1, un
	jne	2f
	mov	%rax, -8(rp)
	mov	%rdx, (rp)
	jmp	L(ret)
2:	add	$1, n
	mov	%rax, w1
	mov	%rdx, w2
	mov	$0, R32(w3)
	mov	(up,n,8), %rax
	lea	0(%rip), outer_addr	C FIXME: combine lea/add
1:	add	$(L(addmul_outer_1) - 1b), outer_addr
	jmp	L(mul_1_entry_1)

L(mul_1_prologue_2):
	add	$-2, n
	mov	%rax, w0
	mov	%rdx, w1
	mov	24(up,n,8), %rax
	mov	$0, R32(w2)
	mov	$0, R32(w3)
	lea	0(%rip), outer_addr	C FIXME: combine lea/add
1:	add	$(L(addmul_outer_2) - 1b), outer_addr
	jmp	L(mul_1_entry_2)

L(mul_1_prologue_3):
	add	$-1, n
	mov	%rax, w3
	mov	%rdx, w0
	lea	0(%rip), outer_addr	C FIXME: combine lea/add
1:	add	$(L(addmul_outer_3) - 1b), outer_addr
	jmp	L(mul_1_entry_3)


	C this loop is 10 c/loop = 2.5 c/l on K8

	ALIGN(32)
L(mul_1_top):
	mov	w0, -16(rp,n,8)
	add	%rax, w1
	mov	(up,n,8), %rax
	adc	%rdx, w2
L(mul_1_entry_1):
	mov	$0, R32(w0)
	mul	v0
	mov	w1, -8(rp,n,8)
	add	%rax, w2
	adc	%rdx, w3
L(mul_1_entry_0):
	mov	8(up,n,8), %rax
	mul	v0
	mov	w2, (rp,n,8)
	add	%rax, w3
	adc	%rdx, w0
L(mul_1_entry_3):
	mov	16(up,n,8), %rax
	mul	v0
	mov	w3, 8(rp,n,8)
	mov	$0, R32(w2)		C zero
	mov	w2, w3			C zero
	add	%rax, w0
	mov	24(up,n,8), %rax
	mov	w2, w1			C zero
	adc	%rdx, w1
L(mul_1_entry_2):
	mul	v0
	add	$4, n
	js	L(mul_1_top)

	mov	w0, -16(rp,n,8)
	add	%rax, w1
	mov	w1, -8(rp,n,8)
	adc	%rdx, w2
	mov	w2, (rp,n,8)

	lea	8(rp), rp		C rp += 1
	lea	8(vp), vp		C vp += 1
	add	$-1, vn			C vn -= 1
	jz	L(ret)

	mov	(vp), v0
	mov	8(vp), v1
	neg	un
	jmp	*outer_addr

C ===========================================================
C     mul_2 for vp[0], vp[1] if vn is even

L(mul_2):
	mov	$0, R32(n)
	sub	un, n

	mov	(vp), v0
	mov	8(vp), v1

	mov	(up,n,8), %rax
	mul	v0

	mov	R32(un), R32(w0)
	and	$3, R32(w0)
	jz	L(mul_2_prologue_0)
	cmp	$2, R32(w0)
	jc	L(mul_2_prologue_1)
	jz	L(mul_2_prologue_2)
	jmp	L(mul_2_prologue_3)

L(mul_2_prologue_0):
	add	$3, n
	mov	%rax, w0
	mov	%rdx, w1
	mov	$0, R32(w2)
	mov	-24(up,n,8), %rax
	lea	0(%rip), outer_addr	C FIXME: combine lea/add
1:	add	$(L(addmul_outer_0) - 1b), outer_addr
	jmp	L(mul_2_entry_0)

L(mul_2_prologue_1):
	mov	%rax, w3
	mov	%rdx, w0
	mov	$0, R32(w1)
	lea	0(%rip), outer_addr	C FIXME: combine lea/add
1:	add	$(L(addmul_outer_1) - 1b), outer_addr
	jmp	L(mul_2_entry_1)

L(mul_2_prologue_2):
	add	$1, n
	mov	%rax, w2
	mov	%rdx, w3
	mov	$0, R32(w0)
	mov	$0, R32(w1)
	mov	-8(up,n,8), %rax
	lea	0(%rip), outer_addr	C FIXME: combine lea/add
1:	add	$(L(addmul_outer_2) - 1b), outer_addr
	jmp	L(mul_2_entry_2)

L(mul_2_prologue_3):
	add	$2, n
	mov	%rax, -16(rp,n,8)
	mov	%rdx, w2
	mov	$0, R32(w3)
	mov	$0, R32(w0)
	mov	-16(up,n,8), %rax
	lea	0(%rip), outer_addr	C FIXME: combine lea/add
1:	add	$(L(addmul_outer_3) - 1b), outer_addr
	jmp	L(mul_2_entry_3)


	C this loop is 18 c/loop = 2.25 c/l on K8

	ALIGN(32)
L(mul_2_top):
	mul	v1
	add	%rax, w0
	adc	%rdx, w1
	mov	-24(up,n,8), %rax
	mov	$0, R32(w2)
	mul	v0
	add	%rax, w0
	mov	-24(up,n,8), %rax
	adc	%rdx, w1
	adc	$0, R32(w2)
L(mul_2_entry_0):
	mul	v1
	add	%rax, w1
	mov	w0, -24(rp,n,8)
	adc	%rdx, w2
	mov	-16(up,n,8), %rax
	mul	v0
	mov	$0, R32(w3)
	add	%rax, w1
	adc	%rdx, w2
	mov	-16(up,n,8), %rax
	adc	$0, R32(w3)
	mov	$0, R32(w0)
	mov	w1, -16(rp,n,8)
L(mul_2_entry_3):
	mul	v1
	add	%rax, w2
	mov	-8(up,n,8), %rax
	adc	%rdx, w3
	mov	$0, R32(w1)
	mul	v0
	add	%rax, w2
	mov	-8(up,n,8), %rax
	adc	%rdx, w3
	adc	$0, R32(w0)
L(mul_2_entry_2):
	mul	v1
	add	%rax, w3
	mov	w2, -8(rp,n,8)
	adc	%rdx, w0
	mov	(up,n,8), %rax
	mul	v0
	add	%rax, w3
	adc	%rdx, w0
	adc	$0, R32(w1)
L(mul_2_entry_1):
	add	$4, n
	mov	-32(up,n,8), %rax
	mov	w3, -32(rp,n,8)
	js	L(mul_2_top)

	mul	v1
	add	%rax, w0
	adc	%rdx, w1
	mov	w0, -24(rp,n,8)
	mov	w1, -16(rp,n,8)

	lea	16(rp), rp		C rp += 2
	lea	16(vp), vp		C vp += 2
	add	$-2, vn			C vn -= 2
	jz	L(ret)

	mov	(vp), v0
	mov	8(vp), v1
	neg	un
	jmp	*outer_addr


C ===========================================================
C     addmul_2 for remaining vp's

	C in the following prologues, we reuse un to store the
	C adjusted value of n that is reloaded on each iteration

L(addmul_outer_0):
	add	$3, un
	lea	0(%rip), outer_addr

	mov	un, n
	mov	-24(up,un,8), %rax
	mul	v0
	mov	%rax, w0
	mov	-24(up,un,8), %rax
	mov	%rdx, w1
	xor	R32(w2), R32(w2)
	jmp	L(addmul_entry_0)

L(addmul_outer_1):
	lea	0(%rip), outer_addr

	mov	un, n
	mov	(up,un,8), %rax
	mul	v0
	mov	%rax, w3
	mov	(up,un,8), %rax
	mov	%rdx, w0
	xor	R32(w1), R32(w1)
	jmp	L(addmul_entry_1)

L(addmul_outer_2):
	add	$1, un
	lea	0(%rip), outer_addr

	mov	un, n
	mov	-8(up,un,8), %rax
	mul	v0
	mov	%rax, w2
	mov	-8(up,un,8), %rax
	mov	%rdx, w3
	xor	R32(w0), R32(w0)
	xor	R32(w1), R32(w1)
	jmp	L(addmul_entry_2)

L(addmul_outer_3):
	add	$2, un
	lea	0(%rip), outer_addr

	mov	un, n
	mov	-16(up,un,8), %rax
	mul	v0
	mov	%rax, w1
	mov	-16(up,un,8), %rax
	mov	%rdx, w2
	xor	R32(w3), R32(w3)
	jmp	L(addmul_entry_3)

	C this loop is 19 c/loop = 2.375 c/l on K8

	ALIGN(32)
L(addmul_top):
	add	w3, -32(rp,n,8)
	adc	%rax, w0
	mov	-24(up,n,8), %rax
	adc	%rdx, w1
	mov	$0, R32(w2)
	mul	v0
	add	%rax, w0
	mov	-24(up,n,8), %rax
	adc	%rdx, w1
	adc	$0, R32(w2)
L(addmul_entry_0):
	mul	v1
	mov	$0, R32(w3)
	add	w0, -24(rp,n,8)
	adc	%rax, w1
	mov	-16(up,n,8), %rax
	adc	%rdx, w2
	mul	v0
	add	%rax, w1
	mov	-16(up,n,8), %rax
	adc	%rdx, w2
	adc	$0, R32(w3)
L(addmul_entry_3):
	mul	v1
	add	w1, -16(rp,n,8)
	adc	%rax, w2
	mov	-8(up,n,8), %rax
	adc	%rdx, w3
	mul	v0
	mov	$0, R32(w0)
	add	%rax, w2
	adc	%rdx, w3
	mov	$0, R32(w1)
	mov	-8(up,n,8), %rax
	adc	$0, R32(w0)
L(addmul_entry_2):
	mul	v1
	add	w2, -8(rp,n,8)
	adc	%rax, w3
	adc	%rdx, w0
	mov	(up,n,8), %rax
	mul	v0
	add	%rax, w3
	mov	(up,n,8), %rax
	adc	%rdx, w0
	adc	$0, R32(w1)
L(addmul_entry_1):
	mul	v1
	add	$4, n
	js	L(addmul_top)

	add	w3, -32(rp,n,8)
	adc	%rax, w0
	adc	%rdx, w1
	mov	w0, -24(rp,n,8)
	mov	w1, -16(rp,n,8)

	lea	16(rp), rp		C rp += 2
	lea	16(vp), vp		C vp += 2
	add	$-2, vn			C vn -= 2
	jz	L(ret)

	mov	(vp), v0
	mov	8(vp), v1
	jmp	*outer_addr


L(ret):	pop	%r15
	pop	%r14
	pop	%r13
	pop	%r12
	pop	%rbp
	pop	%rbx
	ret

EPILOGUE()
