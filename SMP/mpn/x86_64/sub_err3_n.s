













































































	
	





	.text
	.align	16, 0x90
	.globl	__gmpn_sub_err3_n
	
__gmpn_sub_err3_n:

	mov	24(%rsp), %rax
	mov	16(%rsp), %r10

	push	%rbx
	push	%rbp
	push	%r12
	push	%r13
	push	%r14
	push	%r15

	push	%rcx
	mov	64(%rsp), %rcx       

	xor	%ebp, %ebp
	xor	%r11d, %r11d
	xor	%r12d, %r12d
	xor	%r13d, %r13d
	xor	%r14d, %r14d
	xor	%r15d, %r15d

	sub	%r8, %r9
	sub	%r8, %rcx

	lea	-8(%r8,%r10,8), %r8
	lea	(%rdi,%r10,8), %rdi
	lea	(%rsi,%r10,8), %rsi
	lea	(%rdx,%r10,8), %rdx
	neg	%r10

	.align	16, 0x90
Ltop:
	shr	$1, %rax		
	mov	(%rsi,%r10,8), %rax
	sbb	(%rdx,%r10,8), %rax
	mov	%rax, (%rdi,%r10,8)
	sbb	%rax, %rax		

	mov	(%r8), %rbx
	and	%rax, %rbx
	add	%rbx, %rbp
	adc	$0, %r11

	mov	(%r8,%r9), %rbx
	and	%rax, %rbx
	add	%rbx, %r12
	adc	$0, %r13

	mov	(%r8,%rcx), %rbx
	and	%rax, %rbx
	add	%rbx, %r14
	adc	$0, %r15

	lea	-8(%r8), %r8
	inc	%r10
	jnz     Ltop

Lend:
	and	$1, %eax
	pop	%rcx

	mov	%rbp, (%rcx)
	mov	%r11, 8(%rcx)
	mov	%r12, 16(%rcx)
	mov	%r13, 24(%rcx)
	mov	%r14, 32(%rcx)
	mov	%r15, 40(%rcx)

	pop	%r15
	pop	%r14
	pop	%r13
	pop	%r12
	pop	%rbp
	pop	%rbx
	ret
	
