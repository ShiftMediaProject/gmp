# Intel P6 mpn_addmul_1/mpn_submul_1 -- add or subtract mpn multiple.
#
# P6: 6.35 cycles/limb (at 16 limbs/loop).


# Copyright (C) 1999, 2000 Free Software Foundation, Inc.
#
# This file is part of the GNU MP Library.
#
# The GNU MP Library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Library General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# The GNU MP Library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
# License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
# MA 02111-1307, USA.


include(`../config.m4')


dnl  P6 UNROLL_COUNT cycles/limb
dnl          8           6.7
dnl         16           6.35
dnl         32           6.3
dnl         64           6.3
dnl  Maximum possible with the current code is 64.

deflit(UNROLL_COUNT, 16)


ifdef(`OPERATION_addmul_1', `
	define(M4_inst,        addl)
	define(M4_function_1,  mpn_addmul_1)
	define(M4_function_1c, mpn_addmul_1c)
	define(M4_description, add it to)
	define(M4_desc_retval, carry)
',`ifdef(`OPERATION_submul_1', `
	define(M4_inst,        subl)
	define(M4_function_1,  mpn_submul_1)
	define(M4_function_1c, mpn_submul_1c)
	define(M4_description, subtract it from)
	define(M4_desc_retval, borrow)
',`m4_error(`Need OPERATION_addmul_1 or OPERATION_submul_1
')')')

MULFUNC_PROLOGUE(mpn_addmul_1 mpn_addmul_1c mpn_submul_1 mpn_submul_1c)


`#' mp_limb_t M4_function_1 (mp_ptr dst, mp_srcptr src, mp_size_t size,
`#'                            mp_limb_t mult);
`#' mp_limb_t M4_function_1c (mp_ptr dst, mp_srcptr src, mp_size_t size,
`#'                             mp_limb_t mult, mp_limb_t carry);
`#'
`#' Calculate src,size multiplied by mult and M4_description dst,size.
`#' Return the M4_desc_retval limb from the top of the result.
#
# This code is pretty much the same as the K6 code.  The unrolled loop is
# the same, but there's just a few scheduling tweaks in the setups and the
# simple loop.
#
# A number of variations have been tried for the unrolled loop, with one or
# two carries, and with loads scheduled earlier, but nothing faster than 6
# cycles/limb has been found.

ifdef(`PIC',`
deflit(UNROLL_THRESHOLD, 5)
',`
deflit(UNROLL_THRESHOLD, 5)
')

defframe(PARAM_CARRY,     20)
defframe(PARAM_MULTIPLIER,16)
defframe(PARAM_SIZE,      12)
defframe(PARAM_SRC,       8)
defframe(PARAM_DST,       4)

	.text
	ALIGN(32)

PROLOGUE(M4_function_1c)
	pushl	%ebx
deflit(`FRAME',4)
	movl	PARAM_CARRY, %ebx
	jmp	LF(M4_function_1,start_nc)
EPILOGUE()

PROLOGUE(M4_function_1)
	push	%ebx
deflit(`FRAME',4)
	xorl	%ebx, %ebx	# initial carry

L(start_nc):
	movl	PARAM_SIZE, %ecx
	pushl	%esi
deflit(`FRAME',8)

	movl	PARAM_SRC, %esi
	pushl	%edi
deflit(`FRAME',12)

	movl	PARAM_DST, %edi
	pushl	%ebp
deflit(`FRAME',16)
	cmpl	$UNROLL_THRESHOLD, %ecx

	movl	PARAM_MULTIPLIER, %ebp
	jae	L(unroll)

	
	# simple loop
	# this is offset 0x22, so close enough to aligned
L(simple):
	# eax	scratch
	# ebx	carry
	# ecx	counter
	# edx	scratch
	# esi	src
	# edi	dst
	# ebp	multiplier

	movl	(%esi), %eax
	addl	$4, %edi

	mull	%ebp

	addl	%ebx, %eax
	adcl	$0, %edx

	M4_inst	%eax, -4(%edi)
	movl	%edx, %ebx

	adcl	$0, %ebx
	decl	%ecx

	leal	4(%esi), %esi
	jnz	L(simple)


	popl	%ebp
	popl	%edi

	popl	%esi
	movl	%ebx, %eax

	popl	%ebx
	ret



#------------------------------------------------------------------------------
# VAR_JUMP holds the computed jump temporarily because there's not enough
# registers when doing the mul for the initial two carry limbs.
#
# The add/adc for the initial carry in %ebx is necessary only for the
# mpn_add/submul_1c entry points.  Duplicating the startup code to
# eliminiate this for the plain mpn_add/submul_1 doesn't seem like a good
# idea.

dnl  overlapping with parameters already fetched
define(VAR_COUNTER,`PARAM_SIZE')
define(VAR_JUMP,   `PARAM_DST')

	# this is offset 0x43, so close enough to aligned
L(unroll):
	# eax
	# ebx	initial carry
	# ecx	size
	# edx
	# esi	src
	# edi	dst
	# ebp

	movl	%ecx, %edx
	decl	%ecx

	subl	$2, %edx
	negl	%ecx

	shrl	$UNROLL_LOG2, %edx
	andl	$UNROLL_MASK, %ecx

	movl	%edx, VAR_COUNTER
	movl	%ecx, %edx

	# 15 code bytes per limb
ifdef(`PIC',`
	call	L(pic_calc)
L(here):
',`
	shll	$4, %edx
	negl	%ecx

	leal	L(entry) (%edx,%ecx,1), %edx
')
	movl	(%esi), %eax		# src low limb

	movl	%edx, VAR_JUMP
	leal	ifelse(UNROLL_BYTES,256,128+) 4(%esi,%ecx,4), %esi

	mull	%ebp

	addl	%ebx, %eax	# initial carry (from _1c)
	adcl	$0, %edx

	movl	%edx, %ebx	# high carry
	leal	ifelse(UNROLL_BYTES,256,128) (%edi,%ecx,4), %edi

	movl	VAR_JUMP, %edx
	testl	$1, %ecx
	movl	%eax, %ecx	# low carry

	cmovnz_ebx_ecx		# high,low carry other way around
	cmovnz_eax_ebx

	jmp	*%edx


ifdef(`PIC',`
L(pic_calc):
	shll	$4, %edx
	negl	%ecx

	# See README.family about old gas bugs
	leal	(%edx,%ecx,1), %edx
	addl	$L(entry)-L(here), %edx

	addl	(%esp), %edx

	ret
')


# -----------------------------------------------------------
	ALIGN(32)
L(top):
deflit(`FRAME',16)
	# eax	scratch
	# ebx	carry hi
	# ecx	carry lo
	# edx	scratch
	# esi	src
	# edi	dst
	# ebp	multiplier
	#
	# VAR_COUNTER	loop counter
	#
	# 15 code bytes per limb

	addl	$UNROLL_BYTES, %edi

L(entry):
deflit(CHUNK_COUNT,2)
forloop(`i', 0, UNROLL_COUNT/CHUNK_COUNT-1, `
	deflit(`disp0', eval(i*4*CHUNK_COUNT ifelse(UNROLL_BYTES,256,-128)))
	deflit(`disp1', eval(disp0 + 4))

Zdisp(	movl,	disp0,(%esi), %eax)
	mull	%ebp
Zdisp(	M4_inst,%ecx, disp0,(%edi))
	adcl	%eax, %ebx
	movl	%edx, %ecx
	adcl	$0, %ecx

	movl	disp1(%esi), %eax
	mull	%ebp
	M4_inst	%ebx, disp1(%edi)
	adcl	%eax, %ecx
	movl	%edx, %ebx
	adcl	$0, %ebx
')

	decl	VAR_COUNTER
	leal	UNROLL_BYTES(%esi), %esi

	jns	L(top)


deflit(`disp0',	eval(UNROLL_BYTES ifelse(UNROLL_BYTES,256,-128)))

	M4_inst	%ecx, disp0(%edi)
	movl	%ebx, %eax

	popl	%ebp
	popl	%edi

	popl	%esi
	popl	%ebx
	adcl	$0, %eax

	ret

EPILOGUE()
