#include "asm-syntax.h"

	TEXT
	ALIGN (3)
	GLOBL	C_SYMBOL_NAME(__udiv_qrnnd)
C_SYMBOL_NAME(__udiv_qrnnd:)
	movl 12(%esp),%eax
	movl 8(%esp),%edx
	divl 16(%esp)
	movl 4(%esp),%ecx
	movl %edx,(%ecx)
	ret
