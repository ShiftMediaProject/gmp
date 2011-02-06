dnl  AMD K7 mpn_addlsh1_n -- rp[] = up[] + (vp[] << 1)

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

C This is an attempt at a addlsh1_n for x86-32, not relying on sse2 insns.  The
C innerloop is 2*3-way unrolled, which is best we can do with the available
C registers.  It seems tricky to use the same structure for rsblsh1_n, since we
C cannot feed carry between operations there.

C                           cycles/limb
C P5
C P6 model 0-8,10-12)
C P6 model 9  (Banias)
C P6 model 13 (Dothan)           5.4	(worse than add_n + lshift)
C P4 model 0  (Willamette)
C P4 model 1  (?)
C P4 model 2  (Northwood)
C P4 model 3  (Prescott)
C P4 model 4  (Nocona)
C Intel Atom			 6
C AMD K6                         ?
C AMD K7                         2.5
C AMD K8

C This is a basic addlsh1_n for k7, atom, and perhaps some other x86-32
C processors.  It uses 2*3-way unrolling, for good reasons.  Unfortunately,
C that means we need an initial magic multiply.
C 
C It is not clear how to do sublsh1_n or rsblsh1_n using the same pattern.  We
C cannot do rsblsh1_n since we feed carry from the shift blocks to the
C add/subtract blocks, which is right for addition but reversed for
C subtraction.  We could perhaps do sublsh1_n, with some extra move insns,
C without losing any time, since we're not issue limited but carry recurrency
C latency.
C 
C Breaking carry recurrency might be a good idea.  We would then need separate
C registers for the shift carry and add/subtract carry, which in turn would
C force is to 2*2-way unrolling.

defframe(PARAM_SIZE,	16)
defframe(PARAM_DBLD,	12)
defframe(PARAM_SRC,	 8)
defframe(PARAM_DST,	 4)

dnl  re-use parameter space
define(VAR_COUNT,`PARAM_DST')
define(VAR_TMP,`PARAM_DBLD')

ASM_START()
	TEXT
	ALIGN(8)
PROLOGUE(mpn_addlsh1_n)
deflit(`FRAME',0)

define(`rp',  `%edi')
define(`up',  `%esi')
define(`vp',  `%ebp')

	mov	$0x20000000, %eax

	push	%ebx			FRAME_pushl()
	movl	PARAM_SIZE, %ebx	C size

	push	rp			FRAME_pushl()
	movl	PARAM_DST, rp

	mull	%ebx

	push	up			FRAME_pushl()
	movl	PARAM_SRC, up

	not	%edx			C count = -(size\8)-1
	mov	%edx, VAR_COUNT

	push	vp			FRAME_pushl()
	movl	PARAM_DBLD, vp

	leal	4(,%edx,4), %ecx	C count*4+4 = -(size\8)*4
	xorl	%edx, %edx
	leal	(%ebx,%ecx,2), %ebx	C size + (count*4+4)*2 = size % 8
	orl	%ebx, %ebx
	jz	L(exact)

L(oop):
ifdef(`CPU_P6',`
	shr	%edx ')			C restore 2nd saved carry bit
	mov	(vp), %eax
	adc	%eax, %eax
	rcr	%edx			C restore 1st saved carry bit
	adc	(up), %eax
	mov	%eax, (rp)
	adc	%edx, %edx		C save a carry bit in edx
	lea	4(rp), rp
	lea	4(up), up
	lea	4(vp), vp
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
	mov	12(vp), vp
	adc	vp, vp

	rcr	%edx			C restore 1st saved carry bit

	adc	(up), %eax
	mov	%eax, (rp)
	adc	4(up), %ebx
	mov	%ebx, 4(rp)
	adc	8(up), %ecx
	mov	%ecx, 8(rp)
	adc	12(up), vp
	mov	vp, 12(rp)

	movl	VAR_TMP, vp

	mov	16(vp), %eax
	adc	%eax, %eax
	mov	20(vp), %ebx
	adc	%ebx, %ebx
	mov	24(vp), %ecx
	adc	%ecx, %ecx
	mov	28(vp), vp
	adc	vp, vp

	adc	%edx, %edx		C save a carry bit in edx

	adc	16(up), %eax
	mov	%eax, 16(rp)
	adc	20(up), %ebx
	mov	%ebx, 20(rp)
	adc	24(up), %ecx
	mov	%ecx, 24(rp)
	adc	28(up), vp
	mov	vp, 28(rp)

	movl	VAR_TMP, vp
	lea	32(rp), rp
	lea	32(up), up
	lea	32(vp), vp

ifdef(`CPU_P6',`
	adc	%edx, %edx ')		C save another carry bit in edx
	incl	VAR_COUNT
	movl	vp, VAR_TMP
	jne	L(top)

L(end):
	pop	vp			FRAME_popl()
	pop	up			FRAME_popl()

ifdef(`CPU_P6',`
	xor	%eax, %eax
	shr	$1, %edx
	adc	%edx, %eax
',`
	adc	$0, %edx
	mov	%edx, %eax
')
	pop	rp			FRAME_popl()
	pop	%ebx			FRAME_popl()
	ret
EPILOGUE()
ASM_END()
