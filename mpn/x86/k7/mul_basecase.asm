# AMD K7 mpn_mul_basecase -- multiply two mpn numbers.
#
# K7: approx 4.42 cycles per cross product at around 20x20 limbs (16
#     limbs/loop unrolling).


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


dnl  K7 UNROLL_COUNT cycles/product (at around 20x20)
dnl           8           4.67    
dnl          16           4.59
dnl          32           4.42
dnl  Maximum possible with the current code is 32.
dnl
dnl  At 32 the typical 13-26 limb sizes from the karatsuba code will get
dnl  done with a straight run through a block of code, no inner loop.  Using
dnl  32 gives 1k of code, but the k7 has a 64k L1 code cache.

deflit(UNROLL_COUNT, 32)


# void mpn_mul_basecase (mp_ptr wp,
#                        mp_srcptr xp, mp_size_t xsize,
#                        mp_srcptr yp, mp_size_t ysize);
#
# Calculate xp,xsize multiplied by yp,ysize, storing the result in
# wp,xsize+ysize.
#
# This routine is essentially the same as mpn/generic/mul_basecase.c, but
# it's faster because it does most of the mpn_addmul_1() startup
# calculations only once.  The saving is 15-25% on typical sizes coming from
# the Karatsuba multiply code.

ifdef(`PIC',`
deflit(UNROLL_THRESHOLD, 5)
',`
deflit(UNROLL_THRESHOLD, 5)
')

defframe(PARAM_YSIZE,20)
defframe(PARAM_YP,   16)
defframe(PARAM_XSIZE,12)
defframe(PARAM_XP,   8)
defframe(PARAM_WP,   4)

	.text
	ALIGN(32)
PROLOGUE(mpn_mul_basecase)
deflit(`FRAME',0)

	movl	PARAM_XSIZE, %ecx
	movl	PARAM_YP, %eax

	movl	PARAM_XP, %edx
	movl	(%eax), %eax	# yp low limb

	cmpl	$2, %ecx
	ja	L(xsize_more_than_two)
	je	L(two_by_something)


	# one limb by one limb

	mull	(%edx)

	movl	PARAM_WP, %ecx
	movl	%eax, (%ecx)
	movl	%edx, 4(%ecx)
	ret


#------------------------------------------------------------------------------
L(two_by_something):
deflit(`FRAME',0)
	decl	PARAM_YSIZE
	pushl	%ebx		defframe_pushl(`SAVE_EBX')
	movl	%eax, %ecx	# yp low limb

	movl	PARAM_WP, %ebx
	pushl	%esi		defframe_pushl(`SAVE_ESI')
	movl	%edx, %esi	# xp

	movl	(%edx), %eax	# xp low limb	
	jnz	L(two_by_two)


	# two limbs by one limb

	mull	%ecx	

	movl	%eax, (%ebx)
	movl	4(%esi), %eax
	movl	%edx, %esi	# carry

	mull	%ecx

	addl	%eax, %esi

	movl	%esi, 4(%ebx)
	movl	SAVE_ESI, %esi

	adcl	$0, %edx

	movl	%edx, 8(%ebx)
	movl	SAVE_EBX, %ebx
	addl	$FRAME, %esp

	ret
	


#------------------------------------------------------------------------------
# Could load yp earlier into another register.

	ALIGN(16)
L(two_by_two):
	# eax	xp low limb
	# ebx	wp
	# ecx	yp low limb
	# edx
	# esi	xp
	# edi
	# ebp

dnl  FRAME carries on from previous

	mull	%ecx		# xp[0] * yp[0]

	push	%edi		defframe_pushl(`SAVE_EDI')
	movl	%edx, %edi	# carry, for wp[1]

	movl	%eax, (%ebx)
	movl	4(%esi), %eax

	mull	%ecx		# xp[1] * yp[0]

	addl	%eax, %edi
	movl	PARAM_YP, %ecx

	adcl	$0, %edx
	movl	4(%ecx), %ecx	# yp[1]
	movl	%edi, 4(%ebx)

	movl	4(%esi), %eax	# xp[1]
	movl	%edx, %edi	# carry, for wp[2]

	mull	%ecx		# xp[1] * yp[1]

	addl	%eax, %edi

	adcl	$0, %edx
	movl	(%esi), %eax	# xp[0]

	movl	%edx, %esi	# carry, for wp[3]

	mull	%ecx		# xp[0] * yp[1]

	addl	%eax, 4(%ebx)
	adcl	%edx, %edi
	movl	%edi, 8(%ebx)

	adcl	$0, %esi
	movl	SAVE_EDI, %edi
	movl	%esi, 12(%ebx)

	movl	SAVE_ESI, %esi
	movl	SAVE_EBX, %ebx
	addl	$FRAME, %esp

	ret

	
#------------------------------------------------------------------------------
	ALIGN(16)
L(xsize_more_than_two):

# The first limb of yp is processed with a simple mpn_mul_1 style loop
# inline.  Unrolling this doesn't seem worthwhile since it's only run once
# (whereas the addmul below is run ysize-1 many times).  A call to the
# actual mpn_mul_1 will be slowed down by the call and parameter pushing and
# popping, and doesn't seem likely to be worthwhile on the typical 13-26
# limb operations the Karatsuba code calls here with.

	# eax	yp[0]
	# ebx
	# ecx	xsize
	# edx	xp
	# esi
	# edi
	# ebp

dnl  FRAME doesn't carry on from previous, no pushes yet here
defframe(`SAVE_EBX',-4)
defframe(`SAVE_ESI',-8)
defframe(`SAVE_EDI',-12)
defframe(`SAVE_EBP',-16)
deflit(`FRAME',0)

	subl	$16, %esp
deflit(`FRAME',16)

	movl	%edi, SAVE_EDI
	movl	PARAM_WP, %edi

	movl	%ebx, SAVE_EBX
	movl	%ebp, SAVE_EBP
	movl	%eax, %ebp

	movl	%esi, SAVE_ESI
	xorl	%ebx, %ebx
	leal	(%edx,%ecx,4), %esi	# xp end

	leal	(%edi,%ecx,4), %edi	# wp end of mul1
	negl	%ecx


L(mul1):
	# eax	scratch
	# ebx	carry
	# ecx	counter, negative
	# edx	scratch
	# esi	xp end
	# edi	wp end of mul1
	# ebp	multiplier

	movl	(%esi,%ecx,4), %eax

	mull	%ebp

	addl	%ebx, %eax
	movl	%eax, (%edi,%ecx,4)
	movl	$0, %ebx

	adcl	%edx, %ebx
	incl	%ecx
	jnz	L(mul1)


	movl	PARAM_YSIZE, %edx
	movl	PARAM_XSIZE, %ecx

	movl	%ebx, (%edi)		# final carry
	decl	%edx

	jnz	L(ysize_more_than_one)


	movl	SAVE_EDI, %edi
	movl	SAVE_EBX, %ebx

	movl	SAVE_EBP, %ebp
	movl	SAVE_ESI, %esi
	addl	$FRAME, %esp

	ret


L(ysize_more_than_one):
	cmpl	$UNROLL_THRESHOLD, %ecx
	movl	PARAM_YP, %eax

	jae	L(unroll)


#------------------------------------------------------------------------------
	# simple addmul looping
	#
	# eax	yp
	# ebx
	# ecx	xsize
	# edx	ysize-1
	# esi	xp end
	# edi	wp end of mul1
	# ebp

	leal	4(%eax,%edx,4), %ebp	# yp end
	negl	%ecx
	negl	%edx

	movl	(%esi,%ecx,4), %eax	# xp low limb
	movl	%edx, PARAM_YSIZE	# -(ysize-1)
	incl	%ecx

	xorl	%ebx, %ebx		# initial carry
	movl	%ecx, PARAM_XSIZE	# -(xsize-1)
	movl	%ebp, PARAM_YP

	movl	(%ebp,%edx,4), %ebp	# yp second lowest limb - multiplier
	jmp	L(simple_outer_entry)


	# this is offset 0x121 so close enough to aligned
L(simple_outer_top):	
	# ebp	ysize counter, negative

	movl	PARAM_YP, %edx
	movl	PARAM_XSIZE, %ecx	# -(xsize-1)
	xorl	%ebx, %ebx		# carry

	movl	%ebp, PARAM_YSIZE
	addl	$4, %edi		# next position in wp

	movl	(%edx,%ebp,4), %ebp	# yp limb - multiplier
	movl	-4(%esi,%ecx,4), %eax	# xp low limb


L(simple_outer_entry):

L(simple_inner):
	# eax	xp limb
	# ebx	carry limb
	# ecx	loop counter (negative)
	# edx	scratch
	# esi	xp end
	# edi	wp end
	# ebp	multiplier

	mull	%ebp

	addl	%eax, %ebx
	adcl	$0, %edx

	addl	%ebx, (%edi,%ecx,4)
	movl	(%esi,%ecx,4), %eax
	adcl	$0, %edx

	incl	%ecx
	movl	%edx, %ebx
	jnz	L(simple_inner)


	mull	%ebp

	movl	PARAM_YSIZE, %ebp
	addl	%eax, %ebx

	adcl	$0, %edx
	addl	%ebx, (%edi)

	adcl	$0, %edx
	incl	%ebp

	movl	%edx, 4(%edi)
	jnz	L(simple_outer_top)


	movl	SAVE_EBX, %ebx
	movl	SAVE_ESI, %esi

	movl	SAVE_EDI, %edi
	movl	SAVE_EBP, %ebp
	addl	$FRAME, %esp

	ret



#------------------------------------------------------------------------------
#
# The unrolled loop is the same as in mpn_addmul_1(), see that code for some
# comments.
#
# VAR_ADJUST is the negative of how many limbs the leals in the inner loop
# increment xp and wp.  This is used to adjust back xp and wp, and rshifted
# to given an initial VAR_COUNTER at the top of the outer loop.
#
# VAR_COUNTER is for the unrolled loop, running from VAR_ADJUST/UNROLL_COUNT
# up to -1, inclusive.
#
# VAR_JMP is the computed jump into the unrolled loop.
#
# VAR_XP_LOW is the least significant limb of xp, which is needed at the
# start of the unrolled loop.
#
# PARAM_YSIZE is the outer loop counter, going from -(ysize-1) up to -1,
# inclusive.
#
# PARAM_YP is offset appropriately so that the PARAM_YSIZE counter can be
# added to give the location of the next limb of yp, which is the multiplier
# in the unrolled loop.
#
# The trick with VAR_ADJUST means it's only necessary to do one fetch in the
# outer loop to take care of xp, wp and the inner loop counter.

defframe(VAR_COUNTER,  -20)
defframe(VAR_ADJUST,   -24)
defframe(VAR_JMP,      -28)
defframe(VAR_XP_LOW,   -32)
deflit(VAR_EXTRA_SPACE, 16)


L(unroll):
	# eax	yp
	# ebx
	# ecx	xsize
	# edx	ysize-1
	# esi	xp end
	# edi	wp end of mul1
	# ebp

	movl	PARAM_XP, %esi
	movl	4(%eax), %ebp		# multiplier (yp second limb)
	leal	4(%eax,%edx,4), %eax	# yp adjust for ysize indexing

	movl	PARAM_WP, %edi
	movl	%eax, PARAM_YP
	negl	%edx

	movl	%edx, PARAM_YSIZE
	leal	UNROLL_COUNT-2(%ecx), %ebx	# (xsize-1)+UNROLL_COUNT-1
	decl	%ecx				# xsize-1

	movl	(%esi), %eax		# xp low limb
	andl	$~UNROLL_MASK, %ebx
	negl	%ecx

	subl	$VAR_EXTRA_SPACE, %esp
deflit(`FRAME',16+VAR_EXTRA_SPACE)
	negl	%ebx
	andl	$UNROLL_MASK, %ecx

	movl	%ebx, VAR_ADJUST
	movl	%ecx, %edx
	shll	$4, %ecx

	sarl	$UNROLL_LOG2, %ebx

	# 17 code bytes per limb
ifdef(`PIC',`
	call	L(pic_calc)
L(unroll_here):
',`
	leal	L(unroll_entry) (%ecx,%edx,1), %ecx
')
	negl	%edx

	movl	%eax, VAR_XP_LOW
	movl	%ecx, VAR_JMP
	leal	4(%edi,%edx,4), %edi	# wp and xp, adjust for unrolling,
	leal	4(%esi,%edx,4), %esi	#  and start at second limb
	jmp	L(unroll_outer_entry)


ifdef(`PIC',`
L(pic_calc):
	# See README.family about old gas bugs
	leal	(%ecx,%edx,1), %ecx
	addl	$L(unroll_entry)-L(unroll_here), %ecx
	addl	(%esp), %ecx
	ret
')


#---------------------------------------------------------------------------
	ALIGN(32)
L(unroll_outer_top):
	# ebp	ysize counter, negative

	movl	VAR_ADJUST, %ebx
	movl	PARAM_YP, %edx

	movl	VAR_XP_LOW, %eax
	movl	%ebp, PARAM_YSIZE	# store incremented ysize counter

	leal	4(%edi,%ebx,4), %edi
	leal	(%esi,%ebx,4), %esi
	sarl	$UNROLL_LOG2, %ebx

	movl	(%edx,%ebp,4), %ebp	# yp next multiplier
	movl	VAR_JMP, %ecx

L(unroll_outer_entry):
	mull	%ebp

	testb	$1, %cl		# and clear carry bit
	movl	%ebx, VAR_COUNTER
	movl	$0, %ebx

	movl	$0, %ecx
	cmovz_eax_ecx		# eax into low carry, zero into high carry limb
	cmovnz_eax_ebx

	# Extra fetch of VAR_JMP is bad, but registers are tight
	jmp	*VAR_JMP


#---------------------------------------------------
	ALIGN(32)
L(unroll_top):
	# eax	xp limb
	# ebx	carry high
	# ecx	carry low
	# edx	scratch
	# esi	xp+8
	# edi	wp
	# ebp	yp multiplier limb
	#
	# VAR_COUNTER  loop counter, negative
	#
	# 17 bytes each limb

L(unroll_entry):

deflit(CHUNK_COUNT,2)
forloop(`i', 0, UNROLL_COUNT/CHUNK_COUNT-1, `
	deflit(`disp0', eval(i*CHUNK_COUNT*4 ifelse(UNROLL_BYTES,256,-128)))
	deflit(`disp1', eval(disp0 + 4))

Zdisp(	movl,	disp0,(%esi), %eax)
	adcl	%edx, %ebx

	mull	%ebp

Zdisp(	addl,	%ecx, disp0,(%edi))
	movl	$0, %ecx

	adcl	%eax, %ebx


	movl	disp1(%esi), %eax
	adcl	%edx, %ecx	

	mull	%ebp

	addl	%ebx, disp1(%edi)
	movl	$0, %ebx

	adcl	%eax, %ecx
')


	incl	VAR_COUNTER
	leal	UNROLL_BYTES(%esi), %esi
	leal	UNROLL_BYTES(%edi), %edi

	jnz	L(unroll_top)


	# eax
	# ebx	zero
	# ecx	low
	# edx	high
	# esi
	# edi	wp, pointing at second last limb)
	# ebp
	#
	# carry flag to be added to high

deflit(`disp0', ifelse(UNROLL_BYTES,256,-128))
deflit(`disp1', eval(disp0-0 + 4))

	movl	PARAM_YSIZE, %ebp
	adcl	$0, %edx
	addl	%ecx, disp0(%edi)

	adcl	$0, %edx
	incl	%ebp

	movl	%edx, disp1(%edi)
	jnz	L(unroll_outer_top)


	movl	SAVE_ESI, %esi
	movl	SAVE_EBP, %ebp

	movl	SAVE_EDI, %edi
	movl	SAVE_EBX, %ebx
	addl	$FRAME, %esp

	ret

EPILOGUE()
