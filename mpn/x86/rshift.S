/* i80386 __gmpn_rshift -- 

Copyright (C) 1992, 1994, 1996, 1999, 2000 Free Software Foundation, Inc.

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
  s_ptr		(sp + 8)
  size		(sp + 12)
  cnt		(sp + 16)
*/

#include "asm-syntax.h"

.text
	ALIGN (3)
PROLOGUE(C_SYMBOL_NAME(__gmpn_rshift))
	.globl C_SYMBOL_NAME(__gmpn_rshift)
C_SYMBOL_NAME(__gmpn_rshift:)
	pushl	%edi
	pushl	%esi
	pushl	%ebx

	movl	16(%esp),%edi		/* res_ptr */
	movl	20(%esp),%esi		/* s_ptr */
	movl	24(%esp),%edx		/* size */
	movl	28(%esp),%ecx		/* cnt */

	leal	-4(%edi,%edx,4),%edi
	leal	(%esi,%edx,4),%esi
	negl	%edx

	movl	(%esi,%edx,4),%ebx	/* read least significant limb */
	xorl	%eax,%eax
	INSND(shrd,l	,R(eax),R(ebx),R(cl))	/* compute carry limb */
	incl	%edx
	jz	Lend
	pushl	%eax			/* push carry limb onto stack */
	testb	$1,%dl
	jnz	L1			/* enter loop in the middle */
	movl	%ebx,%eax

	ALIGN (3)
Loop:	movl	(%esi,%edx,4),%ebx	/* load next higher limb */
	INSND(shrd,l	,R(eax),R(ebx),R(cl))	/* compute result limb */
	movl	%eax,(%edi,%edx,4)	/* store it */
	incl	%edx
L1:	movl	(%esi,%edx,4),%eax
	INSND(shrd,l	,R(ebx),R(eax),R(cl))
	movl	%ebx,(%edi,%edx,4)
	incl	%edx
	jnz	Loop

	shrl	%cl,%eax		/* compute most significant limb */
	movl	%eax,(%edi)		/* store it */

	popl	%eax			/* pop carry limb */

	popl	%ebx
	popl	%esi
	popl	%edi
	ret

Lend:	shrl	%cl,%ebx		/* compute most significant limb */
	movl	%ebx,(%edi)		/* store it */

	popl	%ebx
	popl	%esi
	popl	%edi
	ret

EPILOGUE(C_SYMBOL_NAME(__gmpn_rshift))
