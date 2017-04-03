



































































	.text
	.align	8, 0x90
	.globl	___gmpn_mul_1
	
	.def	___gmpn_mul_1
	.scl	2
	.type	32
	.endef
___gmpn_mul_1:






	pushl	%edi
	pushl	%esi
	pushl	%ebx
	pushl	%ebp


	movl	20(%esp),%edi
	movl	24(%esp),%esi
	movl	28(%esp),%ecx

	xorl	%ebx,%ebx
	andl	$3,%ecx
	jz	Lend0

Loop0:
	movl	(%esi),%eax
	mull	32(%esp)
	leal	4(%esi),%esi
	addl	%ebx,%eax
	movl	$0,%ebx
	adcl	%ebx,%edx
	movl	%eax,(%edi)
	movl	%edx,%ebx	

	leal	4(%edi),%edi
	decl	%ecx
	jnz	Loop0

Lend0:
	movl	28(%esp),%ecx
	shrl	$2,%ecx
	jz	Lend


	.align	8, 0x90
Loop:	movl	(%esi),%eax
	mull	32(%esp)
	addl	%eax,%ebx
	movl	$0,%ebp
	adcl	%edx,%ebp

	movl	4(%esi),%eax
	mull	32(%esp)
	movl	%ebx,(%edi)
	addl	%eax,%ebp	
	movl	$0,%ebx
	adcl	%edx,%ebx

	movl	8(%esi),%eax
	mull	32(%esp)
	movl	%ebp,4(%edi)
	addl	%eax,%ebx	
	movl	$0,%ebp
	adcl	%edx,%ebp

	movl	12(%esi),%eax
	mull	32(%esp)
	movl	%ebx,8(%edi)
	addl	%eax,%ebp	
	movl	$0,%ebx
	adcl	%edx,%ebx

	movl	%ebp,12(%edi)

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



	
