



































































   
   
   
   















	.text
	.align	32, 0x90
	.globl	__gmpn_addmul_2
	
__gmpn_addmul_2:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	push	%rbx
	push	%rbp
	push	%r12
	push	%r13

	mov	(%rcx), %rbx
	mov	8(%rcx), %rbp

	mov	(%rsi), %rax

	mov	%rdx, %rcx
	neg	%rcx

	lea	(%rsi,%rdx,8), %rsi
	lea	8(%rdi,%rdx,8), %rdi
	mul	%rbx

	test	$1, %cl
	jnz	Lbx1

Lbx0:	mov	-8(%rdi,%rcx,8), %r12
	mov	%rdx, %r9
	add	%rax, %r12
	adc	$0, %r9
	mov	(%rsi,%rcx,8), %rax
	xor	%r8, %r8
	xor	%r11, %r11
	test	$2, %cl
	jnz	Lb10

Lb00:	nop				
	mul	%rbp
	mov	(%rdi,%rcx,8), %r13
	jmp	Llo0

Lb10:	lea	-2(%rcx), %rcx
	jmp	Llo2

Lbx1:	mov	-8(%rdi,%rcx,8), %r13
	mov	%rdx, %r11
	add	%rax, %r13
	adc	$0, %r11
	mov	(%rsi,%rcx,8), %rax
	xor	%r9, %r9
	xor	%r10, %r10
	test	$2, %cl
	jz	Lb11

Lb01:	mov	(%rdi,%rcx,8), %r12
	inc	%rcx
	jmp	Llo1

Lb11:	dec	%rcx
	jmp	Llo3

	.align	32, 0x90
Ltop:
Llo1:	mul	%rbp
	mov	%rdx, %r8		
	add	%rax, %r12		
	adc	$0, %r8			
	add	%r9, %r13			
	adc	$0, %r11			
	add	%r10, %r12			
	adc	$0, %r8			
	mov	(%rsi,%rcx,8), %rax
	mul	%rbx
	add	%rax, %r12		
	mov	%rdx, %r9		
	adc	$0, %r9			
	mov	(%rsi,%rcx,8), %rax
	mul	%rbp
	mov	%r13, -16(%rdi,%rcx,8)		
	mov	(%rdi,%rcx,8), %r13		
	add	%r11, %r12			
	adc	$0, %r9			
Llo0:	mov	%rdx, %r10		
	mov	%r12, -8(%rdi,%rcx,8)		
	add	%rax, %r13		
	adc	$0, %r10			
	mov	8(%rsi,%rcx,8), %rax
	add	%r8, %r13			
	adc	$0, %r10			
	mul	%rbx
	add	%rax, %r13		
	mov	%rdx, %r11		
	adc	$0, %r11			
	mov	8(%rsi,%rcx,8), %rax
Llo3:	mul	%rbp
	add	%r9, %r13			
	mov	8(%rdi,%rcx,8), %r12		
	adc	$0, %r11			
	mov	%rdx, %r8		
	add	%rax, %r12		
	adc	$0, %r8			
	mov	16(%rsi,%rcx,8), %rax
	mul	%rbx
	add	%r10, %r12			
	mov	%r13, (%rdi,%rcx,8)		
	mov	%rdx, %r9		
	adc	$0, %r8			
	add	%rax, %r12		
	adc	$0, %r9			
	mov	16(%rsi,%rcx,8), %rax
	add	%r11, %r12			
	adc	$0, %r9			
Llo2:	mul	%rbp
	mov	16(%rdi,%rcx,8), %r13		
	add	%rax, %r13		
	mov	%rdx, %r10		
	adc	$0, %r10			
	mov	24(%rsi,%rcx,8), %rax
	mov	%r12, 8(%rdi,%rcx,8)		
	mul	%rbx
	add	%r8, %r13			
	mov	%rdx, %r11		
	adc	$0, %r10			
	add	%rax, %r13		
	mov	24(%rsi,%rcx,8), %rax
	mov	24(%rdi,%rcx,8), %r12		
	adc	$0, %r11			
	add	$4, %rcx
	jnc	Ltop

Lend:	mul	%rbp
	add	%r9, %r13
	adc	$0, %r11
	add	%r10, %rax
	adc	$0, %rdx
	mov	%r13, -16(%rdi)
	add	%r11, %rax
	adc	$0, %rdx
	mov	%rax, -8(%rdi)
	mov	%rdx, %rax

	pop	%r13
	pop	%r12
	pop	%rbp
	pop	%rbx
	pop	%rsi
	pop	%rdi
	ret
	
