

























































		
		





	.text
	.align	16, 0x90
	.globl	__gmpn_bdiv_q_1
	
__gmpn_bdiv_q_1:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	push	%rbx

	mov	%rcx, %rax
	xor	%ecx, %ecx	
	mov	%rdx, %r10

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

	lea	(%rax,%rax), %r8	
	imul	%rax, %rax		
	imul	%rbx, %rax		
	sub	%rax, %r8		

	jmp	Lcom

Levn:	bsf	%rax, %rcx
	shr	%cl, %rax
	jmp	Lodd
	

	.globl	__gmpn_pi1_bdiv_q_1
	
__gmpn_pi1_bdiv_q_1:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	mov	56(%rsp), %r8	
	mov	64(%rsp), %r9	
	push	%rbx

	mov	%rcx, %r11		
	mov	%rdx, %r10		
	mov	%r9, %rcx		

Lcom:	mov	(%rsi), %rax		

	dec	%r10
	jz	Lone

	mov	8(%rsi), %rdx		
	lea	(%rsi,%r10,8), %rsi		
	lea	(%rdi,%r10,8), %rdi		
	neg	%r10			

	shrd	%cl, %rdx, %rax

	xor	%ebx, %ebx
	jmp	Lent

	.align	8, 0x90
Ltop:
	
	
	
	
	

	mul	%r11			
	mov	(%rsi,%r10,8), %rax
	mov	8(%rsi,%r10,8), %r9
	shrd	%cl, %r9, %rax
	nop
	sub	%rbx, %rax		
	setc	%bl
	sub	%rdx, %rax		
	adc	$0, %rbx
Lent:	imul	%r8, %rax
	mov	%rax, (%rdi,%r10,8)
	inc	%r10
	jnz	Ltop

	mul	%r11			
	mov	(%rsi), %rax		
	shr	%cl, %rax
	sub	%rbx, %rax		
	sub	%rdx, %rax		
	imul	%r8, %rax
	mov	%rax, (%rdi)
	pop	%rbx
	pop	%rsi
	pop	%rdi
	ret

Lone:	shr	%cl, %rax
	imul	%r8, %rax
	mov	%rax, (%rdi)
	pop	%rbx
	pop	%rsi
	pop	%rdi
	ret
	
