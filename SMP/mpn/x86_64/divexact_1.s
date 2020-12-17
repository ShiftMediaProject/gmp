

































































	.text
	.align	16, 0x90
	.globl	__gmpn_divexact_1
	
	.def	__gmpn_divexact_1
	.scl	2
	.type	32
	.endef
__gmpn_divexact_1:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	push	%rbx

	mov	%rcx, %rax
	xor	%ecx, %ecx	
	mov	%rdx, %r8

	bt	$0, %eax
	jnc	Levn			

Lodd:	mov	%rax, %rbx
	shr	%eax
	and	$127, %eax		

	
	lea	__gmp_binvert_limb_table(%rip), %rdx


	movzbl	(%rdx,%rax), %eax	

	mov	%rbx, %r11		

	lea	(%rax,%rax), %edx	
	imul	%eax, %eax	
	imul	%ebx, %eax	
	sub	%eax, %edx	

	lea	(%rdx,%rdx), %eax	
	imul	%edx, %edx	
	imul	%ebx, %edx	
	sub	%edx, %eax	

	lea	(%rax,%rax), %r10	
	imul	%rax, %rax		
	imul	%rbx, %rax		
	sub	%rax, %r10		

	lea	(%rsi,%r8,8), %rsi	
	lea	-8(%rdi,%r8,8), %rdi	
	neg	%r8			

	mov	(%rsi,%r8,8), %rax	

	inc	%r8
	jz	Lone

	mov	(%rsi,%r8,8), %rdx	

	shrd	%cl, %rdx, %rax

	xor	%ebx, %ebx
	jmp	Lent

Levn:	bsf	%rax, %rcx
	shr	%cl, %rax
	jmp	Lodd

	.align	8, 0x90
Ltop:
	
	
	
	
	
	
	
	
	

	mul	%r11			
	mov	-8(%rsi,%r8,8), %rax	
	mov	(%rsi,%r8,8), %r9	
	shrd	%cl, %r9, %rax	
	nop				
	sub	%rbx, %rax		
	setc	%bl			
	sub	%rdx, %rax		
	adc	$0, %rbx		
Lent:	imul	%r10, %rax		
	mov	%rax, (%rdi,%r8,8)	
	inc	%r8			
	jnz	Ltop

	mul	%r11			
	mov	-8(%rsi), %rax		
	shr	%cl, %rax
	sub	%rbx, %rax		
	sub	%rdx, %rax		
	imul	%r10, %rax
	mov	%rax, (%rdi)
	pop	%rbx
	pop	%rsi
	pop	%rdi
	ret

Lone:	shr	%cl, %rax
	imul	%r10, %rax
	mov	%rax, (%rdi)
	pop	%rbx
	pop	%rsi
	pop	%rdi
	ret

	
