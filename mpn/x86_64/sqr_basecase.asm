dnl  AMD64 mpn_sqr_basecase.

dnl  Contributed to the GNU project by Torbjorn Granlund.

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

C The inner loops of this code are the result of running a code generation and
C permutation tool suite written by David Harvey and Torbjorn Granlund.

C NOTES
C   * This code only handles operands up to SQR_KARATSUBA_THRESHOLD_MAX.  That
C     means we can safely use 32-bit operations for all sizes, unlike in e.g.,
C     mpn_addmul_1.
C   * We can reach the stack without a frame pointer even after the tp
C     allocation, since the allocation is actually fixed.
C   * Needed variables:
C    n   r11  input size
C    i   r8   work left, initially n
C    j   r9   inner loop count
C        r15  FIXME: unused, don't save/restore
C    v0  r13
C    v1  r14
C    rp  rdi
C    up  rsi
C    w0  rbx
C    w1  rcx
C    w2  rbp
C    w3  r10
C    tp  r12
C    lo  rax
C    hi  rdx
C        rsp

C INPUT PARAMETERS
define(`rp',	  `%rdi')
define(`up',	  `%rsi')
define(`n_param', `%rdx')

define(`SQR_KARATSUBA_THRESHOLD_MAX', 200)
define(`STACK_ALLOC', eval(8*2*SQR_KARATSUBA_THRESHOLD_MAX))

define(`n',     `%r11')
define(`tp',	`%r12')
define(`i',	`%r8')
define(`j',	`%r9')
define(`v0',	`%r13')
define(`v1',	`%r14')

FIXME:
define(`w0', `%rbx')
define(`w1', `%rcx')
define(`w2', `%rbp')
define(`w3', `%r10')



ASM_START()
	TEXT
	ALIGN(16)

PROLOGUE(mpn_sqr_basecase)
	add	$-48, %rsp

	mov	%rbx, 40(%rsp)
	mov	%rbp, 32(%rsp)
	mov	%r12, 24(%rsp)
	mov	%r13, 16(%rsp)
	mov	%r14, 8(%rsp)
	mov	%r15, (%rsp)

	mov	R32(n_param), R32(n)		C free original n register (rdx)
	mov	R32(n_param), R32(%rcx)
	and	$3, R32(%rcx)
	lea	4(%rcx), %rbx
	cmp	$4, R32(n)
	cmovge	%rbx, %rcx
	lea	L(jmptab)(%rip), %rdx
	movslq	(%rdx,%rcx,4), %rcx
	add	%rdx, %rcx
	jmp	*%rcx
	DATA
	ALIGN(8)
L(jmptab):
	.long	L(0)-L(jmptab)
	.long	L(1)-L(jmptab)
	.long	L(2)-L(jmptab)
	.long	L(3)-L(jmptab)
	.long	L(4)-L(jmptab)
	.long	L(5)-L(jmptab)
	.long	L(6)-L(jmptab)
	.long	L(7)-L(jmptab)
	TEXT

L(1):	mov	(up), %rax
	mul	%rax
	mov	%rax, (rp)
	mov	%rdx, 8(rp)
	add	$40, %rsp
	pop	%rbx
L(0):	ret

L(2):	mov	(up), %rax
	mul	%rax
	mov	%rax, (rp)
	mov	%rdx, %r9
	mov	8(up), %rax
	mul	%rax
	mov	%rax, %r10
	mov	%rdx, %r11
	mov	8(up), %rax
	mov	(up), %rbx
	mul	%rbx
	add	%rax, %r9
	adc	%rdx, %r10
	adc	$0, %r11
	add	%rax, %r9
	mov	%r9, 8(rp)
	adc	%rdx, %r10
	mov	%r10, 16(rp)
	adc	$0, %r11
	mov	%r11, 24(rp)
	add	$40, %rsp
	pop	%rbx
	ret

L(3):	mov	(up), %rax
	mul	%rax
	mov	%rax, (rp)
	mov	%rdx, 8(rp)
	mov	8(up), %rax
	mul	%rax
	mov	%rax, 16(rp)
	mov	%rdx, 24(rp)
	mov	16(up), %rax
	mul	%rax
	mov	%rax, 32(rp)
	mov	%rdx, 40(rp)
	mov	(up), %rbx
	mov	8(up), %rax
	mul	%rbx
	mov	%rax, %r8
	mov	%rdx, %r9
	mov	16(up), %rax
	mul	%rbx
	xor	R32(%r10), R32(%r10)
	add	%rax, %r9
	adc	%rdx, %r10
	mov	8(up), %rbx
	mov	16(up), %rax
	mul	%rbx
	xor	R32(%r11), R32(%r11)
	add	%rax, %r10
	adc	%rdx, %r11
	add	%r8, %r8
	adc	%r9, %r9
	adc	%r10, %r10
	adc	%r11, %r11
	mov	$0, R32(%rbx)
	adc	%rbx, %rbx
	add	%r8, 8(rp)
	adc	%r9, 16(rp)
	adc	%r10, 24(rp)
	adc	%r11, 32(rp)
	adc	%rbx, 40(rp)
	add	$40, %rsp
	pop	%rbx
	ret


L(4):	add	$-STACK_ALLOC, %rsp
	lea	(%rsp,n,8), tp		C point tp in middle of result operand
	lea	(up,n,8), up		C point up at end of input operand

	lea	-1(n), i
C Function mpn_mul_1_m3(tp, up - i, i, up[-i - 1])
	mov	$-1, j
	sub	i, j

	lea	-24(tp), tp		C offset FIXME

	mov	(up,j,8), v0
	mov	8(up,j,8), %rax
	mul	v0
	xor	R32(w2), R32(w2)
	mov	%rax, w0
	mov	16(up,j,8), %rax
	mov	%rdx, w3
	jmp	L(L3)

	ALIGN(16)
L(mul_1_m3_top):
	add	%rax, w2
	mov	w3, (tp,j,8)
	mov	(up,j,8), %rax
	adc	%rdx, w1
	xor	R32(w0), R32(w0)
	mul	v0
	xor	R32(w3), R32(w3)
	mov	w2, 8(tp,j,8)
	add	%rax, w1
	adc	%rdx, w0
	mov	8(up,j,8), %rax
	mov	w1, 16(tp,j,8)
	xor	R32(w2), R32(w2)
	mul	v0
	add	%rax, w0
	mov	16(up,j,8), %rax
	adc	%rdx, w3
L(L3):	xor	R32(w1), R32(w1)
	mul	v0
	add	%rax, w3
	mov	24(up,j,8), %rax
	adc	%rdx, w2
	mov	w0, 24(tp,j,8)
	mul	v0
	add	$4, j
	js	L(mul_1_m3_top)

	add	%rax, w2
	mov	w3, (tp)
	adc	%rdx, w1
	mov	w2, 8(tp)
	mov	w1, 16(tp)
	lea	24(tp), tp		C undo offset FIXME
	lea	eval(2*8)(tp), tp	C tp += 2
C	dec	R32(i)			C i -= 1
	cmp	$3, R32(i)
	je	L(last)
	jmp	L(dowhile)

L(5):	add	$-STACK_ALLOC, %rsp
	lea	(%rsp,n,8), tp		C point tp in middle of result operand
	lea	(up,n,8), up		C point up at end of input operand

	lea	(n), i
C Function mpn_mul_2s_m0(tp, up - i, i, up - i - 1)
	mov	$3, R32(j)
	sub	i, j

	lea	8(up), up		C offset FIXME

	mov	-32(up,j,8), v0		C u0
	mov	-24(up,j,8), v1		C u1
	mov	-24(up,j,8), %rax	C u1
	mul	v0			C u0 * u1
	mov	%rdx, w1
	xor	R32(w2), R32(w2)
	mov	%rax, -24(tp,j,8)
	jmp	L(m0)

	ALIGN(16)
L(mul_2_m0_top):
	mul	v1
	add	%rax, w0
	adc	%rdx, w1
	mov	-24(up,j,8), %rax
	mov	$0, R32(w2)
	mul	v0
	add	%rax, w0
	mov	-24(up,j,8), %rax
	adc	%rdx, w1
	adc	$0, R32(w2)
	mul	v1			C v1 * u0
	add	%rax, w1
	mov	w0, -24(tp,j,8)
	adc	%rdx, w2
L(m0):	mov	-16(up,j,8), %rax	C u2, u6 ...
	mul	v0			C u0 * u2
	mov	$0, R32(w3)
	add	%rax, w1
	adc	%rdx, w2
	mov	-16(up,j,8), %rax
	adc	$0, R32(w3)
	mov	$0, R32(w0)
	mov	w1, -16(tp,j,8)
	mul	v1
	add	%rax, w2
	mov	-8(up,j,8), %rax
	adc	%rdx, w3
	mov	$0, R32(w1)
	mul	v0
	add	%rax, w2
	mov	-8(up,j,8), %rax
	adc	%rdx, w3
	adc	$0, R32(w0)
	mul	v1
	add	%rax, w3
	mov	w2, -8(tp,j,8)
	adc	%rdx, w0
	mov	(up,j,8), %rax
	mul	v0
	add	%rax, w3
	adc	%rdx, w0
	adc	$0, R32(w1)
	add	$4, j
	mov	-32(up,j,8), %rax
	mov	w3, -32(tp,j,8)
	js	L(mul_2_m0_top)

	mul	v1
	add	%rax, w0
	adc	%rdx, w1
	mov	w0, -8(tp)
	mov	w1, (tp)

	lea	-8(up), up		C undo offset FIXME
	lea	eval(3*8)(tp), tp	C tp += 3
	add	$-2, R32(i)		C i -= 1
	cmp	$3, R32(i)
	je	L(last)
	jmp	L(dowhile)



L(6):	add	$-STACK_ALLOC, %rsp
	lea	(%rsp,n,8), tp		C point tp in middle of result operand
	lea	(up,n,8), up		C point up at end of input operand

	lea	-1(n), i
C Function mpn_mul_1_m1(tp, up - (i - 1), i - 1, up[-i])
	mov	$1, R32(j)
	sub	i, j

	lea	-24(tp), tp		C offset FIXME

	mov	-16(up,j,8), v0
	mov	-8(up,j,8), %rax
	mul	v0
	mov	%rax, w2
	mov	(up,j,8), %rax
	mov	%rdx, w1
	jmp	L(L1)

	ALIGN(16)
L(mul_1_m1_top):
	add	%rax, w2
	mov	w3, (tp,j,8)
	mov	(up,j,8), %rax
	adc	%rdx, w1
L(L1):	xor	R32(w0), R32(w0)
	mul	v0
	xor	R32(w3), R32(w3)
	mov	w2, 8(tp,j,8)
	add	%rax, w1
	adc	%rdx, w0
	mov	8(up,j,8), %rax
	mov	w1, 16(tp,j,8)
	xor	R32(w2), R32(w2)
	mul	v0
	add	%rax, w0
	mov	16(up,j,8), %rax
	adc	%rdx, w3
	xor	R32(w1), R32(w1)
	mul	v0
	add	%rax, w3
	mov	24(up,j,8), %rax
	adc	%rdx, w2
	mov	w0, 24(tp,j,8)
	mul	v0
	add	$4, j
	js	L(mul_1_m1_top)

	add	%rax, w2
	mov	w3, (tp)
	adc	%rdx, w1
	mov	w2, 8(tp)
	mov	w1, 16(tp)

	lea	24(tp), tp		C undo offset FIXME
	lea	eval(2*8)(tp), tp	C tp += FIXME
	jmp	L(dowhile_mid)



L(7):	add	$-STACK_ALLOC, %rsp
	lea	(%rsp,n,8), tp		C point tp in middle of result operand
	lea	(up,n,8), up		C point up at end of input operand

	lea	(n), i
C Function mpn_mul_2s_m2(tp, up - i + 1, i - 1, up - i)
	mov	$1, R32(j)
	sub	i, j

	lea	8(up), up		C offset FIXME

	mov	-16(up,j,8), v0
	mov	-8(up,j,8), v1
	mov	-8(up,j,8), %rax
	mul	v0			C v0 * u0
	mov	%rdx, w3
	xor	R32(w0), R32(w0)
	xor	R32(w1), R32(w1)
	mov	%rax, -8(tp,j,8)
	jmp	L(m2)

	ALIGN(16)
L(mul_2_m2_top):
	mul	v1
	add	%rax, w0
	adc	%rdx, w1
	mov	-24(up,j,8), %rax
	mov	$0, R32(w2)
	mul	v0
	add	%rax, w0
	mov	-24(up,j,8), %rax
	adc	%rdx, w1
	adc	$0, R32(w2)
	mul	v1			C v1 * u0
	add	%rax, w1
	mov	w0, -24(tp,j,8)
	adc	%rdx, w2
	mov	-16(up,j,8), %rax
	mul	v0
	mov	$0, R32(w3)
	add	%rax, w1
	adc	%rdx, w2
	mov	-16(up,j,8), %rax
	adc	$0, R32(w3)
	mov	$0, R32(w0)
	mov	w1, -16(tp,j,8)
	mul	v1
	add	%rax, w2
	mov	-8(up,j,8), %rax
	adc	%rdx, w3
	mov	$0, R32(w1)
	mul	v0
	add	%rax, w2
	mov	-8(up,j,8), %rax
	adc	%rdx, w3
	adc	$0, R32(w0)
	mul	v1
	add	%rax, w3
	mov	w2, -8(tp,j,8)
	adc	%rdx, w0
L(m2):	mov	(up,j,8), %rax
	mul	v0
	add	%rax, w3
	adc	%rdx, w0
	adc	$0, R32(w1)
	add	$4, j
	mov	-32(up,j,8), %rax
	mov	w3, -32(tp,j,8)
	js	L(mul_2_m2_top)

	mul	v1
	add	%rax, w0
	adc	%rdx, w1
	mov	w0, -8(tp)
	mov	w1, (tp)

	lea	-8(up), up		C undo offset FIXME
	lea	eval(3*8)(tp), tp	C tp += 3
	add	$-2, R32(i)		C i -= 1
	jmp	L(dowhile_mid)

L(dowhile):
C Function mpn_addmul_2s_m2(tp, up - (i - 1), i - 1, up - i)
	mov	$-1, j
	sub	i, j

	lea	-24(tp), tp		C offset FIXME
	lea	-8(up), up		C offset FIXME

	mov	16(up,j,8), v0
	mov	24(up,j,8), v1
	mov	24(up,j,8), %rax
	mul	v0
	xor	R32(w3), R32(w3)
	add	%rax, 24(tp,j,8)
	adc	%rdx, w3
	xor	R32(w0), R32(w0)
	xor	R32(w1), R32(w1)
	jmp	L(am2)

	ALIGN(16)
L(addmul_2_m2_top):
	add	w3, (tp,j,8)
	adc	%rax, w0
	mov	8(up,j,8), %rax
	adc	%rdx, w1
	mov	$0, R32(w2)
	mul	v0
	add	%rax, w0
	mov	8(up,j,8), %rax
	adc	%rdx, w1
	adc	$0, R32(w2)
	mul	v1				C v1 * u0
	add	w0, 8(tp,j,8)
	adc	%rax, w1
	adc	%rdx, w2
	mov	16(up,j,8), %rax
	mov	$0, R32(w3)
	mul	v0				C v0 * u1
	add	%rax, w1
	mov	16(up,j,8), %rax
	adc	%rdx, w2
	adc	$0, R32(w3)
	mul	v1				C v1 * u1
	add	w1, 16(tp,j,8)
	adc	%rax, w2
	mov	24(up,j,8), %rax
	adc	%rdx, w3
	mul	v0
	mov	$0, R32(w0)
	add	%rax, w2
	adc	%rdx, w3
	mov	$0, R32(w1)
	mov	24(up,j,8), %rax
	adc	$0, R32(w0)
	mul	v1
	add	w2, 24(tp,j,8)
	adc	%rax, w3
	adc	%rdx, w0
L(am2):	mov	32(up,j,8), %rax
	mul	v0
	add	%rax, w3
	mov	32(up,j,8), %rax
	adc	%rdx, w0
	adc	$0, R32(w1)
	mul	v1
	add	$4, j
	js	L(addmul_2_m2_top)

	add	w3, (tp)
	adc	%rax, w0
	adc	%rdx, w1
	mov	w0, 8(tp)
	mov	w1, 16(tp)

	lea	eval(2*8)(tp), tp	C tp += 2
	add	$-2, R32(i)		C i -= 2

	lea	24(tp), tp		C undo offset FIXME
	lea	8(up), up		C undo offset FIXME

L(dowhile_mid):
C Function mpn_addmul_2s_m0(tp, up - (i - 1), i - 1, up - i)

	mov	$1, R32(j)
	sub	i, j

	lea	-24(tp), tp		C offset FIXME
	lea	-8(up), up		C offset FIXME

	mov	(up,j,8), v0
	mov	8(up,j,8), v1
	mov	8(up,j,8), %rax
	mul	v0
	xor	R32(w1), R32(w1)
	add	%rax, 8(tp,j,8)
	adc	%rdx, w1
	xor	R32(w2), R32(w2)
	jmp	L(20)

	ALIGN(16)
L(addmul_2_m0_top):
	add	w3, (tp,j,8)
	adc	%rax, w0
	mov	8(up,j,8), %rax
	adc	%rdx, w1
	mov	$0, R32(w2)
	mul	v0
	add	%rax, w0
	mov	8(up,j,8), %rax
	adc	%rdx, w1
	adc	$0, R32(w2)
	mul	v1				C v1 * u0
	add	w0, 8(tp,j,8)
	adc	%rax, w1
	adc	%rdx, w2
L(20):	mov	16(up,j,8), %rax
	mov	$0, R32(w3)
	mul	v0				C v0 * u1
	add	%rax, w1
	mov	16(up,j,8), %rax
	adc	%rdx, w2
	adc	$0, R32(w3)
	mul	v1				C v1 * u1
	add	w1, 16(tp,j,8)
	adc	%rax, w2
	mov	24(up,j,8), %rax
	adc	%rdx, w3
	mul	v0
	mov	$0, R32(w0)
	add	%rax, w2
	adc	%rdx, w3
	mov	$0, R32(w1)
	mov	24(up,j,8), %rax
	adc	$0, R32(w0)
	mul	v1
	add	w2, 24(tp,j,8)
	adc	%rax, w3
	adc	%rdx, w0
	mov	32(up,j,8), %rax
	mul	v0
	add	%rax, w3
	mov	32(up,j,8), %rax
	adc	%rdx, w0
	adc	$0, R32(w1)
	mul	v1
	add	$4, j
	js	L(addmul_2_m0_top)

	add	w3, (tp)
	adc	%rax, w0
	adc	%rdx, w1
	mov	w0, 8(tp)
	mov	w1, 16(tp)

	lea	24(tp), tp		C undo offset FIXME
	lea	8(up), up		C undo offset FIXME

	lea	eval(2*8)(tp), tp	C tp += 2
	add	$-2, R32(i)		C i -= 2

	cmp	$3, R32(i)
	jne	L(dowhile)

L(last):

C Function mpn_addmul_2s_2

	mov	$3, R32(j)		C FIXME ?? replace n by 3 ??
	neg	j

	mov	-24(up), v0
	mov	-16(up), v1

	mov	-16(up), %rax
	mul	v0
	mov	$0, R32(w3)
	add	%rax, -8(tp,j,8)
	adc	%rdx, w3
	xor	R32(w0), R32(w0)
	xor	R32(w1), R32(w1)
	mov	-8(up), %rax
	mul	v0
	add	%rax, w3
	mov	-8(up), %rax
	adc	%rdx, w0
	mul	v1
	add	w3, (tp,j,8)
	adc	%rax, w0
	adc	%rdx, w1
	mov	w0, 8(tp,j,8)
	mov	w1, 16(tp,j,8)

C Function mpn_sqr_diagonal
	mov	R32(n), R32(j)
	neg	j
L(diag_top):
	mov	(up,j,8), %rax
	mul	%rax
	mov	%rax, (rp)
	mov	%rdx, 8(rp)
	lea	16(rp), rp
	inc	j
	js	L(diag_top)

	mov	rp, %r12		C Save rp+2n. CAUTION: overwrites tp
	mov	R32(n), R32(%rax)
	shl	$4, R32(%rax)
	sub	%rax, rp		C rp restored from last loop

C Call mpn_addlsh1_n.  Note that rp lives in %rdi, so we have to be careful
C with the order in which we assign the parameters.  Do arg2 before arg1!
	lea	8(rp), %rsi		C arg2: rp + 1
	lea	8(rp), %rdi		C arg1: rp + 1
	lea	(%rsp), %rdx		C arg3: tp
	lea	-2(,n,2), %rcx	C arg4: 2n - 2
	CALL(	mpn_addlsh1_n)
	add	%rax, -8(%r12)

L(rt):
	add	$STACK_ALLOC, %rsp
	pop	%r15
	pop	%r14
	pop	%r13
	pop	%r12
	pop	%rbp
	pop	%rbx
	ret
EPILOGUE()
