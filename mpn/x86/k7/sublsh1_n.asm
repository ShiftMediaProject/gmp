dnl  AMD K7 mpn_sublsh1_n -- rp[] = up[] - (vp[] << 1)

dnl  Copyright 2011 Free Software Foundation, Inc.

dnl  Contributed to the GNU project by Torbjorn Granlund and Marco Bodrato.

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
dnl  along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.

include(`../config.m4')

C This is an attempt at a sublsh1_n for x86-32, not relying on sse2 insns.  The
C innerloop is 2*3-way unrolled, which is best we can do with the available
C registers.  It seems tricky to use the same structure for rsblsh1_n, since we
C cannot feed carry between operations there.

C                           cycles/limb
C P5
C P6 model 0-8,10-12)
C P6 model 9  (Banias)
C P6 model 13 (Dothan)           
C P4 model 0  (Willamette)
C P4 model 1  (?)
C P4 model 2  (Northwood)
C P4 model 3  (Prescott)
C P4 model 4  (Nocona)
C Intel Atom			 6.75
C AMD K6                         ?
C AMD K7                         
C AMD K8

C This is a basic sublsh1_n for k7, atom, and perhaps some other x86-32
C processors.  It uses 2*3-way unrolling, for good reasons.  Unfortunately,
C that means we need an initial magic multiply.
C 
C Breaking carry recurrency might be a good idea.  We would then need separate
C registers for the shift carry and add/subtract carry, which in turn would
C force is to 2*2-way unrolling.

defframe(PARAM_SIZE,	16)
defframe(PARAM_DBLD,	12)
defframe(PARAM_SRC,	 8)
defframe(PARAM_DST,	 4)

dnl  re-use parameter space
define(VAR_COUNT,`PARAM_SIZE')
define(VAR_TMP,`PARAM_DBLD')
define(SAVE_EBX,`PARAM_SRC')
define(SAVE_VP,`PARAM_DST')

ASM_START()
	TEXT
	ALIGN(8)
PROLOGUE(mpn_sublsh1_n)
deflit(`FRAME',0)

define(`rp',  `%edi')
define(`up',  `%esi')
define(`vp',  `%ebp')

	push	up			FRAME_pushl()
	movl	PARAM_SRC, up
	push	rp			FRAME_pushl()
	movl	PARAM_DST, rp
	movl	%ebx, SAVE_EBX
	movl	PARAM_SIZE, %ebx	C size
	movl	vp, SAVE_VP
	movl	PARAM_DBLD, vp
	cmp	up, rp
	je	L(inplace)

	mov	$0x2aaaaaab, %eax
	mull	%ebx

	not	%edx			C count = -(size\6)-1
	mov	%edx, VAR_COUNT

	leal	3(%edx,%edx,2), %ecx	C count*3+3 = -(size\6)*3
	xorl	%edx, %edx
	leal	(%ebx,%ecx,2), %ebx	C size + (count*3+3)*2 = size % 6
	orl	%ebx, %ebx
	jz	L(exact)

L(oop):
ifdef(`CPU_P6',`
	shr	%edx ')			C restore 2nd saved carry bit
	mov	(vp), %eax
	adc	%eax, %eax
	rcr	%edx			C restore 1st saved carry bit
	mov	(up), %ecx
	sbb	%eax, %ecx
	mov	%ecx, (rp)
	adc	%edx, %edx		C save a carry bit in edx
	lea	4(vp), vp
	lea	4(up), up
	lea	4(rp), rp
ifdef(`CPU_P6',`
	adc	%edx, %edx ')		C save another carry bit in edx
	decl	%ebx
	jnz	L(oop)
	movl	vp, VAR_TMP
L(exact):
	incl	VAR_COUNT
	jz	L(end)

	ALIGN(16)
L(top):
ifdef(`CPU_P6',`
	shr	%edx ')			C restore 2nd saved carry bit
	mov	(vp), %eax
	adc	%eax, %eax
	mov	4(vp), %ebx
	adc	%ebx, %ebx
	mov	8(vp), %ecx
	adc	%ecx, %ecx

	rcr	%edx			C restore 1st saved carry bit
	
	mov	(up), vp
	sbb	%eax, vp
	mov	4(up), %eax
	mov	vp, (rp)
	sbb	%ebx, %eax
	movl	VAR_TMP, vp
	mov	8(up), %ebx
	mov	%eax, 4(rp)
	sbb	%ecx, %ebx
	mov	%ebx, 8(rp)

	mov	12(vp), %eax
	adc	%eax, %eax
	mov	16(vp), %ebx
	adc	%ebx, %ebx
	mov	20(vp), %ecx
	adc	%ecx, %ecx

	adc	%edx, %edx		C save a carry bit in edx

	mov	12(up), vp
	sbb	%eax, vp
	mov	16(up), %eax
	mov	vp, 12(rp)
	movl	VAR_TMP, vp
	sbb	%ebx, %eax
	mov	20(up), %ebx
	mov	%eax, 16(rp)
	sbb	%ecx, %ebx
	mov	%ebx, 20(rp)

	lea	24(vp), vp
	lea	24(up), up
	lea	24(rp), rp

ifdef(`CPU_P6',`
	adc	%edx, %edx ')		C save another carry bit in edx
	incl	VAR_COUNT
	movl	vp, VAR_TMP
	jne	L(top)

	jmp	L(end)

L(inplace):
	xorl	%edx, %edx
	movl	%ebx, %eax

	shr	$3, %eax

	not	%eax			C count = -(size\8)-1
	mov	%eax, VAR_COUNT

	andl	$7, %ebx		C size % 8
	jz	L(exactinplace)

L(oopinplace):
ifdef(`CPU_P6',`
	shr	%edx ')			C restore 2nd saved carry bit
	mov	(vp), %eax
	adc	%eax, %eax
	rcr	%edx			C restore 1st saved carry bit
	sbb	%eax, (rp)
	adc	%edx, %edx		C save a carry bit in edx
	lea	4(vp), vp
	lea	4(rp), rp
ifdef(`CPU_P6',`
	adc	%edx, %edx ')		C save another carry bit in edx
	decl	%ebx
	jnz	L(oopinplace)
L(exactinplace):
	incl	VAR_COUNT
	jz	L(end)

	ALIGN(16)
L(unrolleight):
ifdef(`CPU_P6',`
	shr	%edx ')			C restore 2nd saved carry bit
	mov	(vp), %eax
	adc	%eax, %eax
	mov	4(vp), %ebx
	adc	%ebx, %ebx
	mov	8(vp), %ecx
	adc	%ecx, %ecx
	mov	12(vp), %esi
	adc	%esi, %esi

	rcr	%edx			C restore 1st saved carry bit
	
	sbb	%eax, (rp)
	sbb	%ebx, 4(rp)
	sbb	%ecx, 8(rp)
	sbb	%esi, 12(rp)

	mov	16(vp), %eax
	adc	%eax, %eax
	mov	20(vp), %ebx
	adc	%ebx, %ebx
	mov	24(vp), %ecx
	adc	%ecx, %ecx
	mov	28(vp), %esi
	adc	%esi, %esi

	adc	%edx, %edx		C save a carry bit in edx

	sbb	%eax, 16(rp)
	sbb	%ebx, 20(rp)
	sbb	%ecx, 24(rp)
	sbb	%esi, 28(rp)

	lea	32(vp), vp
	lea	32(rp), rp

ifdef(`CPU_P6',`
	adc	%edx, %edx ')		C save another carry bit in edx
	incl	VAR_COUNT
	jne	L(unrolleight)

L(end):
	movl	SAVE_VP, vp
	movl	SAVE_EBX, %ebx

ifdef(`CPU_P6',`
	xor	%eax, %eax
	shr	$1, %edx
	adc	%edx, %eax
',`
	adc	$0, %edx
	mov	%edx, %eax
')
	pop	rp			FRAME_popl()
	pop	up			FRAME_popl()
	ret
EPILOGUE()
ASM_END()
