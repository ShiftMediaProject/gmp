# AMD K6 mpn_addmul_1/mpn_submul_1 -- add or subtract mpn multiple.
#
# K6: 7.65 to 8.5 cycles/limb (at 16 limbs/loop and depending on the data),
# PIC adds about 6 cycles at the start.


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


dnl  K6:           large multpliers  small multpliers
dnl  UNROLL_COUNT    cycles/limb       cycles/limb
dnl        4             9.5              7.78
dnl        8             9.0              7.78
dnl       16             8.4              7.65
dnl       32             8.4              8.2
dnl
dnl  Maximum possible unrolling with the current code is 32.
dnl
dnl  Unrolling to 16 limbs/loop makes the unrolled loop fit exactly in a 256
dnl  byte block, which might explain the good speed at that unrolling.

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
`#'                          mp_limb_t mult);
`#' mp_limb_t M4_function_1c (mp_ptr dst, mp_srcptr src, mp_size_t size,
`#'                           mp_limb_t mult, mp_limb_t carry);
`#'
`#' Calculate src,size multiplied by mult and M4_description dst,size.
`#' Return the M4_desc_retval limb from the top of the result.
#
# The jadcl0()s in the unrolled loop makes the speed data dependent.  Small
# multipliers (most significant few bits clear) result in few carry bits and
# speeds up to 7.65 cycles/limb are attained.  Large multipliers (most
# significant few bits set) make the carry bits 50/50 and lead to something
# more like 8.4 c/l.  (With adcl's both of these would be 9.3 c/l.)
#
# It's important that the gains for jadcl0 on small multipliers don't come
# at the cost of slowing down other data.  Tests on uniformly distributed
# random data, designed to confound branch prediction, show about a 7%
# speed-up using jadcl0 over adcl (8.93 versus 9.57 cycles/limb, with all
# overheads included).
#
# In the simple loop, jadcl0() measures slower than adcl (11.9-14.7 versus
# 11.0 cycles/limb), and hence isn't used.
#
# In the simple loop, note that running ecx from negative to zero and using
# it as an index in the two movs wouldn't help.  It would save one
# instruction (2*addl+loop becoming incl+jnz), but there's nothing unpaired
# that would be collapsed by this.
#
#
# jadcl0
# ------
#
# jadcl0() being faster than adcl $0 seems to be an artifact of two things,
# firstly the instruction decoding and secondly the fact that there's a
# carry bit for the jadcl0 only on average about 1/4 of the time.
#
# The code in the unrolled loop decodes something like the following.
#
#                                         decode cycles
#		mull	%ebp                    2
#		M4_inst	%esi, disp(%edi)        1
#		adcl	%eax, %ecx              2
#		movl	%edx, %esi            \ 1
#		jnc	1f                    /
#		incl	%esi                  \ 1
#	1:	movl	disp(%ebx), %eax      /
#                                              ---
#                                               7
#
# In a back-to-back style test this measures 7 with the jnc not taken, or 8
# with it taken (both when correctly predicted).  This is opposite to the
# measurements showing small multipliers running faster than large ones.
# Watch this space for more info ...
#
# It's not clear how much branch misprediction might be costing.  The K6
# doco says it will be 1 to 4 cycles, but presumably it's near the low end
# of that range to get the measured results.
#
#
# In the code the two carries are more or less the preceding mul product and
# the calculation is roughly
#
#	x*y + u*b+v
#
# where b=2^32 is the size of a limb, x*y is the two carry limbs, and u and
# v are the two limbs it's added to (being the low of the next mul, and a
# limb from the destination).
#
# To get a carry requires x*y+u*b+v >= b^2, which is u*b+v >= b^2-x*y, and
# there are b^2-(b^2-x*y) = x*y many such values, giving a probability of
# x*y/b^2.  If x, y, u and v are random and uniformly distributed between 0
# and b-1, then the total probability can be summed over x and y,
#
#	 1    b-1 b-1 x*y    1    b*(b-1)   b*(b-1)
#	--- * sum sum --- = --- * ------- * ------- = 1/4
#       b^2   x=0 y=1 b^2   b^4      2         2
#
# Actually it's a very tiny bit less than 1/4 of course.  If y is fixed,
# then the probability is 1/2*y/b thus varying linearly between 0 and 1/2.


ifdef(`PIC',`
deflit(UNROLL_THRESHOLD, 9)
',`
deflit(UNROLL_THRESHOLD, 6)
')

defframe(PARAM_CARRY,     20)
defframe(PARAM_MULTIPLIER,16)
defframe(PARAM_SIZE,      12)
defframe(PARAM_SRC,       8)
defframe(PARAM_DST,       4)

	.text
	ALIGN(32)

PROLOGUE(M4_function_1c)
	pushl	%esi
deflit(`FRAME',4)
	movl	PARAM_CARRY, %esi
	jmp	LF(M4_function_1,start_nc)
EPILOGUE()

PROLOGUE(M4_function_1)
	push	%esi
deflit(`FRAME',4)
	xorl	%esi, %esi	# initial carry

L(start_nc):
	movl	PARAM_SIZE, %ecx
	pushl	%ebx
deflit(`FRAME',8)

	movl	PARAM_SRC, %ebx
	pushl	%edi
deflit(`FRAME',12)

	cmpl	$UNROLL_THRESHOLD, %ecx
	movl	PARAM_DST, %edi

	pushl	%ebp
deflit(`FRAME',16)
	jae	L(unroll)

	
	# simple loop

	movl	PARAM_MULTIPLIER, %ebp

L(simple):
	# eax	scratch
	# ebx	src
	# ecx	counter
	# edx	scratch
	# esi	carry
	# edi	dst
	# ebp	multiplier

	movl	(%ebx), %eax
	addl	$4, %ebx

	mull	%ebp

	addl	$4, %edi
	addl	%esi, %eax

	adcl	$0, %edx

	M4_inst	%eax, -4(%edi)

	adcl	$0, %edx

	movl	%edx, %esi
	loop	L(simple)


	popl	%ebp
	popl	%edi

	popl	%ebx
	movl	%esi, %eax

	popl	%esi
	ret



#------------------------------------------------------------------------------
# The unrolled loop uses a "two carry limbs" scheme.  At the top of the loop
# the carries are ecx=lo, esi=hi, then they swap for each limb processed.
# For the computed jump an odd size means they start one way around, an even
# size the other.
#
# VAR_JUMP holds the computed jump temporarily because there's not enough
# registers at the point of doing the mul for the initial two carry limbs.
#
# The add/adc for the initial carry in %esi is necessary only for the
# mpn_addmul/submul_1c entry points.  Duplicating the startup code to
# eliminiate this for the plain mpn_add/submul_1 doesn't seem like a good
# idea.

dnl  overlapping with parameters already fetched
define(VAR_COUNTER, `PARAM_SIZE')
define(VAR_JUMP,    `PARAM_DST')

L(unroll):
	# eax
	# ebx	src
	# ecx	size
	# edx
	# esi	initial carry
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

	shll	$4, %edx
	negl	%ecx

	# 15 code bytes per limb
ifdef(`PIC',`
	call	L(pic_calc)
L(here):
',`
	leal	L(entry) (%edx,%ecx,1), %edx
')
	movl	(%ebx), %eax		# src low limb

	movl	PARAM_MULTIPLIER, %ebp
	movl	%edx, VAR_JUMP

	mull	%ebp

	addl	%esi, %eax	# initial carry (from _1c)
	jadcl0(	%edx)


	leal	4(%ebx,%ecx,4), %ebx
	movl	%edx, %esi  # high carry

	movl	VAR_JUMP, %edx
	leal	(%edi,%ecx,4), %edi

	testl	$1, %ecx
	movl	%eax, %ecx  # low carry

	jz	L(noswap)
	movl	%esi, %ecx  # high,low carry other way around

	movl	%eax, %esi
L(noswap):

	jmp	*%edx


ifdef(`PIC',`
L(pic_calc):
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
	# ebx	src
	# ecx	carry lo
	# edx	scratch
	# esi	carry hi
	# edi	dst
	# ebp	multiplier
	#
	# 15 code bytes per limb

	leal	UNROLL_BYTES(%edi), %edi

L(entry):
forloop(`i', 0, UNROLL_COUNT/2-1, `
	deflit(`disp0', eval(2*i*4))
	deflit(`disp1', eval(disp0 + 4))

Zdisp(	movl,	disp0,(%ebx), %eax)
	mull	%ebp
Zdisp(	M4_inst,%ecx, disp0,(%edi))
	adcl	%eax, %esi
	movl	%edx, %ecx
	jadcl0(	%ecx)

	movl	disp1(%ebx), %eax
	mull	%ebp
	M4_inst	%esi, disp1(%edi)
	adcl	%eax, %ecx
	movl	%edx, %esi
	jadcl0(	%esi)
')

	decl	VAR_COUNTER
	leal	UNROLL_BYTES(%ebx), %ebx

	jns	L(top)


	popl	%ebp
	M4_inst	%ecx, UNROLL_BYTES(%edi)

	popl	%edi
	movl	%esi, %eax

	popl	%ebx
	jadcl0(	%eax)

	popl	%esi
	ret

EPILOGUE()
