dnl  Intel Atom mpn_addlshC_n/mpn_rsblshC_n -- rp[] = (vp[] << C) +- up[]

dnl  Contributed to the GNU project by Marco Bodrato.

dnl  Copyright 2011 Free Software Foundation, Inc.
dnl
dnl  This file is part of the GNU MP Library.
dnl
dnl  The GNU MP Library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Lesser General Public License as
dnl  published by the Free Software Foundation; either version 3 of the
dnl  License, or (at your option) any later version.
dnl
dnl  The GNU MP Library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Lesser General Public License for more details.
dnl
dnl  You should have received a copy of the GNU Lesser General Public License
dnl  along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.

include(`../config.m4')

C mp_limb_t mpn_addlshC_n (mp_ptr dst, mp_srcptr src1, mp_srcptr src2,
C                          mp_size_t size,);
C mp_limb_t mpn_addlshC_nc (mp_ptr dst, mp_srcptr src1, mp_srcptr src2,
C                           mp_size_t size, mp_limb_t carry);
C mp_limb_t mpn_rsblshC_n (mp_ptr dst, mp_srcptr src1, mp_srcptr src2,
C                          mp_size_t size,);
C mp_limb_t mpn_rsblshC_nc (mp_ptr dst, mp_srcptr src1, mp_srcptr src2,
C                           mp_size_t size, mp_signed_limb_t carry);

defframe(PARAM_CORB,	20)
defframe(PARAM_SIZE,	16)
defframe(PARAM_DBLD,	12)
defframe(PARAM_SRC,	 8)
defframe(PARAM_DST,	 4)

dnl  re-use parameter space
define(VAR_COUNT,`PARAM_SIZE')
define(SAVE_EBX,`PARAM_DBLD')
define(SAVE_VP,`PARAM_SRC')
define(SAVE_UP,`PARAM_DST')

define(M, eval(m4_lshift(1,LSH)))
define(`rp',  `%edi')
define(`up',  `%esi')
define(`vp',  `%ebp')

ASM_START()
	TEXT
	ALIGN(8)

PROLOGUE(M4_function_c)
deflit(`FRAME',0)
	movl	PARAM_CORB, %eax
	movl	%eax, %edx
	shr	$LSH, %edx
	andl	$1, %edx
	M4_opp	%edx, %eax
	jmp	L(start_nc)
EPILOGUE()

PROLOGUE(M4_function)
deflit(`FRAME',0)

	xor	%eax, %eax
	xor	%edx, %edx
L(start_nc):
	push	rp			FRAME_pushl()

	mov	PARAM_SIZE, %ecx	C size
	mov	PARAM_DST, rp
	mov	up, SAVE_UP
	incl	%ecx			C size + 1
	mov	PARAM_SRC, up
	mov	vp, SAVE_VP
	shr	$1, %ecx		C (size+1)\2
	mov	PARAM_DBLD, vp
	mov	%ebx, SAVE_EBX
	mov	%ecx, VAR_COUNT
	jnc	L(entry)		C size odd

	shr	$1, %edx		C size even
	mov	(vp), %ecx
	lea	4(vp), vp
	lea	(%eax,%ecx,M), %edx
	mov	%ecx, %eax
	lea	-4(up), up
	lea	-4(rp), rp
	jmp	L(enteven)

	ALIGN(16)
L(oop):
	lea	(%eax,%ecx,M), %ebx
	shr	$RSH, %ecx
	mov	4(vp), %eax
	lea 	8(vp), vp
	shr	$1, %edx
	M4_inst	(up), %ebx
	lea	(%ecx,%eax,M), %edx
	mov	%ebx, (rp)
L(enteven):
	M4_inst	4(up), %edx
	lea	8(up), up
	mov	%edx, 4(rp)
	adc	%edx, %edx
	shr	$RSH, %eax
	lea	8(rp), rp
L(entry):
	mov	(vp), %ecx
	decl	VAR_COUNT
	jnz	L(oop)

	lea	(%eax,%ecx,M), %ebx
	shr	$RSH, %ecx
	shr	$1, %edx
	mov	SAVE_VP, vp
	M4_inst	(up), %ebx
	mov	%ecx, %eax
	mov	SAVE_UP, up
	M4_inst	$0, %eax
	mov	%ebx, (rp)
	mov	SAVE_EBX, %ebx
	pop	rp			FRAME_popl()
	ret
EPILOGUE()

ASM_END()
