











































































	
	






	.text
	.align	16, 0x90
	.globl	__gmpn_add_err2_n
	
__gmpn_add_err2_n:

	mov	16(%rsp), %rax
	mov	8(%rsp), %r10

	push	%rbx
	push	%rbp
	push	%r12
	push	%r13
	push	%r14

	xor	%ebp, %ebp
	xor	%r11d, %r11d
	xor	%r12d, %r12d
	xor	%r13d, %r13d

	sub	%r8, %r9

	lea	(%rdi,%r10,8), %rdi
	lea	(%rsi,%r10,8), %rsi
	lea	(%rdx,%r10,8), %rdx

	test	$1, %r10
	jnz	Lodd

	lea	-8(%r8,%r10,8), %r8
	neg	%r10
	jmp	Ltop

	.align	16, 0x90
Lodd:
	lea	-16(%r8,%r10,8), %r8
	neg	%r10
	shr	$1, %rax
	mov	(%rsi,%r10,8), %rbx
	adc	(%rdx,%r10,8), %rbx
	cmovc	8(%r8), %rbp
	cmovc	8(%r8,%r9), %r12
	mov	%rbx, (%rdi,%r10,8)
	sbb	%rax, %rax
	inc	%r10
	jz	Lend

	.align	16, 0x90
Ltop:
	mov	(%rsi,%r10,8), %rbx
	shr	$1, %rax		
	adc	(%rdx,%r10,8), %rbx
	mov	%rbx, (%rdi,%r10,8)
	sbb	%r14, %r14	

	mov	8(%rsi,%r10,8), %rbx
	adc	8(%rdx,%r10,8), %rbx
	mov	%rbx, 8(%rdi,%r10,8)
	sbb	%rax, %rax	

	mov	(%r8), %rbx	
	and	%r14, %rbx
	add	%rbx, %rbp
	adc	$0, %r11

	and	(%r8,%r9), %r14	
	add	%r14, %r12
	adc	$0, %r13

	mov	-8(%r8), %rbx	
	and	%rax, %rbx
	add	%rbx, %rbp
	adc	$0, %r11

	mov	-8(%r8,%r9), %rbx	
	and	%rax, %rbx
	add	%rbx, %r12
	adc	$0, %r13

	add	$2, %r10
	lea	-16(%r8), %r8
	jnz	Ltop
Lend:

	mov	%rbp, (%rcx)
	mov	%r11, 8(%rcx)
	mov	%r12, 16(%rcx)
	mov	%r13, 24(%rcx)

	and	$1, %eax	

	pop	%r14
	pop	%r13
	pop	%r12
	pop	%rbp
	pop	%rbx
	ret
	
