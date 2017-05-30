dnl  AMD64 SSSE3/XOP mpn_hamdist -- hamming distance.

dnl  Copyright 2010-2017 Free Software Foundation, Inc.

dnl  This file is part of the GNU MP Library.
dnl
dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of either:
dnl
dnl    * the GNU Lesser General Public License as published by the Free
dnl      Software Foundation; either version 3 of the License, or (at your
dnl      option) any later version.
dnl
dnl  or
dnl
dnl    * the GNU General Public License as published by the Free Software
dnl      Foundation; either version 2 of the License, or (at your option) any
dnl      later version.
dnl
dnl  or both in parallel, as here.
dnl
dnl  The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
dnl  for more details.
dnl
dnl  You should have received copies of the GNU General Public License and the
dnl  GNU Lesser General Public License along with the GNU MP Library.  If not,
dnl  see https://www.gnu.org/licenses/.


include(`../config.m4')

C		    cycles/limb	  good for cpu?
C AMD K8,K9		n/a
C AMD K10		n/a
C AMD bd1	     1.93-2.49		y
C AMD bd2	     1.81-2.30		y
C AMD bd3		 ?
C AMD bd4		 ?
C AMD zen		n/a
C AMD bobcat		n/a
C AMD jaguar		n/a
C Intel P4		n/a
C Intel PNR		n/a
C Intel NHM		n/a
C Intel SBR		n/a
C Intel IBR		n/a
C Intel HWL		n/a
C Intel BWL		n/a
C Intel SKL		n/a
C Intel atom		n/a
C Intel SLM		n/a
C VIA nano		n/a

define(`up',		`%rdi')
define(`vp',		`%rsi')
define(`n',		`%rdx')

ASM_START()
	TEXT
	ALIGN(32)
PROLOGUE(mpn_hamdist)
	lea	L(cnsts)(%rip), %r9

ifdef(`PIC', `define(`OFF1',32) define(`OFF2',48) define(`OFF3',64)',
	     `define(`OFF1',64) define(`OFF2',80) define(`OFF3',96)')
	movdqa	OFF1`'(%r9), %xmm7	C nibble counts table
	movdqa	OFF2`'(%r9), %xmm6 	C splat shift counts
	movdqa	OFF3`'(%r9), %xmm9 	C masks
	pxor	%xmm4, %xmm4
	pxor	%xmm5, %xmm5		C 0-reg for psadbw
	pxor	%xmm8, %xmm8		C grand total count

	xor	R32(%r10), R32(%r10)

	mov	R32(n), R32(%rax)
	and	$7, R32(%rax)
ifdef(`PIC',`
	movslq	(%r9,%rax,4), %rax
	add	%r9, %rax
	jmp	*%rax
',`
	jmp	*(%r9,%rax,8)
')

L(1):	mov	(up), %r10
	add	$8, up
	xor	(vp), %r10
	add	$8, vp
	.byte	0xf3,0x4d,0x0f,0xb8,0xd2	C popcnt %r10,%r10
	dec	n
	jnz	L(top)
	mov	%r10, %rax
	ret

L(2):	add	$-48, up
	add	$-48, vp
	jmp	L(e2)

L(3):	mov	(up), %r10
	add	$-40, up
	xor	(vp), %r10
	add	$-40, vp
	.byte	0xf3,0x4d,0x0f,0xb8,0xd2	C popcnt %r10,%r10
	jmp	L(e2)

L(4):	add	$-32, up
	add	$-32, vp
	jmp	L(e4)

L(5):	mov	(up), %r10
	add	$-24, up
	xor	(vp), %r10
	add	$-24, vp
	.byte	0xf3,0x4d,0x0f,0xb8,0xd2	C popcnt %r10,%r10
	jmp	L(e4)

L(6):	add	$-16, up
	add	$-16, vp
	jmp	L(e6)

L(7):	mov	(up), %r10
	add	$-8, up
	xor	(vp), %r10
	add	$-8, vp
	.byte	0xf3,0x4d,0x0f,0xb8,0xd2	C popcnt %r10,%r10
	jmp	L(e6)

	ALIGN(32)
L(top):	lddqu	(up), %xmm0
	lddqu	(vp), %xmm10
	pxor	%xmm10, %xmm0
	vpshlb	%xmm6, %xmm0, %xmm1
	pand	%xmm9, %xmm0
	pand	%xmm9, %xmm1
	vpperm	%xmm0, %xmm7, %xmm7, %xmm2
	vpperm	%xmm1, %xmm7, %xmm7, %xmm3
	paddb	%xmm2, %xmm3
	paddb	%xmm3, %xmm4
L(e6):	lddqu	16(up), %xmm0
	lddqu	16(vp), %xmm10
	pxor	%xmm10, %xmm0
	vpshlb	%xmm6, %xmm0, %xmm1
	pand	%xmm9, %xmm0
	pand	%xmm9, %xmm1
	vpperm	%xmm0, %xmm7, %xmm7, %xmm2
	vpperm	%xmm1, %xmm7, %xmm7, %xmm3
	paddb	%xmm2, %xmm3
	paddb	%xmm3, %xmm4
L(e4):	lddqu	32(up), %xmm0
	lddqu	32(vp), %xmm10
	pxor	%xmm10, %xmm0
	vpshlb	%xmm6, %xmm0, %xmm1
	pand	%xmm9, %xmm0
	pand	%xmm9, %xmm1
	vpperm	%xmm0, %xmm7, %xmm7, %xmm2
	vpperm	%xmm1, %xmm7, %xmm7, %xmm3
	paddb	%xmm2, %xmm3
	paddb	%xmm3, %xmm4
L(e2):	lddqu	48(up), %xmm0
	add	$64, up
	lddqu	48(vp), %xmm10
	add	$64, vp
	pxor	%xmm10, %xmm0
	vpshlb	%xmm6, %xmm0, %xmm1
	pand	%xmm9, %xmm0
	pand	%xmm9, %xmm1
	vpperm	%xmm0, %xmm7, %xmm7, %xmm2
	psadbw	%xmm5, %xmm4		C sum to 8 x 16-bit counts
	paddq	%xmm4, %xmm8		C sum to 2 x 64-bit counts
	vpperm	%xmm1, %xmm7, %xmm7, %xmm4
	paddb	%xmm2, %xmm4
	sub	$8, n
	jg	L(top)

	psadbw	%xmm5, %xmm4
	paddq	%xmm4, %xmm8
	pshufd	$14, %xmm8, %xmm0
	paddq	%xmm8, %xmm0
	movq	%xmm0, %rax
	add	%r10, %rax
	ret
EPILOGUE()
DEF_OBJECT(L(cnsts),16)
	JMPENT(	L(top), L(cnsts))
	JMPENT(	L(1), L(cnsts))
	JMPENT(	L(2), L(cnsts))
	JMPENT(	L(3), L(cnsts))
	JMPENT(	L(4), L(cnsts))
	JMPENT(	L(5), L(cnsts))
	JMPENT(	L(6), L(cnsts))
	JMPENT(	L(7), L(cnsts))
	.byte	0x00,0x01,0x01,0x02,0x01,0x02,0x02,0x03
	.byte	0x01,0x02,0x02,0x03,0x02,0x03,0x03,0x04
	.byte	-4,-4,-4,-4,-4,-4,-4,-4
	.byte	-4,-4,-4,-4,-4,-4,-4,-4
	.byte	0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f
	.byte	0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f
END_OBJECT(L(cnsts))
