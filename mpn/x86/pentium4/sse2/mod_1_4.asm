dnl  mpn_mod_1_4 for Pentium 4 and P6 models with SSE2 (i.e., 9,D,E,F).

dnl  Copyright 2009 Free Software Foundation, Inc.
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

C TODO:
C  * Optimize.  The present code was written quite straightforwardly.
C  * Write a proper mpn_mod_1s_4p_cps.  The code below was compiler generated,
C    with very minor hand optimization.  Newer GCC versions might actually beat
C    our code.
C  * Optimize post-loop reduction code.
C  * Align loop?

C                           cycles/limb
C P6 model 0-8,10-12)           -
C P6 model 9   (Banias)         ?
C P6 model 13  (Dothan)         3.4
C P4 model 0-1 (Willamette):    ?
C P4 model 2   (Northwood):     4
C P4 model 3-4 (Prescott):      ?

C INPUT PARAMETERS
C ap		sp + 4
C n		sp + 8
C b		sp + 12
C cps		sp + 16

define(`B1modb', `%mm1')
define(`B2modb', `%mm2')
define(`B3modb', `%mm3')
define(`B4modb', `%mm4')
define(`B5modb', `%mm5')
define(`ap', `%edx')
define(`n', `%eax')

	TEXT
	ALIGN(16)
PROLOGUE(mpn_mod_1s_4p)
	push	%ebx
	mov	8(%esp), ap
	mov	12(%esp), n
	mov	20(%esp), %ecx

	movd	8(%ecx), B1modb
	movd	12(%ecx), B2modb
	movd	16(%ecx), B3modb
	movd	20(%ecx), B4modb
	movd	24(%ecx), B5modb

	mov	n, %ebx
	lea	-4(ap,n,4), ap
	and	$3, %ebx
	je	L(b0)
	cmp	$2, %ebx
	jc	L(b1)
	je	L(b2)

L(b3):	movd	-4(ap), %mm7
	pmuludq	B1modb, %mm7
	movd	-8(ap), %mm6
	paddq	%mm6, %mm7
	movd	(ap), %mm6
	pmuludq	B2modb, %mm6
	paddq	%mm6, %mm7
	lea	-24(ap), ap
	add	$-3, n
	jz	L(end)
	jmp	L(top)

L(b0):	movd	-8(ap), %mm7
	pmuludq	B1modb, %mm7
	movd	-12(ap), %mm6
	paddq	%mm6, %mm7
	movd	-4(ap), %mm6
	pmuludq	B2modb, %mm6
	paddq	%mm6, %mm7
	movd	(ap), %mm6
	pmuludq	B3modb, %mm6
	paddq	%mm6, %mm7
	lea	-28(ap), ap
	add	$-4, n
	jz	L(end)
	jmp	L(top)

L(b1):	movd	(ap), %mm7
	lea	-16(ap), ap
	dec	n
	jz	L(x)
	jmp	L(top)

L(b2):	movd	(ap), %mm7
	pmuludq	B1modb, %mm7
	movd	-4(ap), %mm6
	paddq	%mm6, %mm7
	lea	-20(ap), ap
	add	$-2, n
	jz	L(end)

	ALIGN(8)
L(top):	movd	4(ap), %mm0
	pmuludq	B1modb, %mm0
	movd	0(ap), %mm6
	paddq	%mm6, %mm0

	movd	8(ap), %mm6
	pmuludq	B2modb, %mm6
	paddq	%mm6, %mm0

	movd	12(ap), %mm6
	pmuludq	B3modb, %mm6
	paddq	%mm6, %mm0

	movq	%mm7, %mm6
	psrlq	$32, %mm7		C rh
	pmuludq	B5modb, %mm7
	pmuludq	B4modb, %mm6

	paddq	%mm0, %mm7
	paddq	%mm6, %mm7

	add	$-16, ap
	add	$-4, n
	jnz	L(top)
L(end):

	pcmpeqd	%mm4, %mm4
	psrlq	$32, %mm4		C 0x00000000FFFFFFFF
	pand	%mm7, %mm4		C rl
	psrlq	$32, %mm7		C rh
	pmuludq	B1modb, %mm7		C rh,cl
	paddq	%mm4, %mm7		C rh,rl

L(x):	movd	4(%ecx), %mm4		C cnt
	psllq	%mm4, %mm7		C rh,rl normalized
	movq	%mm7, %mm2		C rl in low half
	psrlq	$32, %mm7		C rh
	movd	(%ecx), %mm1		C bi
	pmuludq	%mm7, %mm1		C qh,ql
	paddq	%mm2, %mm1		C qh-1,ql
	movd	%mm1, %ecx		C ql
	psrlq	$32, %mm1		C qh-1
	movd	16(%esp), %mm3		C b
	pmuludq	%mm1, %mm3		C (qh-1) * b
	psubq	%mm3, %mm2		C r in low half (could use psubd)
	movd	%mm2, %eax		C r
	mov	16(%esp), %ebx
	sub	%ebx, %eax		C r
	cmp	%eax, %ecx
	lea	(%eax,%ebx), %edx
	cmovc(	%edx, %eax)
	movd	%mm4, %ecx		C cnt
	cmp	%ebx, %eax
	jae	L(fix)
	emms
	pop	%ebx
	shr	%cl, %eax
	ret

L(fix):	sub	%ebx, %eax
	emms
	pop	%ebx
	shr	%cl, %eax
	ret
EPILOGUE()

PROLOGUE(mpn_mod_1s_4p_cps)
	sub	$28, %esp
	mov	%ebx, 12(%esp)
	mov	%esi, 16(%esp)
	mov	%edi, 20(%esp)
	mov	%ebp, 24(%esp)
	mov	36(%esp), %ebx
	bsr	%ebx, %edi
	xor	$31, %edi
	mov	%edi, %ecx
	sal	%cl, %ebx
	mov	%ebx, %edx
	not	%edx
	mov	$-1, %eax
	div	%ebx
	mov	%eax, %esi
	mov	$32, %ecx
	sub	%edi, %ecx
	mov	%eax, %edx
	shr	%cl, %edx
	mov	$1, %eax
	mov	%edi, %ecx
	sal	%cl, %eax
	or	%eax, %edx
	mov	%ebx, %eax
	neg	%eax
	imul	%eax, %edx
	mov	%edx, 4(%esp)
	mov	%edx, %eax
	mul	%esi
	mov	4(%esp), %ecx
	lea	1(%edx,%ecx), %edx
	neg	%edx
	imul	%ebx, %edx
	cmp	%edx, %eax
	mov	%edx, 8(%esp)
	jae	L(16)
	add	%ebx, %edx
	mov	%edx, 8(%esp)
L(16):	mov	8(%esp), %eax
	mul	%esi
	mov	8(%esp), %ecx
	lea	1(%edx,%ecx), %edx
	neg	%edx
	mov	%edx, %ebp
	imul	%ebx, %ebp
	cmp	%ebp, %eax
	jae	L(17)
	add	%ebx, %ebp
L(17):	mov	%ebp, %eax
	mul	%esi
	lea	1(%edx,%ebp), %edx
	neg	%edx
	imul	%ebx, %edx
	mov	%edx, (%esp)
	cmp	%edx, %eax
	jae	L(18)
	add	%ebx, %edx
	mov	%edx, (%esp)
L(18):	mov	(%esp), %eax
	mul	%esi
	mov	(%esp), %ecx
	lea	1(%edx,%ecx), %edx
	neg	%edx
	imul	%ebx, %edx
	cmp	%edx, %eax
	jae	L(19)
	add	%ebx, %edx
L(19):	mov	32(%esp), %ebx
	mov	%esi, (%ebx)
	mov	%edi, 4(%ebx)
	mov	4(%esp), %eax
	mov	%edi, %ecx
	shr	%cl, %eax
	mov	%eax, 8(%ebx)
	mov	8(%esp), %eax
	shr	%cl, %eax
	mov	%eax, 12(%ebx)
	shr	%cl, %ebp
	mov	%ebp, 16(%ebx)
	mov	(%esp), %eax
	shr	%cl, %eax
	mov	%eax, 20(%ebx)
	shr	%cl, %edx
	mov	%edx, 24(%ebx)
	mov	12(%esp), %ebx
	mov	16(%esp), %esi
	mov	20(%esp), %edi
	mov	24(%esp), %ebp
	add	$28, %esp
	ret
EPILOGUE()
