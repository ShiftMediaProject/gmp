# AMD K6 mpn_copyi -- copy limb vector.
#
# K6: 0.56 or 1.0 cycles/limb (at 32 limbs/loop), depending on data
# alignment.  PIC adds 4 cycles at the start of the unrolled loop.
#
# Future: Maybe it's not necessary to unroll quite so much.


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


dnl  K6 aligned:
dnl  UNROLL_COUNT cycles/limb
dnl        8          0.75
dnl       16          0.63
dnl       32          0.56
dnl       64          0.53
dnl  Maximum possible with the current code is 64, the minimum is 2.

define(UNROLL_COUNT, 32)


# void mpn_copyi (mp_ptr dst, mp_srcptr src, mp_size_t size);
#
# Copy src,size to dst,size.
#
# Sizes less than REP_MOVS_THRESHOLD are handled by a "movsl loop", which
# avoids the setup time for the "rep movsl".  The code to reach this case is
# as fast as possible, to minimize overhead.  A "rep movsl" is 1 cycle/limb
# but takes about 9 cycles to start, a "movs loop" is 2 cycles/limb but has
# no startup time.
#
# Sizes over UNROLL_THRESHOLD are handled by an unrolled MMX based loop.
# This is 0.5 cycles/limb plus loop overhead, provided source and
# destination are aligned to 8 byte boundaries.  If not, the rep movs is the
# same speed.  The alignment cases modulo 8 bytes are as follows,
#
#	src   dst
#	 0     0	both aligned, use mmx
#	 0     4        unaligned, use rep movs
#	 4     0        unaligned, use rep movs
#	 4     4	do one movs, then both aligned, use mmx
#
# A pattern of two movq loads and two movq stores (or four and four) was
# tried, but found to be the same speed as just one of each.

deflit(REP_MOVS_THRESHOLD, 6)

dnl  emms is a couple of cycles slower than femms, hence the different
dnl  thresholds when it's available (K6-2,K6-3) or not (K6)

ifelse(femms_available_p,1,`
ifdef(`PIC',`
deflit(UNROLL_THRESHOLD, 54)
',`
deflit(UNROLL_THRESHOLD, 46)
')
',`
ifdef(`PIC',`
deflit(UNROLL_THRESHOLD, 58)
',`
deflit(UNROLL_THRESHOLD, 51)
')
')

defframe(PARAM_SIZE,12)
defframe(PARAM_SRC, 8)
defframe(PARAM_DST, 4)
deflit(`FRAME',0)

	.text

	# Aligning to 64 bytes, rather than 32, saves a couple of cycles and
	# smooths out the times so that an n+1 limb operation isn't faster
	# than an n limb operation.  Maybe it's because with 64 the unrolled
	# loop is in 3 rather than 4 cache sectors (of 64 bytes each).
	ALIGN(64)

PROLOGUE(mpn_copyi)
	movl	PARAM_SIZE, %ecx
	movl	%esi, %eax

	movl	PARAM_SRC, %esi
	movl	%edi, %edx

	cmpl	$REP_MOVS_THRESHOLD, %ecx
	movl	PARAM_DST, %edi

	jae	L(rep_movs_maybe)


	# eax	saved esi
	# ebx
	# ecx	counter
	# edx	saved edi
	# esi	src
	# edi	dst
	# ebp
L(simple):
	movsl
	loop	L(simple)

	movl	%eax, %esi
	movl	%edx, %edi
	ret


# --------------------------------------------------
	ALIGN(16)
L(rep_movs_maybe):
	cmpl	$UNROLL_THRESHOLD, %ecx
	jae	L(unroll_maybe)

L(rep_movs):
	rep
	movsl

	movl	%eax, %esi
	movl	%edx, %edi
	ret


# --------------------------------------------------
	ALIGN(16)
L(unroll_maybe):
	# different alignments mod 8 are no good
	pushl	%eax	# saved esi
deflit(`FRAME',4)
	leal	(%esi,%edi,1), %eax
	testb	$4, %al
	jz	L(unroll)

	rep
	movsl

	popl	%esi
	movl	%edx, %edi
	ret


# --------------------------------------------------
	ALIGN(16)
L(unroll):
	# step src and dst to 0 mod 8 if not already
	testl	$4, %esi
	pushl	%edx	# saved edi
deflit(`FRAME',8)

	jz	L(already_aligned)
	decl	%ecx

	movsl
L(already_aligned):

	movl	%ecx, PARAM_SIZE	# new size
	andl	$~1, %ecx		# handle even part only in unrolled

	movl	%ecx, %eax
	negl	%ecx

	decl	%eax
	andl	$UNROLL_MASK, %ecx

	shll	$2, %ecx
	shrl	$UNROLL_LOG2, %eax
	
	# 4 code bytes per limb processed
ifdef(`PIC',`
	call	L(pic_calc)
L(here):
',`
	leal	L(entry) (%ecx), %edx
')
	subl	%ecx, %edi

	subl	%ecx, %esi
ifelse(UNROLL_BYTES,256,`
	addl	$128, %edi
	addl	$128, %esi
')
	jmp	*%edx


ifdef(`PIC',`
L(pic_calc):
	movl	(%esp), %edx
	leal	L(entry)-L(here) (%edx,%ecx), %edx
	ret
')


# ----------------------------------------------
	ALIGN(32)
L(top):
	# eax	unroll counter
	# ebx
	# ecx	scratch
	# edx	was computed jump
	# esi	src
	# edi	dst
	# ebp
	#
	# 4 code bytes per limb processed

L(entry):
deflit(CHUNK_COUNT, 2)
forloop(`i', 0, UNROLL_COUNT/CHUNK_COUNT-1, `
	deflit(`disp', eval(i*CHUNK_COUNT*4 ifelse(UNROLL_BYTES,256, -128)))

	movq	disp(%esi), %mm0
	movq	%mm0, disp(%edi)
')

	addl	$UNROLL_BYTES, %esi
	addl	$UNROLL_BYTES, %edi

	decl	%eax
	jns	L(top)


	testb	$1, PARAM_SIZE	# possible odd limb
	jz	L(no_extra)

	movsl
L(no_extra):

	popl	%edi
	popl	%esi

	emms_or_femms

	ret


EPILOGUE()
