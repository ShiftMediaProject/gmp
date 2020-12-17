








































































	.text
	.align	16, 0x90
	.globl	__gmpn_divrem_2
	
	.def	__gmpn_divrem_2
	.scl	2
	.type	32
	.endef
__gmpn_divrem_2:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	mov	56(%rsp), %r8	
	push	%r15
	push	%r14
	push	%r13
	push	%r12
	lea	-24(%rdx,%rcx,8), %r12	
	mov	%rsi, %r13
	push	%rbp
	mov	%rdi, %rbp
	push	%rbx
	mov	8(%r8), %r11		
	mov	16(%r12), %rbx
	mov	(%r8), %r8		
	mov	8(%r12), %r10

	xor	%r15d, %r15d
	cmp	%rbx, %r11
	ja	L2
	setb	%dl
	cmp	%r10, %r8
	setbe	%al
	orb	%al, %dl		
	je	L2
	inc	%r15d
	sub	%r8, %r10
	sbb	%r11, %rbx
L2:
	lea	-3(%rcx,%r13), %r14	
	test	%r14, %r14
	js	Lend

	push	%r8
	push	%r10
	push	%r11

	mov	%r11, %rcx	
	sub	$32, %rsp	
	
	call	__gmpn_invert_limb
	add	$32, %rsp	
	pop	%r11
	pop	%r10
	pop	%r8

	mov	%r11, %rdx
	mov	%rax, %rdi
	imul	%rax, %rdx
	mov	%rdx, %r9
	mul	%r8
	xor	%ecx, %ecx
	add	%r8, %r9
	adc	$-1, %rcx
	add	%rdx, %r9
	adc	$0, %rcx
	js	2f
1:	dec	%rdi
	sub	%r11, %r9
	sbb	$0, %rcx
	jns	1b
2:

	lea	(%rbp,%r14,8), %rbp
	mov	%r11, %rsi
	neg	%rsi			




	.align	16, 0x90
Ltop:	mov	%rdi, %rax		
	mul	%rbx			
	mov	%r10, %rcx		
	add	%rax, %rcx		
	adc	%rbx, %rdx		
	mov	%rdx, %r9		
	imul	%rsi, %rdx		
	mov	%r8, %rax		
	lea	(%rdx, %r10), %rbx	
	xor	%r10d, %r10d	
	mul	%r9			
	cmp	%r14, %r13		
	jg	L19			
	mov	(%r12), %r10		
	sub	$8, %r12		
L19:	sub	%r8, %r10		
	sbb	%r11, %rbx		
	sub	%rax, %r10		
	sbb	%rdx, %rbx		
	xor	%eax, %eax	
	xor	%edx, %edx	
	cmp	%rcx, %rbx		
	cmovnc	%r8, %rax		
	cmovnc	%r11, %rdx		
	adc	$0, %r9			
	nop
	add	%rax, %r10		
	adc	%rdx, %rbx		
	cmp	%r11, %rbx		
	jae	Lfix			
Lbck:	mov	%r9, (%rbp)		
	sub	$8, %rbp		
	dec	%r14
	jns	Ltop

Lend:	mov	%r10, 8(%r12)
	mov	%rbx, 16(%r12)
	pop	%rbx
	pop	%rbp
	pop	%r12
	pop	%r13
	pop	%r14
	mov	%r15, %rax
	pop	%r15
	pop	%rsi
	pop	%rdi
	ret

Lfix:	seta	%dl
	cmp	%r8, %r10
	setae	%al
	orb	%dl, %al		
	je	Lbck
	inc	%r9
	sub	%r8, %r10
	sbb	%r11, %rbx
	jmp	Lbck
	
