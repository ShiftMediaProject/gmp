





















































	
	
	

















	.text
	.align	8, 0x90

	.globl	___gmpn_add_nc
	
	.def	___gmpn_add_nc
	.scl	2
	.type	32
	.endef
___gmpn_add_nc:






	pushl	%edi		
	pushl	%esi		

	movl	12(%esp),%edi
	movl	16(%esp),%esi
	movl	20(%esp),%edx
	movl	24(%esp),%ecx

	movl	%ecx,%eax
	shrl	$3,%ecx			
	negl	%eax
	andl	$7,%eax			
	jz	Loopgo		
	incl	%ecx			
	shll	$2,%eax			
	subl	%eax,%edi		
	subl	%eax,%esi		
	subl	%eax,%edx		
	shrl	$2,%eax			


	
	leal	Loop-3(%eax,%eax,8),%eax


	
	
	pushl	%ebp		
	movl	32(%esp),%ebp
	shrl	%ebp			
	popl	%ebp		

	jmp	*%eax			

	


	.align	16, 0x90
	.globl	___gmpn_add_n
	
	.def	___gmpn_add_n
	.scl	2
	.type	32
	.endef
___gmpn_add_n:






	pushl	%edi		
	pushl	%esi		

	movl	12(%esp),%edi
	movl	16(%esp),%esi
	movl	20(%esp),%edx
	movl	24(%esp),%ecx

	movl	%ecx,%eax
	shrl	$3,%ecx			
	negl	%eax
	andl	$7,%eax			
	jz	Loop			
	incl	%ecx			
	shll	$2,%eax			
	subl	%eax,%edi		
	subl	%eax,%esi		
	subl	%eax,%edx		
	shrl	$2,%eax			


	
	leal	Loop-3(%eax,%eax,8),%eax

	jmp	*%eax			

Loopgo:
	pushl	%ebp		
	movl	32(%esp),%ebp
	shrl	%ebp			
	popl	%ebp		

	.align	16, 0x90
Loop:	movl	(%esi),%eax
	adcl	(%edx),%eax
	movl	%eax,(%edi)
	movl	4(%esi),%eax
	adcl	4(%edx),%eax
	movl	%eax,4(%edi)
	movl	8(%esi),%eax
	adcl	8(%edx),%eax
	movl	%eax,8(%edi)
	movl	12(%esi),%eax
	adcl	12(%edx),%eax
	movl	%eax,12(%edi)
	movl	16(%esi),%eax
	adcl	16(%edx),%eax
	movl	%eax,16(%edi)
	movl	20(%esi),%eax
	adcl	20(%edx),%eax
	movl	%eax,20(%edi)
	movl	24(%esi),%eax
	adcl	24(%edx),%eax
	movl	%eax,24(%edi)
	movl	28(%esi),%eax
	adcl	28(%edx),%eax
	movl	%eax,28(%edi)
	leal	32(%edi),%edi
	leal	32(%esi),%esi
	leal	32(%edx),%edx
	decl	%ecx
	jnz	Loop

	sbbl	%eax,%eax
	negl	%eax

	popl	%esi
	popl	%edi
	ret



	
