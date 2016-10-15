















































 









 




	.text
	.align	16, 0x90

	.globl	__gmpn_div_qr_2u_pi1
	
__gmpn_div_qr_2u_pi1:

	mov	0+16(%rsp), %r10
	mov	%rdx, %r11
	push	%r15
	push	%r14
	push	%r13
	push	%r12
	push	%rbx
	push	%rbp
	push	%rsi

	lea	-2(%rcx), %rbp
	mov	%r8, %r15
	neg	%r15

	
	movl	56+8(%rsp), %ecx

	

	xor	%ebx, %ebx
	mov	8(%r11, %rbp, 8), %r12
	shld	%cl, %r12, %rbx
	
	
	mov	%r10, %rax
	mul	%rbx
	mov	(%r11, %rbp, 8), %rsi
	shld	%cl, %rsi, %r12
	mov	%r12, %r14
	add	%rax, %r14	
	adc	%rbx, %rdx
	mov	%rdx, %r13	
	imul	%r15, %rdx
	mov	%r9, %rax
	lea	(%rdx, %r12), %rbx
	mul	%r13
	mov	%rsi, %r12
	shl	%cl, %r12
	sub	%r9, %r12
	sbb	%r8, %rbx
	sub	%rax, %r12
	sbb	%rdx, %rbx
	xor	%eax, %eax
	xor	%edx, %edx
	cmp	%r14, %rbx
	cmovnc	%r9, %rax
	cmovnc	%r8, %rdx
	adc	$0, %r13
	nop
	add	%rax, %r12
	adc	%rdx, %rbx
	cmp	%r8, %rbx
	jae	Lfix_qh
Lbck_qh:
	push	%r13	

	jmp	Lnext

	.align	16, 0x90
Lloop:
	
	

	mov	%r10, %rax
	mul	%rbx
	mov	(%r11, %rbp, 8), %rsi
	xor	%r13d, %r13d
	shld	%cl, %rsi, %r13
	or	%r13, %r12
	mov	%r12, %r14
	add	%rax, %r14	
	adc	%rbx, %rdx
	mov	%rdx, %r13	
	imul	%r15, %rdx
	mov	%r9, %rax
	lea	(%rdx, %r12), %rbx
	mul	%r13
	mov	%rsi, %r12
	shl	%cl, %r12
	sub	%r9, %r12
	sbb	%r8, %rbx
	sub	%rax, %r12
	sbb	%rdx, %rbx
	xor	%eax, %eax
	xor	%edx, %edx
	cmp	%r14, %rbx
	cmovnc	%r9, %rax
	cmovnc	%r8, %rdx
	adc	$0, %r13
	nop
	add	%rax, %r12
	adc	%rdx, %rbx
	cmp	%r8, %rbx
	jae	Lfix
Lbck:
	mov	%r13, (%rdi, %rbp, 8)
Lnext:
	sub	$1, %rbp
	jnc	Lloop
Lend:
	
	pop	%rax
	pop	%rsi
	shrd	%cl, %rbx, %r12
	shr	%cl, %rbx
	mov	%rbx, 8(%rsi)
	mov	%r12, (%rsi)

	pop	%rbp
	pop	%rbx
	pop	%r12
	pop	%r13
	pop	%r14
	pop	%r15
	ret

Lfix:	
	seta	%dl
	cmp	%r9, %r12
	setae	%al
	orb	%dl, %al		
	je	Lbck
	inc	%r13
	sub	%r9, %r12
	sbb	%r8, %rbx
	jmp	Lbck


Lfix_qh:	
	seta	%dl
	cmp	%r9, %r12
	setae	%al
	orb	%dl, %al		
	je	Lbck_qh
	inc	%r13
	sub	%r9, %r12
	sbb	%r8, %rbx
	jmp	Lbck_qh
	
