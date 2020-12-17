





































































		
		





	.text
	.align	16, 0x90
	.globl	__gmpn_bdiv_q_1
	
	.def	__gmpn_bdiv_q_1
	.scl	2
	.type	32
	.endef
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

	jmp	Lpi1

Levn:	bsf	%rax, %rcx
	shr	%cl, %rax
	jmp	Lodd
	

	.globl	__gmpn_pi1_bdiv_q_1
	
	.def	__gmpn_pi1_bdiv_q_1
	.scl	2
	.type	32
	.endef
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

Lpi1:	mov	(%rsi), %rax		

	dec	%r10
	jz	Lone

	lea	8(%rsi,%r10,8), %rsi	
	lea	(%rdi,%r10,8), %rdi		
	neg	%r10			

	test	%ecx, %ecx
	jnz	Lunorm		
	xor	%ebx, %ebx
	jmp	Lnent

	.align	8, 0x90
Lntop:mul	%r11			
	mov	-8(%rsi,%r10,8), %rax	
	sub	%rbx, %rax		
	setc	%bl		
	sub	%rdx, %rax		
	adc	$0, %ebx		
Lnent:imul	%r8, %rax		
	mov	%rax, (%rdi,%r10,8)	
	inc	%r10			
	jnz	Lntop

	mov	-8(%rsi), %r9		
	jmp	Lcom

Lunorm:
	mov	(%rsi,%r10,8), %r9	
	shr	%cl, %rax		
	neg	%ecx
	shl	%cl, %r9		
	neg	%ecx
	or	%r9, %rax
	xor	%ebx, %ebx
	jmp	Luent

	.align	8, 0x90
Lutop:mul	%r11			
	mov	(%rsi,%r10,8), %rax	
	shl	%cl, %rax		
	neg	%ecx
	or	%r9, %rax
	sub	%rbx, %rax		
	setc	%bl		
	sub	%rdx, %rax		
	adc	$0, %ebx		
Luent:imul	%r8, %rax		
	mov	(%rsi,%r10,8), %r9	
	shr	%cl, %r9		
	neg	%ecx
	mov	%rax, (%rdi,%r10,8)	
	inc	%r10			
	jnz	Lutop

Lcom:	mul	%r11			
	sub	%rbx, %r9		
	sub	%rdx, %r9		
	imul	%r8, %r9
	mov	%r9, (%rdi)
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
	
