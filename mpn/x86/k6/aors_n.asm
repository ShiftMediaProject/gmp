# AMD K6 mpn_add/sub_n -- mpn addition or subtraction.
#
# K6: normal 3.25 cycles/limb, in-place 2.75 cycles/limb.


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


ifdef(`OPERATION_add_n', `
	define(M4_inst,        adcl)
	define(M4_function_n,  mpn_add_n)
	define(M4_function_nc, mpn_add_nc)
	define(M4_description, add)
',`ifdef(`OPERATION_sub_n', `
	define(M4_inst,        sbbl)
	define(M4_function_n,  mpn_sub_n)
	define(M4_function_nc, mpn_sub_nc)
	define(M4_description, subtract)
',`m4_error(`Need OPERATION_add_n or OPERATION_sub_n
')')')

MULFUNC_PROLOGUE(mpn_add_n mpn_add_nc mpn_sub_n mpn_sub_nc)


`#' mp_limb_t M4_function_n (mp_ptr dst, mp_srcptr src1, mp_srcptr src2,
`#'                          mp_size_t size);
`#' mp_limb_t M4_function_nc (mp_ptr dst, mp_srcptr src1, mp_srcptr src2,
`#'	                      mp_size_t size, mp_limb_t carry);
`#'
`#' Calculate src1,size M4_description src2,size, and store the result in
`#' dst,size.  The return value is the carry bit from the top of the result
`#' (1 or 0).
#
# The _nc version accepts 1 or 0 for an initial carry into the low limb of
# the calculation.  Note values other than 1 or 0 here will lead to garbage
# results.
#
# Instruction decoding limits a normal dst=src1+src2 operation to 3 c/l, and
# an in-place dst+=src to 2.5 c/l.  The unrolled loops have 1 cycle/loop of
# loop control, which with 4 limbs/loop means an extra 0.25 c/l.

define(PARAM_CARRY, `FRAME+20(%esp)')
define(PARAM_SIZE,  `FRAME+16(%esp)')
define(PARAM_SRC2,  `FRAME+12(%esp)')
define(PARAM_SRC1,  `FRAME+8(%esp)')
define(PARAM_DST,   `FRAME+4(%esp)')
deflit(`FRAME',0)

dnl  minimum 5 because the unrolled code can't handle less
deflit(UNROLL_THRESHOLD, 5)

	.text
	ALIGN(32)

PROLOGUE(M4_function_nc)
	movl	PARAM_CARRY, %eax
	jmp	LF(M4_function_n,start)
EPILOGUE()


PROLOGUE(M4_function_n)
	xorl	%eax, %eax
L(start):
	movl	PARAM_SIZE, %ecx
	pushl	%ebx
FRAME_pushl()

	movl	PARAM_SRC1, %ebx
	pushl	%edi
FRAME_pushl()

	movl	PARAM_SRC2, %edx
	cmpl	$UNROLL_THRESHOLD, %ecx

	movl	PARAM_DST, %edi
	jae	L(unroll)


	shrl	%eax		# initial carry flag

	# offset 0x21 here, close enough to aligned
L(simple):
	# eax	scratch
	# ebx	src1
	# ecx	counter
	# edx	src2
	# esi
	# edi	dst
	# ebp
	#
	# The store to (%edi) could be done with a stosl; it'd be smaller
	# code, but there's no speed gain and a cld would have to be added
	# (per mpn/x86/README.family).

	movl	(%ebx), %eax
	leal	4(%ebx), %ebx
	
	M4_inst	(%edx), %eax

	movl	%eax, (%edi)
	leal	4(%edi), %edi

	leal	4(%edx), %edx
	loop	L(simple)

	
	movl	$0, %eax
	popl	%edi

	setc	%al

	popl	%ebx
	ret


#------------------------------------------------------------------------------
L(unroll):
	# eax	carry
	# ebx	src1
	# ecx	counter
	# edx	src2
	# esi
	# edi	dst
	# ebp

	cmpl	%edi, %ebx
	pushl	%esi

	je	L(inplace)

ifdef(`OPERATION_add_n',`
	cmpl	%edi, %edx

	je	L(inplace_reverse)
')

	movl	%ecx, %esi

	andl	$~3, %ecx
	andl	$3, %esi

	leal	(%ebx,%ecx,4), %ebx
	leal	(%edx,%ecx,4), %edx
	leal	(%edi,%ecx,4), %edi

	negl	%ecx
	shrl	%eax

	ALIGN(32)
L(normal_top):
	# eax	counter, qwords, negative
	# ebx	src1
	# ecx	scratch
	# edx	src2
	# esi
	# edi	dst
	# ebp

 	movl	(%ebx,%ecx,4), %eax
	leal	5(%ecx), %ecx
 	M4_inst	-20(%edx,%ecx,4), %eax
 	movl	%eax, -20(%edi,%ecx,4)

 	movl	4-20(%ebx,%ecx,4), %eax
 	M4_inst	4-20(%edx,%ecx,4), %eax
 	movl	%eax, 4-20(%edi,%ecx,4)

 	movl	8-20(%ebx,%ecx,4), %eax
 	M4_inst	8-20(%edx,%ecx,4), %eax
 	movl	%eax, 8-20(%edi,%ecx,4)

 	movl	12-20(%ebx,%ecx,4), %eax
 	M4_inst	12-20(%edx,%ecx,4), %eax
 	movl	%eax, 12-20(%edi,%ecx,4)

	loop	L(normal_top)


	decl	%esi
	jz	L(normal_finish_one)
	js	L(normal_done)

	# two or three more limbs

 	movl	(%ebx), %eax
 	M4_inst	(%edx), %eax
 	movl	%eax, (%edi)

 	movl	4(%ebx), %eax
 	M4_inst	4(%edx), %eax
	decl	%esi
 	movl	%eax, 4(%edi)

	jz	L(normal_done)
	movl	$2, %ecx

L(normal_finish_one):
 	movl	(%ebx,%ecx,4), %eax
 	M4_inst	(%edx,%ecx,4), %eax
 	movl	%eax, (%edi,%ecx,4)

L(normal_done):	
	popl	%esi
	popl	%edi

	movl	$0, %eax
	popl	%ebx

	setc	%al

	ret


#------------------------------------------------------------------------------

ifdef(`OPERATION_add_n',`
L(inplace_reverse):
	# dst==src2

	movl	%ebx, %edx
')

L(inplace):
	# eax	initial carry
	# ebx
	# ecx	size
	# edx	src
	# esi
	# edi	dst
	# ebp

	leal	-1(%ecx), %esi
	decl	%ecx

	andl	$~3, %ecx
	andl	$3, %esi

 	movl	(%edx), %ebx		# src low limb
	leal	(%edx,%ecx,4), %edx

	leal	(%edi,%ecx,4), %edi
	negl	%ecx

	shrl	%eax


	ALIGN(32)
L(inplace_top):
	# eax
	# ebx	next src limb
	# ecx	size
	# edx	src
	# esi
	# edi	dst
	# ebp

 	M4_inst	%ebx, (%edi,%ecx,4)

 	movl	4(%edx,%ecx,4), %eax
	leal	5(%ecx), %ecx

 	M4_inst	%eax, 4-20(%edi,%ecx,4)

 	movl	8-20(%edx,%ecx,4), %eax
 	movl	12-20(%edx,%ecx,4), %ebx

 	M4_inst	%eax, 8-20(%edi,%ecx,4)
 	M4_inst	%ebx, 12-20(%edi,%ecx,4)

 	movl	16-20(%edx,%ecx,4), %ebx
	loop	L(inplace_top)


	# now %esi is 0 to 3 representing respectively 1 to 4 limbs more

 	M4_inst	%ebx, (%edi)

	decl	%esi
	jz	L(inplace_finish_one)
	js	L(inplace_done)

	# two or three more limbs

 	movl	4(%edx), %eax
 	movl	8(%edx), %ebx
 	M4_inst	%eax, 4(%edi)
 	M4_inst	%ebx, 8(%edi)

	decl	%esi
	movl	$2, %ecx

	jz	L(normal_done)

L(inplace_finish_one):
 	movl	4(%edx,%ecx,4), %eax
 	M4_inst	%eax, 4(%edi,%ecx,4)

L(inplace_done):	
	popl	%esi
	popl	%edi

	movl	$0, %eax
	popl	%ebx

	setc	%al

	ret

EPILOGUE()
