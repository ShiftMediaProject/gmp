





















































	
	
	

















	.text
	.align	8, 0x90

	.globl	___gmpn_sub_nc
	
	.def	___gmpn_sub_nc
	.scl	2
	.type	32
	.endef
___gmpn_sub_nc:






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
	.globl	___gmpn_sub_n
	
	.def	___gmpn_sub_n
	.scl	2
	.type	32
	.endef
___gmpn_sub_n:






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
	sbbl	(%edx),%eax
	movl	%eax,(%edi)
	movl	4(%esi),%eax
	sbbl	4(%edx),%eax
	movl	%eax,4(%edi)
	movl	8(%esi),%eax
	sbbl	8(%edx),%eax
	movl	%eax,8(%edi)
	movl	12(%esi),%eax
	sbbl	12(%edx),%eax
	movl	%eax,12(%edi)
	movl	16(%esi),%eax
	sbbl	16(%edx),%eax
	movl	%eax,16(%edi)
	movl	20(%esi),%eax
	sbbl	20(%edx),%eax
	movl	%eax,20(%edi)
	movl	24(%esi),%eax
	sbbl	24(%edx),%eax
	movl	%eax,24(%edi)
	movl	28(%esi),%eax
	sbbl	28(%edx),%eax
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



	
