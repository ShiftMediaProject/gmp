# AMD K6 mpn_mul_basecase -- multiply two mpn numbers.
#
# K6: approx 9.0 cycles per cross product on 30x30 limbs (with 16 limbs/loop
#     unrolling).


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


dnl  K6: UNROLL_COUNT cycles/product (approx)
dnl           8           9.75
dnl          16           9.3
dnl          32           9.3
dnl  Maximum possible with the current code is 32.
dnl
dnl  With 16 the inner unrolled loop fits exactly in a 256 byte block, which
dnl  might explain it's good performance.

deflit(UNROLL_COUNT, 16)


# void mpn_mul_basecase (mp_ptr wp,
#                        mp_srcptr xp, mp_size_t xsize,
#                        mp_srcptr yp, mp_size_t ysize);
#
# Calculate xp,xsize multiplied by yp,ysize, storing the result in
# wp,xsize+ysize.
#
# This routine is essentially the same as mpn/generic/mul_basecase.c, but
# it's faster because it does most of the mpn_addmul_1() entry code only
# once.  The saving is about 10-20% on typical sizes coming from the
# Karatsuba multiply code.
#
# Future:
#
# The unrolled loop could be shared by mpn_addmul_1, with some extra stack
# setups and maybe 2 or 3 wasted cycles at the end.  Code saving would be
# 256 bytes.

ifdef(`PIC',`
deflit(UNROLL_THRESHOLD, 8)
',`
deflit(UNROLL_THRESHOLD, 8)
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
	ja	L(xsize_more_than_two_limbs)
	je	L(two_by_something)


	# one limb by one limb

	movl	(%edx), %edx	# xp low limb
	movl	PARAM_WP, %ecx
	
	mull	%edx

	movl	%eax, (%ecx)
	movl	%edx, 4(%ecx)
	ret


#------------------------------------------------------------------------------
L(two_by_something):
	decl	PARAM_YSIZE
	pushl	%ebx
deflit(`FRAME',4)

	movl	PARAM_WP, %ebx
	pushl	%esi
deflit(`FRAME',8)

	movl	%eax, %ecx	# yp low limb
	movl	(%edx), %eax	# xp low limb	

	movl	%edx, %esi	# xp
	jnz	L(two_by_two)


	# two limbs by one limb

	mull	%ecx	

	movl	%eax, (%ebx)
	movl	4(%esi), %eax

	movl	%edx, %esi	# carry

	mull	%ecx

	addl	%eax, %esi
	movl	%esi, 4(%ebx)

	adcl	$0, %edx

	movl	%edx, 8(%ebx)
	popl	%esi

	popl	%ebx
	ret
	


#------------------------------------------------------------------------------
	ALIGN(16)
L(two_by_two):
	# eax	xp low limb
	# ebx	wp
	# ecx	yp low limb
	# edx
	# esi	xp
	# edi
	# ebp
deflit(`FRAME',8)

	mull	%ecx		# xp[0] * yp[0]

	push	%edi
deflit(`FRAME',12)
	movl	%eax, (%ebx)

	movl	4(%esi), %eax
	movl	%edx, %edi	# carry, for wp[1]

	mull	%ecx		# xp[1] * yp[0]

	addl	%eax, %edi
	movl	PARAM_YP, %ecx

	adcl	$0, %edx

	movl	%edi, 4(%ebx)
	movl	4(%ecx), %ecx	# yp[1]

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
	adcl	$0, %esi

	movl	%edi, 8(%ebx)
	popl	%edi

	movl	%esi, 12(%ebx)
	popl	%esi

	popl	%ebx
	ret

	
#------------------------------------------------------------------------------
	ALIGN(16)
L(xsize_more_than_two_limbs):

# The first limb of yp is processed with a simple mpn_mul_1 style loop
# inline.  Unrolling this doesn't seem worthwhile since it's only run once
# (whereas the addmul below is run ysize-1 many times).  A call to the
# actual mpn_mul_1 will be slowed down by the call and parameter pushing and
# popping, and doesn't seem likely to be worthwhile on the typical 10-20
# limb operations the Karatsuba code calls here with.

	# eax	yp[0]
	# ebx
	# ecx	xsize
	# edx	xp
	# esi
	# edi
	# ebp
deflit(`FRAME',0)

	pushl	%edi		defframe_pushl(SAVE_EDI)
	pushl	%ebp		defframe_pushl(SAVE_EBP)

	movl	PARAM_WP, %edi
	pushl	%esi		defframe_pushl(SAVE_ESI)

	movl	%eax, %ebp
	pushl	%ebx		defframe_pushl(SAVE_EBX)

	leal	(%edx,%ecx,4), %ebx	# xp end
	xorl	%esi, %esi

	leal	(%edi,%ecx,4), %edi	# wp end of mul1
	negl	%ecx


L(mul1):
	# eax	scratch
	# ebx	xp end
	# ecx	counter, negative
	# edx	scratch
	# esi	carry
	# edi	wp end of mul1
	# ebp	multiplier

	movl	(%ebx,%ecx,4), %eax

	mull	%ebp

	addl	%esi, %eax
	movl	$0, %esi

	adcl	%edx, %esi

	movl	%eax, (%edi,%ecx,4)
	incl	%ecx

	jnz	L(mul1)


	movl	PARAM_YSIZE, %edx
	movl	%esi, (%edi)		# final carry

	movl	PARAM_XSIZE, %ecx
	decl	%edx

	jnz	L(ysize_more_than_one_limb)

	popl	%ebx
	popl	%esi
	popl	%ebp
	popl	%edi
	ret


L(ysize_more_than_one_limb):
	cmpl	$UNROLL_THRESHOLD, %ecx
	movl	PARAM_YP, %eax

	jae	L(unroll)


#------------------------------------------------------------------------------
# Simple addmul loop.
#
# Using ebx and edi pointing at the ends of their respective locations saves
# a couple of instructions in the outer loop.  The inner loop is still 11
# cycles, the same as the simple loop in aorsmul_1.asm.

	# eax	yp
	# ebx	xp end
	# ecx	xsize
	# edx	ysize-1
	# esi
	# edi	wp end of mul1
	# ebp

	movl	4(%eax), %ebp		# multiplier
	negl	%ecx

	movl	%ecx, PARAM_XSIZE	# -xsize
	xorl	%esi, %esi		# initial carry

	leal	4(%eax,%edx,4), %eax	# yp end
	negl	%edx

	movl	%eax, PARAM_YP
	movl	%edx, PARAM_YSIZE

	jmp	L(simple_outer_entry)


	# aligning here saves a couple of cycles
	ALIGN(16)
L(simple_outer_top):	
	# edx	ysize counter, negative

	movl	PARAM_YP, %eax		# yp end
	xorl	%esi, %esi		# carry

	movl	PARAM_XSIZE, %ecx	# -xsize
	movl	%edx, PARAM_YSIZE

	movl	(%eax,%edx,4), %ebp	# yp limb multiplier
L(simple_outer_entry):
	addl	$4, %edi


L(simple_inner):
	# eax	scratch
	# ebx	xp end
	# ecx	counter, negative
	# edx	scratch
	# esi	carry
	# edi	wp end of this addmul
	# ebp	multiplier

	movl	(%ebx,%ecx,4), %eax

	mull	%ebp

	addl	%esi, %eax
	movl	$0, %esi

	adcl	$0, %edx
	addl	%eax, (%edi,%ecx,4)
	adcl	%edx, %esi

	incl	%ecx
	jnz	L(simple_inner)


	movl	PARAM_YSIZE, %edx
	movl	%esi, (%edi)

	incl	%edx
	jnz	L(simple_outer_top)


	popl	%ebx
	popl	%esi
	popl	%ebp
	popl	%edi
	ret


#------------------------------------------------------------------------------
# Unrolled loop.
#
# The unrolled inner loop is the same as in aorsmul_1.asm, see that code for
# some comments.
#
# VAR_COUNTER is for the inner loop, running from VAR_COUNTER_INIT down to
# 0, inclusive.
#
# VAR_JMP is the computed jump into the unrolled loop.
#
# PARAM_XP and PARAM_WP get offset appropriately for where the unrolled loop
# is entered.
#
# VAR_XP_LOW is the least significant limb of xp, which is needed at the
# start of the unrolled loop.  This can't just be fetched through the xp
# pointer because of the offset applied to it.
#
# PARAM_YSIZE is the outer loop counter, going from -(ysize-1) up to -1,
# inclusive.
#
# PARAM_YP is offset appropriately so that the PARAM_YSIZE counter can be
# added to give the location of the next limb of yp, which is the multiplier
# in the unrolled loop.
#
# PARAM_WP is similarly offset so that the PARAM_YSIZE counter can be added
# to give the starting point in the destination for each unrolled loop (this
# point is one limb upwards for each limb of yp processed).
#
# Having PARAM_YSIZE count negative to zero means it's not necessary to
# store new values of PARAM_YP and PARAM_WP on each loop.  Those values on
# the stack remain constant and on each loop an leal adjusts them with the
# PARAM_YSIZE counter value.


defframe(VAR_COUNTER,      -20)
defframe(VAR_COUNTER_INIT, -24)
defframe(VAR_JMP,          -28)
defframe(VAR_XP_LOW,       -32)
deflit(VAR_STACK_SPACE, 16)

dnl  For some strange reason using (%esp) instead of 0(%esp) is a touch
dnl  slower in this code, hence the defframe empty-if-zero feature is
dnl  disabled.
dnl
dnl  If VAR_COUNTER is at (%esp), the effect is worse.  In this case the
dnl  unrolled loop is 255 instead of 256 bytes, but quite how this affects
dnl  anything isn't clear.
dnl
define(`defframe_empty_if_zero_disabled',1)

L(unroll):
	# eax	yp (not used)
	# ebx	xp end (not used)
	# ecx	xsize
	# edx	ysize-1
	# esi
	# edi	wp end of mul1 (not used)
	# ebp
deflit(`FRAME', 16)

	leal	-2(%ecx), %ebp	# one limb processed at start,
	decl	%ecx		# and ebp is one less

	shrl	$UNROLL_LOG2, %ebp
	negl	%ecx

	subl	$VAR_STACK_SPACE, %esp
deflit(`FRAME', 16+VAR_STACK_SPACE)
	andl	$UNROLL_MASK, %ecx

	movl	%ecx, %esi
	shll	$4, %ecx

	movl	%ebp, VAR_COUNTER_INIT
	negl	%esi

	# 15 code bytes per limb
ifdef(`PIC',`
	call	L(pic_calc)
L(unroll_here):
',`
	leal	L(unroll_entry) (%ecx,%esi,1), %ecx
')

	movl	PARAM_XP, %ebx
	movl	%ebp, VAR_COUNTER

	movl	PARAM_WP, %edi
	movl	%ecx, VAR_JMP

	movl	(%ebx), %eax
	leal	4(%edi,%esi,4), %edi	# wp adjust for unrolling and mul1

	leal	(%ebx,%esi,4), %ebx	# xp adjust for unrolling

	movl	%eax, VAR_XP_LOW

	movl	%ebx, PARAM_XP
	movl	PARAM_YP, %ebx

	leal	(%edi,%edx,4), %ecx	# wp adjust for ysize indexing
	movl	4(%ebx), %ebp		# multiplier (yp second limb)

	leal	4(%ebx,%edx,4), %ebx	# yp adjust for ysize indexing

	movl	%ecx, PARAM_WP

	leal	1(%esi), %ecx	# adjust parity for decl %ecx above

	movl	%ebx, PARAM_YP
	negl	%edx

	movl	%edx, PARAM_YSIZE
	jmp	L(unroll_outer_entry)


ifdef(`PIC',`
L(pic_calc):
	# See README.family about old gas bugs
	leal	(%ecx,%esi,1), %ecx
	addl	$L(unroll_entry)-L(unroll_here), %ecx
	addl	(%esp), %ecx
	ret
')


#------------------------------------------------------------------------------
	# Aligning here saves a couple of cycles per loop.  Using 32 doesn't
	# cost any extra space, since the inner unrolled loop below is
	# aligned to 32.
	ALIGN(32)
L(unroll_outer_top):
	# edx	ysize

	movl	PARAM_YP, %eax
	movl	%edx, PARAM_YSIZE	# incremented ysize counter

	movl	PARAM_WP, %edi

	movl	VAR_COUNTER_INIT, %ebx
	movl	(%eax,%edx,4), %ebp	# next multiplier

	movl	PARAM_XSIZE, %ecx
	leal	(%edi,%edx,4), %edi	# adjust wp for where we are in yp

	movl	VAR_XP_LOW, %eax
	movl	%ebx, VAR_COUNTER

L(unroll_outer_entry):
	mull	%ebp

	# using testb is a tiny bit faster than testl
	testb	$1, %cl

	movl	%eax, %ecx	# low carry
	movl	VAR_JMP, %eax

	movl	%edx, %esi	# high carry
	movl	PARAM_XP, %ebx

	jnz	L(unroll_noswap)
	movl	%ecx, %esi	# high,low carry other way around

	movl	%edx, %ecx
L(unroll_noswap):

	jmp	*%eax



#------------------------------------------------------------------------------
	ALIGN(32)
L(unroll_top):
	# eax	scratch
	# ebx	xp
	# ecx	carry low
	# edx	scratch
	# esi	carry high
	# edi	wp
	# ebp	multiplier
	# VAR_COUNTER  loop counter
	#
	# 15 code bytes each limb

	leal	UNROLL_BYTES(%edi), %edi

L(unroll_entry):
deflit(CHUNK_COUNT,2)
forloop(`i', 0, UNROLL_COUNT/CHUNK_COUNT-1, `
	deflit(`disp0', eval(i*CHUNK_COUNT*4))
	deflit(`disp1', eval(disp0 + 4))
	deflit(`disp2', eval(disp1 + 4))

	movl	disp1(%ebx), %eax
	mull	%ebp
Zdisp(	addl,	%ecx, disp0,(%edi))
	adcl	%eax, %esi
	movl	%edx, %ecx
	jadcl0( %ecx)

	movl	disp2(%ebx), %eax
	mull	%ebp
	addl	%esi, disp1(%edi)
	adcl	%eax, %ecx
	movl	%edx, %esi
	jadcl0( %esi)
')

	decl	VAR_COUNTER
	leal	UNROLL_BYTES(%ebx), %ebx

	jns	L(unroll_top)


	movl	PARAM_YSIZE, %edx
	addl	%ecx, UNROLL_BYTES(%edi)

	adcl	$0, %esi

	incl	%edx
	movl	%esi, UNROLL_BYTES+4(%edi)

	jnz	L(unroll_outer_top)


	movl	SAVE_ESI, %esi
	movl	SAVE_EBP, %ebp
	movl	SAVE_EDI, %edi
	movl	SAVE_EBX, %ebx

	addl	$FRAME, %esp
	ret

EPILOGUE()
