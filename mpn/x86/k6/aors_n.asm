# AMD K6 mpn_add/sub_n -- mpn addition or subtraction.
#
# K6: 3.2 cycles/limb, or in-place 2.9 cycles/limb (at 16 limbs per loop),
# PIC adds 3 cycles at the start.


# Copyright (C) 1999-2000 Free Software Foundation, Inc.
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
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


include(`../config.m4')


dnl  K6                cycles/limb
dnl  UNROLL_COUNT  separate  in-place
dnl       8           3.5      3.0
dnl      16           3.2      2.9
dnl      32           3.2      2.95
dnl      64           3.18     2.9
dnl  Maximum possible with the current code is 64.

deflit(UNROLL_COUNT, 16)


ifdef(`OPERATION_add_n', `
	define(M4_inst,        adcl)
	define(M4_function_n,  mpn_add_n)
	define(M4_function_nc, mpn_add_nc)
	define(M4_description, add)
	define(M4_bitdesc,     carry)
',`ifdef(`OPERATION_sub_n', `
	define(M4_inst,        sbbl)
	define(M4_function_n,  mpn_sub_n)
	define(M4_function_nc, mpn_sub_nc)
	define(M4_description, subtract)
	define(M4_bitdesc,     borrow)
',`m4_error(`Need OPERATION_add_n or OPERATION_sub_n
')')')


`#' mp_limb_t M4_function_n (mp_ptr wp, mp_srcptr src1, mp_srcptr src2,
`#'                            mp_size_t size);
`#' mp_limb_t M4_function_nc (mp_ptr dst, mp_srcptr src1, mp_srcptr src2,
`#'                             mp_size_t size, mp_limb_t carry);
`#'
`#' Calculate src1,size M4_description src2,size, and store the result in
`#' dst,size.  The return value is the M4_bitdesc bit from the top of the
`#' result (1 or 0).
`#'
`#' The _nc version accepts 1 or 0 for an initial M4_bitdesc into the low limb
# of the calculation.  Note values other than 1 or 0 will lead to garbage
# results.
#
# In the unrolled loop, the adcl seems to take two cycles of decode, and
# since it needs at least one more instruction, the current 3 cycles/limb is
# as good as possible with this technique.
#
# The in-place unrolled loop looks like it should be 2.5 cycles/limb but
# comes out about 2.9.  Maybe code fetching causes this.

ifdef(`PIC',`
deflit(UNROLL_THRESHOLD, 10)
',`
deflit(UNROLL_THRESHOLD, 7)
')

defframe(PARAM_CARRY,20)
defframe(PARAM_SIZE, 16)
defframe(PARAM_SRC2, 12)
defframe(PARAM_SRC1, 8)
defframe(PARAM_DST,  4)
deflit(`FRAME',0)

	.text

	ALIGN(32)
PROLOGUE(M4_function_nc)
	movl	PARAM_CARRY, %eax
	jmp	LF(M4_function_n,start)
EPILOGUE()


PROLOGUE(M4_function_n)
	xorl	%eax, %eax	# carry
L(start):
	movl	PARAM_SIZE, %ecx

	movl	PARAM_SRC2, %edx
	pushl	%ebx
deflit(`FRAME',4)

	movl	PARAM_SRC1, %ebx
	cmpl	$UNROLL_THRESHOLD, %ecx

	pushl	%edi
deflit(`FRAME',8)
	jae	L(unroll)

	shrl	%eax
	movl	PARAM_DST, %edi

	# this is offset 0x22, so it's close enough to aligned
L(simple):
	# eax	scratch
	# ebx	src1
	# ecx	counter
	# edx	src2
	# esi
	# edi	dst
	# ebp
	#
	# stosl here is the same speed as a separate movl and leal, and it
        # takes fewer code bytes.

	movl	(%ebx), %eax
	leal	4(%ebx), %ebx
	
	M4_inst	(%edx), %eax

	stosl

	leal	4(%edx), %edx
	loop	L(simple)

	
	movl	$0, %eax
	popl	%edi

	setc	%al

	popl	%ebx
	ret


#-----------------------------------------------------------------
L(unroll):
deflit(`FRAME',8)
	movl	PARAM_DST, %edi
	pushl	%esi
deflit(`FRAME',12)

	cmpl	%ebx, %edi
	je	L(inplace)

ifdef(`OPERATION_add_n',`
	# reverse overlap can be handled in add
	cmpl	%edx, %edi
	je	L(inplace_reverse)
')

	movl	%ecx, %edi
	decl	%ecx

	shrl	$UNROLL_LOG2, %ecx
	negl	%edi

	andl	$UNROLL_MASK, %edi

ifdef(`PIC',`
	call	L(pic_calc)
L(pic_ret):
',`
	leal	L(entry) (%edi,%edi,8), %esi
')
	negl	%edi

	# the following will decode as shown, but only catch up later

	leal	ifelse(UNROLL_BYTES,256,128) (%ebx,%edi,4), %ebx
	leal	ifelse(UNROLL_BYTES,256,128) (%edx,%edi,4), %edx

	shll	$2, %edi
	addl	PARAM_DST, %edi
	ifelse(UNROLL_BYTES,256,`addl $128, %edi')

	shrl	%eax
	jmp	*%esi



#---------------------------------------------------------------------
# This is within range of a short jump for the above tests.

	ALIGN(16)
deflit(`FRAME',12)

ifdef(`OPERATION_add_n',`
L(inplace_reverse):
	movl	%ebx, %edx
')

L(inplace):
	# eax
	# ebx	(has been saved)
	# ecx	size
	# edx	src
	# esi	(has been saved)
	# edi	dst
	# ebp

	movl	%ecx, %ebx
	andl	$~1, %ecx	# handle even num in loop

	pushl	%ebp
deflit(`FRAME',16)
	movl	%ecx, %ebp

	negl	%ebp
	andl	$1, %ebx	# the remaining 0 or 1

	andl	$UNROLL_MASK, %ebp
	decl	%ecx

	addl	%ebp, %ebp

ifdef(`PIC',`
	call	L(inplace_pic_calc)
L(inplace_pic_ret):
',`
	leal	L(inplace_entry) (%ebp,%ebp,2), %esi
')
	negl	%ebp

	shrl	$UNROLL_LOG2, %ecx
	leal	ifelse(UNROLL_BYTES,256,128) (%edx,%ebp,2), %edx

	shrl	%eax
	leal	ifelse(UNROLL_BYTES,256,128) (%edi,%ebp,2), %edi

	jmp	*%esi



#--------------------------------------------------------------------
# needs to be 32-byte aligned to get the speed claimed
	ALIGN(32)
L(top):
	# eax	zero
	# ebx	src1
	# ecx	counter
	# edx	src2
	# esi	scratch
	# edi	dst
	# ebp
	#
	# 9 code bytes per limb

	leal	UNROLL_BYTES(%edx), %edx
	leal	UNROLL_BYTES(%edi), %edi

L(entry):
forloop(`i', 0, UNROLL_COUNT-1, `
	deflit(`disp', eval(i*4 ifelse(UNROLL_BYTES,256,-128)))

	movl	disp(%ebx), %esi
	M4_inst	disp(%edx), %esi
	movl	%esi, disp(%edi)
')

	leal	UNROLL_BYTES(%ebx), %ebx

	decl	%ecx
	jns	L(top)


	setc	%al

	popl	%esi
	popl	%edi

	popl	%ebx
	ret


ifdef(`PIC',`
L(pic_calc):
	leal	L(entry)-L(pic_ret) (%edi,%edi,8), %esi
	addl	(%esp), %esi
	ret
')


#----------------------------------------------------------------------
ifdef(`PIC',`
L(inplace_pic_calc):
	leal	L(inplace_entry)-L(inplace_pic_ret) (%ebp,%ebp,2), %esi
	addl	(%esp), %esi
	ret
')

	# needs to be 32-byte aligned to get the speed claimed
	ALIGN(32)
L(inplace_top):
	# eax	zero
	# ebx	0 or 1 leftover limbs
	# ecx	counter
	# edx	src
	# esi	scratch
	# edi	dst
	# ebp	scratch
	#
	# 12 code bytes per 2 limbs

	leal	UNROLL_BYTES(%edx), %edx
	leal	UNROLL_BYTES(%edi), %edi

L(inplace_entry):
deflit(`CHUNK_COUNT',2)
forloop(`i', 0, UNROLL_COUNT/CHUNK_COUNT-1, `
	deflit(`disp0', eval(i*CHUNK_COUNT*4 ifelse(UNROLL_BYTES,256,-128)))
	deflit(`disp1', eval(disp0 + 4))

	movl	disp0(%edx), %esi
	movl	disp1(%edx), %ebp
	M4_inst	%esi, disp0(%edi)
	M4_inst	%ebp, disp1(%edi)
')

	decl	%ecx
	jns	L(inplace_top)


deflit(`disp0', eval(UNROLL_BYTES ifelse(UNROLL_BYTES,256,-128)))
	decl	%ebx	# 0 or 1 left over
	popl	%ebp

	jnz	L(inplace_even)
	movl	disp0(%edx), %ecx

	M4_inst	%ecx, disp0(%edi)
L(inplace_even):

	popl	%esi
	popl	%edi

	setc	%al

	popl	%ebx

	ret

EPILOGUE()
