dnl  x86-32 mpn_mod_1_1p, requiring cmov.

dnl  Contributed to the GNU project by Torbjorn Granlund.
dnl
dnl  Copyright 2010 Free Software Foundation, Inc.
dnl
dnl  This file is part of the GNU MP Library.
dnl
dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU Lesser General Public License as published
dnl  by the Free Software Foundation; either version 3 of the License, or (at
dnl  your option) any later version.
dnl
dnl  The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
dnl  License for more details.
dnl
dnl  You should have received a copy of the GNU Lesser General Public License
dnl  along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.

include(`../config.m4')

C			    cycles/limb
C P5:				 ?
C P6 model 0-8,10-12)		 ?
C P6 model 9  (Banias)		 ?
C P6 model 13 (Dothan)		11.75
C P4 model 0  (Willamette)	 ?
C P4 model 1  (?)		 ?
C P4 model 2  (Northwood)	 ?
C P4 model 3  (Prescott)	 ?
C P4 model 4  (Nocona)		 ?
C K6:				 ?
C K7:				 8
C K8:				 ?

ASM_START()
	TEXT
	ALIGN(8)
PROLOGUE(mpn_mod_1_1p)
	push	%ebp
	push	%edi
	push	%esi
	push	%ebx
	mov	24(%esp), %ebx
	mov	20(%esp), %esi
	mov	32(%esp), %ebp		C cps[]
	lea	(%esi,%ebx,4), %esi

	mov	8(%ebp), %edi		C B1modb
	mov	12(%ebp), %ebp		C B2modb
	mov	-4(%esi), %eax
	mul	%edi
	xor	%ecx, %ecx
	add	-8(%esi), %eax
	adc	%edx, %ecx
	sub	$2, 24(%esp)
	jle	L(end)

	ALIGN(16)
L(top):	mul	%edi			C 0
	mov	-12(%esi), %ebx		C
	add	%eax, %ebx		C 4	
	mov	%ecx, %eax		C 2
	mov	$0, %ecx		C
	adc	%edx, %ecx		C 6
	mul	%ebp			C 3
	add	%ebx, %eax		C 7
	adc	%edx, %ecx		C 9
	decl	24(%esp)
	lea	-4(%esi), %esi
	jg	L(top)

L(end):	mov	%eax, %ebp
	mov	%ecx, %eax
	mul	%edi
	mov	32(%esp), %edi
	add	%eax, %ebp
	adc	$0, %edx
	mov	4(%edi), %ecx
	mov	%edx, %eax		C rh
	mov	%ebp, %esi		C rl
	sal	%cl, %eax
	mov	%ecx, %ebx
	test	%ecx, %ecx
	je	L(nrm)
	neg	%ecx
	shr	%cl, %esi
	or	%esi, %eax
	neg	%ecx
L(nrm):	lea	1(%eax), %esi
	mull	(%edi)
	mov	%eax, %ebx
	mov	%ebp, %eax
	mov	28(%esp), %ebp
	sal	%cl, %eax
	add	%eax, %ebx
	adc	%esi, %edx
	imul	%ebp, %edx
	sub	%edx, %eax
	lea	(%eax,%ebp), %edx
	cmp	%eax, %ebx
	cmovb(	%edx, %eax)
	mov	%eax, %edx
	sub	%ebp, %eax
	cmovb(	%edx, %eax)
	pop	%ebx
	pop	%esi
	pop	%edi
	pop	%ebp
	shr	%cl, %eax
	ret
EPILOGUE()

PROLOGUE(mpn_mod_1_1p_cps)
C CAUTION: This is the same code as in pentium4/sse2//mod_1_1.asm
	push	%ebp
	mov	12(%esp), %ebp
	push	%esi
	bsr	%ebp, %ecx
	push	%ebx
	xor	$31, %ecx
	mov	16(%esp), %esi
	sal	%cl, %ebp
	mov	%ebp, %edx
	not	%edx
	mov	$-1, %eax
	div	%ebp
	mov	%eax, (%esi)		C store bi
	mov	%ecx, 4(%esi)		C store cnt
	xor	%ebx, %ebx
	sub	%ebp, %ebx
	mov	$1, %edx
	shld	%cl, %eax, %edx
	imul	%edx, %ebx
	mul	%ebx
	add	%ebx, %edx
	not	%edx
	imul	%ebp, %edx
	add	%edx, %ebp
	cmp	%edx, %eax
	cmovb(	%ebp, %edx)
	shr	%cl, %ebx
	mov	%ebx, 8(%esi)		C store B1modb
	shr	%cl, %edx
	mov	%edx, 12(%esi)		C store B2modb
	pop	%ebx
	pop	%esi
	pop	%ebp
	ret
EPILOGUE()
