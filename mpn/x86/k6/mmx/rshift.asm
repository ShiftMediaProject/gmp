dnl  AMD K6 mpn_rshift -- mpn right shift.
dnl 
dnl  K6: 1.75 cycles/limb


dnl  Copyright (C) 2000 Free Software Foundation, Inc.
dnl 
dnl  This file is part of the GNU MP Library.
dnl 
dnl  The GNU MP Library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Library General Public License as
dnl  published by the Free Software Foundation; either version 2 of the
dnl  License, or (at your option) any later version.
dnl 
dnl  The GNU MP Library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Library General Public License for more details.
dnl 
dnl  You should have received a copy of the GNU Library General Public
dnl  License along with the GNU MP Library; see the file COPYING.LIB.  If
dnl  not, write to the Free Software Foundation, Inc., 59 Temple Place -
dnl  Suite 330, Boston, MA 02111-1307, USA.


include(`../config.m4')


C mp_limb_t mpn_rshift (mp_ptr dst, mp_srcptr src, mp_size_t size,
C                       unsigned shift);
C
C Shift src,size right by shift many bits and store the result in dst,size.
C Zeros are shifted in at the left.  Return the bits shifted out at the
C right.

defframe(PARAM_SHIFT,16)
defframe(PARAM_SIZE, 12)
defframe(PARAM_SRC,  8)
defframe(PARAM_DST,  4)
deflit(`FRAME',0)

dnl  Minimum 9, because the unrolled loop can't handle less.
dnl  Doesn't depend on femms_available_p because there's the same
dnl  emms_or_femms in both the simple and unrolled.

deflit(UNROLL_THRESHOLD, 9)

	.text
	ALIGN(32)

PROLOGUE(mpn_rshift)
	C The 1 limb case can be done without the push %ebx, but it's then
	C still the same speed.  The push is left as a free helping hand for
	C the two_or_more code.

	movl	PARAM_SIZE, %eax
	pushl	%ebx
deflit(`FRAME',4)

	movl	PARAM_SRC, %ebx
	decl	%eax

	movl	PARAM_SHIFT, %ecx
	jnz	L(two_or_more)

	movl	(%ebx), %edx		C src limb
	movl	PARAM_DST, %ebx

	shrdl(	%cl, %edx, %eax)	C return value

 	shrl	%cl, %edx

	movl	%edx, (%ebx)		C dst limb
	popl	%ebx

	ret


C -----------------------------------------------------------------------------
	ALIGN(16)	C avoid offset 0x1f
L(two_or_more):
	C eax	size-1
	C ebx	src
	C ecx	shift
	C edx

	movl	(%ebx), %edx	C src low limb
	negl	%ecx

	addl	$32, %ecx
	movd	PARAM_SHIFT, %mm6

	shll	%cl, %edx
	cmpl	$UNROLL_THRESHOLD-1, %eax

	jae	L(unroll)


	C eax	size-1
	C ebx	src
	C ecx	32-shift
	C edx	retval
	C
	C mm6	shift

	movl	PARAM_DST, %ecx
	leal	(%ebx,%eax,4), %ebx

	leal	-4(%ecx,%eax,4), %ecx
	negl	%eax

	C This loop runs at about 3 cycles/limb, which is the amount of
	C decoding, this is despite every second access being unaligned.

L(simple):
	C eax	counter (negative)
	C ebx	src, pointing at last limb
	C ecx	dst, pointing at last limb
	C edx	retval
	C
	C mm0	scratch
	C mm6	shift

	movq	(%ebx,%eax,4), %mm0
	incl	%eax

 	psrlq	%mm6, %mm0

	movd	%mm0, (%ecx,%eax,4)
	jnz	L(simple)


	movq	%mm0, (%ecx)
	movl	%edx, %eax

	popl	%ebx

	emms_or_femms
	ret


C -----------------------------------------------------------------------------
C The strange offsets used on src and dst are due to the following,
C - needing no displacement (%ebx,%eax,4) and (%edx,%eax,4) first in the loop
C - needing the loop running %eax upwards from negative values and wanting
C   to stop when %eax goes positive
C - wanting to end up with %eax between 0 and 3 so as to be able to test for
C   0-3 extras with test $2 and test $1

L(unroll):
	C eax	size-1
	C ebx	src
	C ecx	32-shift
	C edx	retval
	C
	C mm6	shift

	addl	$32, %ecx
	subl	$7, %eax		C size-8

	movd	%ecx, %mm7
	movl	PARAM_DST, %ecx

	movq	(%ebx), %mm2		C src low qword
	leal	4(%ebx,%eax,4), %ebx	C src end - 28

	testb	$4, %cl
	leal	-12(%ecx,%eax,4), %ecx	C dst end - 44

	notl	%eax			C -(size-7)
	jz	L(dst_aligned)

	psrlq	%mm6, %mm2
	incl	%eax

	movd	%mm2, 12(%ecx,%eax,4)
	movq	(%ebx,%eax,4), %mm2	C new src low qword
L(dst_aligned):

	movq	8(%ebx,%eax,4), %mm0	C src second lowest qword


	C This loop is the important bit, the rest is just support for it.
	C Four src limbs are held at the start, and four more will be read.
	C Four dst limbs will be written.
	C
	C The magic ingredients for speed here are
	C
	C - aligning the code to 32 bytes
	C - fitting the first 10 instructions into 32 bytes (the first fetch
	C   and store must have no displacements)
	C - the instruction scheduling shown


	C Offset 0x88 here, so use a jump to get to L(top) in one cycle.
	C This guards against executing through a bunch of nop's if a dumb
	C assembler doesn't generate multi-byte do-nothing instructions when
	C aligning.

	jmp	L(top)

	ALIGN(32)
L(top):
	C eax	limb counter, negative
	C ebx	src end - 28
	C ecx	dst end - 44
	C ecx	retval
	C
	C mm0	src next qword
	C mm1	scratch
	C mm2	src prev qword
	C mm6	shift
	C mm7	64-shift

	psrlq	%mm6, %mm2
	addl	$4, %eax

	movq	%mm0, %mm1
	psllq	%mm7, %mm0

	por	%mm0, %mm2
	movq	(%ebx,%eax,4), %mm0

	psrlq	%mm6, %mm1
	movq	%mm2, (%ecx,%eax,4)

	movq	%mm0, %mm2
	psllq	%mm7, %mm0

	por	%mm0, %mm1
	movq	8(%ebx,%eax,4), %mm0

	movq	%mm1, 8(%ecx,%eax,4)
	ja	L(top)		C jump if no carry and not zero



	C Now have the four limbs in mm2 (low) and mm0 (high), and %eax is 0
	C to 3 representing respectively 3 to 0 further limbs.


	testb	$2, %al
	jnz	L(finish_nottwo)

	C Two or three extra limbs: rshift mm2, OR it with lshifted mm0, mm0
	C becomes new mm2 and a new mm0 is loaded.

	psrlq	%mm6, %mm2
	movq	%mm0, %mm1

	psllq	%mm7, %mm0
	addl	$2, %eax

	por	%mm0, %mm2
	movq	8(%ebx,%eax,4), %mm0

	movq	%mm2, 8(%ecx,%eax,4)
	movq	%mm1, %mm2
L(finish_nottwo):


	testb	$1, %al
	psrlq	%mm6, %mm2

	movq	%mm0, %mm1
	psllq	%mm7, %mm0

	por	%mm0, %mm2
	psrlq	%mm6, %mm1

	movq	%mm2, 16(%ecx,%eax,4)
	jnz	L(finish_even)


	C one further extra limb to process

	movd	28-4(%ebx), %mm0	C src[size-1], most significant limb
	popl	%ebx

	movq	%mm0, %mm2
	psllq	%mm7, %mm0

	por	%mm0, %mm1
	psrlq	%mm6, %mm2

	movq	%mm1, 44-12(%ecx)	C dst[size-3,size-2]
	movd	%mm2, 44-4(%ecx)	C dst[size-1]

	movl	%edx, %eax		C retval

	emms_or_femms
	ret


L(finish_even):
	C no further extra limbs

	movq	%mm1, 44-8(%ecx)	C dst[size-2,size-1]
	movl	%edx, %eax		C retval

	popl	%ebx

	emms_or_femms
	ret

EPILOGUE()
