dnl  PowerPC-64 mpn_addlsh1_n -- rp[] = up[] + (vp[] << 1)

dnl  Copyright 2003, 2005, 2009, 2010 Free Software Foundation, Inc.

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

C		cycles/limb
C POWER3/PPC630:     1.75	(1.5 c/l should be possible)
C POWER4/PPC970:     2		(2.0 c/l should be possible)
C POWER5:	     ?

C INPUT PARAMETERS
C rp	r3
C up	r4
C vp	r5
C n	r6

C STATUS
C  * Works for all sizes.  Needs optimization and cleanup of feed-in code.
C  * Combine u0 and u1.
C  * Try combining upx+up, and vpx+vp.

define(`rp',`r3')
define(`up',`r4')
define(`vp',`r5')

define(`rpx', `r6')
define(`upx', `r7')

define(`s0',`r0')  define(`s1',`r9')
define(`u0',`r8')
define(`v0',`r10') define(`v1',`r11')
define(`vpx',`r12')

ifdef(`OPERATION_addlsh1_n',`
  define(ADDSUBC,	addc)
  define(ADDSUBE,      	adde)
  define(INITCY,      	`addic $1, r1, 0')
  define(RETVAL,      	`addze  r3, $1')
  define(func, mpn_addlsh1_n)
')
ifdef(`OPERATION_sublsh1_n',`
  define(ADDSUBC,	subfc)
  define(ADDSUBE,      	subfe)
  define(INITCY,      	`addic $1, r1, -1')
  define(RETVAL,      	`subfze  r3, $1
			neg	r3, r3')
  define(func, mpn_sublsh1_n)
')


MULFUNC_PROLOGUE(mpn_addlsh1_n mpn_sublsh1_n)

ASM_START()
PROLOGUE(func)
	cmpli	cr0, r6, 13
	bgt	L(big)

	mtctr	r6		C copy n in ctr
	INITCY(	r0)		C clear cy

	ld	v0, 0(vp)	C load v limb
	ld	u0, 0(up)	C load u limb
	addi	up, up, -8	C update up
	addi	rp, rp, -8	C update rp
	sldi	s1, v0, 1
	bdz	L(ex1)		C If done, skip loop

	ALIGN(16)
L(lo0):	ld	v1, 8(vp)	C load v limb
	ADDSUBE	s1, s1, u0	C add limbs with cy, set cy
	ldu	u0, 16(up)	C load u limb and update up
	srdi	s0, v0, 63	C shift down previous v limb
	std	s1, 8(rp)	C store result limb
	rldimi	s0, v1, 1, 0	C left shift v limb and merge with prev v limb
	bdz	L(ex0)		C decrement ctr and exit if done
	ldu	v0, 16(vp)	C load v limb and update vp
	ADDSUBE	s0, s0, u0	C add limbs with cy, set cy
	ld	u0, 8(up)	C load u limb
	srdi	s1, v1, 63	C shift down previous v limb
	stdu	s0, 16(rp)	C store result limb and update rp
	rldimi	s1, v0, 1, 0	C left shift v limb and merge with prev v limb
	bdnz	L(lo0)		C decrement ctr and loop back

L(ex1):	ADDSUBE	r7, s1, u0
	std	r7, 8(rp)	C store last result limb
	srdi	r0, v0, 63
	RETVAL(	r0)
	blr
L(ex0):	ADDSUBE	r7, s0, u0
	std	r7, 16(rp)	C store last result limb
	srdi	r0, v1, 63
	RETVAL(	r0)
	blr


L(big):	rldicl.	r0, r6, 0,63	C r0 = n & 1, set cr0
	addi	r6, r6, -1	C ...for ctr
	srdi	r6, r6, 1	C ...for ctr
	mtctr	r6		C copy count into ctr
	beq	cr0, L(b0)

L(b1):	ld	v1, 0(vp)
	ld	u0, 0(up)
	sldi	s1, v1, 1
	srdi	s0, v1, 63
	ld	v0, 8(vp)
	ADDSUBC	s1, s1, u0	C add limbs without cy, set cy
	addi	rpx, rp, -16
	addi	rp, rp, -8
	sub	upx, up, rp
	sub	vpx, vp, rp
	sub	up, up, rpx
	sub	vp, vp, rpx
	addi	up, up, 8
	addi	upx, upx, 16
	addi	vp, vp, 16
	addi	vpx, vpx, 24
	b	L(mid)

L(b0):	ld	v0, 0(vp)
	ld	u0, 0(up)
	sldi	s0, v0, 1
	srdi	s1, v0, 63
	ld	v1, 8(vp)
	ADDSUBC	s0, s0, u0	C add limbs without cy, set cy
	addi	rpx, rp, -8
	addi	rp, rp, -16
	sub	upx, up, rpx
	sub	vpx, vp, rpx
	sub	up, up, rp
	sub	vp, vp, rp
	addi	up, up, 8
	addi	upx, upx, 16
	addi	vp, vp, 16
	addi	vpx, vpx, 24

	ALIGN(16)
L(top):	ldx	u0, up, rp
	ldx	v0, vp, rp
	rldimi	s1, v1, 1, 0
	stdu	s0, 16(rp)
	srdi	s0, v1, 63
	ADDSUBE	s1, s1, u0	C add limbs with cy, set cy
L(mid):	ldx	u0, upx, rpx
	ldx	v1, vpx, rpx
	rldimi	s0, v0, 1, 0
	stdu	s1, 16(rpx)
	srdi	s1, v0, 63
	ADDSUBE	s0, s0, u0	C add limbs with cy, set cy
	bdnz	L(top)		C decrement CTR and loop back

	ldx	u0, up, rp
	rldimi	s1, v1, 1, 0
	std	s0, 16(rp)
	srdi	s0, v1, 63
	ADDSUBE	s1, s1, u0	C add limbs with cy, set cy
	std	s1, 24(rp)

	RETVAL(	r0)
	blr
EPILOGUE()
