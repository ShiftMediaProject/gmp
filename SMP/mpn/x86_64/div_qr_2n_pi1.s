






































































	.text
	.align	16, 0x90
	.globl	__gmpn_div_qr_2n_pi1
	
	.def	__gmpn_div_qr_2n_pi1
	.scl	2
	.type	32
	.endef
__gmpn_div_qr_2n_pi1:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	mov	56(%rsp), %r8	
	mov	64(%rsp), %r9	

	mov	72(%rsp), %r10
	mov	%rdx, %r11
	push	%r15
	push	%r14
	push	%r13
	push	%r12
	push	%rbx

	mov	-16(%r11, %rcx, 8), %r12
	mov	-8(%r11, %rcx, 8), %rbx

	mov	%r12, %r14
	mov	%rbx, %r13
	sub	%r9, %r14
	sbb	%r8, %r13
	cmovnc  %r14, %r12
	cmovnc	%r13, %rbx
	
	sbb	%rax, %rax
	inc	%rax
	push	%rax
	lea	-2(%rcx), %rcx
	mov	%r8, %r15
	neg	%r15

	jmp	Lnext

	.align	16, 0x90
Lloop:
	
	

	mov	%r10, %rax
	mul	%rbx
	mov	%r12, %r14
	add	%rax, %r14	
	adc	%rbx, %rdx
	mov	%rdx, %r13	
	imul	%r15, %rdx
	mov	%r9, %rax
	lea	(%rdx, %r12), %rbx
	mul	%r13
	mov	(%r11, %rcx, 8), %r12
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
	mov	%r13, (%rdi, %rcx, 8)
Lnext:
	sub	$1, %rcx
	jnc	Lloop
Lend:
	mov	%rbx, 8(%rsi)
	mov	%r12, (%rsi)

	
	pop	%rax

	pop	%rbx
	pop	%r12
	pop	%r13
	pop	%r14
	pop	%r15
	pop	%rsi
	pop	%rdi
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
	
