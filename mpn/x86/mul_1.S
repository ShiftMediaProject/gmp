/* x86 __gmpn_mul_1 (for 386, 486, and Pentium Pro) -- Multiply a limb vector
   with a limb and store the result in a second limb vector.

Copyright (C) 1992, 1994, 1997, 1998, 1999, 2000 Free Software Foundation,
Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

/*
   INPUT PARAMETERS
   res_ptr	(sp + 4)
   s1_ptr	(sp + 8)
   n		(sp + 12)
   s2_limb	(sp + 16)
*/

#include "asm-syntax.h"

#define res_ptr edi
#define s1_ptr esi
#define n ecx
#define s2_limb ebp

	TEXT
	ALIGN (3)
PROLOGUE(C_SYMBOL_NAME(__gmpn_mul_1))
	GLOBL	C_SYMBOL_NAME(__gmpn_mul_1)
C_SYMBOL_NAME(__gmpn_mul_1:)

	INSN1(push,l	,R(edi))
	INSN1(push,l	,R(esi))
	INSN1(push,l	,R(ebx))
	INSN1(push,l	,R(ebp))

	movl	20(%esp),%edi
	movl	24(%esp),%esi
	movl	28(%esp),%ecx

	xorl	%ebx,%ebx
	andl	$3,%ecx
	jz	Lend0

Loop0:	movl	(%esi),%eax
	mull	32(%esp)
	leal	4(%esi),%esi
	addl	%ebx,%eax
	movl	$0,%ebx
	adcl	%ebx,%edx
	movl	%eax,(%edi)
	movl	%edx,%ebx		/* propagate carry into cylimb */

	leal	4(%edi),%edi
	decl	%ecx
	jnz	Loop0

Lend0:	movl	28(%esp),%ecx
	shrl	$2,%ecx
	jz	Lend

/* This runs at 50 cycles on a P5 */
	ALIGN (3)
Loop:	movl	(%esi),%eax
	mull	32(%esp)
	addl	%eax,%ebx
	movl	$0,%ebp
	adcl	%edx,%ebp

	movl	4(%esi),%eax
	mull	32(%esp)
	movl	%ebx,(%edi)
	addl	%eax,%ebp	/* new lo + cylimb */
	movl	$0,%ebx
	adcl	%edx,%ebx

	movl	8(%esi),%eax
	mull	32(%esp)
	movl	%ebp,4(%edi)
	addl	%eax,%ebx	/* new lo + cylimb */
	movl	$0,%ebp
	adcl	%edx,%ebp

	movl	12(%esi),%eax
	mull	32(%esp)
	movl	%ebx,8(%edi)
	addl	%eax,%ebp	/* new lo + cylimb */
	movl	$0,%ebx
	adcl	%edx,%ebx

	movl	%ebp,12(%edi)

	leal	16(%esi),%esi
	leal	16(%edi),%edi
	decl	%ecx
	jnz	Loop

Lend:	movl	%ebx,%eax

	INSN1(pop,l	,R(ebp))
	INSN1(pop,l	,R(ebx))
	INSN1(pop,l	,R(esi))
	INSN1(pop,l	,R(edi))
	ret

EPILOGUE(C_SYMBOL_NAME(__gmpn_mul_1))
