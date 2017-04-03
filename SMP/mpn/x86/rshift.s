





























































	.text
	.align	8, 0x90
	.globl	___gmpn_rshift
	
	.def	___gmpn_rshift
	.scl	2
	.type	32
	.endef
___gmpn_rshift:





	pushl	%edi
	pushl	%esi
	pushl	%ebx


	movl	16(%esp),%edi
	movl	20(%esp),%esi
	movl	24(%esp),%edx
	movl	28(%esp),%ecx

	leal	-4(%edi,%edx,4),%edi
	leal	(%esi,%edx,4),%esi
	negl	%edx

	movl	(%esi,%edx,4),%ebx	
	xorl	%eax,%eax
	shrdl	%cl, %ebx, %eax	
	incl	%edx
	jz	Lend
	pushl	%eax			
	testb	$1,%dl
	jnz	L1			
	movl	%ebx,%eax

	.align	8, 0x90
Loop:	movl	(%esi,%edx,4),%ebx	
	shrdl	%cl, %ebx, %eax	
	movl	%eax,(%edi,%edx,4)	
	incl	%edx
L1:	movl	(%esi,%edx,4),%eax
	shrdl	%cl, %eax, %ebx
	movl	%ebx,(%edi,%edx,4)
	incl	%edx
	jnz	Loop

	shrl	%cl,%eax		
	movl	%eax,(%edi)		

	popl	%eax			

	popl	%ebx
	popl	%esi
	popl	%edi
	ret



Lend:	shrl	%cl,%ebx		
	movl	%ebx,(%edi)		

	popl	%ebx
	popl	%esi
	popl	%edi
	ret



	
