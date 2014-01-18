dnl  Sparc v9 32-bit time stamp counter access routine.

dnl  Copyright 2000, 2005 Free Software Foundation, Inc.

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


C void speed_cyclecounter (unsigned p[2]);
C
C Get the sparc v9 tick counter.

ASM_START()
PROLOGUE(speed_cyclecounter)
	rd	%tick,%g1
	st	%g1,[%o0]		C low 32 bits
	srlx	%g1,32,%g4
	retl
	st	%g4,[%o0+4]		C high 32 bits
EPILOGUE(speed_cyclecounter)
