





























































      
      














	.text
	.align	8, 0x90

	.globl	___gmpn_submul_1
	
	.def	___gmpn_submul_1
	.scl	2
	.type	32
	.endef
___gmpn_submul_1:






	pushl	%edi
	pushl	%esi
	pushl	%ebx
	pushl	%ebp


	movl	16+4(%esp),%edi
	movl	16+8(%esp),%esi
	movl	16+12(%esp),%ecx

	xorl	%ebx,%ebx
	andl	$3,%ecx
	jz	Lend0

Loop0:
	movl	(%esi),%eax
	mull	16+16(%esp)
	leal	4(%esi),%esi
	addl	%ebx,%eax
	movl	$0,%ebx
	adcl	%ebx,%edx
	subl	%eax,(%edi)
	adcl	%edx,%ebx	

	leal	4(%edi),%edi
	decl	%ecx
	jnz	Loop0

Lend0:
	movl	16+12(%esp),%ecx
	shrl	$2,%ecx
	jz	Lend

	.align	8, 0x90
Loop:	movl	(%esi),%eax
	mull	16+16(%esp)
	addl	%eax,%ebx
	movl	$0,%ebp
	adcl	%edx,%ebp

	movl	4(%esi),%eax
	mull	16+16(%esp)
	subl	%ebx,(%edi)
	adcl	%eax,%ebp	
	movl	$0,%ebx
	adcl	%edx,%ebx

	movl	8(%esi),%eax
	mull	16+16(%esp)
	subl	%ebp,4(%edi)
	adcl	%eax,%ebx	
	movl	$0,%ebp
	adcl	%edx,%ebp

	movl	12(%esi),%eax
	mull	16+16(%esp)
	subl	%ebx,8(%edi)
	adcl	%eax,%ebp	
	movl	$0,%ebx
	adcl	%edx,%ebx

	subl	%ebp,12(%edi)
	adcl	$0,%ebx		

	leal	16(%esi),%esi
	leal	16(%edi),%edi
	decl	%ecx
	jnz	Loop

Lend:	movl	%ebx,%eax

	popl	%ebp
	popl	%ebx
	popl	%esi
	popl	%edi
	ret



	
