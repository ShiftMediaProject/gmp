



















































	.text
	.align	8, 0x90
	.globl	___gmpn_umul_ppmm
	
	.def	___gmpn_umul_ppmm
	.scl	2
	.type	32
	.endef
___gmpn_umul_ppmm:





	movl	4(%esp), %ecx
	movl	8(%esp), %eax
	mull	12(%esp)
	movl	%eax, (%ecx)
	movl	%edx, %eax
	ret


	
