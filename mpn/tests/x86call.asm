dnl  x86 calling conventions checking.

dnl  Copyright (C) 2000 Free Software Foundation, Inc.
dnl 
dnl  This file is part of the GNU MP Library.
dnl 
dnl  The GNU MP Library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Lesser General Public License as
dnl  published by the Free Software Foundation; either version 2.1 of the
dnl  License, or (at your option) any later version.
dnl 
dnl  The GNU MP Library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Lesser General Public License for more details.
dnl 
dnl  You should have received a copy of the GNU Lesser General Public
dnl  License along with the GNU MP Library; see the file COPYING.LIB.  If
dnl  not, write to the Free Software Foundation, Inc., 59 Temple Place -
dnl  Suite 330, Boston, MA 02111-1307, USA.


include(`../config.m4')


C int calling_conventions (...);
C
C The global variable "calling_conventions_function" is the function to
C call, with the arguments as passed here.
C
C Perhaps the finit should be done only if the tags word isn't clear, but
C nothing uses the rounding mode or anything at the moment.

define(G,
m4_assert_numargs(1)
`GSYM_PREFIX`'$1')

	.text
	ALIGN(8)
PROLOGUE(calling_conventions)
	movl	(%esp), %eax
	movl	%eax, G(calling_conventions_retaddr)

	movl	$L(return), (%esp)

	movl	%ebx, G(calling_conventions_save_ebx)
	movl	%esi, G(calling_conventions_save_esi)
	movl	%edi, G(calling_conventions_save_edi)
	movl	%ebp, G(calling_conventions_save_ebp)

	movl	$0x01234567, %ebx
	movl	$0x89ABCDEF, %esi
	movl	$0xFEDCBA98, %edi
	movl	$0x76543210, %ebp

	C try to provoke a problem by starting with junk in the registers,
	C especially in %eax and %edx which will be return values
	movl	$0x70246135, %eax
	movl	$0x8ACE9BDF, %ecx
	movl	$0xFDB97531, %edx

	jmp	*G(calling_conventions_function)

L(return):
	movl	%ebx, G(calling_conventions_ebx)
	movl	%esi, G(calling_conventions_esi)
	movl	%edi, G(calling_conventions_edi)
	movl	%ebp, G(calling_conventions_ebp)

	pushf
	popl	%ebx
	movl	%ebx, G(calling_conventions_eflags)

	fstenv	G(calling_conventions_fenv)
	finit

	movl	G(calling_conventions_save_ebx), %ebx
	movl	G(calling_conventions_save_esi), %esi
	movl	G(calling_conventions_save_edi), %edi
	movl	G(calling_conventions_save_ebp), %ebp

	jmp	*G(calling_conventions_retaddr)

EPILOGUE()

