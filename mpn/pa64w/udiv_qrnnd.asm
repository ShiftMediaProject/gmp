dnl  HP-PA 2.0W 64-bit mpn_udiv_qrnnd.

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

include(`../config.m4')

C This runs at about 430 cycles, meaning each bits takes 6 cycles to develop.

C INPUT PARAMETERS
define(`n1',`%r26')
define(`n0',`%r25')
define(`d',`%r24')
define(`remptr',`%r23')

define(`divstep',
       `add,dc		$2,$2,$2
	add,dc		$1,$1,$1
	sub,*<<		$1,$3,%r28
	copy		%r28,$1')

	.level	2.0W
PROLOGUE(mpn_udiv_qrnnd)
	.proc
	.entry
	.callinfo	frame=0,no_calls,save_rp,entry_gr=7

	cmpib,*>=	0,d,large_divisor
	addi		8,%r0,%r31	C setup loop counter and clear carry

Loop	divstep(n1,n0,d)
	divstep(n1,n0,d)
	divstep(n1,n0,d)
	divstep(n1,n0,d)
	divstep(n1,n0,d)
	divstep(n1,n0,d)
	divstep(n1,n0,d)
	divstep(n1,n0,d)
	addib,<>	-1,%r31,Loop
	nop

	std		n1,0(remptr)	C store remainder
	bve		(%r2)
	add,dc		n0,n0,%r28	C quotient: add last carry from divstep

large_divisor
	extrd,u		n0,63,1,%r19	C save lsb of dividend
	shrpd		n1,n0,1,n0	C n0 = lo(n1n0 >> 1)
	shrpd		%r0,n1,1,n1	C n1 = hi(n1n0 >> 1)
	extrd,u		d,63,1,%r20	C save lsb of divisor
	shrpd		%r0,d,1,d	C d = floor(orig_d / 2)
	add,l		%r20,d,d	C d = ceil(orig_d / 2)

Loop2	divstep(n1,n0,d)
	divstep(n1,n0,d)
	divstep(n1,n0,d)
	divstep(n1,n0,d)
	divstep(n1,n0,d)
	divstep(n1,n0,d)
	divstep(n1,n0,d)
	divstep(n1,n0,d)
	addib,<>	-1,%r31,Loop2
	nop

	add,dc		n0,n0,n0	C shift and add last carry from divstep
	cmpib,*=	0,%r20,even_divisor
	shladd		n1,1,%r19,n1	C shift in omitted dividend lsb

	add		d,d,d		C restore orig...
	sub		d,%r20,d	C ...d value
	sub		%r0,d,%r21	C r21 = -d

	add,*nuv	n1,n0,n1	C fix remainder for omitted divisor lsb
	add,l		n1,%r21,n1	C adjust quotient if rem. fix carried
	add,dc		%r0,n0,n0	C adjust remainder accordingly

	sub,*<<		n1,d,%r0	C remainder >= divisor?
	add,l		n1,%r21,n1	C adjust remainder
	add,dc		%r0,n0,n0	C adjust quotient

even_divisor
	std		n1,0(remptr)	C store remainder
	bve		(%r2)
	copy		n0,%r28		C quotient

	.procend
EPILOGUE(mpn_udiv_qrnnd)
