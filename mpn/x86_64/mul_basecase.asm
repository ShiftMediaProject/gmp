dnl  AMD64 mpn_mul_basecase.

dnl  Copyright 2003, 2004, 2005, 2007 Free Software Foundation, Inc.

dnl  This file is part of the GNU MP Library.

dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU Lesser General Public License as published
dnl  by the Free Software Foundation; either version 3 of the License, or (at
dnl  your option) any later version.

dnl  The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
dnl  License for more details.

dnl  You should have received a copy of the GNU Lesser General Public License
dnl  along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.

include(`../config.m4')

C	     cycles/limb
C K8,K9:	 2.86    (3.14 at 20x20)
C K10:		 2.86    (3.14 at 20x20)
C P4:		13.65
C P6-15:	 4.67

C Outline of algorithm:
C
C   if (vn & 1)
C     {
C       rp[] = mpn_mul_1 (rp, up, un, vp[0]);
C       rp++, vp++, vn--;
C     }
C   else
C     {
C       rp[] = mpn_mul_2 (rp, up, un, vp);
C       rp += 2, vp += 2, vn -= 2;
C     }
C   if (vn == 0)
C     return;
C   switch (un % 3)
C     {
C     case 0:
C       do
C         {
C           rp[un + 1] = _0_mpn_addmul_2 (rp, up, un, vp);
C           rp += 2, vp += 2, vn -= 2;
C         }
C       while (vn != 0);
C       break;
C     case 1:
C       do
C         {
C           rp[un + 1] = _1_mpn_addmul_2 (rp, up, un, vp);
C           rp += 2, vp += 2, vn -= 2;
C         }
C       while (vn != 0);
C       break;
C     case 2:
C       do
C         {
C           rp[un + 1] = _2_mpn_addmul_2 (rp, up, un, vp);
C           rp += 2, vp += 2, vn -= 2;
C         }
C       while (vn != 0);
C       break;
C     }

C STATUS
C  * This is fairly raw, the code could surely take some polishing.  Perhaps a
C    register or two less could be used.
C  * This code is slower than the old, plain code for vn<=3, in particular for
C    vn=3.  The best way of improving it might be writing some sort of mul_3 or
C    addmul_3 building block.  That block might very well become faster than
C    out present addmul_2, and should then be made the main worker.

C INPUT PARAMETERS
define(`rp',	`%rdi')
define(`up',	`%rsi')
define(`un',	`%rdx')
define(`vp',	`%rcx')
define(`vn',	`%r8')

ASM_START()
	TEXT
	ALIGN(16)
PROLOGUE(mpn_mul_basecase)

	push	%rbx
	push	%rbp
	push	%r12
	push	%r13
	push	%r14

	lea	(up,un,8), up		C
	lea	(rp,un,8), rp		C
	lea	(vp), %r14		C
define(`vp', `%r14')
	mov	un, %r11		C move away from rdx
define(`un', `%r11')
	test	$1, vn			C vn odd?
	je	L(use_mul_2)
	bt	$0, vn			C vn odd?
	jnc	L(use_mul_2)

L(use_mul_1):
	mov	(vp), %rbp		C read vp[0]
	lea	8(vp), vp		C vp += 1
	dec	vn			C vn -= 1
	mov	un, %rbx		C				  mul_1
	neg	%rbx			C				  mul_1
	xor	%ecx, %ecx		C clear carry limb		  mul_1
	add	$3, %rbx		C				  mul_1
	jb	L(ed1)			C jump for n = 1, 2, 3		  mul_1

L(lp1):	mov	-24(up,%rbx,8), %rax	C				  mul_1
	mul	%rbp			C				  mul_1
	xor	%r9, %r9		C				  mul_1
	add	%rax, %rcx		C				  mul_1
	adc	%rdx, %r9		C				  mul_1
	mov	-16(up,%rbx,8), %rax	C				  mul_1
	mul	%rbp			C				  mul_1
	xor	%r10, %r10		C				  mul_1
	add	%rax, %r9		C				  mul_1
	adc	%rdx, %r10		C				  mul_1
	mov	%rcx, -24(rp,%rbx,8)	C				  mul_1
	mov	%r9, -16(rp,%rbx,8)	C				  mul_1
	mov	-8(up,%rbx,8), %rax	C				  mul_1
	mul	%rbp			C				  mul_1
	xor	%r9, %r9		C				  mul_1
	add	%rax, %r10		C				  mul_1
	adc	%rdx, %r9		C				  mul_1
	mov	(up,%rbx,8), %rax	C				  mul_1
	mul	%rbp			C				  mul_1
	xor	%rcx, %rcx		C				  mul_1
	add	%rax, %r9		C				  mul_1
	adc	%rdx, %rcx		C				  mul_1
	mov	%r10, -8(rp,%rbx,8)	C				  mul_1
	mov	%r9, (rp,%rbx,8)	C				  mul_1
	add	$4, %rbx		C				  mul_1
	jae	L(lp1)			C				  mul_1

	cmp	$3, %ebx		C				  mul_1
	jne	L(ed1)			C				  mul_1

	mov	%rcx, (rp)		C				  mul_1
	lea	8(rp), rp		C rp += 1			  mul_1
	jmp	L(jn1)			C				  mul_1

L(ed1):	mov	-24(up,%rbx,8), %rax	C				  mul_1
	mul	%rbp			C				  mul_1
	xor	%r9, %r9		C				  mul_1
	add	%rax, %rcx		C				  mul_1
	adc	%rdx, %r9		C				  mul_1
	cmp	$2, %ebx		C				  mul_1
	jne	L(1)			C				  mul_1
	mov	%rcx, -8(rp)		C				  mul_1
	mov	%r9, (rp)		C				  mul_1
	lea	8(rp), rp		C rp += 1			  mul_1
	jmp	L(jn1)			C				  mul_1

L(1):	mov	-16(up,%rbx,8), %rax	C				  mul_1
	mul	%rbp			C				  mul_1
	xor	%r10, %r10		C				  mul_1
	add	%rax, %r9		C				  mul_1
	adc	%rdx, %r10		C				  mul_1
	cmp	$1, %ebx		C				  mul_1
	jne	L(2)			C				  mul_1
	mov	%rcx, -16(rp)		C				  mul_1
	mov	%r9, -8(rp)		C				  mul_1
	mov	%r10, (rp)		C				  mul_1
	lea	8(rp), rp		C rp += 1			  mul_1
	jmp	L(jn1)			C				  mul_1

L(2):	mov	-8(up), %rax		C				  mul_1
	mul	%rbp			C				  mul_1
	xor	%rbx, %rbx		C				  mul_1
	add	%rax, %r10		C				  mul_1
	adc	%rdx, %rbx		C				  mul_1
	mov	%rcx, -24(rp)		C				  mul_1
	mov	%r9, -16(rp)		C				  mul_1
	mov	%r10, -8(rp)		C				  mul_1
	mov	%rbx, (rp)		C				  mul_1
	lea	8(rp), rp		C rp += 1			  mul_1
	jmp	L(jn1)			C				  mul_1

L(use_mul_2):

define(`v0', `%r9')  define(`v1', `%r10')
define(`j',`%r13')

	mov	(vp), v0		C				  mul_2
	mov	8(vp), v1		C				  mul_2
	lea	16(vp), vp		C vp += 2
	sub	$2, vn			C vn -= 2

	mov	un, j			C				  mul_2
	neg	j			C				  mul_2

	xor	%ebx, %ebx		C				  mul_2
	xor	%ecx, %ecx		C				  mul_2
	xor	%rbp, %rbp		C				  mul_2

	mov	(up,j,8), %r12		C				  mul_2
	mov	%r12, %rax		C				  mul_2
	add	$3, j			C				  mul_2
	jns	L(ed2)			C <= 4 iterations

	ALIGN(16)
L(lp2):	mul	v0			C				  mul_2
	add	%rax, %rbx		C				  mul_2
	mov	%r12, %rax		C				  mul_2
	adc	%rdx, %rcx		C				  mul_2
	adc	$0, %ebp		C				  mul_2
	mul	v1			C				  mul_2
	mov	%rbx, -24(rp,j,8)	C				  mul_2
	mov	-16(up,j,8), %r12	C				  mul_2
	mov	$0, %ebx		C				  mul_2
	add	%rax, %rcx		C				  mul_2
	mov	%r12, %rax		C				  mul_2
	adc	%rdx, %rbp		C				  mul_2
	mul	v0			C				  mul_2
	add	%rax, %rcx		C				  mul_2
	mov	%r12, %rax		C				  mul_2
	adc	%rdx, %rbp		C				  mul_2
	adc	$0, %ebx		C				  mul_2
	mul	v1			C				  mul_2
	mov	%rcx, -16(rp,j,8)	C				  mul_2
	mov	-8(up,j,8), %r12	C				  mul_2
	mov	$0, %ecx		C				  mul_2
	add	%rax, %rbp		C				  mul_2
	mov	%r12, %rax		C				  mul_2
	adc	%rdx, %rbx		C				  mul_2
	mul	v0			C				  mul_2
	add	%rax, %rbp		C				  mul_2
	mov	%r12, %rax		C				  mul_2
	adc	%rdx, %rbx		C				  mul_2
	adc	$0, %ecx		C				  mul_2
	mul	v1			C				  mul_2
	mov	%rbp, -8(rp,j,8)	C				  mul_2
	mov	(up,j,8), %r12		C				  mul_2
	mov	$0, %ebp		C				  mul_2
	add	%rax, %rbx		C				  mul_2
	mov	%r12, %rax		C				  mul_2
	adc	%rdx, %rcx		C				  mul_2
	add	$3, j			C				  mul_2
	jns	L(ed2)			C				  mul_2
	mul	v0			C				  mul_2
	add	%rax, %rbx		C				  mul_2
	mov	%r12, %rax		C				  mul_2
	adc	%rdx, %rcx		C				  mul_2
	adc	$0, %ebp		C				  mul_2
	mul	v1			C				  mul_2
	mov	%rbx, -24(rp,j,8)	C				  mul_2
	mov	-16(up,j,8), %r12	C				  mul_2
	mov	$0, %ebx		C				  mul_2
	add	%rax, %rcx		C				  mul_2
	mov	%r12, %rax		C				  mul_2
	adc	%rdx, %rbp		C				  mul_2
	mul	v0			C				  mul_2
	add	%rax, %rcx		C				  mul_2
	mov	%r12, %rax		C				  mul_2
	adc	%rdx, %rbp		C				  mul_2
	adc	$0, %ebx		C				  mul_2
	mul	v1			C				  mul_2
	mov	%rcx, -16(rp,j,8)	C				  mul_2
	mov	-8(up,j,8), %r12	C				  mul_2
	mov	$0, %ecx		C				  mul_2
	add	%rax, %rbp		C				  mul_2
	mov	%r12, %rax		C				  mul_2
	adc	%rdx, %rbx		C				  mul_2
	mul	v0			C				  mul_2
	add	%rax, %rbp		C				  mul_2
	mov	%r12, %rax		C				  mul_2
	adc	%rdx, %rbx		C				  mul_2
	adc	$0, %ecx		C				  mul_2
	mul	v1			C				  mul_2
	mov	%rbp, -8(rp,j,8)	C				  mul_2
	mov	(up,j,8), %r12		C				  mul_2
	mov	$0, %ebp		C				  mul_2
	add	%rax, %rbx		C				  mul_2
	mov	%r12, %rax		C				  mul_2
	adc	%rdx, %rcx		C				  mul_2
	add	$3, j			C				  mul_2
	js	L(lp2)			C				  mul_2

L(ed2):	jne	L(n3)			C				  mul_2
	mul	v0			C				  mul_2
	add	%rax, %rbx		C				  mul_2
	mov	%r12, %rax		C				  mul_2
	adc	%rdx, %rcx		C				  mul_2
	adc	$0, %ebp		C				  mul_2
	mul	v1			C				  mul_2
	mov	%rbx, -24(rp)		C				  mul_2
	mov	$0, %ebx		C				  mul_2
	add	%rax, %rcx		C				  mul_2
	mov	-16(up), %rax		C				  mul_2
	adc	%rdx, %rbp		C				  mul_2
	mul	v0			C				  mul_2
	add	%rax, %rcx		C				  mul_2
	mov	-16(up), %rax		C				  mul_2
	adc	%rdx, %rbp		C				  mul_2
	adc	$0, %ebx		C				  mul_2
	mul	v1			C				  mul_2
	mov	%rcx, -16(rp)		C				  mul_2
	mov	$0, %ecx		C				  mul_2
	add	%rax, %rbp		C				  mul_2
	mov	-8(up), %rax		C				  mul_2
	adc	%rdx, %rbx		C				  mul_2
	mul	v0			C				  mul_2
	add	%rax, %rbp		C				  mul_2
	mov	-8(up), %rax		C				  mul_2
	adc	%rdx, %rbx		C				  mul_2
	adc	$0, %ecx		C				  mul_2
	mul	v1			C				  mul_2
	mov	%rbp, -8(rp)		C				  mul_2
	add	%rax, %rbx		C				  mul_2
	adc	%rdx, %rcx		C				  mul_2
	mov	%rbx, (rp)		C				  mul_2
	mov	%rcx, 8(rp)		C				  mul_2
	lea	16(rp), rp		C rp += 2
	jmp	L(jn1)			C				  mul_2

L(n3):	cmp	$1, j			C				  mul_2
	jne	L(n2)			C				  mul_2
	mul	v0			C				  mul_2
	add	%rax, %rbx		C				  mul_2
	mov	-16(up), %rax		C				  mul_2
	adc	%rdx, %rcx		C				  mul_2
	adc	$0, %ebp		C				  mul_2
	mul	v1			C				  mul_2
	mov	%rbx, -16(rp)		C				  mul_2
	mov	$0, %ebx		C				  mul_2
	add	%rax, %rcx		C				  mul_2
	mov	-8(up), %rax		C				  mul_2
	adc	%rdx, %rbp		C				  mul_2
	mul	v0			C				  mul_2
	add	%rax, %rcx		C				  mul_2
	mov	-8(up), %rax		C				  mul_2
	adc	%rdx, %rbp		C				  mul_2
	adc	$0, %ebx		C				  mul_2
	mul	v1			C				  mul_2
	mov	%rcx, -8(rp)		C				  mul_2
	add	%rax, %rbp		C				  mul_2
	adc	%rdx, %rbx		C				  mul_2
	mov	%rbp, (rp)		C				  mul_2
	mov	%rbx, 8(rp)		C				  mul_2
	lea	16(rp), rp		C rp += 2
	jmp	L(jn1)			C				  mul_2

L(n2):	mul	v0			C				  mul_2
	add	%rax, %rbx		C				  mul_2
	mov	-8(up), %rax		C				  mul_2
	adc	%rdx, %rcx		C				  mul_2
	adc	$0, %ebp		C				  mul_2
	mul	v1			C				  mul_2
	mov	%rbx, -8(rp)		C				  mul_2
	add	%rax, %rcx		C				  mul_2
	adc	%rdx, %rbp		C				  mul_2
	mov	%rcx, (rp)		C				  mul_2
	mov	%rbp, 8(rp)		C				  mul_2
	lea	16(rp), rp		C rp += 2
C	jmp	L(jn1)			C				  mul_2

L(jn1):	test	vn, vn
	je	L(olex)

C In order to choose the right variant of code, compute un mod 3.
	movabs	$0x5555555555555555, %rdx
	mov	un, %rax
	mul	%rdx			C rdx = un / 3 (for relevant un)
	lea	2(%rdx,%rdx,2), %rdx
	sub	un, %rdx		C rdx = -(un mod 3)	FIXME
	js	L(M0)
	je	L(M2)

define(`v0', `%r9')  define(`v1', `%r10')
define(`j',`%r13')

L(M1):
L(M1_lpo):
	mov	(vp), v0		C				    LM1
	mov	8(vp), v1		C				    LM1
	lea	16(vp), vp		C vp += 2

	mov	un, j			C				    LM1
	neg	j			C				    LM1

	xor	%ebx, %ebx		C				    LM1
	xor	%ecx, %ecx		C				    LM1
	xor	%ebp, %ebp		C				    LM1

	mov	(up,j,8), %r12		C				    LM1
	mov	%r12, %rax		C				    LM1
	add	$3, j			C				    LM1
	jns	L(M1_lpie)		C <= 4 iterations

	ALIGN(16)
L(M1_lpi):
	mul	v0			C				    LM1
	add	%rax, %rbx		C				    LM1
	mov	%r12, %rax		C				    LM1
	adc	%rdx, %rcx		C				    LM1
	adc	$0, %ebp		C				    LM1
	mul	v1			C				    LM1
	add	%rbx, -24(rp,j,8)	C				    LM1
	mov	-16(up,j,8), %r12	C				    LM1
	mov	$0, %ebx		C				    LM1
	adc	%rax, %rcx		C				    LM1
	mov	%r12, %rax		C				    LM1
	adc	%rdx, %rbp		C				    LM1
	mul	v0			C				    LM1
	add	%rax, %rcx		C				    LM1
	mov	%r12, %rax		C				    LM1
	adc	%rdx, %rbp		C				    LM1
	adc	$0, %ebx		C				    LM1
	mul	v1			C				    LM1
	add	%rcx, -16(rp,j,8)	C				    LM1
	mov	-8(up,j,8), %r12	C				    LM1
	mov	$0, %ecx		C				    LM1
	adc	%rax, %rbp		C				    LM1
	mov	%r12, %rax		C				    LM1
	adc	%rdx, %rbx		C				    LM1
	mul	v0			C				    LM1
	add	%rax, %rbp		C				    LM1
	mov	%r12, %rax		C				    LM1
	adc	%rdx, %rbx		C				    LM1
	adc	$0, %ecx		C				    LM1
	mul	v1			C				    LM1
	add	%rbp, -8(rp,j,8)	C				    LM1
	mov	(up,j,8), %r12		C				    LM1
	mov	$0, %ebp		C				    LM1
	adc	%rax, %rbx		C				    LM1
	mov	%r12, %rax		C				    LM1
	adc	%rdx, %rcx		C				    LM1
	add	$3, j			C				    LM1
	jns	L(M1_lpie)		C				    LM1
	mul	v0			C				    LM1
	add	%rax, %rbx		C				    LM1
	mov	%r12, %rax		C				    LM1
	adc	%rdx, %rcx		C				    LM1
	adc	$0, %ebp		C				    LM1
	mul	v1			C				    LM1
	add	%rbx, -24(rp,j,8)	C				    LM1
	mov	-16(up,j,8), %r12	C				    LM1
	mov	$0, %ebx		C				    LM1
	adc	%rax, %rcx		C				    LM1
	mov	%r12, %rax		C				    LM1
	adc	%rdx, %rbp		C				    LM1
	mul	v0			C				    LM1
	add	%rax, %rcx		C				    LM1
	mov	%r12, %rax		C				    LM1
	adc	%rdx, %rbp		C				    LM1
	adc	$0, %ebx		C				    LM1
	mul	v1			C				    LM1
	add	%rcx, -16(rp,j,8)	C				    LM1
	mov	-8(up,j,8), %r12	C				    LM1
	mov	$0, %ecx		C				    LM1
	adc	%rax, %rbp		C				    LM1
	mov	%r12, %rax		C				    LM1
	adc	%rdx, %rbx		C				    LM1
	mul	v0			C				    LM1
	add	%rax, %rbp		C				    LM1
	mov	%r12, %rax		C				    LM1
	adc	%rdx, %rbx		C				    LM1
	adc	$0, %ecx		C				    LM1
	mul	v1			C				    LM1
	add	%rbp, -8(rp,j,8)	C				    LM1
	mov	(up,j,8), %r12		C				    LM1
	mov	$0, %ebp		C				    LM1
	adc	%rax, %rbx		C				    LM1
	mov	%r12, %rax		C				    LM1
	adc	%rdx, %rcx		C				    LM1
	add	$3, j			C				    LM1
	js	L(M1_lpi)		C				    LM1

L(M1_lpie):
	mul	v0			C				    LM1
	add	%rax, %rbx		C				    LM1
	mov	%r12, %rax		C				    LM1
	adc	%rdx, %rcx		C				    LM1
	adc	$0, %ebp		C				    LM1
	mul	v1			C				    LM1
	add	%rbx, -8(rp)		C				    LM1
	adc	%rax, %rcx		C				    LM1
	adc	%rdx, %rbp		C				    LM1
	mov	%rcx, (rp)		C				    LM1
	mov	%rbp, 8(rp)		C				    LM1

	lea	16(rp), rp		C rp += 2			    LM1
	sub	$2, vn			C vn -= 2			    LM1
	jne	L(M1_lpo)		C				    LM1
	jmp	L(olex)			C				    LM1

L(M2):
L(M2_lpo):
	mov	(vp), v0		C				    LM2
	mov	8(vp), v1		C				    LM2
	lea	16(vp), vp		C vp += 2

	mov	un, j			C				    LM2
	neg	j			C				    LM2

	xor	%ebx, %ebx		C				    LM2
	xor	%ecx, %ecx		C				    LM2
	xor	%ebp, %ebp		C				    LM2

	mov	(up,j,8), %r12		C				    LM2
	mov	%r12, %rax		C				    LM2
	add	$3, j			C				    LM2
	jns	L(M2_lpie)		C <= 4 iterations

	ALIGN(16)
L(M2_lpi):
	mul	v0			C				    LM2
	add	%rax, %rbx		C				    LM2
	mov	%r12, %rax		C				    LM2
	adc	%rdx, %rcx		C				    LM2
	adc	$0, %ebp		C				    LM2
	mul	v1			C				    LM2
	add	%rbx, -24(rp,j,8)	C				    LM2
	mov	-16(up,j,8), %r12	C				    LM2
	mov	$0, %ebx		C				    LM2
	adc	%rax, %rcx		C				    LM2
	mov	%r12, %rax		C				    LM2
	adc	%rdx, %rbp		C				    LM2
	mul	v0			C				    LM2
	add	%rax, %rcx		C				    LM2
	mov	%r12, %rax		C				    LM2
	adc	%rdx, %rbp		C				    LM2
	adc	$0, %ebx		C				    LM2
	mul	v1			C				    LM2
	add	%rcx, -16(rp,j,8)	C				    LM2
	mov	-8(up,j,8), %r12	C				    LM2
	mov	$0, %ecx		C				    LM2
	adc	%rax, %rbp		C				    LM2
	mov	%r12, %rax		C				    LM2
	adc	%rdx, %rbx		C				    LM2
	mul	v0			C				    LM2
	add	%rax, %rbp		C				    LM2
	mov	%r12, %rax		C				    LM2
	adc	%rdx, %rbx		C				    LM2
	adc	$0, %ecx		C				    LM2
	mul	v1			C				    LM2
	add	%rbp, -8(rp,j,8)	C				    LM2
	mov	(up,j,8), %r12		C				    LM2
	mov	$0, %ebp		C				    LM2
	adc	%rax, %rbx		C				    LM2
	mov	%r12, %rax		C				    LM2
	adc	%rdx, %rcx		C				    LM2
	add	$3, j			C				    LM2
	jns	L(M2_lpie)		C				    LM2
	mul	v0			C				    LM2
	add	%rax, %rbx		C				    LM2
	mov	%r12, %rax		C				    LM2
	adc	%rdx, %rcx		C				    LM2
	adc	$0, %ebp		C				    LM2
	mul	v1			C				    LM2
	add	%rbx, -24(rp,j,8)	C				    LM2
	mov	-16(up,j,8), %r12	C				    LM2
	mov	$0, %ebx		C				    LM2
	adc	%rax, %rcx		C				    LM2
	mov	%r12, %rax		C				    LM2
	adc	%rdx, %rbp		C				    LM2
	mul	v0			C				    LM2
	add	%rax, %rcx		C				    LM2
	mov	%r12, %rax		C				    LM2
	adc	%rdx, %rbp		C				    LM2
	adc	$0, %ebx		C				    LM2
	mul	v1			C				    LM2
	add	%rcx, -16(rp,j,8)	C				    LM2
	mov	-8(up,j,8), %r12	C				    LM2
	mov	$0, %ecx		C				    LM2
	adc	%rax, %rbp		C				    LM2
	mov	%r12, %rax		C				    LM2
	adc	%rdx, %rbx		C				    LM2
	mul	v0			C				    LM2
	add	%rax, %rbp		C				    LM2
	mov	%r12, %rax		C				    LM2
	adc	%rdx, %rbx		C				    LM2
	adc	$0, %ecx		C				    LM2
	mul	v1			C				    LM2
	add	%rbp, -8(rp,j,8)	C				    LM2
	mov	(up,j,8), %r12		C				    LM2
	mov	$0, %ebp		C				    LM2
	adc	%rax, %rbx		C				    LM2
	mov	%r12, %rax		C				    LM2
	adc	%rdx, %rcx		C				    LM2
	add	$3, j			C				    LM2
	js	L(M2_lpi)		C				    LM2

L(M2_lpie):
	mul	v0			C				    LM2
	add	%rax, %rbx		C				    LM2
	mov	%r12, %rax		C				    LM2
	adc	%rdx, %rcx		C				    LM2
	adc	$0, %ebp		C				    LM2
	mul	v1			C				    LM2
	add	%rbx, -16(rp)		C				    LM2
	mov	-8(up), %r12		C				    LM2
	mov	$0, %ebx		C				    LM2
	adc	%rax, %rcx		C				    LM2
	mov	%r12, %rax		C				    LM2
	adc	%rdx, %rbp		C				    LM2
	mul	v0			C				    LM2
	add	%rax, %rcx		C				    LM2
	mov	%r12, %rax		C				    LM2
	adc	%rdx, %rbp		C				    LM2
	adc	$0, %ebx		C				    LM2
	mul	v1			C				    LM2
	add	%rcx, -8(rp)		C				    LM2
	adc	%rax, %rbp		C				    LM2
	adc	%rdx, %rbx		C				    LM2
	mov	%rbp, (rp)		C				    LM2
	mov	%rbx, 8(rp)		C				    LM2

	lea	16(rp), rp		C rp += 2			    LM2
	sub	$2, vn			C vn -= 2			    LM2
	jne	L(M2_lpo)		C				    LM2
	jmp	L(olex)			C				    LM2

L(M0):
L(M0_lpo):
	mov	(vp), v0		C				    LM0
	mov	8(vp), v1		C				    LM0
	lea	16(vp), vp		C vp += 2

	mov	un, j			C				    LM0
	neg	j			C				    LM0

	xor	%ebx, %ebx		C				    LM0
	xor	%ecx, %ecx		C				    LM0
	xor	%ebp, %ebp		C				    LM0

	mov	(up,j,8), %r12		C				    LM0
	mov	%r12, %rax		C				    LM0
	add	$3, j			C				    LM0
	jns	L(M0_lpie)		C <= 4 iterations

	ALIGN(16)
L(M0_lpi):
	mul	v0			C				    LM0
	add	%rax, %rbx		C				    LM0
	mov	%r12, %rax		C				    LM0
	adc	%rdx, %rcx		C				    LM0
	adc	$0, %ebp		C				    LM0
	mul	v1			C				    LM0
	add	%rbx, -24(rp,j,8)	C				    LM0
	mov	-16(up,j,8), %r12	C				    LM0
	mov	$0, %ebx		C				    LM0
	adc	%rax, %rcx		C				    LM0
	mov	%r12, %rax		C				    LM0
	adc	%rdx, %rbp		C				    LM0
	mul	v0			C				    LM0
	add	%rax, %rcx		C				    LM0
	mov	%r12, %rax		C				    LM0
	adc	%rdx, %rbp		C				    LM0
	adc	$0, %ebx		C				    LM0
	mul	v1			C				    LM0
	add	%rcx, -16(rp,j,8)	C				    LM0
	mov	-8(up,j,8), %r12	C				    LM0
	mov	$0, %ecx		C				    LM0
	adc	%rax, %rbp		C				    LM0
	mov	%r12, %rax		C				    LM0
	adc	%rdx, %rbx		C				    LM0
	mul	v0			C				    LM0
	add	%rax, %rbp		C				    LM0
	mov	%r12, %rax		C				    LM0
	adc	%rdx, %rbx		C				    LM0
	adc	$0, %ecx		C				    LM0
	mul	v1			C				    LM0
	add	%rbp, -8(rp,j,8)	C				    LM0
	mov	(up,j,8), %r12		C				    LM0
	mov	$0, %ebp		C				    LM0
	adc	%rax, %rbx		C				    LM0
	mov	%r12, %rax		C				    LM0
	adc	%rdx, %rcx		C				    LM0
	add	$3, j			C				    LM0
	jns	L(M0_lpie)		C				    LM0
	mul	v0			C				    LM0
	add	%rax, %rbx		C				    LM0
	mov	%r12, %rax		C				    LM0
	adc	%rdx, %rcx		C				    LM0
	adc	$0, %ebp		C				    LM0
	mul	v1			C				    LM0
	add	%rbx, -24(rp,j,8)	C				    LM0
	mov	-16(up,j,8), %r12	C				    LM0
	mov	$0, %ebx		C				    LM0
	adc	%rax, %rcx		C				    LM0
	mov	%r12, %rax		C				    LM0
	adc	%rdx, %rbp		C				    LM0
	mul	v0			C				    LM0
	add	%rax, %rcx		C				    LM0
	mov	%r12, %rax		C				    LM0
	adc	%rdx, %rbp		C				    LM0
	adc	$0, %ebx		C				    LM0
	mul	v1			C				    LM0
	add	%rcx, -16(rp,j,8)	C				    LM0
	mov	-8(up,j,8), %r12	C				    LM0
	mov	$0, %ecx		C				    LM0
	adc	%rax, %rbp		C				    LM0
	mov	%r12, %rax		C				    LM0
	adc	%rdx, %rbx		C				    LM0
	mul	v0			C				    LM0
	add	%rax, %rbp		C				    LM0
	mov	%r12, %rax		C				    LM0
	adc	%rdx, %rbx		C				    LM0
	adc	$0, %ecx		C				    LM0
	mul	v1			C				    LM0
	add	%rbp, -8(rp,j,8)	C				    LM0
	mov	(up,j,8), %r12		C				    LM0
	mov	$0, %ebp		C				    LM0
	adc	%rax, %rbx		C				    LM0
	mov	%r12, %rax		C				    LM0
	adc	%rdx, %rcx		C				    LM0
	add	$3, j			C				    LM0
	js	L(M0_lpi)		C				    LM0

L(M0_lpie):
	mul	v0			C				    LM0
	add	%rax, %rbx		C				    LM0
	mov	%r12, %rax		C				    LM0
	adc	%rdx, %rcx		C				    LM0
	adc	$0, %ebp		C				    LM0
	mul	v1			C				    LM0
	add	%rbx, -24(rp)		C				    LM0
	mov	-16(up), %r12		C				    LM0
	mov	$0, %ebx		C				    LM0
	adc	%rax, %rcx		C				    LM0
	mov	%r12, %rax		C				    LM0
	adc	%rdx, %rbp		C				    LM0
	mul	v0			C				    LM0
	add	%rax, %rcx		C				    LM0
	mov	%r12, %rax		C				    LM0
	adc	%rdx, %rbp		C				    LM0
	adc	$0, %ebx		C				    LM0
	mul	v1			C				    LM0
	add	%rcx, -16(rp)		C				    LM0
	mov	-8(up), %r12		C				    LM0
	mov	$0, %ecx		C				    LM0
	adc	%rax, %rbp		C				    LM0
	mov	%r12, %rax		C				    LM0
	adc	%rdx, %rbx		C				    LM0
	mul	v0			C				    LM0
	add	%rax, %rbp		C				    LM0
	mov	%r12, %rax		C				    LM0
	adc	%rdx, %rbx		C				    LM0
	adc	$0, %ecx		C				    LM0
	mul	v1			C				    LM0
	add	%rbp, -8(rp)		C				    LM0
	adc	%rax, %rbx		C				    LM0
	adc	%rdx, %rcx		C				    LM0
	mov	%rbx, (rp)		C				    LM0
	mov	%rcx, 8(rp)		C				    LM0

	lea	16(rp), rp		C rp += 2			    LM0
	sub	$2, vn			C vn -= 2			    LM0
	jne	L(M0_lpo)		C				    LM0

L(olex):
	pop	%r14
	pop	%r13
	pop	%r12
	pop	%rbp
	pop	%rbx
	ret
EPILOGUE()
