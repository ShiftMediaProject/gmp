dnl  Intel Pentium mpn_addmul_1 -- mpn by limb multiplication.
dnl 
dnl  P5: 14.0 cycles/limb


dnl  Copyright (C) 1992, 1994, 1996, 1999, 2000 Free Software Foundation,
dnl  Inc.
dnl 
dnl  This file is part of the GNU MP Library.
dnl 
dnl  The GNU MP Library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Library General Public License as
dnl  published by the Free Software Foundation; either version 2 of the
dnl  License, or (at your option) any later version.
dnl 
dnl  The GNU MP Library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Library General Public License for more details.
dnl 
dnl  You should have received a copy of the GNU Library General Public
dnl  License along with the GNU MP Library; see the file COPYING.LIB.  If
dnl  not, write to the Free Software Foundation, Inc., 59 Temple Place -
dnl  Suite 330, Boston, MA 02111-1307, USA. */


include(`../config.m4')


ifdef(`OPERATION_addmul_1', `
      define(M4_inst,        addl)
      define(M4_function_1,  mpn_addmul_1)

',`ifdef(`OPERATION_submul_1', `
      define(M4_inst,        subl)
      define(M4_function_1,  mpn_submul_1)

',`m4_error(`Need OPERATION_addmul_1 or OPERATION_submul_1
')')')


C mp_limb_t M4_function_1 (mp_ptr dst, mp_srcptr src, mp_size_t size,
C                          mp_limb_t mult);

define(PARAM_MULTIPLIER, `FRAME+16(%esp)')
define(PARAM_SIZE,       `FRAME+12(%esp)')
define(PARAM_SRC,        `FRAME+8(%esp)')
define(PARAM_DST,        `FRAME+4(%esp)')

	.text
	ALIGN(8)

PROLOGUE(M4_function_1)

	pushl	%edi
	pushl	%esi
	pushl	%ebx
	pushl	%ebp
deflit(`FRAME',16)

	movl	PARAM_DST, %edi
	movl	PARAM_SRC, %esi
	movl	PARAM_SIZE, %ecx
	movl	PARAM_MULTIPLIER, %ebp

	leal	(%edi,%ecx,4), %edi
	leal	(%esi,%ecx,4), %esi
	negl	%ecx
	xorl	%ebx, %ebx
	ALIGN(8)

L(oop):	adcl	$0, %ebx
	movl	(%esi,%ecx,4), %eax

	mull	%ebp

	addl	%ebx, %eax
	movl	(%edi,%ecx,4), %ebx

	adcl	$0, %edx
	M4_inst	%eax, %ebx

	movl	%ebx, (%edi,%ecx,4)
	incl	%ecx

	movl	%edx, %ebx
	jnz	L(oop)

	adcl	$0, %ebx
	movl	%ebx, %eax
	popl	%ebp
	popl	%ebx
	popl	%esi
	popl	%edi
	ret

EPILOGUE()
