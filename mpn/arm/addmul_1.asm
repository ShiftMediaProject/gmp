dnl  ARM mpn_addmul_1 -- Multiply a limb vector with a limb and add the result
dnl  to a second limb vector.
dnl  Contributed by Robert Harley.

dnl  Copyright 1998, 2000, 2001 Free Software Foundation, Inc.

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

C This runs at 10 cycles/limb in the StrongARM.  Should make this look more
C like the submul code, since that uses fewer registers and runs slightly
C faster.

define(`rp',`r0')
define(`up',`r1')
define(`n',`r2')
define(`v',`r3')


ASM_START()
PROLOGUE(mpn_addmul_1)
	stmfd	sp!, { r8-r11, lr }
	mov	r11, #0
	mov	ip, #0
	movs	n, n, lsr #1
	bcc	L(skip1)
	ldr	lr, [up], #4
	ldr	r9, [rp]
	umlal	r9, ip, v, lr
	str	r9, [rp], #4
L(skip1):
	movs	n, n, lsr #1
	bcc	L(skip2)
	ldmia	rp, { r9, r10 }
	adds	r8, ip, r9
	adc	r9, r11, #0
	ldmia	up!, { ip, lr }
	umlal	r8, r9, v, ip
	adds	r9, r9, r10
	adc	ip, r11, #0
	umlal	r9, ip, v, lr
	stmia	rp!, { r8, r9 }
L(skip2):
	teq	n, #0
	beq	L(return)
	stmfd	sp!, { r4-r7 }
L(addmul_loop):
	ldmia	rp, { r5, r6, r7, r8 }
	adds	r4, ip, r5
	adc	r5, r11, #0
	ldmia	up!, { r9, r10, ip, lr }
	umlal	r4, r5, v, r9
	adds	r5, r5, r6
	adc	r6, r11, #0
	umlal	r5, r6, v, r10
	adds	r6, r6, r7
	adc	r7, r11, #0
	umlal	r6, r7, v, ip
	adds	r7, r7, r8
	adc	ip, r11, #0
	umlal	r7, ip, v, lr
	subs	n, n, #1
	stmia	rp!, { r4, r5, r6, r7 }
	bne	L(addmul_loop)
	ldmfd	sp!, { r4-r7 }
L(return):
	mov	r0, ip
	ldmfd	sp!, { r8-r11, pc }
EPILOGUE(mpn_addmul_1)
