! SPARC v9 32-bit __mpn_mul_1 -- Multiply a limb vector with a limb and
! store the result in a second limb vector.

! Copyright (C) 1998 Free Software Foundation, Inc.

! This file is part of the GNU MP Library.

! The GNU MP Library is free software; you can redistribute it and/or modify
! it under the terms of the GNU Library General Public License as published by
! the Free Software Foundation; either version 2 of the License, or (at your
! option) any later version.

! The GNU MP Library is distributed in the hope that it will be useful, but
! WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
! or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
! License for more details.

! You should have received a copy of the GNU Library General Public License
! along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
! the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
! MA 02111-1307, USA.


! INPUT PARAMETERS
! res_ptr	i0
! s1_ptr	i1
! size		i2
! s2_limb	i3

.section	".rodata"
	.align 4
.Lnoll:
	.uaword 0x0
.section	".text"
	.align 4
	.global __mpn_mul_1
	.type	 __mpn_mul_1,#function
	.proc	016
__mpn_mul_1:
	!#PROLOGUE# 0
	save %sp,-256,%sp
	!#PROLOGUE# 1

	sethi	%hi(.Lnoll),%g1
	ld	[%g1+%lo(.Lnoll)],%f10

	sethi	%hi(0xffff0000),%o0
	andn	 %i3,%o0,%o0
	st	%o0,[%fp-16]
	ld	[%fp-16],%f11
	fxtod	%f10,%f6

	srl	%i3,16,%o0
	st	%o0,[%fp-16]
	ld	[%fp-16],%f11
	fxtod	%f10,%f8

	mov	0,%g3			! cy = 0

	ld	[%i1],%f11
	add	%i1,4,%i1		! s1_ptr++
	subcc	%i2,1,%i2
	be,pn	%icc,.Lend1
	nop

	fxtod	%f10,%f2
	ld	[%i1],%f11
	add	%i1,4,%i1		! s1_ptr++
	fmuld	%f2,%f8,%f16
	fmuld	%f2,%f6,%f4
	fdtox	%f16,%f14
	std	%f14,[%fp-24]
	fdtox	%f4,%f12
	std	%f12,[%fp-16]
	subcc	%i2,1,%i2
	be,pn	%icc,.Lend2

	fxtod	%f10,%f2
	ld	[%i1],%f11
	add	%i1,4,%i1		! s1_ptr++
	fmuld	%f2,%f8,%f16
	fmuld	%f2,%f6,%f4
	fdtox	%f16,%f14
	std	%f14,[%fp-40]
	fdtox	%f4,%f12
	std	%f12,[%fp-32]
	subcc	%i2,1,%i2
	be,pt	%icc,.Lend3

	fxtod	%f10,%f2
	ld	[%i1],%f11
	add	%i1,4,%i1		! s1_ptr++
	ldx	[%fp-24],%g2		! p16
	fmuld	%f2,%f8,%f16
	ldx	[%fp-16],%g1		! p0
	fmuld	%f2,%f6,%f4
	sllx	%g2,16,%g2		! align p16
	fdtox	%f16,%f14
	add	%g2,%g1,%g1		! add p16 to p0 (ADD1)
	std	%f14,[%fp-24]
	fdtox	%f4,%f12
	std	%f12,[%fp-16]
	subcc	%i2,1,%i2
	be,pn	%icc,.Lend4
	add	%i0,4,%i0		! res_ptr++

	b,a	.Loopm

	.align 16
! BEGIN LOOP
.Loop:	fxtod	%f10,%f2
	ld	[%i1],%f11
	add	%i1,4,%i1		! s1_ptr++
	add	%g3,%g1,%g4		! p += cy
	srlx	%g4,32,%g3
	ldx	[%fp-24],%g2		! p16
	fmuld	%f2,%f8,%f16
	ldx	[%fp-16],%g1		! p0
	fmuld	%f2,%f6,%f4
	sllx	%g2,16,%g2		! align p16
	st	%g4,[%i0-4]
	fdtox	%f16,%f14
	add	%g2,%g1,%g1		! add p16 to p0 (ADD1)
	std	%f14,[%fp-24]
	fdtox	%f4,%f12
	std	%f12,[%fp-16]
	subcc	%i2,1,%i2
	be,pn	%icc,.Loope
	add	%i0,4,%i0		! res_ptr++
.Loopm:
	fxtod	%f10,%f2
	ld	[%i1],%f11
	add	%i1,4,%i1		! s1_ptr++
	add	%g3,%g1,%g4		! p += cy
	srlx	%g4,32,%g3
	ldx	[%fp-40],%g2		! p16
	fmuld	%f2,%f8,%f16
	ldx	[%fp-32],%g1		! p0
	fmuld	%f2,%f6,%f4
	sllx	%g2,16,%g2		! align p16
	st	%g4,[%i0-4]
	fdtox	%f16,%f14
	add	%g2,%g1,%g1		! add p16 to p0 (ADD1)
	std	%f14,[%fp-40]
	fdtox	%f4,%f12
	std	%f12,[%fp-32]
	subcc	%i2,1,%i2
	bne,pt	%icc,.Loop
	add	%i0,4,%i0		! res_ptr++
! END LOOP

	fxtod	%f10,%f2
	add	%g3,%g1,%g4		! p += cy
	srlx	%g4,32,%g3
	ldx	[%fp-24],%g2		! p16
	fmuld	%f2,%f8,%f16
	ldx	[%fp-16],%g1		! p0
	fmuld	%f2,%f6,%f4
	sllx	%g2,16,%g2		! align p16
	st	%g4,[%i0-4]
	fdtox	%f16,%f14
	add	%g2,%g1,%g1		! add p16 to p0 (ADD1)
	std	%f14,[%fp-24]
	fdtox	%f4,%f12
	std	%f12,[%fp-16]
	add	%i0,4,%i0		! res_ptr++

	add	%g3,%g1,%g4		! p += cy
	srlx	%g4,32,%g3
	ldx	[%fp-40],%g2		! p16
	ldx	[%fp-32],%g1		! p0
	sllx	%g2,16,%g2		! align p16
	st	%g4,[%i0-4]
	add	%g2,%g1,%g1		! add p16 to p0 (ADD1)
	add	%i0,4,%i0		! res_ptr++

	add	%g3,%g1,%g4		! p += cy
	srlx	%g4,32,%g3
	ldx	[%fp-24],%g2		! p16
	ldx	[%fp-16],%g1		! p0
	sllx	%g2,16,%g2		! align p16
	st	%g4,[%i0-4]
	add	%g2,%g1,%g1		! add p16 to p0 (ADD1)
	add	%i0,4,%i0		! res_ptr++

	add	%g3,%g1,%g4		! p += cy
	srlx	%g4,32,%g3
	st	%g4,[%i0-4]
	b,a	.Lret
.Loope:
.Lend4:	fxtod	%f10,%f2
	add	%g3,%g1,%g4		! p += cy
	srlx	%g4,32,%g3
	ldx	[%fp-40],%g2		! p16
	fmuld	%f2,%f8,%f16
	ldx	[%fp-32],%g1		! p0
	fmuld	%f2,%f6,%f4
	sllx	%g2,16,%g2		! align p16
	st	%g4,[%i0-4]
	fdtox	%f16,%f14
	add	%g2,%g1,%g1		! add p16 to p0 (ADD1)
	std	%f14,[%fp-40]
	fdtox	%f4,%f12
	std	%f12,[%fp-32]
	add	%i0,4,%i0		! res_ptr++

	add	%g3,%g1,%g4		! p += cy
	srlx	%g4,32,%g3
	ldx	[%fp-24],%g2		! p16
	ldx	[%fp-16],%g1		! p0
	sllx	%g2,16,%g2		! align p16
	st	%g4,[%i0-4]
	add	%g2,%g1,%g1		! add p16 to p0 (ADD1)
	add	%i0,4,%i0		! res_ptr++

	add	%g3,%g1,%g4		! p += cy
	srlx	%g4,32,%g3
	ldx	[%fp-40],%g2		! p16
	ldx	[%fp-32],%g1		! p0
	sllx	%g2,16,%g2		! align p16
	st	%g4,[%i0-4]
	add	%g2,%g1,%g1		! add p16 to p0 (ADD1)
	add	%i0,4,%i0		! res_ptr++

	add	%g3,%g1,%g4		! p += cy
	srlx	%g4,32,%g3
	st	%g4,[%i0-4]
	b,a	.Lret

.Lend3:	fxtod	%f10,%f2
	ldx	[%fp-24],%g2		! p16
	fmuld	%f2,%f8,%f16
	ldx	[%fp-16],%g1		! p0
	fmuld	%f2,%f6,%f4
	sllx	%g2,16,%g2		! align p16
	fdtox	%f16,%f14
	add	%g2,%g1,%g1		! add p16 to p0 (ADD1)
	std	%f14,[%fp-24]
	fdtox	%f4,%f12
	std	%f12,[%fp-16]
	add	%i0,4,%i0		! res_ptr++

	add	%g3,%g1,%g4		! p += cy
	srlx	%g4,32,%g3
	ldx	[%fp-40],%g2		! p16
	ldx	[%fp-32],%g1		! p0
	sllx	%g2,16,%g2		! align p16
	st	%g4,[%i0-4]
	add	%g2,%g1,%g1		! add p16 to p0 (ADD1)
	add	%i0,4,%i0		! res_ptr++

	add	%g3,%g1,%g4		! p += cy
	srlx	%g4,32,%g3
	ldx	[%fp-24],%g2		! p16
	ldx	[%fp-16],%g1		! p0
	sllx	%g2,16,%g2		! align p16
	st	%g4,[%i0-4]
	add	%g2,%g1,%g1		! add p16 to p0 (ADD1)
	add	%i0,4,%i0		! res_ptr++

	add	%g3,%g1,%g4		! p += cy
	srlx	%g4,32,%g3
	st	%g4,[%i0-4]
	b,a	.Lret

.Lend2:	fxtod	%f10,%f2
	fmuld	%f2,%f8,%f16
	fmuld	%f2,%f6,%f4
	fdtox	%f16,%f14
	std	%f14,[%fp-40]
	fdtox	%f4,%f12
	std	%f12,[%fp-32]
	ldx	[%fp-24],%g2		! p16
	ldx	[%fp-16],%g1		! p0
	sllx	%g2,16,%g2		! align p16
	add	%g2,%g1,%g1		! add p16 to p0 (ADD1)
	add	%i0,4,%i0		! res_ptr++

	add	%g3,%g1,%g4		! p += cy
	srlx	%g4,32,%g3
	ldx	[%fp-40],%g2		! p16
	ldx	[%fp-32],%g1		! p0
	sllx	%g2,16,%g2		! align p16
	st	%g4,[%i0-4]
	add	%g2,%g1,%g1		! add p16 to p0 (ADD1)
	add	%i0,4,%i0		! res_ptr++

	add	%g3,%g1,%g4		! p += cy
	srlx	%g4,32,%g3
	st	%g4,[%i0-4]
	b,a	.Lret

.Lend1:	fxtod	%f10,%f2
	fmuld	%f2,%f8,%f16
	fmuld	%f2,%f6,%f4
	fdtox	%f16,%f14
	std	%f14,[%fp-24]
	fdtox	%f4,%f12
	std	%f12,[%fp-16]

	ldx	[%fp-24],%g2		! p16
	ldx	[%fp-16],%g1		! p0
	sllx	%g2,16,%g2		! align p16
	add	%g2,%g1,%g1		! add p16 to p0 (ADD1)
	add	%i0,4,%i0		! res_ptr++

	add	%g3,%g1,%g4		! p += cy
	srlx	%g4,32,%g3
	st	%g4,[%i0-4]
	b,a	.Lret

.Lret:	ret
	restore %g0,%g3,%o0		! sideeffect: put cy in retreg
.LLfe1:
	.size	 __mpn_mul_1,.LLfe1-__mpn_mul_1
