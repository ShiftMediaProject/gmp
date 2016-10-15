







































































	
	






	.text
	.align	16, 0x90
	.globl	__gmpn_add_err1_n
	
__gmpn_add_err1_n:

	mov	8(%rsp), %rax

	push	%rbx
	push	%rbp
	push	%r12
	push	%r13
	push	%r14
	push	%r15

	lea	(%rsi,%r9,8), %rsi
	lea	(%rdx,%r9,8), %rdx
	lea	(%rdi,%r9,8), %rdi

	mov	%r9d, %r10d
	and	$3, %r10d
	jz	L0mod4
	cmp	$2, %r10d
	jc	L1mod4
	jz	L2mod4
L3mod4:
	xor	%ebx, %ebx
	xor	%ebp, %ebp
	xor	%r10d, %r10d
	xor	%r11d, %r11d
	lea	-24(%r8,%r9,8), %r8
	neg	%r9

	shr	$1, %al		   
	mov	(%rsi,%r9,8), %r14
	mov	8(%rsi,%r9,8), %r15
	adc	(%rdx,%r9,8), %r14
	mov	%r14, (%rdi,%r9,8)
	cmovc	16(%r8), %rbx
	adc	8(%rdx,%r9,8), %r15
	mov	%r15, 8(%rdi,%r9,8)
	cmovc	8(%r8), %r10
	mov	16(%rsi,%r9,8), %r14
	adc	16(%rdx,%r9,8), %r14
	mov	%r14, 16(%rdi,%r9,8)
	cmovc	(%r8), %r11
	setc	%al		   
	add	%r10, %rbx
	adc	$0, %rbp
	add	%r11, %rbx
	adc	$0, %rbp

	add	$3, %r9
	jnz	Lloop
	jmp	Lend

	.align	16, 0x90
L0mod4:
	xor	%ebx, %ebx
	xor	%ebp, %ebp
	lea	(%r8,%r9,8), %r8
	neg	%r9
	jmp	Lloop

	.align	16, 0x90
L1mod4:
	xor	%ebx, %ebx
	xor	%ebp, %ebp
	lea	-8(%r8,%r9,8), %r8
	neg	%r9

	shr	$1, %al		   
	mov	(%rsi,%r9,8), %r14
	adc	(%rdx,%r9,8), %r14
	mov	%r14, (%rdi,%r9,8)
	cmovc	(%r8), %rbx
	setc	%al		   

	add	$1, %r9
	jnz	Lloop
	jmp	Lend

	.align	16, 0x90
L2mod4:
	xor	%ebx, %ebx
	xor	%ebp, %ebp
	xor	%r10d, %r10d
	lea	-16(%r8,%r9,8), %r8
	neg	%r9

	shr	$1, %al		   
	mov	(%rsi,%r9,8), %r14
	mov	8(%rsi,%r9,8), %r15
	adc	(%rdx,%r9,8), %r14
	mov	%r14, (%rdi,%r9,8)
	cmovc	8(%r8), %rbx
	adc	8(%rdx,%r9,8), %r15
	mov	%r15, 8(%rdi,%r9,8)
	cmovc	(%r8), %r10
	setc	%al		   
	add	%r10, %rbx
	adc	$0, %rbp

	add	$2, %r9
	jnz	Lloop
	jmp	Lend

	.align	32, 0x90
Lloop:
	mov	(%rsi,%r9,8), %r14
	shr	$1, %al		   
	mov	-8(%r8), %r10
	mov	$0, %r13d
	adc	(%rdx,%r9,8), %r14
	cmovnc	%r13, %r10
	mov	%r14, (%rdi,%r9,8)
	mov	8(%rsi,%r9,8), %r15
	mov	16(%rsi,%r9,8), %r14
	adc	8(%rdx,%r9,8), %r15
	mov	-16(%r8), %r11
	cmovnc	%r13, %r11
	mov	-24(%r8), %r12
	mov	%r15, 8(%rdi,%r9,8)
	adc	16(%rdx,%r9,8), %r14
	cmovnc	%r13, %r12
	mov	24(%rsi,%r9,8), %r15
	adc	24(%rdx,%r9,8), %r15
	cmovc	-32(%r8), %r13
	setc	%al		   
	add	%r10, %rbx
	adc	$0, %rbp
	add	%r11, %rbx
	adc	$0, %rbp
	add	%r12, %rbx
	adc	$0, %rbp
	lea	-32(%r8), %r8
	mov	%r14, 16(%rdi,%r9,8)
	add	%r13, %rbx
	adc	$0, %rbp
	add	$4, %r9
	mov	%r15, -8(%rdi,%r9,8)
	jnz	Lloop

Lend:
	mov	%rbx, (%rcx)
	mov	%rbp, 8(%rcx)

	pop	%r15
	pop	%r14
	pop	%r13
	pop	%r12
	pop	%rbp
	pop	%rbx
	ret
	
