













































































	.text
	.align	8, 0x90
	.globl	___gmpn_sqr_basecase
	
	.def	___gmpn_sqr_basecase
	.scl	2
	.type	32
	.endef
___gmpn_sqr_basecase:






	movl	12(%esp), %edx

	movl	8(%esp), %eax

	cmpl	$2, %edx
	movl	4(%esp), %ecx

	je	Ltwo_limbs
	ja	Lthree_or_more




	
	
	
	

	movl	(%eax), %eax
	mull	%eax
	movl	%eax, (%ecx)
	movl	%edx, 4(%ecx)
	ret





	.align	8, 0x90
Ltwo_limbs:
	
	
	
	

	pushl	%ebx
	pushl	%ebp

	movl	%eax, %ebx
	movl	(%eax), %eax

	mull	%eax		

	pushl	%esi
	pushl	%edi

	movl	%edx, %esi	
	movl	%eax, (%ecx)	

	movl	4(%ebx), %eax
	mull	%eax		

	movl	%eax, %edi	
	movl	%edx, %ebp	

	movl	(%ebx), %eax
	mull	4(%ebx)		

	addl	%eax, %esi

	adcl	%edx, %edi

	adcl	$0, %ebp
	addl	%esi, %eax

	adcl	%edi, %edx
	movl	%eax, 4(%ecx)

	adcl	$0, %ebp

	movl	%edx, 8(%ecx)
	movl	%ebp, 12(%ecx)

	popl	%edi
	popl	%esi

	popl	%ebp
	popl	%ebx

	ret





	.align	8, 0x90
Lthree_or_more:

	
	
	
	

	pushl	%ebx	
	pushl	%edi	

	pushl	%esi	
	pushl	%ebp	

	leal	(%ecx,%edx,4), %edi	
	leal	(%eax,%edx,4), %esi	



	movl	(%eax), %ebp		
	movl	%edx, %ecx

	negl	%ecx			
	xorl	%ebx, %ebx		

	incl	%ecx			

Lmul1:
	
	
	
	
	
	
	

	movl	(%esi,%ecx,4), %eax
	mull	%ebp
	addl	%eax, %ebx
	adcl	$0, %edx
	movl	%ebx, (%edi,%ecx,4)
	movl	%edx, %ebx
	incl	%ecx
	jnz	Lmul1

	movl	%ebx, (%edi)


	
	
	
	
	
	
	
	
	
	
	

	
	
	
	
	
	
	

	movl	28(%esp), %ecx
	subl	$3, %ecx
	jz	Lcorner

	negl	%ecx



Louter:
	
	
	
	
	
	
	

	movl	%ecx, 20(%esp)
	addl	$4, %edi		

	movl	-8(%esi,%ecx,4), %ebp	
	subl	$1, %ecx

	xorl	%ebx, %ebx		

Linner:
	
	
	
	
	
	
	

	movl	(%esi,%ecx,4), %eax
	mull	%ebp
	addl	%ebx, %eax
	adcl	$0, %edx
	addl	%eax, (%edi,%ecx,4)
	adcl	$0, %edx
	movl	%edx, %ebx
	addl	$1, %ecx
	jl	Linner


	movl	%ebx, (%edi)
	movl	20(%esp), %ecx
	incl	%ecx
	jnz	Louter


Lcorner:
	
	

	movl	-4(%esi), %eax
	mull	-8(%esi)		
	addl	%eax, 0(%edi)
	adcl	$0, %edx
	movl	%edx, 4(%edi)		





	movl	28(%esp), %eax
	negl	%eax
	addl	$1, %eax		

Llshift:
	
	
	
	
	
	
	

	rcll	8(%edi,%eax,8)
	rcll	12(%edi,%eax,8)
	incl	%eax
	jnz	Llshift


	adcl	%eax, %eax		
	movl	%eax, 8(%edi)		






	movl	24(%esp), %esi
	movl	(%esi), %eax		
	mull	%eax			

	movl	28(%esp), %ecx
	leal	(%esi,%ecx,4), %esi	

	negl	%ecx			
	movl	%edx, %ebx		

	movl	%eax, 12(%edi,%ecx,8)	
	incl	%ecx			

Ldiag:
	
	
	
	
	
	
	

	movl	(%esi,%ecx,4), %eax
	mull	%eax

	addl	%ebx, 8(%edi,%ecx,8)
	movl	%edx, %ebx

	adcl	%eax, 12(%edi,%ecx,8)
	adcl	$0, %ebx

	incl	%ecx
	jnz	Ldiag


	addl	%ebx, 8(%edi)		

	popl	%ebp
	popl	%esi

	popl	%edi
	popl	%ebx

	ret



	
