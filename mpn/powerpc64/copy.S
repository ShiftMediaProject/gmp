/* PowerPC-64 __mpn_copy -- Copy a limb vector.

Copyright (C) 1999 Free Software Foundation, Inc.

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

/* INPUT PARAMETERS
   rptr	r3
   sptr	r4
   n	r5 */

#include "asm-syntax.h"

#ifdef AIX_SYNTAX
	.machine	"ppc64"
	.toc
	.globl	__mpn_copy
	.globl	.__mpn_copy
	.csect	__mpn_copy[DS],3
__mpn_copy:
	.llong	.__mpn_copy, TOC[tc0], 0
	.csect	.text[PR]
	.align	2
.__mpn_copy:
#else
	.text
	.align	4
	.globl	C_SYMBOL_NAME(__mpn_copy)
C_SYMBOL_NAME(__mpn_copy):
#endif
/* Unrolling this analogous to sparc64/copy.s doesn't help for any
   operand sizes.  */
	mtctr	r5
	addi	r4,r4,-8
	addi	r3,r3,-8
.Loop:	ldu	r0,8(r4)
	stdu	r0,8(r3)
	bdnz	.Loop
	blr
