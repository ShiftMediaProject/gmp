C SPARC __umul_ppmm -- support for longlong.h for non-gcc.

C Copyright (C) 1995, 1996 Free Software Foundation, Inc.

C This file is part of the GNU MP Library.

C The GNU MP Library is free software; you can redistribute it and/or modify
C it under the terms of the GNU Library General Public License as published by
C the Free Software Foundation; either version 2 of the License, or (at your
C option) any later version.

C The GNU MP Library is distributed in the hope that it will be useful, but
C WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
C or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
C License for more details.

C You should have received a copy of the GNU Library General Public License
C along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
C the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
C MA 02111-1307, USA.

include(`../config.m4')

ASM_START()
PROLOGUE(__umul_ppmm)
	wr	%g0,%o1,%y
	sra	%o2,31,%g2	C Don't move this insn
	and	%o1,%g2,%g2	C Don't move this insn
	andcc	%g0,0,%g1	C Don't move this insn
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,%o2,%g1
	mulscc	%g1,0,%g1
	rd	%y,%g3
	st	%g3,[%o0]
	retl
	add	%g1,%g2,%o0
EPILOGUE(__umul_ppmm)
