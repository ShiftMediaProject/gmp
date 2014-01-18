dnl  HPPA 64-bit time stamp counter access routine.

dnl  Copyright 2000, 2002, 2005 Free Software Foundation, Inc.

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

dnl void speed_cyclecounter (unsigned p[2]);
dnl
dnl Get the HPPA interval timer.

	.level 2.0
PROLOGUE(speed_cyclecounter)
	mfctl	%cr16,%r28
	stw	%r28,0(0,%r26)		; low word
	extrd,u	%r28,31,32,%r28
	bve	(%r2)
	stw	%r28,4(0,%r26)		; high word
EPILOGUE(speed_cyclecounter)
