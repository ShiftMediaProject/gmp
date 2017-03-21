
















































	
	
	

















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
	pushl	%ebx
	pushl	%ebp


	movl	20(%esp),%edi
	movl	24(%esp),%esi
	movl	28(%esp),%ebp
	movl	32(%esp),%ecx

	movl	(%ebp),%ebx

	decl	%ecx
	movl	%ecx,%edx
	shrl	$3,%ecx
	andl	$7,%edx
	testl	%ecx,%ecx		
	jz	Lendgo

	pushl	%edx

	movl	40(%esp),%eax
	shrl	%eax			
	jmp	Loop

Lendgo:

	movl	36(%esp),%eax
	shrl	%eax			
	jmp	Lend

	


	.align	8, 0x90
	.globl	___gmpn_sub_n
	
	.def	___gmpn_sub_n
	.scl	2
	.type	32
	.endef
___gmpn_sub_n:





	pushl	%edi
	pushl	%esi
	pushl	%ebx
	pushl	%ebp


	movl	20(%esp),%edi
	movl	24(%esp),%esi
	movl	28(%esp),%ebp
	movl	32(%esp),%ecx

	movl	(%ebp),%ebx

	decl	%ecx
	movl	%ecx,%edx
	shrl	$3,%ecx
	andl	$7,%edx
	testl	%ecx,%ecx		
	jz	Lend
	pushl	%edx


	.align	8, 0x90
Loop:	movl	28(%edi),%eax		
	leal	32(%edi),%edi

L1:	movl	(%esi),%eax
	movl	4(%esi),%edx
	sbbl	%ebx,%eax
	movl	4(%ebp),%ebx
	sbbl	%ebx,%edx
	movl	8(%ebp),%ebx
	movl	%eax,-32(%edi)
	movl	%edx,-28(%edi)

L2:	movl	8(%esi),%eax
	movl	12(%esi),%edx
	sbbl	%ebx,%eax
	movl	12(%ebp),%ebx
	sbbl	%ebx,%edx
	movl	16(%ebp),%ebx
	movl	%eax,-24(%edi)
	movl	%edx,-20(%edi)

L3:	movl	16(%esi),%eax
	movl	20(%esi),%edx
	sbbl	%ebx,%eax
	movl	20(%ebp),%ebx
	sbbl	%ebx,%edx
	movl	24(%ebp),%ebx
	movl	%eax,-16(%edi)
	movl	%edx,-12(%edi)

L4:	movl	24(%esi),%eax
	movl	28(%esi),%edx
	sbbl	%ebx,%eax
	movl	28(%ebp),%ebx
	sbbl	%ebx,%edx
	movl	32(%ebp),%ebx
	movl	%eax,-8(%edi)
	movl	%edx,-4(%edi)

	leal	32(%esi),%esi
	leal	32(%ebp),%ebp
	decl	%ecx
	jnz	Loop

	popl	%edx

Lend:
	decl	%edx			
	js	Lend2
	incl	%edx
Loop2:
	leal	4(%edi),%edi
	movl	(%esi),%eax
	sbbl	%ebx,%eax
	movl	4(%ebp),%ebx
	movl	%eax,-4(%edi)
	leal	4(%esi),%esi
	leal	4(%ebp),%ebp
	decl	%edx
	jnz	Loop2
Lend2:
	movl	(%esi),%eax
	sbbl	%ebx,%eax
	movl	%eax,(%edi)

	sbbl	%eax,%eax
	negl	%eax

	popl	%ebp
	popl	%ebx
	popl	%esi
	popl	%edi
	ret



	
