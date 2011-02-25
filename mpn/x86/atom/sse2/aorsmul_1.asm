dnl x86-32 mpn_addmul_1 and mpn_submul_1 optimised for Intel Atom.

dnl  Contributed to the GNU project by Torbjorn Granlund.
dnl
dnl  Copyright 2011 Free Software Foundation, Inc.
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
C			    cycles/limb
C P5				 -
C P6 model 0-8,10-12		 -
C P6 model 9  (Banias)
C P6 model 13 (Dothan)
C P4 model 0  (Willamette)
C P4 model 1  (?)
C P4 model 2  (Northwood)
C P4 model 3  (Prescott)
C P4 model 4  (Nocona)
C Intel Atom			 8
C AMD K6
C AMD K7			 -
C AMD K8
C AMD K10

define(`rp', `%edi')
define(`up', `%esi')
define(`n',  `%ecx')

ifdef(`OPERATION_addmul_1',`
	define(ADDSUB,  add)
	define(func_1,  mpn_addmul_1)
	define(func_1c, mpn_addmul_1c)')
ifdef(`OPERATION_submul_1',`
	define(ADDSUB,  sub)
	define(func_1,  mpn_submul_1)
	define(func_1c, mpn_submul_1c)')

MULFUNC_PROLOGUE(mpn_addmul_1 mpn_addmul_1c mpn_submul_1 mpn_submul_1c)

	TEXT
	ALIGN(16)
PROLOGUE(func_1)
	xor	%edx, %edx
L(ent):	push	%edi
	push	%esi
	push	%ebx
	mov	16(%esp), rp
	mov	20(%esp), up
	mov	24(%esp), n
	movd	28(%esp), %mm7
	mov	n, %eax
	shr	$2, n
	and	$3, %eax
	jz	L(fi0)
	cmp	$2, %eax
	jc	L(fi1)
	jz	L(fi2)

L(fi3):	lea	-12(rp), rp
	movd	(up), %mm1
	lea	4(up), up
	pmuludq	%mm7, %mm1
	movd	%mm1, %ebx
	movd	(up), %mm0
	inc	n
	jmp	L(lo3)

L(fi0):	lea	-8(rp), rp
	movd	(up), %mm0
	lea	-8(up), up
	pmuludq	%mm7, %mm0
	movd	%mm0, %eax
	movd	12(up), %mm1
	pmuludq	%mm7, %mm1
	jmp	L(lo0)

L(fi1):	lea	-4(rp), rp
	movd	(up), %mm1
	lea	-4(up), up
	pmuludq	%mm7, %mm1
	movd	%mm1, %ebx
	test	n, n
	jz	L(wd1)
	movd	8(up), %mm0
	pmuludq	%mm7, %mm0
	jmp	L(lo1)

L(fi2):	movd	(up), %mm0
	pmuludq	%mm7, %mm0
	movd	4(up), %mm1
	movd	%mm0, %eax
	pmuludq	%mm7, %mm1
	test	n, n
	jnz	L(lo2)
	jmp	L(wd2)

C	ALIGN(16)			C alignment seems irrelevant
L(top):	adc	$0, %edx
	ADDSUB	%ebx, 12(rp)
	movd	%mm0, %eax
	pmuludq	%mm7, %mm1
	lea	16(rp), rp
L(lo2):	psrlq	$32, %mm0
	adc	%edx, %eax
	movd	%mm0, %edx
	movd	%mm1, %ebx
	movd	8(up), %mm0
	pmuludq	%mm7, %mm0
	adc	$0, %edx
	ADDSUB	%eax, (rp)
L(lo1):	psrlq	$32, %mm1
	adc	%edx, %ebx
	movd	%mm1, %edx
	movd	%mm0, %eax
	movd	12(up), %mm1
	pmuludq	%mm7, %mm1
	adc	$0, %edx
	ADDSUB	%ebx, 4(rp)
L(lo0):	psrlq	$32, %mm0
	adc	%edx, %eax
	movd	%mm0, %edx
	movd	%mm1, %ebx
	lea	16(up), up
	movd	(up), %mm0
	adc	$0, %edx
	ADDSUB	%eax, 8(rp)
L(lo3):	psrlq	$32, %mm1
	adc	%edx, %ebx
	movd	%mm1, %edx
	pmuludq	%mm7, %mm0
	dec	n
	movd	4(up), %mm1
	jnz	L(top)

L(end):	adc	$0, %edx
	ADDSUB	%ebx, 12(rp)
	movd	%mm0, %eax
	pmuludq	%mm7, %mm1
	lea	16(rp), rp
L(wd2):	psrlq	$32, %mm0
	adc	%edx, %eax
	movd	%mm0, %edx
	movd	%mm1, %ebx
	adc	$0, %edx
	ADDSUB	%eax, (rp)
L(wd1):	psrlq	$32, %mm1
	adc	%edx, %ebx
	movd	%mm1, %edx
	adc	$0, %edx
	ADDSUB	%ebx, 4(rp)
	mov	%edx, %eax
	adc	$0, %eax
	emms
	pop	%ebx
	pop	%esi
	pop	%edi
	ret
EPILOGUE()
PROLOGUE(func_1c)
	mov	20(%esp), %edx		C carry
	jmp	L(ent)
EPILOGUE()
