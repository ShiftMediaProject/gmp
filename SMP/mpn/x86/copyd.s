


































































	.text
	.align	32, 0x90

	.globl	___gmpn_copyd
	
	.def	___gmpn_copyd
	.scl	2
	.type	32
	.endef
___gmpn_copyd:




	
	
	
	
	
	
	

	movl	12(%esp), %ecx
	movl	%esi, %eax

	movl	8(%esp), %esi
	movl	%edi, %edx

	movl	4(%esp), %edi
	leal	-4(%esi,%ecx,4), %esi

	leal	-4(%edi,%ecx,4), %edi

	std

	rep
	movsl

	cld

	movl	%eax, %esi
	movl	%edx, %edi

	ret



	
