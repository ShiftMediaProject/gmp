dnl  AMD64 mpn_addmul_1 for CPUs with mulx and adx.

dnl  Contributed to the GNU project by Torbjörn Granlund.

dnl  Copyright 2012, 2013 Free Software Foundation, Inc.

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
C AMD K8,K9	 -
C AMD K10	 -
C AMD bd1	 -
C AMD bobcat	 -
C Intel P4	 -
C Intel PNR	 -
C Intel NHM	 -
C Intel SBR	 -
C Intel HWL	 -
C Intel BWL	 ?
C Intel atom	 -
C VIA nano	 -

define(`rp',      `%rdi')   C rcx
define(`up',      `%rsi')   C rdx
define(`n_param', `%rdx')   C r8
define(`v0_param',`%rcx')   C r9

define(`n',       `%rcx')
define(`v0',      `%rdx')

IFDOS(`	define(`up',       ``%rsi'')	') dnl
IFDOS(`	define(`rp',       ``%rcx'')	') dnl
IFDOS(`	define(`v0_param', ``%r9'')	') dnl
IFDOS(`	define(`r9',       ``rdi'')	') dnl
IFDOS(`	define(`n_param',  ``%r8'')	') dnl
IFDOS(`	define(`r8',       ``r11'')	') dnl

C Testing mechanism for running this on older AMD64 processrs
ifdef(`FAKE_MULXADX',`
  include(`missing-call.m4')
',`
  define(`adox',	``adox'	$1, $2')
  define(`adcx',	``adcx'	$1, $2')
  define(`mulx',	``mulx'	$1, $2, $3')
')

ASM_START()
	TEXT
	ALIGN(16)
PROLOGUE(mpn_addmul_1)
	mov	(up), %r8

	push	%rbx
	push	%r12
	push	%r13

	lea	(up,n_param,8), up
	lea	-16(rp,n_param,8), rp
	mov	R32(n_param), R32(%rax)
	xchg	v0_param, v0		C FIXME: is this insn fast?

	neg	n

	and	$3, R8(%rax)
	jz	L(b0)
	cmp	$2, R8(%rax)
	jl	L(b1)
	jz	L(b2)

L(b3):	xor	%r8, %r8
	mulx(	(up,n,8), %r11, %r10)
	mulx(	8(up,n,8), %r13, %r12)
	mulx(	16(up,n,8), %rbx, %rax)
	add	%r8, %r11
	dec	n
	jmp	L(lo3)

L(b0):	xor	%rax, %rax
	mulx(	(up,n,8), %r9, %r8)
	mulx(	8(up,n,8), %r11, %r10)
	mulx(	16(up,n,8), %r13, %r12)
	adcx(	%rax, %r9)
	jmp	L(lo0)

L(b2):	xor	%r10, %r10
	mulx(	(up,n,8), %r13, %r12)
	mulx(	8(up,n,8), %rbx, %rax)
	lea	2(n), n
	jrcxz	L(wd2)
L(gt2):	mulx(	(up,n,8), %r9, %r8)
	adcx(	%r10, %r13)
	jmp	L(lo2)

L(b1):	xor	%r12, %r12
	mulx(	(up,n,8), %rbx, %rax)
	lea	1(n), n
	jrcxz	L(wd1)
	mulx(	(up,n,8), %r9, %r8)
	mulx(	8(up,n,8), %r11, %r10)
	adcx(	%r12, %rbx)
	jmp	L(lo1)

L(end):	adcx(	%r10, %r13)
	mov	%r11, -8(rp)
L(wd2):	adox(	(rp), %r13)
	adcx(	%r12, %rbx)
	mov	%r13, (rp)
L(wd1):	adox(	8(rp), %rbx)
	adcx(	%rcx, %rax)
	adox(	%rcx, %rax)
	mov	%rbx, 8(rp)
	pop	%r13
	pop	%r12
	pop	%rbx
	ret

L(top):	jrcxz	L(end)
	mulx(	(up,n,8), %r9, %r8)
	adcx(	%r10, %r13)
	mov	%r11, -8(rp,n,8)
L(lo2):	adox(	(rp,n,8), %r13)
	mulx(	8(up,n,8), %r11, %r10)
	adcx(	%r12, %rbx)
	mov	%r13, (rp,n,8)
L(lo1):	adox(	8(rp,n,8), %rbx)
	mulx(	16(up,n,8), %r13, %r12)
	adcx(	%rax, %r9)
	mov	%rbx, 8(rp,n,8)
L(lo0):	adox(	16(rp,n,8), %r9)
	mulx(	24(up,n,8), %rbx, %rax)
	adcx(	%r8, %r11)
	mov	%r9, 16(rp,n,8)
L(lo3):	adox(	24(rp,n,8), %r11)
	lea	4(n), n
	jmp	L(top)
EPILOGUE()
ASM_END()
