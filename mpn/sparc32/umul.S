#include "asm-syntax.h"

	 .text
	.align 4
	.global C_SYMBOL_NAME(__umul_ppmm)
C_SYMBOL_NAME(__umul_ppmm):
	wr	%g0,%o1,%y
	sra	%o2,31,%g2	! Don't move this insn
	and	%o1,%g2,%g2	! Don't move this insn
	andcc	%g0,0,%g1	! Don't move this insn
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
