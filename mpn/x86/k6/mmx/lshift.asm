dnl  AMD K6 mpn_lshift -- mpn left shift.
dnl 
dnl  K6: 1.75 cycles/limb


dnl  Copyright (C) 1999, 2000 Free Software Foundation, Inc.
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


C mp_limb_t mpn_lshift (mp_ptr dst, mp_srcptr src, mp_size_t size,
C                       unsigned shift);
C
C Shift src,size left by shift many bits and store the result in dst,size.
C Zeros are shifted in at the right.  Return the bits shifted out at the
C left.

defframe(PARAM_SHIFT,16)
defframe(PARAM_SIZE, 12)
defframe(PARAM_SRC,  8)
defframe(PARAM_DST,  4)
deflit(`FRAME',0)

dnl  used after src has been fetched
define(VAR_RETVAL,`PARAM_SRC')

dnl  minimum 9, because unrolled loop can't handle less
deflit(UNROLL_THRESHOLD, 12)

	.text
	ALIGN(32)

PROLOGUE(mpn_lshift)
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

	shldl(	%cl, %edx, %eax)	C return value

 	shll	%cl, %edx

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

	movl	(%ebx,%eax,4), %edx	C src high limb
	negl	%ecx

	movd	PARAM_SHIFT, %mm6
	addl	$32, %ecx

	shrl	%cl, %edx
	cmpl	$UNROLL_THRESHOLD-1, %eax

	movl	%edx, VAR_RETVAL
	jae	L(unroll)


	C eax	size-1
	C ebx	src
	C ecx	32-shift
	C edx	retval
	C
	C mm6	shift

	movd	%ecx, %mm7
	movl	%eax, %ecx

	movl	PARAM_DST, %eax

L(simple):
	C ecx	dst
	C ebx	src
	C ecx	counter
	C edx	retval
	C
	C mm0	scratch
	C mm6	shift
	C mm7	32-shift

	movq	-4(%ebx,%ecx,4), %mm0
 	psrlq	%mm7, %mm0

	movd	%mm0, (%eax,%ecx,4)
	loop	L(simple)


	movd	(%ebx), %mm0
	popl	%ebx

 	psllq	%mm6, %mm0

	movd	%mm0, (%eax)
	movl	%edx, %eax

	emms_or_femms
	ret


C -----------------------------------------------------------------------------
C The strange offsets used on src and dst are due to the following,
C - needing no displacement (%ebx,%eax,4) and (%edx,%eax,4) first in the loop
C - needing the loop running %eax downwards and wanting to stop when %eax
C   goes negative
C - wanting to end up with %eax set to -1 to -4 so as to be able to test for
C   0-3 extras with test $2 and test $1

L(unroll):
	C eax	size-1
	C ebx	src
	C ecx	32-shift
	C edx	retval (but instead VAR_RETVAL is used)
	C
	C mm6	shift

	addl	$32, %ecx
	movl	PARAM_DST, %edx

	movd	%ecx, %mm7
	subl	$7, %eax		C size-8

	leal	(%edx,%eax,4), %ecx	C alignment of dst
	addl	$40, %edx

	movq	32-8(%ebx,%eax,4), %mm2		C src high qword
	testb	$4, %cl

	jz	L(dst_aligned)
	psllq	%mm6, %mm2

	psrlq	$32, %mm2
	decl	%eax

	movd	%mm2, 32-40+4-4(%edx,%eax,4)	C dst high limb
	movq	32-8(%ebx,%eax,4), %mm2		C new src high qword
L(dst_aligned):

	movq	32-16(%ebx,%eax,4), %mm0	C src second highest qword
	addl	$24, %ebx



	C This loop is the important bit, the rest is just support for it.
	C Four src limbs are held at the start, and four more will be read.
	C Four dst limbs will be written.
	C
	C The magic ingredients for speed here are the same as in rshift,
	C
	C - aligning the code to 32 bytes
	C - fitting the first 10 instructions into 32 bytes (the first fetch
	C   and store must have no displacements)
	C - the instruction scheduling shown


	C Offset 0x95 here, so use a jump to get to L(top) in one cycle.
	C This guards against executing through a bunch of nop's if a dumb
	C assembler doesn't generate multi-byte do-nothing instructions when
	C aligning.

	jmp L(top)

	ALIGN(32)
L(top):
	C eax	limb counter
	C ebx	src + 24
	C ecx
	C edx	dst + 40
	C
	C mm0	src next qword
	C mm1	scratch
	C mm2	src prev qword
	C mm6	shift
	C mm7	64-shift

	psllq	%mm6, %mm2
	subl	$4, %eax

	movq	%mm0, %mm1
	psrlq	%mm7, %mm0

	por	%mm0, %mm2
	movq	(%ebx,%eax,4), %mm0

	psllq	%mm6, %mm1
	movq	%mm2, (%edx,%eax,4)

	movq	%mm0, %mm2
	psrlq	%mm7, %mm0

	por	%mm0, %mm1
	movq	-8(%ebx,%eax,4), %mm0

	movq	%mm1, -8(%edx,%eax,4)
	jnc	L(top)


	C Now have four limbs in mm2 (prev) and mm0 (next), plus eax mod 4.
	C
	C -16(%ebx) is the next source, and -16(%edx) is the next destination.
	C %eax is between -4 and -1, representing respectively 0 to 3 extra
	C limbs that must be read.


	testb	$2, %al
	jz	L(finish_nottwo)

	C Two more limbs: lshift mm2, OR it with rshifted mm0, mm0 becomes
	C new mm2 and a new mm0 is loaded.

	psllq	%mm6, %mm2
	movq	%mm0, %mm1

	psrlq	%mm7, %mm0
	subl	$2, %eax

	por	%mm0, %mm2
	movq	-8(%ebx,%eax,4), %mm0

	movq	%mm2, -8(%edx,%eax,4)
	movq	%mm1, %mm2
L(finish_nottwo):


	C lshift mm2, OR with rshifted mm0, mm1 becomes lshifted mm0

	testb	$1, %al
	psllq	%mm6, %mm2

	movq	%mm0, %mm1
	psrlq	%mm7, %mm0

	por	%mm0, %mm2
	psllq	%mm6, %mm1

	movq	%mm2, -16(%edx,%eax,4)
	jz	L(finish_even)


	C Size is odd, so mm1 and one extra limb to process.

	movd	-24(%ebx), %mm0		C src[0]
	popl	%ebx
deflit(`FRAME',0)

	movq	%mm0, %mm2
	psllq	$32, %mm0

	psrlq	%mm7, %mm0

	psllq	%mm6, %mm2
	por	%mm0, %mm1

	movq	%mm1, 4-40(%edx)	C dst[1,2]
	movd	%mm2, -40(%edx)		C dst[0]

	movl	VAR_RETVAL, %eax

	emms_or_femms
	ret


L(finish_even):
deflit(`FRAME',4)
	C Size is even, so only mm1 left to process.

	movq	%mm1, -40(%edx)		C dst[0,1]
	movl	VAR_RETVAL, %eax

	popl	%ebx
	emms_or_femms
	ret

EPILOGUE()
