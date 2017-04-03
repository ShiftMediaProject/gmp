




























































	.text
	.align	16, 0x90
	.globl	___gmpn_mod_34lsub1
	
	.def	___gmpn_mod_34lsub1
	.scl	2
	.type	32
	.endef
___gmpn_mod_34lsub1:






	movl	8(%esp), %ecx
	movl	4(%esp), %edx

	subl	$2, %ecx
	ja	Lthree_or_more

	movl	(%edx), %eax
	jb	Lone

	movl	4(%edx), %ecx
	movl	%eax, %edx
	shrl	$24, %eax		

	andl	$0xFFFFFF, %edx		
	addl	%edx, %eax
	movl	%ecx, %edx

	andl	$0xFFFF, %ecx
	shrl	$16, %edx		
	addl	%edx, %eax

	shll	$8, %ecx		
	addl	%ecx, %eax

Lone:
	ret




Lthree_or_more:
	
	
	
	
	
	
	

	movl	%ebx, 4(%esp)		
	xorl	%ebx, %ebx

	pushl	%esi	
	xorl	%esi, %esi

	pushl	%edi	
	xorl	%eax, %eax		


	
Ltop:
	
	
	
	
	
	
	

	leal	12(%edx), %edx
	leal	-2(%ecx), %ecx

	adcl	-12(%edx), %eax
	adcl	-8(%edx), %ebx
	adcl	-4(%edx), %esi

	decl	%ecx
	jg	Ltop


	

	movl	$0xFFFFFFFF, %edi
	incl	%ecx
	js	Lcombine

	adcl	(%edx), %eax
	movl	$0xFFFFFF00, %edi
	decl	%ecx
	js	Lcombine

	adcl	4(%edx), %ebx
	movl	$0xFFFF0000, %edi


Lcombine:
	
	
	
	
	
	
	

	sbbl	%ecx, %ecx		
	movl	%eax, %edx		

	shrl	$24, %eax		
	andl	%edi, %ecx		

	subl	%ecx, %eax		
	movl	%ebx, %edi		

	shrl	$16, %ebx		
	andl	$0x00FFFFFF, %edx	

	addl	%edx, %eax		
	andl	$0xFFFF, %edi

	shll	$8, %edi		
	addl	%ebx, %eax		

	addl	%edi, %eax		
	movl	%esi, %edx		

	shrl	$8, %esi		
	andl	$0xFF, %edx		

	shll	$16, %edx		
	addl	%esi, %eax		

	addl	%edx, %eax		
	popl	%edi	

	movl	8(%esp), %ebx
	popl	%esi	

	ret



	
