

























































	.text
	.align	16, 0x90
	.globl	__gmpn_mod_1s_2p
	
	.def	__gmpn_mod_1s_2p
	.scl	2
	.type	32
	.endef
__gmpn_mod_1s_2p:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	push	%r14
	test	$1, %sil
	mov	%rdx, %r14
	push	%r13
	mov	%rcx, %r13
	push	%r12
	push	%rbp
	push	%rbx
	mov	16(%rcx), %r10
	mov	24(%rcx), %rbx
	mov	32(%rcx), %rbp
	je	Lb0
	dec	%rsi
	je	Lone
	mov	-8(%rdi,%rsi,8), %rax
	mul	%r10
	mov	%rax, %r9
	mov	%rdx, %r8
	mov	(%rdi,%rsi,8), %rax
	add	-16(%rdi,%rsi,8), %r9
	adc	$0, %r8
	mul	%rbx
	add	%rax, %r9
	adc	%rdx, %r8
	jmp	L11

Lb0:	mov	-8(%rdi,%rsi,8), %r8
	mov	-16(%rdi,%rsi,8), %r9

L11:	sub	$4, %rsi
	jb	Led2
	lea	40(%rdi,%rsi,8), %rdi
	mov	-40(%rdi), %r11
	mov	-32(%rdi), %rax
	jmp	Lm0

	.align	16, 0x90
Ltop:	mov	-24(%rdi), %r9
	add	%rax, %r11
	mov	-16(%rdi), %rax
	adc	%rdx, %r12
	mul	%r10
	add	%rax, %r9
	mov	%r11, %rax
	mov	%rdx, %r8
	adc	$0, %r8
	mul	%rbx
	add	%rax, %r9
	mov	%r12, %rax
	adc	%rdx, %r8
	mul	%rbp
	sub	$2, %rsi
	jb	Led1
	mov	-40(%rdi), %r11
	add	%rax, %r9
	mov	-32(%rdi), %rax
	adc	%rdx, %r8
Lm0:	mul	%r10
	add	%rax, %r11
	mov	%r9, %rax
	mov	%rdx, %r12
	adc	$0, %r12
	mul	%rbx
	add	%rax, %r11
	lea	-32(%rdi), %rdi		
	mov	%r8, %rax
	adc	%rdx, %r12
	mul	%rbp
	sub	$2, %rsi
	jae	Ltop

Led0:	mov	%r11, %r9
	mov	%r12, %r8
Led1:	add	%rax, %r9
	adc	%rdx, %r8
Led2:	mov	8(%r13), %edi		
	mov	%r8, %rax
	mov	%r9, %r8
	mul	%r10
	add	%rax, %r8
	adc	$0, %rdx
L1:	xor	%ecx, %ecx
	mov	%r8, %r9
	sub	%edi, %ecx
	shr	%cl, %r9
	mov	%edi, %ecx
	sal	%cl, %rdx
	or	%rdx, %r9
	sal	%cl, %r8
	mov	%r9, %rax
	mulq	(%r13)
	mov	%rax, %rsi
	inc	%r9
	add	%r8, %rsi
	adc	%r9, %rdx
	imul	%r14, %rdx
	sub	%rdx, %r8
	lea	(%r8,%r14), %rax
	cmp	%r8, %rsi
	cmovc	%rax, %r8
	mov	%r8, %rax
	sub	%r14, %rax
	cmovc	%r8, %rax
	mov	%edi, %ecx
	shr	%cl, %rax
	pop	%rbx
	pop	%rbp
	pop	%r12
	pop	%r13
	pop	%r14
	pop	%rsi
	pop	%rdi
	ret
Lone:
	mov	(%rdi), %r8
	mov	8(%rcx), %edi
	xor	%rdx, %rdx
	jmp	L1
	

	.align	16, 0x90
	.globl	__gmpn_mod_1s_2p_cps
	
	.def	__gmpn_mod_1s_2p_cps
	.scl	2
	.type	32
	.endef
__gmpn_mod_1s_2p_cps:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi

	push	%rbp
	bsr	%rsi, %rcx
	push	%rbx
	mov	%rdi, %rbx
	push	%r12
	xor	$63, %ecx
	mov	%rsi, %r12
	mov	%ecx, %ebp	
	sal	%cl, %r12		
	
	mov	%r12, %rcx		
	sub	$32, %rsp	
	
	call	__gmpn_invert_limb
	add	$32, %rsp	
	mov	%r12, %r8
	mov	%rax, %r11
	mov	%rax, (%rbx)		
	mov	%rbp, 8(%rbx)		
	neg	%r8
	mov	%ebp, %ecx
	mov	$1, %esi

	shld	%cl, %rax, %rsi	

	imul	%r8, %rsi
	mul	%rsi

	add	%rsi, %rdx
	shr	%cl, %rsi
	mov	%rsi, 16(%rbx)		

	not	%rdx
	imul	%r12, %rdx
	lea	(%rdx,%r12), %rsi
	cmp	%rdx, %rax
	cmovnc	%rdx, %rsi
	mov	%r11, %rax
	mul	%rsi

	add	%rsi, %rdx
	shr	%cl, %rsi
	mov	%rsi, 24(%rbx)		

	not	%rdx
	imul	%r12, %rdx
	add	%rdx, %r12
	cmp	%rdx, %rax
	cmovnc	%rdx, %r12

	shr	%cl, %r12
	mov	%r12, 32(%rbx)		

	pop	%r12
	pop	%rbx
	pop	%rbp
	pop	%rsi
	pop	%rdi
	ret
	
