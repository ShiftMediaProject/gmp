dnl  AMD K8 mpn_divexact_1 -- mpn by limb exact division.

dnl  Copyright 2001, 2002, 2004 Free Software Foundation, Inc.
dnl
dnl  This file is part of the GNU MP Library.
dnl
dnl  The GNU MP Library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Lesser General Public License as
dnl  published by the Free Software Foundation; either version 2.1 of the
dnl  License, or (at your option) any later version.
dnl
dnl  The GNU MP Library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Lesser General Public License for more details.
dnl
dnl  You should have received a copy of the GNU Lesser General Public
dnl  License along with the GNU MP Library; see the file COPYING.LIB.  If
dnl  not, write to the Free Software Foundation, Inc., 59 Temple Place -
dnl  Suite 330, Boston, MA 02111-1307, USA.

include(`../config.m4')


C     cycles/limb
C K8:    10

C A quick adoption of the 32-bit K7 code.

C INPUT PARAMETERS
C rp		rdi
C up		rsi
C n		rdx
C divisor	rcx

	TEXT
	ALIGN(16)
PROLOGUE(mpn_divexact_1)
	pushq	%rbx

	movq	%rcx, %rax
	movl	$-1, %ecx		C shift count
	movq	%rdx, %r8

	C Strip low zero bits (optimized for few bits)
L(strip_twos):
	incl	%ecx
	shrq	%rax
	jnc	L(strip_twos)

	leaq	1(%rax,%rax), %rbx	C d without twos
	andq	$127, %rax		C d/2, 7 bits

ifdef(`PIC',`
	movq	modlimb_invert_table@GOTPCREL(%rip), %rdx
	movzbl	(%rax,%rdx), %eax			C inv 8 bits
',`
	movzbl	modlimb_invert_table(%rax), %eax	C inv 8 bits
')

	movq	%rbx, %r11		C d without twos

	leal	(%rax,%rax), %edx	C 2*inv
	imull	%eax, %eax		C inv*inv
	imull	%ebx, %eax		C inv*inv*d
	subl	%eax, %edx		C inv = 2*inv - inv*inv*d

	leal	(%rdx,%rdx), %eax	C 2*inv
	imull	%edx, %edx		C inv*inv
	imull	%ebx, %edx		C inv*inv*d
	subl	%edx, %eax		C inv = 2*inv - inv*inv*d

	leaq	(%rax,%rax), %rdx	C 2*inv
	imulq	%rax, %rax		C inv*inv
	imulq	%rbx, %rax		C inv*inv*d
	subq	%rax, %rdx		C inv = 2*inv - inv*inv*d

	leaq	(%rsi,%r8,8), %rsi	C up end
	leaq	-8(%rdi,%r8,8), %rdi	C rp end
	negq	%r8			C -n

	movq	%rdx, %r10		C final inverse
	movq	(%rsi,%r8,8), %rax	C up[0]

	incq	%r8
	jz	L(one)

	movq	(%rsi,%r8,8), %rdx	C up[1]

	shrdq	%cl, %rdx, %rax

	xorl	%ebx, %ebx
	jmp	L(entry)

	ALIGN(8)
L(top):
	C eax	q
	C ebx	carry bit, 0 or 1
	C ecx	shift
	C edx
	C esi	up end
	C edi	rp end
	C ebp	counter, limbs, negative

	mulq	%r11	 		C carry limb in edx

	movq	-8(%rsi,%r8,8), %rax
	movq	(%rsi,%r8,8), %r9

	shrdq	%cl, %r9, %rax
	nop

	subq	%rbx, %rax		C apply carry bit
	setc	%bl

	subq	%rdx, %rax		C apply carry limb
	adcq	$0, %rbx

L(entry):
	imulq	%r10, %rax

	movq	%rax, (%rdi,%r8,8)
	incq	%r8
	jnz	L(top)


	mulq	%r11			C carry limb in edx

	movq	-8(%rsi), %rax		C up high limb
	shrq	%cl, %rax

	subq	%rbx, %rax		C apply carry bit

	subq	%rdx, %rax		C apply carry limb

	imulq	%r10, %rax

	movq	%rax, (%rdi)

	popq	%rbx
	ret


L(one):
	shrq	%cl, %rax

	imulq	%r10, %rax

	movq	%rax, (%rdi)

	popq	%rbx
	ret

EPILOGUE()
