#include "asm-syntax.h"

	 .text
	.align 4
	.global C_SYMBOL_NAME(__umul_ppmm)
C_SYMBOL_NAME(__umul_ppmm):
	umul %o1,%o2,%g2
	st %g2,[%o0]
	retl
	rd %y,%o0
