




















































	.text
	.align	8, 0x90
	.globl	___gmpn_udiv_qrnnd
	
	.def	___gmpn_udiv_qrnnd
	.scl	2
	.type	32
	.endef
___gmpn_udiv_qrnnd:





	movl	12(%esp), %eax
	movl	8(%esp), %edx
	divl	16(%esp)
	movl	4(%esp), %ecx
	movl	%edx, (%ecx)
	ret


	
