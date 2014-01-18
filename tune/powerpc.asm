dnl  PowerPC mftb_function -- read time base registers.

dnl  Copyright 2002 Free Software Foundation, Inc.

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
dnl  along with the GNU MP Library.  If not, see https://www.gnu.org/licenses/.

include(`../config.m4')


C void mftb_function (unsigned a[2]);
C

ASM_START()
PROLOGUE(mftb_function)

	C r3	a

L(again):
	mftbu	r4
	mftb	r5
	mftbu	r6
	cmpw	cr0, r4, r6
	bne	L(again)

	stw	r5, 0(r3)
	stw	r4, 4(r3)
	blr

EPILOGUE()
