
































































	.text





	.align	16, 0x90
	.globl	___gmpn_pi1_bdiv_q_1
	
	.def	___gmpn_pi1_bdiv_q_1
	.scl	2
	.type	32
	.endef
___gmpn_pi1_bdiv_q_1:






	movl	24(%esp), %ecx
	pushl	%ebp	

	movl	24(%esp), %eax
	movl	16(%esp), %ebp
	pushl	%ebx	
Lcommon:
	pushl	%edi	
	pushl	%esi	

	movl	24(%esp), %esi
	movl	20(%esp), %edi

	leal	(%esi,%ebp,4), %esi	
	leal	(%edi,%ebp,4), %edi	
	negl	%ebp			

	movl	%eax, 24(%esp)
	movl	(%esi,%ebp,4), %eax	

	xorl	%ebx, %ebx
	xorl	%edx, %edx

	incl	%ebp
	jz	Lone

	movl	(%esi,%ebp,4), %edx	

	shrdl	%cl, %edx, %eax

	movl	24(%esp), %edx
	jmp	Lentry


	.align	8, 0x90
	nop	
	nop
Ltop:
	
	
	
	
	
	
	

	movl	-4(%esi,%ebp,4), %eax
	subl	%ebx, %edx		

	movl	(%esi,%ebp,4), %ebx

	shrdl	%cl, %ebx, %eax

	subl	%edx, %eax		
	movl	24(%esp), %edx

	sbbl	%ebx, %ebx

Lentry:
	imull	%edx, %eax

	movl	%eax, -4(%edi,%ebp,4)
	movl	32(%esp), %edx

	mull	%edx

	incl	%ebp
	jnz	Ltop


	movl	-4(%esi), %eax		
Lone:
	shrl	%cl, %eax
	popl	%esi	

	addl	%ebx, %eax		

	subl	%edx, %eax		

	imull	20(%esp), %eax

	movl	%eax, -4(%edi)

	popl	%edi
	popl	%ebx
	popl	%ebp

	ret



	





	.align	16, 0x90
	.globl	___gmpn_bdiv_q_1
	
	.def	___gmpn_bdiv_q_1
	.scl	2
	.type	32
	.endef
___gmpn_bdiv_q_1:






	movl	16(%esp), %eax
	pushl	%ebp	

	movl	$-1, %ecx		
	movl	16(%esp), %ebp

	pushl	%ebx	

Lstrip_twos:
	incl	%ecx

	shrl	%eax
	jnc	Lstrip_twos

	leal	1(%eax,%eax), %ebx	
	andl	$127, %eax		


	movzbl	___gmp_binvert_limb_table(%eax), %eax	


	leal	(%eax,%eax), %edx	
	movl	%ebx, 24(%esp)	
	imull	%eax, %eax		
	imull	%ebx, %eax		
	subl	%eax, %edx		

	leal	(%edx,%edx), %eax	
	imull	%edx, %edx		
	imull	%ebx, %edx		
	subl	%edx, %eax		

	

	jmp	Lcommon
	

