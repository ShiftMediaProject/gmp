dnl  MIPS3 mpn_sqr_diagonal.

dnl  Copyright 2001 Free Software Foundation, Inc.

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


dnl  INPUT PARAMETERS
dnl  rp		$4
dnl  up		$5
dnl  n		$6

include(`../config.m4')

ASM_START()
PROLOGUE(mpn_sqr_diagonal)
	ld	$8,0($5)
	daddiu	$6,$6,-2
	dmultu	$8,$8
	bltz	$6,$Lend1
	nop
	ld	$8,8($5)
	beq	$6,$0,$Lend2
	nop

$Loop:	mflo	$10
	mfhi	$9
	daddiu	$6,$6,-1
	sd	$10,0($4)
	sd	$9,8($4)
	dmultu	$8,$8
	ld	$8,16($5)
	daddiu	$5,$5,8
	bne	$6,$0,$Loop
	daddiu	$4,$4,16

$Lend2: mflo	$10
	mfhi	$9
	sd	$10,0($4)
	sd	$9,8($4)
	dmultu	$8,$8
	mflo	$10
	mfhi	$9
	sd	$10,16($4)
	j	$31
	sd	$9,24($4)

$Lend1: mflo	$10
	mfhi	$9
	sd	$10,0($4)
	j	$31
	sd	$9,8($4)
EPILOGUE(mpn_sqr_diagonal)
