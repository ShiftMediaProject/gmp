





























































	.text
	.align	8, 0x90
	.globl	___gmpn_lshift
	
	.def	___gmpn_lshift
	.scl	2
	.type	32
	.endef
___gmpn_lshift:





	pushl	%edi
	pushl	%esi
	pushl	%ebx


	movl	16(%esp),%edi
	movl	20(%esp),%esi
	movl	24(%esp),%edx
	movl	28(%esp),%ecx

	subl	$4,%esi			

	movl	(%esi,%edx,4),%ebx	
	xorl	%eax,%eax
	shldl	%cl, %ebx, %eax	
	decl	%edx
	jz	Lend
	pushl	%eax			
	testb	$1,%dl
	jnz	L1			
	movl	%ebx,%eax

	.align	8, 0x90
Loop:	movl	(%esi,%edx,4),%ebx	
	shldl	%cl, %ebx, %eax	
	movl	%eax,(%edi,%edx,4)	
	decl	%edx
L1:	movl	(%esi,%edx,4),%eax
	shldl	%cl, %eax, %ebx
	movl	%ebx,(%edi,%edx,4)
	decl	%edx
	jnz	Loop

	shll	%cl,%eax		
	movl	%eax,(%edi)		

	popl	%eax			

	popl	%ebx
	popl	%esi
	popl	%edi
	ret



Lend:	shll	%cl,%ebx		
	movl	%ebx,(%edi)		

	popl	%ebx
	popl	%esi
	popl	%edi
	ret



	
