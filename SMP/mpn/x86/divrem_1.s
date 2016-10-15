












































































































	.text
	.align	16, 0x90

	.globl	___gmpn_divrem_1c
	
	.def	___gmpn_divrem_1c
	.scl	2
	.type	32
	.endef
___gmpn_divrem_1c:






	movl	16(%esp), %ecx
	pushl	%edi		

	movl	16(%esp), %edi
	pushl	%esi		

	movl	28(%esp), %esi
	pushl	%ebx		

	movl	16(%esp), %ebx
	pushl	%ebp		

	movl	24(%esp), %ebp
	orl	%ecx, %ecx

	movl	40(%esp), %edx
	jz	Lfraction

	leal	-4(%ebx,%ebp,4), %ebx	
	jmp	Linteger_top

	


	.globl	___gmpn_divrem_1
	
	.def	___gmpn_divrem_1
	.scl	2
	.type	32
	.endef
___gmpn_divrem_1:






	movl	16(%esp), %ecx
	pushl	%edi		

	movl	16(%esp), %edi
	pushl	%esi		

	movl	28(%esp), %esi
	orl	%ecx,%ecx

	jz	Lsize_zero
	pushl	%ebx		

	movl	-4(%edi,%ecx,4), %eax	
	xorl	%edx, %edx

	movl	16(%esp), %ebx
	pushl	%ebp		

	movl	24(%esp), %ebp
	cmpl	%esi, %eax

	leal	-4(%ebx,%ebp,4), %ebx	
	jae	Linteger_entry


	

	movl	%edx, (%ebx,%ecx,4)
	decl	%ecx

	movl	%eax, %edx
	jz	Lfraction


Linteger_top:
	
	
	
	
	
	
	

	movl	-4(%edi,%ecx,4), %eax
Linteger_entry:

	divl	%esi

	movl	%eax, (%ebx,%ecx,4)
	decl	%ecx
	jnz	Linteger_top


Lfraction:
	orl	%ebp, %ecx
	jz	Ldone

	movl	20(%esp), %ebx


Lfraction_top:
	
	
	
	
	
	
	

	xorl	%eax, %eax

	divl	%esi

	movl	%eax, -4(%ebx,%ecx,4)
	decl	%ecx
	jnz	Lfraction_top


Ldone:
	popl	%ebp
	movl	%edx, %eax
	popl	%ebx
	popl	%esi
	popl	%edi
	ret




Lsize_zero:

	movl	16(%esp), %ecx
	xorl	%eax, %eax

	movl	12(%esp), %edi

	cld	

	rep
	stosl

	popl	%esi
	popl	%edi
	ret


	
