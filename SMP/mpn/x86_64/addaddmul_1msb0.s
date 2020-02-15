

































































	.text
	.align	16, 0x90
	.globl	__gmpn_addaddmul_1msb0
	
__gmpn_addaddmul_1msb0:

	push	%r12
	push	%rbp

	lea	(%rsi,%rcx,8), %rsi
	lea	(%rdx,%rcx,8), %rbp
	lea	(%rdi,%rcx,8), %rdi
	neg	%rcx

	mov	(%rsi,%rcx,8), %rax
	mul	%r8
	mov	%rax, %r12
	mov	(%rbp,%rcx,8), %rax
	mov	%rdx, %r10
	add	$3, %rcx
	jns	Lend

	.align	16, 0x90
Ltop:	mul	%r9
	add	%rax, %r12
	mov	-16(%rsi,%rcx,8), %rax
	adc	%rdx, %r10
	mov	%r12, -24(%rdi,%rcx,8)
	mul	%r8
	add	%rax, %r10
	mov	-16(%rbp,%rcx,8), %rax
	mov	$0, %r11d
	adc	%rdx, %r11
	mul	%r9
	add	%rax, %r10
	mov	-8(%rsi,%rcx,8), %rax
	adc	%rdx, %r11
	mov	%r10, -16(%rdi,%rcx,8)
	mul	%r8
	add	%rax, %r11
	mov	-8(%rbp,%rcx,8), %rax
	mov	$0, %r12d
	adc	%rdx, %r12
	mul	%r9
	add	%rax, %r11
	adc	%rdx, %r12
	mov	(%rsi,%rcx,8), %rax
	mul	%r8
	add	%rax, %r12
	mov	%r11, -8(%rdi,%rcx,8)
	mov	(%rbp,%rcx,8), %rax
	mov	$0, %r10d
	adc	%rdx, %r10
	add	$3, %rcx
	js	Ltop

Lend:	cmp	$1, %ecx
	ja	2f
	jz	1f

	mul	%r9
	add	%rax, %r12
	mov	-16(%rsi), %rax
	adc	%rdx, %r10
	mov	%r12, -24(%rdi)
	mul	%r8
	add	%rax, %r10
	mov	-16(%rbp), %rax
	mov	$0, %r11d
	adc	%rdx, %r11
	mul	%r9
	add	%rax, %r10
	mov	-8(%rsi), %rax
	adc	%rdx, %r11
	mov	%r10, -16(%rdi)
	mul	%r8
	add	%rax, %r11
	mov	-8(%rbp), %rax
	mov	$0, %r12d
	adc	%rdx, %r12
	mul	%r9
	add	%rax, %r11
	adc	%rdx, %r12
	mov	%r11, -8(%rdi)
	mov	%r12, %rax
	pop	%rbp
	pop	%r12
	ret

1:	mul	%r9
	add	%rax, %r12
	mov	-8(%rsi), %rax
	adc	%rdx, %r10
	mov	%r12, -16(%rdi)
	mul	%r8
	add	%rax, %r10
	mov	-8(%rbp), %rax
	mov	$0, %r11d
	adc	%rdx, %r11
	mul	%r9
	add	%rax, %r10
	adc	%rdx, %r11
	mov	%r10, -8(%rdi)
	mov	%r11, %rax
	pop	%rbp
	pop	%r12
	ret

2:	mul	%r9
	add	%rax, %r12
	mov	%r12, -8(%rdi)
	adc	%rdx, %r10
	mov	%r10, %rax
	pop	%rbp
	pop	%r12
	ret
	
