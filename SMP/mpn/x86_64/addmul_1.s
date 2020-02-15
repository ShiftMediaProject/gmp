





































































      
      
      








	
	

   

	.text
	.align	16, 0x90
	.globl	__gmpn_addmul_1c
	
__gmpn_addmul_1c:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	push	%rbx
	push	%rbp
	lea	(%rdx), %rbx
	neg	%rbx

	mov	(%rsi), %rax
	mov	(%rdi), %r10

	lea	-16(%rdi,%rdx,8), %rdi
	lea	(%rsi,%rdx,8), %rsi
	mul	%rcx
	add	72(%rsp), %rax
	adc	$0, %rdx
	jmp	Lstart_nc
	

	.align	16, 0x90
	.globl	__gmpn_addmul_1
	
__gmpn_addmul_1:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	push	%rbx
	push	%rbp
	lea	(%rdx), %rbx
	neg	%rbx

	mov	(%rsi), %rax
	mov	(%rdi), %r10

	lea	-16(%rdi,%rdx,8), %rdi
	lea	(%rsi,%rdx,8), %rsi
	mul	%rcx

Lstart_nc:
	test	$1, %bl
	jnz	Lodd

	lea	(%rax), %r11
	mov	8(%rsi,%rbx,8), %rax
	lea	(%rdx), %rbp
	mul	%rcx
	add	$2, %rbx
	jz	Ln2

	lea	(%rax), %r8
	mov	(%rsi,%rbx,8), %rax
	lea	(%rdx), %r9
	jmp	Lmid

	.align	8, 0x90
Lodd:	inc	%rbx
	jz	Ln1

	lea	(%rax), %r8
	mov	(%rsi,%rbx,8), %rax
	lea	(%rdx), %r9
	mul	%rcx
	lea	(%rax), %r11
	mov	8(%rsi,%rbx,8), %rax
	lea	(%rdx), %rbp
	jmp	Le

	.align	16, 0x90
Ltop:	mul	%rcx
	add	%r8, %r10
	lea	(%rax), %r8
	mov	(%rsi,%rbx,8), %rax
	adc	%r9, %r11
	mov	%r10, -8(%rdi,%rbx,8)
	mov	(%rdi,%rbx,8), %r10
	lea	(%rdx), %r9
	adc	$0, %rbp
Lmid:	mul	%rcx
	add	%r11, %r10
	lea	(%rax), %r11
	mov	8(%rsi,%rbx,8), %rax
	adc	%rbp, %r8
	mov	%r10, (%rdi,%rbx,8)
	mov	8(%rdi,%rbx,8), %r10
	lea	(%rdx), %rbp
	adc	$0, %r9
Le:	add	$2, %rbx
	js	Ltop

	mul	%rcx
	add	%r8, %r10
	adc	%r9, %r11
	mov	%r10, -8(%rdi)
	adc	%rbx, %rbp		
Ln2:	mov	(%rdi), %r10
	add	%r11, %r10
	adc	%rbp, %rax
	mov	%r10, (%rdi)
	adc	%rbx, %rdx		
Ln1:	mov	8(%rdi), %r10
	add	%rax, %r10
	mov	%r10, 8(%rdi)
	mov	%ebx, %eax	
	adc	%rdx, %rax
	pop	%rbp
	pop	%rbx
	pop	%rsi
	pop	%rdi
	ret
	
