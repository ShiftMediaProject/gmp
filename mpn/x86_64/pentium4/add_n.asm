dnl  x86-64/P4 mpn_add_n -- Add two limb vectors of the same length > 0 and
dnl  store sum in a third limb vector.

dnl  Copyright 2005 Free Software Foundation, Inc.

dnl  This file is part of the GNU MP Library.

dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU Lesser General Public License as published
dnl  by the Free Software Foundation; either version 2.1 of the License, or (at
dnl  your option) any later version.

dnl  The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
dnl  License for more details.

dnl  You should have received a copy of the GNU Lesser General Public License
dnl  along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
dnl  the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
dnl  MA 02111-1307, USA.

include(`../config.m4')

C TODO
C  Try to make this as fast as the mpn_sub_n code.

C		    cycles/limb
C Hammer:		4.0
C Prescott/Nocona:	5.37


C INPUT PARAMETERS
define(`rp',	`%rdi')
define(`up',	`%rsi')
define(`vp',	`%rdx')
define(`n',	`%rcx')

	TEXT
	ALIGN(16)
ASM_START()
PROLOGUE(mpn_add_n)
	push	%rbx
	xor	%ebx, %ebx
	xor	%eax, %eax
	bt	$0, %ecx		C n
	jnc	.L1

	mov	(up), %r11
	add	$8, up
	mov	(vp), %r10
	add	$8, vp
	add	%r10, %r11
	setc	%al			C primary carry
	mov	%r11, (rp)
	add	$8, rp

.L1:	sub	$2, n
	jc	.Lend

.Loop:	mov	(up), %r9
	mov	(vp), %r8
	add	$16, up			C up += 2
	add	%r8, %r9
	setc	%bl
	cmp	$-1, %r9
	je	.Lc0			C secondary carry
.Lrc0:	add	%rax, %r9
	mov	%r9, (rp)

	mov	-8(up), %r11
	mov	8(vp), %r10
	add	$16, vp			C vp += 2
	add	%r10, %r11
	setc	%al			C primary carry
	cmp	$-1, %r11
	je	.Lc1			C secondary carry
.Lrc1:	add	%rbx, %r11
	mov	%r11, 8(rp)

	add	$16, rp			C rp += 2
	sub	$2, n
	jnc	.Loop

.Lend:	pop	%rbx
	ret

.Lc0:	mov	%eax, %ebx
	jmp	.Lrc0
.Lc1:	mov	%ebx, %eax
	jmp	.Lrc1
EPILOGUE()
