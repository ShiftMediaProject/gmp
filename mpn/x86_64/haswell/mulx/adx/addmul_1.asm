dnl  AMD64 mpn_mul_1 for Intel Broadwell.

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
C AMD K8,K9
C AMD K10
C AMD bd1
C AMD bobcat
C Intel P4
C Intel PNR
C Intel NHM
C Intel SBR
C Intel HWL
C Intel BWL
C Intel atom
C VIA nano

define(`rp',      `%rdi')   C rcx
define(`up',      `%rsi')   C rdx
define(`n_param', `%rdx')   C r8
define(`v0_param',`%rcx')   C r9

define(`n',       `%rcx')
define(`v0',      `%rdx')

IFDOS(`	define(`up', ``%rsi'')	') dnl
IFDOS(`	define(`rp', ``%rcx'')	') dnl
IFDOS(`	define(`v0', ``%r9'')	') dnl
IFDOS(`	define(`r9', ``rdi'')	') dnl
IFDOS(`	define(`n',  ``%r8'')	') dnl
IFDOS(`	define(`r8', ``r11'')	') dnl

ASM_START()
	TEXT
	ALIGN(16)
PROLOGUE(mpn_addmul_1)
	xor	%rax, %rax
L(ent):	mov	v0_param, %r8
	mov	n_param, n
	mov	%r8, v0
	lea	(rp,n,8), rp
	lea	(up,n,8), up
	neg	n
	test	$1, R8(n)
	jne	L(odd)

L(top):	jrcxz	L(end)
	mulx	(up,n,8), %r9, %r8
	adcx	%rax, %r9
	adox	(rp,n,8), %r9
	mov	%r9, (rp,n,8)
L(mid):	mulx	8(up,n,8), %r11, %rax
	adcx	%r8, %r11
	adox	8(rp,n,8), %r11
	mov	%r11, 8(rp,n,8)
	lea	2(n), n
	jmp	L(top)

L(end):	adcx	%rcx, %rax
	adox	%rcx, %rax
	ret

L(odd):	dec	n
	mov	%rax, %r8
	jmp	L(mid)
EPILOGUE()
ASM_END()
