/* Pentium mpn_mul_basecase -- Multiply two limb vectors and store
   the result in a third limb vector.

Copyright (C) 1996 Free Software Foundation, Inc.

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
40	vsize (smaller size)
36	vptr
32	usize (larger size)
28	uptr			esi
24	rptr			edi
20	return address
16	temp (loop counter)
12	saved reg
8	saved reg
4	saved reg
sp =>	saved reg
*/

#include "asm-syntax.h"

.text
	ALIGN (3)
	.globl C_SYMBOL_NAME(__mpn_mul_basecase)
C_SYMBOL_NAME(__mpn_mul_basecase:)
	pushl	%eax		   /* dummy push for allocating stack slot */
	pushl	%esi
	pushl	%ebp
	pushl	%edi
	movl	28-4(%esp),%esi	   /* uptr */
	movl	24-4(%esp),%edi	   /* rptr */
	movl	36-4(%esp),%ebp	   /* vptr */

	movl	(%esi),%eax	   /* load uptr[0] */
	mull	(%ebp)		   /* multiply by vptr[0] */
	movl	%eax,(%edi)	   /* store to rptr[0] */
	movl	32-4(%esp),%ecx	   /* usize */
	decl	%ecx		   /* If usize = 1, vsize = 1 too */
	jz	Ldone

	pushl	%ebx
	movl	%edx,%ebx
	movl	32(%esp),%eax	   /* usize */
	leal	(%esi,%eax,4),%esi /* make uptr point at end */
	leal	(%edi,%eax,4),%edi /* offset rptr by usize */
	negl	%ecx		   /* negate j size/index for inner loop */

	ALIGN	(3)
Loop1:	movl	(%esi,%ecx,4),%eax /* load next limb at uptr[j] */
	mull	(%ebp)
	addl	%ebx,%eax
	movl	%edx,%ebx
	adcl	$0,%ebx
	movl	%eax,(%edi,%ecx,4)
	incl	%ecx
	jnz	Loop1

	movl	%ebx,(%edi)	   /* store most significant limb of product */
	addl	$4,%edi		   /* increment rptr */

	movl	40(%esp),%eax
	decl	%eax
	jz	Lskip
	movl	%eax,16(%esp)	   /* set index i to vsize */

Louter:	addl	$4,%ebp		   /* make ebp point to next v limb */
	movl	32(%esp),%ecx
	negl	%ecx
	xorl	%ebx,%ebx

	ALIGN	(3)
Loop2:	adcl	$0,%ebx
	movl	(%esi,%ecx,4),%eax
	mull	(%ebp)
	addl	%ebx,%eax
	movl	(%edi,%ecx,4),%ebx
	adcl	$0,%edx
	addl	%eax,%ebx
	movl	%ebx,(%edi,%ecx,4)
	incl	%ecx
	movl	%edx,%ebx
	jnz	Loop2

	adcl	$0,%ebx

	movl	%ebx,(%edi)
	addl	$4,%edi
	movl	16(%esp),%eax
	decl	%eax
	movl	%eax,16(%esp)
	jnz	Louter

Lskip:	popl	%ebx
	popl	%edi
	popl	%ebp
	popl	%esi
	addl	$4,%esp
	ret

Ldone:	movl	%edx,4(%edi)	   /* store to rptr[1] */
	popl	%edi
	popl	%ebp
	popl	%esi
	popl	%eax		   /* dummy pop for deallocating stack slot */
	ret
