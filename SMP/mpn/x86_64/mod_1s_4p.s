
























































	.text
	.align	16, 0x90
	.globl	__gmpn_mod_1s_4p
	
	.def	__gmpn_mod_1s_4p
	.scl	2
	.type	32
	.endef
__gmpn_mod_1s_4p:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	push	%r15
	push	%r14
	push	%r13
	push	%r12
	push	%rbp
	push	%rbx

	mov	%rdx, %r15
	mov	%rcx, %r14
	mov	16(%rcx), %r11		
	mov	24(%rcx), %rbx		
	mov	32(%rcx), %rbp		
	mov	40(%rcx), %r13		
	mov	48(%rcx), %r12		
	xor	%r8d, %r8d
	mov	%esi, %edx
	and	$3, %edx
	je	Lb0
	cmp	$2, %edx
	jc	Lb1
	je	Lb2

Lb3:	lea	-24(%rdi,%rsi,8), %rdi
	mov	8(%rdi), %rax
	mul	%r11
	mov	(%rdi), %r9
	add	%rax, %r9
	adc	%rdx, %r8
	mov	16(%rdi), %rax
	mul	%rbx
	jmp	Lm0

	.align	8, 0x90
Lb0:	lea	-32(%rdi,%rsi,8), %rdi
	mov	8(%rdi), %rax
	mul	%r11
	mov	(%rdi), %r9
	add	%rax, %r9
	adc	%rdx, %r8
	mov	16(%rdi), %rax
	mul	%rbx
	add	%rax, %r9
	adc	%rdx, %r8
	mov	24(%rdi), %rax
	mul	%rbp
	jmp	Lm0

	.align	8, 0x90
Lb1:	lea	-8(%rdi,%rsi,8), %rdi
	mov	(%rdi), %r9
	jmp	Lm1

	.align	8, 0x90
Lb2:	lea	-16(%rdi,%rsi,8), %rdi
	mov	8(%rdi), %r8
	mov	(%rdi), %r9
	jmp	Lm1

	.align	16, 0x90
Ltop:	mov	-24(%rdi), %rax
	mov	-32(%rdi), %r10
	mul	%r11			
	add	%rax, %r10
	mov	-16(%rdi), %rax
	mov	$0, %ecx
	adc	%rdx, %rcx
	mul	%rbx			
	add	%rax, %r10
	mov	-8(%rdi), %rax
	adc	%rdx, %rcx
	sub	$32, %rdi
	mul	%rbp			
	add	%rax, %r10
	mov	%r13, %rax
	adc	%rdx, %rcx
	mul	%r9			
	add	%rax, %r10
	mov	%r12, %rax
	adc	%rdx, %rcx
	mul	%r8			
	mov	%r10, %r9
	mov	%rcx, %r8
Lm0:	add	%rax, %r9
	adc	%rdx, %r8
Lm1:	sub	$4, %rsi
	ja	Ltop

Lend:	mov	8(%r14), %esi
	mov	%r8, %rax
	mul	%r11
	mov	%rax, %r8
	add	%r9, %r8
	adc	$0, %rdx
	xor	%ecx, %ecx
	sub	%esi, %ecx
	mov	%r8, %rdi
	shr	%cl, %rdi
	mov	%esi, %ecx
	sal	%cl, %rdx
	or	%rdx, %rdi
	mov	%rdi, %rax
	mulq	(%r14)
	mov	%r15, %rbx
	mov	%rax, %r9
	sal	%cl, %r8
	inc	%rdi
	add	%r8, %r9
	adc	%rdi, %rdx
	imul	%rbx, %rdx
	sub	%rdx, %r8
	lea	(%r8,%rbx), %rax
	cmp	%r8, %r9
	cmovc	%rax, %r8
	mov	%r8, %rax
	sub	%rbx, %rax
	cmovc	%r8, %rax
	shr	%cl, %rax
	pop	%rbx
	pop	%rbp
	pop	%r12
	pop	%r13
	pop	%r14
	pop	%r15
	pop	%rsi
	pop	%rdi
	ret
	

	.align	16, 0x90
	.globl	__gmpn_mod_1s_4p_cps
	
	.def	__gmpn_mod_1s_4p_cps
	.scl	2
	.type	32
	.endef
__gmpn_mod_1s_4p_cps:

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
	lea	(%rdx,%r12), %rsi
	cmp	%rdx, %rax
	cmovnc	%rdx, %rsi
	mov	%r11, %rax
	mul	%rsi

	add	%rsi, %rdx
	shr	%cl, %rsi
	mov	%rsi, 32(%rbx)		

	not	%rdx
	imul	%r12, %rdx
	lea	(%rdx,%r12), %rsi
	cmp	%rdx, %rax
	cmovnc	%rdx, %rsi
	mov	%r11, %rax
	mul	%rsi

	add	%rsi, %rdx
	shr	%cl, %rsi
	mov	%rsi, 40(%rbx)		

	not	%rdx
	imul	%r12, %rdx
	add	%rdx, %r12
	cmp	%rdx, %rax
	cmovnc	%rdx, %r12

	shr	%cl, %r12
	mov	%r12, 48(%rbx)		

	pop	%r12
	pop	%rbx
	pop	%rbp
	pop	%rsi
	pop	%rdi
	ret
	
