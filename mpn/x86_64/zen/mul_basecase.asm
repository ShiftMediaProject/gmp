dnl  AMD64 mpn_mul_basecase optimised for AMD Zen.

dnl  Copyright 2012, 2013, 2017 Free Software Foundation, Inc.

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

C TODO
C  * Try 2x unrolling instead of current 4x, at least for mul_1.  Else consider
C    shallower sw pipelining of mul_1/addmul_1 loops, allowing 4 instead of 8
C    product registers.
C  * Split up mul_1 into 4 loops in order to fall into the addmul_1 loops
C    without braches.
C  * Do overlapped software pipelining.
C  * Let vn_param be vn to save a copy.
C  * Re-allocate to benefit more from 32-bit encoding.
C  * Polish.

define(`rp',       `%rdi')
define(`up',       `%rsi')
define(`un_param', `%rdx')
define(`vp_param', `%rcx')
define(`vn_param', `%r8')

define(`un',       `%r14')
define(`vp',       `%rbp')
define(`v0',       `%rdx')
define(`n',        `%rcx')
define(`vn',       `%r15')

ABI_SUPPORT(DOS64)
ABI_SUPPORT(STD64)

ASM_START()
	TEXT
	ALIGN(32)
PROLOGUE(mpn_mul_basecase)
	FUNC_ENTRY(4)
IFDOS(`	mov	56(%rsp), %r8d	')

	cmp	$2, un_param
	ja	L(gen)
	mov	(vp_param), %rdx
	mulx(	(up), %rax, %r9)	C 0 1
	je	L(s2x)

L(s11):	mov	%rax, (rp)
	mov	%r9, 8(rp)
	FUNC_EXIT()
	ret

L(s2x):	cmp	$2, vn_param
	mulx(	8,(up), %r8, %r10)	C 1 2
	je	L(s22)

L(s21):	add	%r8, %r9
	adc	$0, %r10
	mov	%rax, (rp)
	mov	%r9, 8(rp)
	mov	%r10, 16(rp)
	FUNC_EXIT()
	ret

L(s22):	add	%r8, %r9		C 1
	adc	$0, %r10		C 2
	mov	8(vp_param), %rdx
	mov	%rax, (rp)
	mulx(	(up), %r8, %r11)	C 1 2
	mulx(	8,(up), %rax, %rdx)	C 2 3
	add	%r11, %rax		C 2
	adc	$0, %rdx		C 3
	add	%r8, %r9		C 1
	adc	%rax, %r10		C 2
	adc	$0, %rdx		C 3
	mov	%r9, 8(rp)
	mov	%r10, 16(rp)
	mov	%rdx, 24(rp)
	FUNC_EXIT()
	ret


L(gen):	push	%r15
	push	%r14
	push	%r13
	push	%r12
	push	%rbp
	push	%rbx

	mov	un_param, un
	mov	vp_param, vp
	mov	vn_param, vn

	mov	(up), %r9
	mov	(vp), v0

	lea	(up,un,8), up
	lea	-32(rp,un,8), rp

	neg	un
	mov	un, n
	test	$1, R32(un)
	jz	L(mx0)
L(mx1):	test	$2, R32(un)
	jz	L(mb3)

L(mb1):	mulx(	%r9, %rbx, %rax)
	inc	n
	.byte	0xc4,0x22,0xb3,0xf6,0x44,0xf6,0x08	C mulx 8(up,un,8), %r9, %r8
	.byte	0xc4,0x22,0xa3,0xf6,0x54,0xf6,0x10	C mulx 16(up,un,8), %r11, %r10
	jmp	L(mlo1)

L(mb3):	mulx(	%r9, %r11, %r10)
	.byte	0xc4,0x22,0x93,0xf6,0x64,0xf6,0x08	C mulx 8(up,un,8), %r13, %r12
	.byte	0xc4,0xa2,0xe3,0xf6,0x44,0xf6,0x10	C mulx 16(up,un,8), %rbx, %rax
	sub	$-3, n
	jz	L(mwd3)
	test	R32(%rdx), R32(%rdx)
	jmp	L(mlo3)

L(mx0):	test	$2, R32(un)
	jz	L(mb0)

L(mb2):	mulx(	%r9, %r13, %r12)
	.byte	0xc4,0xa2,0xe3,0xf6,0x44,0xf6,0x08	C mulx 8(up,un,8), %rbx, %rax
	lea	2(n), n
	.byte	0xc4,0x22,0xb3,0xf6,0x44,0xf6,0x10	C mulx 16(up,un,8), %r9, %r8
	jmp	L(mlo2)

L(mb0):	mulx(	%r9, %r9, %r8)
	.byte	0xc4,0x22,0xa3,0xf6,0x54,0xf6,0x08	C mulx 8(up,un,8), %r11, %r10
	.byte	0xc4,0x22,0x93,0xf6,0x64,0xf6,0x10	C mulx 16(up,un,8), %r13, %r12
	jmp	L(mlo0)

L(mtop):jrcxz	L(mend)
	adc	%r8, %r11
	mov	%r9, (rp,n,8)
L(mlo3):.byte	0xc4,0x62,0xb3,0xf6,0x04,0xce		C mulx (up,n,8), %r9, %r8
	adc	%r10, %r13
	mov	%r11, 8(rp,n,8)
L(mlo2):.byte	0xc4,0x62,0xa3,0xf6,0x54,0xce,0x08	C mulx 8(up,n,8), %r11, %r10
	adc	%r12, %rbx
	mov	%r13, 16(rp,n,8)
L(mlo1):.byte	0xc4,0x62,0x93,0xf6,0x64,0xce,0x10	C mulx 16(up,n,8), %r13, %r12
	adc	%rax, %r9
	mov	%rbx, 24(rp,n,8)
L(mlo0):.byte	0xc4,0xe2,0xe3,0xf6,0x44,0xce,0x18	C mulx 24(up,n,8), %rbx, %rax
	lea	4(n), n
	jmp	L(mtop)

L(mend):mov	%r9, (rp)
	adc	%r8, %r11
L(mwd3):mov	%r11, 8(rp)
	adc	%r10, %r13
	mov	%r13, 16(rp)
	adc	%r12, %rbx
	adc	$0, %rax
	mov	%rbx, 24(rp)
	mov	%rax, 32(rp)
	add	$8, vp
	dec	vn
	jz	L(end)

C The rest of the file are 4 loops around addmul_1

	test	$1, R32(un)
	jnz	L(0x1)

L(0x0):	test	$2, R32(un)
	jnz	L(oloop2)

L(oloop0):
	mov	(vp), %rdx
	mov	un, n
	add	$8, rp
	.byte	0xc4,0x22,0xb3,0xf6,0x04,0xf6		C mulx (up,un,8), %r9, %r8
	.byte	0xc4,0x22,0xa3,0xf6,0x54,0xf6,0x08	C mulx 8(up,un,8), %r11, %r10
	.byte	0xc4,0x22,0x93,0xf6,0x64,0xf6,0x10	C mulx 16(up,un,8), %r13, %r12
	.byte	0xc4,0xe2,0xe3,0xf6,0x44,0xce,0x18	C mulx 24(up,n,8), %rbx, %rax
	add	%r8, %r11
	jmp	L(lo0)

	ALIGN(16)
L(tp0):	add	%r9, (rp,n,8)
	.byte	0xc4,0x62,0xb3,0xf6,0x04,0xce		C mulx (up,n,8), %r9, %r8
	adc	%r11, 8(rp,n,8)
	.byte	0xc4,0x62,0xa3,0xf6,0x54,0xce,0x08	C mulx 8(up,n,8), %r11, %r10
	adc	%r13, 16(rp,n,8)
	.byte	0xc4,0x62,0x93,0xf6,0x64,0xce,0x10	C mulx 16(up,n,8), %r13, %r12
	adc	%rbx, 24(rp,n,8)
	adc	%rax, %r9
	.byte	0xc4,0xe2,0xe3,0xf6,0x44,0xce,0x18	C mulx 24(up,n,8), %rbx, %rax
	adc	%r8, %r11
L(lo0):	adc	%r10, %r13
	adc	%r12, %rbx
	adc	$0, %rax
	add	$4, n
	jnz	L(tp0)

	add	%r9, (rp)
	adc	%r11, 8(rp)
	adc	%r13, 16(rp)
	adc	%rbx, 24(rp)
	adc	$0, %rax
	mov	%rax, 32(rp)
	add	$8, vp
	dec	vn
	jne	L(oloop0)
	jmp	L(end)

L(oloop2):
	mov	(vp), %rdx
	lea	2(un), n
	add	$8, rp
	.byte	0xc4,0x22,0x93,0xf6,0x24,0xf6		C mulx (up,un,8), %r13, %r12
	.byte	0xc4,0xa2,0xe3,0xf6,0x44,0xf6,0x08	C mulx 8(up,un,8), %rbx, %rax
	add	%r12, %rbx
	adc	$0, %rax
	.byte	0xc4,0x22,0xb3,0xf6,0x44,0xf6,0x10	C mulx 16(up,un,8), %r9, %r8
	.byte	0xc4,0x62,0xa3,0xf6,0x54,0xce,0x08	C mulx 8(up,n,8), %r11, %r10
	add	%r13, 16(rp,n,8)
	jmp	L(lo2)

	ALIGN(16)
L(tp2):	add	%r9, (rp,n,8)
	.byte	0xc4,0x62,0xb3,0xf6,0x04,0xce		C mulx (up,n,8), %r9, %r8
	adc	%r11, 8(rp,n,8)
	.byte	0xc4,0x62,0xa3,0xf6,0x54,0xce,0x08	C mulx 8(up,n,8), %r11, %r10
	adc	%r13, 16(rp,n,8)
L(lo2):	.byte	0xc4,0x62,0x93,0xf6,0x64,0xce,0x10	C mulx 16(up,n,8), %r13, %r12
	adc	%rbx, 24(rp,n,8)
	adc	%rax, %r9
	.byte	0xc4,0xe2,0xe3,0xf6,0x44,0xce,0x18	C mulx 24(up,n,8), %rbx, %rax
	adc	%r8, %r11
	adc	%r10, %r13
	adc	%r12, %rbx
	adc	$0, %rax
	add	$4, n
	jnz	L(tp2)

	add	%r9, (rp)
	adc	%r11, 8(rp)
	adc	%r13, 16(rp)
	adc	%rbx, 24(rp)
	adc	$0, %rax
	mov	%rax, 32(rp)
	add	$8, vp
	dec	vn
	jne	L(oloop2)
	jmp	L(end)

L(0x1):	test	$2, R32(un)
	jz	L(oloop3)

L(oloop1):
	mov	(vp), %rdx
	lea	1(un), n
	add	$8, rp
	.byte	0xc4,0xa2,0xe3,0xf6,0x04,0xf6		C mulx (up,un,8), %rbx, %rax
	.byte	0xc4,0x22,0xb3,0xf6,0x44,0xf6,0x08	C mulx 8(up,un,8), %r9, %r8
	.byte	0xc4,0x22,0xa3,0xf6,0x54,0xf6,0x10	C mulx 16(up,un,8), %r11, %r10
	.byte	0xc4,0x62,0x93,0xf6,0x64,0xce,0x10	C mulx 16(up,n,8), %r13, %r12
	add	%rbx, 24(rp,n,8)
	jmp	L(lo1)

	ALIGN(16)
L(tp1):	add	%r9, (rp,n,8)
	.byte	0xc4,0x62,0xb3,0xf6,0x04,0xce		C mulx (up,n,8), %r9, %r8
	adc	%r11, 8(rp,n,8)
	.byte	0xc4,0x62,0xa3,0xf6,0x54,0xce,0x08	C mulx 8(up,n,8), %r11, %r10
	adc	%r13, 16(rp,n,8)
	.byte	0xc4,0x62,0x93,0xf6,0x64,0xce,0x10	C mulx 16(up,n,8), %r13, %r12
	adc	%rbx, 24(rp,n,8)
L(lo1):	adc	%rax, %r9
	.byte	0xc4,0xe2,0xe3,0xf6,0x44,0xce,0x18	C mulx 24(up,n,8), %rbx, %rax
	adc	%r8, %r11
	adc	%r10, %r13
	adc	%r12, %rbx
	adc	$0, %rax
	add	$4, n
	jnz	L(tp1)

	add	%r9, (rp)
	adc	%r11, 8(rp)
	adc	%r13, 16(rp)
	adc	%rbx, 24(rp)
	adc	$0, %rax
	mov	%rax, 32(rp)
	add	$8, vp
	dec	vn
	jne	L(oloop1)
	jmp	L(end)

L(oloop3):
	mov	(vp), %rdx
	lea	3(un), n
	add	$8, rp
	.byte	0xc4,0x22,0xa3,0xf6,0x14,0xf6		C mulx (up,un,8), %r11, %r10
	.byte	0xc4,0x22,0x93,0xf6,0x64,0xf6,0x08	C mulx 8(up,un,8), %r13, %r12
	.byte	0xc4,0xa2,0xe3,0xf6,0x44,0xf6,0x10	C mulx 16(up,un,8), %rbx, %rax
	add	%r10, %r13
	adc	%r12, %rbx
	adc	$0, %rax
	test	n, n
	jz	L(wd3)
	.byte	0xc4,0x62,0xb3,0xf6,0x04,0xce		C mulx (up,n,8), %r9, %r8
	add	%r11, 8(rp,n,8)
	jmp	L(lo3)

	ALIGN(16)
L(tp3):	add	%r9, (rp,n,8)
	.byte	0xc4,0x62,0xb3,0xf6,0x04,0xce		C mulx (up,n,8), %r9, %r8
	adc	%r11, 8(rp,n,8)
L(lo3):	.byte	0xc4,0x62,0xa3,0xf6,0x54,0xce,0x08	C mulx 8(up,n,8), %r11, %r10
	adc	%r13, 16(rp,n,8)
	.byte	0xc4,0x62,0x93,0xf6,0x64,0xce,0x10	C mulx 16(up,n,8), %r13, %r12
	adc	%rbx, 24(rp,n,8)
	adc	%rax, %r9
	.byte	0xc4,0xe2,0xe3,0xf6,0x44,0xce,0x18	C mulx 24(up,n,8), %rbx, %rax
	adc	%r8, %r11
	adc	%r10, %r13
	adc	%r12, %rbx
	adc	$0, %rax
	add	$4, n
	jnz	L(tp3)

	add	%r9, (rp)
L(wd3):	adc	%r11, 8(rp)
	adc	%r13, 16(rp)
	adc	%rbx, 24(rp)
	adc	$0, %rax
	mov	%rax, 32(rp)
	add	$8, vp
	dec	vn
	jne	L(oloop3)

L(end):	pop	%rbx
	pop	%rbp
	pop	%r12
	pop	%r13
	pop	%r14
	pop	%r15
	FUNC_EXIT()
	ret
EPILOGUE()
