/* x86 mpn_mul_basecase -- Multiply two limb vectors and store
   the result in a third limb vector.

Copyright (C) 1996, 1997, 1998, 1999, 2000 Free Software Foundation, Inc.

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

/* This was written in a haste since the Pentium optimized code that was used
   for all x86 machines was slow for the Pentium II.  This code would benefit
   from some cleanup.

   To shave off some percentage of the run-time, one should make 4 variants of
   the Louter loop, for the four different outcomes of un mod 4.  That would
   avoid Loop0 altogether.  Code expansion would be > 4-fold for that part of
   the function, but since it is not very large, that would be acceptable.

   The mul loop (at LoopM) might need some tweaking.  It's current speed is
   unknown.  */

/*
44	vn (smaller size)
40	vptr
36	un (larger size)
32	uptr			esi
28	rptr			edi
24	return address
20	temp
16	temp (loop counter)
12	saved reg
8	saved reg
4	saved reg
sp =>	saved reg
*/

#include "asm-syntax.h"

.text
	ALIGN (3)
PROLOGUE(C_SYMBOL_NAME(__gmpn_mul_basecase))
	.globl C_SYMBOL_NAME(__gmpn_mul_basecase)
C_SYMBOL_NAME(__gmpn_mul_basecase:)
	subl	$8,%esp
	pushl	%esi
	pushl	%ebp
	pushl	%edi
	movl	32-4(%esp),%esi	   /* uptr */
	movl	28-4(%esp),%edi	   /* rptr */
	movl	40-4(%esp),%ebp	   /* vptr */

	movl	(%esi),%eax	   /* load uptr[0] */
	mull	(%ebp)		   /* multiply by vptr[0] */
	movl	%eax,(%edi)	   /* store to rptr[0] */
	movl	36-4(%esp),%ecx	   /* un */
	decl	%ecx		   /* If un = 1, vn = 1 too */
	jz	Ldone

	pushl	%ebx
	movl	%edx,%ebx

	leal	4(%esi),%esi
	leal	4(%edi),%edi

LoopM:	movl	(%esi),%eax /* load next limb at uptr[j] */
	leal	4(%esi),%esi
	mull	(%ebp)
	addl	%ebx,%eax
	movl	%edx,%ebx
	adcl	$0,%ebx
	movl	%eax,(%edi)
	leal	4(%edi),%edi
	decl	%ecx
	jnz	LoopM

	movl	%ebx,(%edi)	   /* store most significant limb of product */
	addl	$4,%edi		   /* increment rptr */
	movl	36(%esp),%eax
	shll	$2,%eax
	subl	%eax,%edi
	subl	%eax,%esi

	movl	44(%esp),%eax	   /* vn */
	decl	%eax
	jz	Lskip
	movl	%eax,16(%esp)	   /* set index i to vn */

Louter:
	movl	40(%esp),%ebp	   /* vptr */
	addl	$4,%ebp		   /* make ebp point to next v limb */
	movl	%ebp,40(%esp)	   /* vptr */
	movl	(%ebp),%eax	   /* copy v limb ... */
	movl	%eax,20(%esp)	   /* ... to stack slot */
	movl	36(%esp),%ecx	   /* un */

	xorl	%ebx,%ebx
	andl	$3,%ecx
	jz	Lend0

Loop0:	movl	(%esi),%eax
	mull	20(%esp)
	leal	4(%esi),%esi
	addl	%ebx,%eax
	movl	$0,%ebx
	adcl	%ebx,%edx
	addl	%eax,(%edi)
	adcl	%edx,%ebx		/* propagate carry into cylimb */

	leal	4(%edi),%edi
	decl	%ecx
	jnz	Loop0

Lend0:	movl	36(%esp),%ecx
	shrl	$2,%ecx
	jz	LendU

	ALIGN (3)
LoopU:	movl	(%esi),%eax
	mull	20(%esp)
	addl	%eax,%ebx
	movl	$0,%ebp
	adcl	%edx,%ebp

	movl	4(%esi),%eax
	mull	20(%esp)
	addl	%ebx,(%edi)
	adcl	%eax,%ebp	/* new lo + cylimb */
	movl	$0,%ebx
	adcl	%edx,%ebx

	movl	8(%esi),%eax
	mull	20(%esp)
	addl	%ebp,4(%edi)
	adcl	%eax,%ebx	/* new lo + cylimb */
	movl	$0,%ebp
	adcl	%edx,%ebp

	movl	12(%esi),%eax
	mull	20(%esp)
	addl	%ebx,8(%edi)
	adcl	%eax,%ebp	/* new lo + cylimb */
	movl	$0,%ebx
	adcl	%edx,%ebx

	addl	%ebp,12(%edi)
	adcl	$0,%ebx		/* propagate carry into cylimb */

	leal	16(%esi),%esi
	leal	16(%edi),%edi
	decl	%ecx
	jnz	LoopU

LendU:
	movl	%ebx,(%edi)
	addl	$4,%edi

	/* we incremented rptr and uptr in the loop above; compensate */
	movl	36(%esp),%eax
	shll	$2,%eax
	subl	%eax,%edi
	subl	%eax,%esi

	movl	16(%esp),%eax
	decl	%eax
	movl	%eax,16(%esp)
	jnz	Louter

Lskip:	popl	%ebx
	popl	%edi
	popl	%ebp
	popl	%esi
	addl	$8,%esp
	ret

Ldone:	movl	%edx,4(%edi)	   /* store to rptr[1] */
	popl	%edi
	popl	%ebp
	popl	%esi
	addl	$8,%esp
	ret

EPILOGUE(C_SYMBOL_NAME(__gmpn_mul_basecase))
