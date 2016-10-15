














































































	.text
	.align	32, 0x90

	
	
	
	
	
	
	

	.globl	___gmpn_copyi
	
	.def	___gmpn_copyi
	.scl	2
	.type	32
	.endef
___gmpn_copyi:





	movl	12(%esp), %ecx
	movl	%esi, %eax

	movl	8(%esp), %esi
	movl	%edi, %edx

	movl	4(%esp), %edi

	cld	

	rep
	movsl

	movl	%eax, %esi
	movl	%edx, %edi

	ret



	
