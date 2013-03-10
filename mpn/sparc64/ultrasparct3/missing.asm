dnl  SPARC v9-2011 simulation support.

dnl  Contributed to the GNU project by Torbjörn Granlund.

dnl  Copyright 2013 Free Software Foundation, Inc.

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

ASM_START()
PROLOGUE(__gmpn_umulh)
	save	%sp, -176, %sp
	ldx	[%sp+2047+176+256], %o0
	ldx	[%sp+2047+176+256+8], %o1
	rd	%ccr, %o4
	srl	%o0, 0, %l4
	srl	%o1, 0, %l1
	srlx	%o1, 32, %o1
	mulx	%o1, %l4, %l2
	srlx	%o0, 32, %o0
	mulx	%o0, %l1, %l3
	mulx	%l1, %l4, %l1
	srlx	%l1, 32, %l1
	add	%l2, %l1, %l2
	addcc	%l2, %l3, %l2
	mulx	%o1, %o0, %o1
	mov	0, %l1
	movcs	%xcc, 1, %l1
	sllx	%l1, 32, %l1
	add	%o1, %l1, %o1
	srlx	%l2, 32, %o0
	add	%o1, %o0, %o0
	stx	%o0, [%sp+2047+176+256]
	wr	%o4, 0, %ccr
	ret
	 restore
EPILOGUE()
