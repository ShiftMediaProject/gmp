#include "asm-syntax.h"

	TEXT
	ALIGN (3)
	GLOBL	C_SYMBOL_NAME(__umul_ppmm)
C_SYMBOL_NAME(__umul_ppmm:)
	movl 4(%esp),%ecx
	movl 8(%esp),%eax
	mull 12(%esp)
	movl %eax,(%ecx)
	movl %edx,%eax
	ret
