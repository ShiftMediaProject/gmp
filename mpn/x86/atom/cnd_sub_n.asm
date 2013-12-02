dnl  X86 mpn_cnd_sub_n optimised for Intel Atom.

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
dnl  along with the GNU MP Library.  If not, see https://www.gnu.org/licenses/.

include(`../config.m4')

C			    cycles/limb
C P5				 ?
C P6 model 0-8,10-12		 ?
C P6 model 9   (Banias)		 ?
C P6 model 13  (Dothan)		 ?
C P4 model 0-1 (Willamette)	 ?
C P4 model 2   (Northwood)	 ?
C P4 model 3-4 (Prescott)	 ?
C Intel atom			 5.67
C AMD K6			 ?
C AMD K7			 ?
C AMD K8			 ?


define(`rp',  `%edi')
define(`up',  `%esi')
define(`vp',  `%ebp')
define(`n',   `%ecx')
define(`cnd', `20(%esp)')

ASM_START()
	TEXT
	ALIGN(16)
PROLOGUE(mpn_cnd_sub_n)
	push	%edi
	push	%esi
	push	%ebx
	push	%ebp

	mov	cnd, %eax		C make cnd into a mask (1)
	mov	24(%esp), rp
	neg	%eax			C make cnd into a mask (1)
	mov	28(%esp), up
	sbb	%eax, %eax		C make cnd into a mask (1)
	mov	32(%esp), vp
	mov	%eax, cnd		C make cnd into a mask (1)
	mov	36(%esp), n

	xor	%edx, %edx

	inc	n
	shr	n
	jnc	L(ent)

	mov	0(vp), %eax
	and	cnd, %eax
	lea	4(vp), vp
	mov	0(up), %edx
	sub	%eax, %edx
	lea	4(rp), rp
	lea	4(up), up
	mov	%edx, -4(rp)
	sbb	%edx, %edx		C save cy

L(ent):	mov	0(vp), %ebx
	and	cnd, %ebx
	add	%edx, %edx		C restore cy
	mov	0(up), %edx
	dec	n
	je	L(end)

L(top):	sbb	%ebx, %edx
	mov	4(vp), %eax
	mov	%edx, 0(rp)
	sbb	%edx, %edx		C save cy
	mov	8(vp), %ebx
	lea	8(up), up
	and	cnd, %ebx
	and	cnd, %eax
	add	%edx, %edx		C restore cy
	mov	-4(up), %edx
	lea	8(rp), rp
	sbb	%eax, %edx
	mov	%edx, -4(rp)
	dec	n
	mov	0(up), %edx
	lea	8(vp), vp
	jne	L(top)

L(end):	sbb	%ebx, %edx
	mov	%edx, 0(rp)

	mov	$0, %eax
	adc	%eax, %eax

	pop	%ebp
	pop	%ebx
	pop	%esi
	pop	%edi
	ret
EPILOGUE()
ASM_END()
