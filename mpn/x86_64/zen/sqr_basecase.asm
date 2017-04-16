dnl  AMD64 mpn_sqr_basecase optimised for AMD Zen.

dnl  Copyright 2012, 2013, 2015, 2017 Free Software Foundation, Inc.

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
C  * Replace sqr_diag_addlsh1 code with zen optimised code.
C  * Do overlapped software pipelining.
C  * Re-allocate to benefit more from 32-bit encoding (register rbp is free).
C  * Polish.

define(`rp',      `%rdi')
define(`up',      `%rsi')
define(`un_param',`%rdx')

define(`un',      `%r14')
define(`n',       `%rcx')

C these are used just for the small op code 
define(`w0',	`%r8')
define(`w1',	`%r9')
define(`w2',	`%r10')
define(`w3',	`%r11')


ABI_SUPPORT(DOS64)
ABI_SUPPORT(STD64)

ASM_START()
	TEXT
	ALIGN(16)
PROLOGUE(mpn_sqr_basecase)
	FUNC_ENTRY(3)

	cmp	$2, un_param
	jae	L(gt1)

	mov	(up), %rdx
	mulx(	%rdx, %rax, %rdx)
	mov	%rax, (rp)
	mov	%rdx, 8(rp)
	FUNC_EXIT()
	ret

L(gt1):	jne	L(gt2)

	mov	(up), %rdx
	mov	8(up), %rcx
	mulx(	%rcx, %r9, %r10)	C v0 * v1	W 1 2
	mulx(	%rdx, %rax, %r8)	C v0 * v0	W 0 1
	mov	%rcx, %rdx
	mulx(	%rdx, %r11, %rdx)	C v1 * v1	W 2 3
	add	%r9, %r9		C		W 1
	adc	%r10, %r10		C		W 2
	adc	$0, %rdx		C		W 3
	add	%r9, %r8		C W 1
	adc	%r11, %r10		C W 2
	adc	$0, %rdx		C W 3
	mov	%rax, (rp)
	mov	%r8, 8(rp)
	mov	%r10, 16(rp)
	mov	%rdx, 24(rp)
	FUNC_EXIT()
	ret

L(gt2):	cmp	$4, un_param
	jae	L(gt3)

	push	%rbx
	mov	(up), %rdx
	mulx(	8,(up), w2, w3)
	mulx(	16,(up), w0, w1)
	add	w3, w0
	mov	8(up), %rdx
	mulx(	16,(up), %rax, w3)
	adc	%rax, w1
	adc	$0, w3
	test	R32(%rbx), R32(%rbx)
	mov	(up), %rdx
	mulx(	%rdx, %rbx, %rcx)
	mov	%rbx, (rp)
	mov	8(up), %rdx
	mulx(	%rdx, %rax, %rbx)
	mov	16(up), %rdx
	mulx(	%rdx, %rsi, %rdx)
	adcx(	w2, w2)
	adcx(	w0, w0)
	adcx(	w1, w1)
	adcx(	w3, w3)
	adox(	w2, %rcx)
	adox(	w0, %rax)
	adox(	w1, %rbx)
	adox(	w3, %rsi)
	mov	$0, R32(%r8)
	adox(	%r8, %rdx)
	adcx(	%r8, %rdx)
	mov	%rcx, 8(rp)
	mov	%rax, 16(rp)
	mov	%rbx, 24(rp)
	mov	%rsi, 32(rp)
	mov	%rdx, 40(rp)
	pop	%rbx
	FUNC_EXIT()
	ret

L(gt3):
	push	%r15
	push	%r14
	push	%r13
	push	%r12
	push	%rbp
	push	%rbx
	mov	un_param, un

	mov	(up), %rdx		C up[0]
	mov	8(up), %r9		C up[1]

	lea	(up,un,8), up
	lea	-32(rp,un,8), rp

	neg	un
	lea	1(un), n

	bt	$0, R32(n)
	jnc	L(mx0)
L(mx1):	bt	$1, R32(n)
	jnc	L(mb3)

L(mb1):	mulx(	%r9, %rbx, %rax)
	add	$1, n					C clear cy as side-effect
	.byte	0xc4,0x62,0xb3,0xf6,0x04,0xce		C mulx (up,n,8), %r9, %r8
	.byte	0xc4,0x62,0xa3,0xf6,0x54,0xce,0x08	C mulx 8(up,n,8), %r11, %r10
	jmp	L(mlo1)

L(mb3):	mulx(	%r9, %r11, %r10)
	.byte	0xc4,0x62,0x93,0xf6,0x64,0xce,0x08	C mulx 8(up,n,8), %r13, %r12
	.byte	0xc4,0xe2,0xe3,0xf6,0x44,0xce,0x10	C mulx 16(up,n,8), %rbx, %rax
	sub	$-3, n					C clear cy as side-effect
	jz	L(mwd3)
	test	R32(%rdx), R32(%rdx)			C clear cy
	jmp	L(mlo3)

L(mx0):	bt	$1, R32(n)
	jnc	L(mb0)

L(mb2):	mulx(	%r9, %r13, %r12)
	.byte	0xc4,0xe2,0xe3,0xf6,0x44,0xce,0x08 	C mulx 8(up,n,8), %rbx, %rax
	add	$2, n					C clear cy as side-effect
	.byte	0xc4,0x62,0xb3,0xf6,0x04,0xce	  	C mulx (up,n,8), %r9, %r8
	jmp	L(mlo2)

L(mb0):	mulx(	%r9, %r9, %r8)
	.byte	0xc4,0x62,0xa3,0xf6,0x54,0xce,0x08	C mulx 8(up,n,8), %r11, %r10
	.byte	0xc4,0x62,0x93,0xf6,0x64,0xce,0x10	C mulx 16(up,n,8), %r13, %r12
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

	lea	2(un), %r15

L(outer):
	mov	-8(up,%r15,8), %rdx	C v0 = up[0]
	mov	%r15, n
	lea	8(rp), rp
	mov	(up,%r15,8), %r8	C v0 = up[1]

	bt	$0, R32(n)
	jnc	L(x0)
L(x1):	bt	$1, R32(n)
	jnc	L(b3)

L(b1):	mulx(	%r8, %rbx, %rax)
	add	$1, n					C clear cy as side-effect
	.byte	0xc4,0x62,0xb3,0xf6,0x04,0xce		C mulx (up,n,8), %r9, %r8
	.byte	0xc4,0x62,0xa3,0xf6,0x54,0xce,0x08	C mulx 8(up,n,8), %r11, %r10
	jmp	L(lo1)

L(b0):	mulx(	%r8, %r9, %r8)
	.byte	0xc4,0x62,0xa3,0xf6,0x54,0xce,0x08	C mulx 8(up,n,8), %r11, %r10
	.byte	0xc4,0x62,0x93,0xf6,0x64,0xce,0x10	C mulx 16(up,n,8), %r13, %r12
	xor	R32(%rax), R32(%rax)
	jmp	L(lo0)

L(b3):	mulx(	%r8, %r11, %r10)
	.byte	0xc4,0x62,0x93,0xf6,0x64,0xce,0x08	C mulx 8(up,n,8), %r13, %r12
	.byte	0xc4,0xe2,0xe3,0xf6,0x44,0xce,0x10	C mulx 16(up,n,8), %rbx, %rax
	add	%r10, %r13
	adc	%r12, %rbx
	adc	$0, %rax
	sub	$-3, n					C clear cy as side-effect
	jz	L(wd3)
	test	R32(%rdx), R32(%rdx)			C clear cy
	jmp	L(lo3)

L(x0):	bt	$1, R32(n)
	jnc	L(b0)

L(b2):	mulx(	%r8, %r13, %r12)
	.byte	0xc4,0xe2,0xe3,0xf6,0x44,0xce,0x08	C mulx 8(up,n,8), %rbx, %rax
	add	%r12, %rbx
	adc	$0, %rax
	lea	2(n), n
	jrcxz	L(xit2)
	.byte	0xc4,0x62,0xb3,0xf6,0x04,0xce		C mulx (up,n,8), %r9, %r8
	jmp	L(lo2)

L(top):	add	%r9, (rp,n,8)
L(lo3):	.byte	0xc4,0x62,0xb3,0xf6,0x04,0xce		C mulx (up,n,8), %r9, %r8
	adc	%r11, 8(rp,n,8)
L(lo2):	.byte	0xc4,0x62,0xa3,0xf6,0x54,0xce,0x08	C mulx 8(up,n,8), %r11, %r10
	adc	%r13, 16(rp,n,8)
L(lo1):	.byte	0xc4,0x62,0x93,0xf6,0x64,0xce,0x10	C mulx 16(up,n,8), %r13, %r12
	adc	%rbx, 24(rp,n,8)
	adc	%rax, %r9
L(lo0):	.byte	0xc4,0xe2,0xe3,0xf6,0x44,0xce,0x18	C mulx 24(up,n,8), %rbx, %rax
	adc	%r8, %r11
	adc	%r10, %r13
	adc	%r12, %rbx
	adc	$0, %rax		C rax = carry limb
	add	$4, n
	js	L(top)

	add	%r9, (rp)
L(wd3):	adc	%r11, 8(rp)
L(wd2):	adc	%r13, 16(rp)
L(wd1):	adc	%rbx, 24(rp)
	adc	$0, %rax
	mov	%rax, 32(rp)

	add	$1, %r15
	jmp	L(outer)

L(xit2):add	%r13, 16(rp)
	adc	%rbx, 24(rp)
	adc	$0, %rax
	mov	%rax, 32(rp)
	mov	-16(up), %rdx
	lea	8(rp), rp
	mov	-8(up), %r8
	mulx(	%r8, %rbx, %rax)
	add	%rbx, 24(rp)
	adc	$0, %rax
	mov	%rax, 32(rp)


C START mpn_sqr_diag_addlsh1
define(`I',`$1')

	mov	(up,un,8), %rax

	lea	2(,un,2), n
	lea	32(rp), rp

	mul	%rax
	mov	%rax, (rp,n,8)

	xor	R32(%rbx), R32(%rbx)
	jmp	L(mid)

	ALIGN(16)
L(dtop):add	%r10, %r8
	adc	%rax, %r9
	mov	%r8, -8(rp,n,8)
	mov	%r9, (rp,n,8)
L(mid):	mov	(up,n,4), %rax
	mov	8(rp,n,8), %r8
	mov	16(rp,n,8), %r9
	adc	%r8, %r8
	adc	%r9, %r9
	lea	(%rdx,%rbx), %r10
	setc	R8(%rbx)
	mul	%rax
	add	$2, n
	js	L(dtop)

	add	%r10, %r8
	adc	%rax, %r9
	mov	%r8, I(-8(rp),-8(rp,n,8))
	mov	%r9, I((rp),(rp,n,8))
	adc	%rbx, %rdx
	mov	%rdx, I(8(rp),8(rp,n,8))
C END mpn_sqr_diag_addlsh1

	pop	%rbx
	pop	%rbp
	pop	%r12
	pop	%r13
	pop	%r14
	pop	%r15

	FUNC_EXIT()
	ret
EPILOGUE()
