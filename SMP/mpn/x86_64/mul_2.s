


































































   
   
   
   














	.text
	.align	32, 0x90
	.globl	__gmpn_mul_2
	
__gmpn_mul_2:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	push	%rbx
	push	%rbp

	mov	(%rcx), %rbx
	mov	8(%rcx), %rbp

	mov	(%rsi), %rax
	lea	(%rsi,%rdx,8), %rsi
	lea	(%rdi,%rdx,8), %rdi

	test	$1, %dl
	jnz	Lb1

Lb0:	mov	$0, %ecx
	sub	%rdx, %rcx
	xor	%r8, %r8
	mul	%rbx
	mov	%rax, %r10
	mov	%rdx, %r9
	mov	(%rsi,%rcx,8), %rax
	jmp	Llo0

Lb1:	mov	$1, %ecx
	sub	%rdx, %rcx
	xor	%r10, %r10
	mul	%rbx
	mov	%rax, %r8
	mov	%rdx, %r11
	mov	-8(%rsi,%rcx,8), %rax
	mul	%rbp
	jmp	Llo1

	.align	32, 0x90
Ltop:	mul	%rbx
	add	%rax, %r8		
	mov	%rdx, %r11		
	adc	$0, %r11			
	mov	-8(%rsi,%rcx,8), %rax
	mul	%rbp
	add	%r9, %r8			
	adc	$0, %r11			
Llo1:	add	%rax, %r10		
	mov	%r8, -8(%rdi,%rcx,8)		
	mov	%rdx, %r8		
	adc	$0, %r8			
	mov	(%rsi,%rcx,8), %rax
	mul	%rbx
	add	%rax, %r10		
	mov	%rdx, %r9		
	adc	$0, %r9			
	add	%r11, %r10			
	mov	(%rsi,%rcx,8), %rax
	adc	$0, %r9			
Llo0:	mul	%rbp
	mov	%r10, (%rdi,%rcx,8)		
	add	%rax, %r8		
	mov	%rdx, %r10		
	mov	8(%rsi,%rcx,8), %rax
	adc	$0, %r10			
	add	$2, %rcx
	jnc	Ltop

Lend:	mul	%rbx
	add	%rax, %r8
	mov	%rdx, %r11
	adc	$0, %r11
	mov	-8(%rsi), %rax
	mul	%rbp
	add	%r9, %r8
	adc	$0, %r11
	add	%rax, %r10
	mov	%r8, -8(%rdi)
	adc	$0, %rdx
	add	%r11, %r10
	mov	%r10, (%rdi)
	adc	$0, %rdx
	mov	%rdx, %rax

	pop	%rbp
	pop	%rbx
	pop	%rsi
	pop	%rdi
	ret
	
