



























































































	.text
	.align	16, 0x90
	.globl	__gmpn_mul_2
	
	.def	__gmpn_mul_2
	.scl	2
	.type	32
	.endef
__gmpn_mul_2:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	push	%rbx
	push	%rbp

	mov	(%rcx), %r8
	mov	8(%rcx), %r9

	mov	(%rsi), %rax

	mov	%rdx, %r11
	neg	%r11
	lea	-8(%rsi,%rdx,8), %rsi
	lea	-8(%rdi,%rdx,8), %rdi

	and	$3, %edx
	jz	Lm2p0
	cmp	$2, %edx
	jc	Lm2p1
	jz	Lm2p2
Lm2p3:
	mul	%r8
	xor	%r10d, %r10d
	mov	%rax, %rcx
	mov	%rdx, %rbp
	mov	8(%rsi,%r11,8), %rax
	add	$-1, %r11
	mul	%r9
	add	%rax, %rbp
	jmp	Lm23
Lm2p0:
	mul	%r8
	xor	%ebp, %ebp
	mov	%rax, %rbx
	mov	%rdx, %rcx
	jmp	Lm20
Lm2p1:
	mul	%r8
	xor	%r10d, %r10d
	xor	%ebx, %ebx
	xor	%ecx, %ecx
	add	$1, %r11
	jmp	Lm2top
Lm2p2:
	mul	%r8
	xor	%ebx, %ebx
	xor	%ecx, %ecx
	mov	%rax, %rbp
	mov	%rdx, %r10
	mov	8(%rsi,%r11,8), %rax
	add	$-2, %r11
	jmp	Lm22


	.align	32, 0x90
Lm2top:
	add	%rax, %r10
	adc	%rdx, %rbx
	mov	0(%rsi,%r11,8), %rax
	adc	$0, %ecx
	mov	$0, %ebp
	mul	%r9
	add	%rax, %rbx
	mov	%r10, 0(%rdi,%r11,8)
	adc	%rdx, %rcx
	mov	8(%rsi,%r11,8), %rax
	mul	%r8
	add	%rax, %rbx
	adc	%rdx, %rcx
	adc	$0, %ebp
Lm20:	mov	8(%rsi,%r11,8), %rax
	mul	%r9
	add	%rax, %rcx
	adc	%rdx, %rbp
	mov	16(%rsi,%r11,8), %rax
	mov	$0, %r10d
	mul	%r8
	add	%rax, %rcx
	mov	16(%rsi,%r11,8), %rax
	adc	%rdx, %rbp
	adc	$0, %r10d
	mul	%r9
	add	%rax, %rbp
	mov	%rbx, 8(%rdi,%r11,8)
Lm23:	adc	%rdx, %r10
	mov	24(%rsi,%r11,8), %rax
	mul	%r8
	mov	$0, %ebx
	add	%rax, %rbp
	adc	%rdx, %r10
	mov	%rcx, 16(%rdi,%r11,8)
	mov	24(%rsi,%r11,8), %rax
	mov	$0, %ecx
	adc	$0, %ebx
Lm22:	mul	%r9
	add	%rax, %r10
	mov	%rbp, 24(%rdi,%r11,8)
	adc	%rdx, %rbx
	mov	32(%rsi,%r11,8), %rax
	mul	%r8
	add	$4, %r11
	js	Lm2top


	add	%rax, %r10
	adc	%rdx, %rbx
	adc	$0, %ecx
	mov	(%rsi), %rax
	mul	%r9
	mov	%r10, (%rdi)
	add	%rax, %rbx
	adc	%rdx, %rcx
	mov	%rbx, 8(%rdi)
	mov	%rcx, %rax

	pop	%rbp
	pop	%rbx
	pop	%rsi
	pop	%rdi
	ret
	
