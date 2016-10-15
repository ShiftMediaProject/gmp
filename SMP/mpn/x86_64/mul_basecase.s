
































































































	.text
	.align	16, 0x90
	.globl	__gmpn_mul_basecase
	
__gmpn_mul_basecase:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	mov	56(%rsp), %r8d	
	push	%rbx
	push	%rbp
	mov	%rdx, %rbx		
	neg	%rbx

	mov	(%rsi), %rax		
	lea	(%rsi,%rdx,8), %rsi	
	lea	(%rdi,%rdx,8), %rdi	

	mov	(%rcx), %r9		
	mul	%r9			

	test	$1, %r8b
	jz	Ldo_mul_2

Ldo_mul_1:
	test	$1, %bl
	jnz	Lm1x1

Lm1x0:mov	%rax, %r10		
	mov	%rdx, %r11
	mov	8(%rsi,%rbx,8), %rax
	test	$2, %bl
	jnz	Lm110

Lm100:lea	2(%rbx), %rbp		
	jmp	Lm1l0

Lm110:lea	(%rbx), %rbp			
	jmp	Lm1l2

Lm1x1:mov	%rax, %r11		
	mov	%rdx, %r10
	test	$2, %bl
	jz	Lm111

Lm101:lea	3(%rbx), %rbp		
	test	%rbp, %rbp
	js	Lm1l1
	mov	%rax, -8(%rdi)
	mov	%rdx, (%rdi)
	pop	%rbp
	pop	%rbx
	pop	%rsi
	pop	%rdi
	ret

Lm111:lea	1(%rbx), %rbp		
	mov	8(%rsi,%rbx,8), %rax
	jmp	Lm1l3

	.align	16, 0x90		
Lm1tp:mov	%rdx, %r10
	add	%rax, %r11
Lm1l1:mov	-16(%rsi,%rbp,8), %rax
	adc	$0, %r10
	mul	%r9
	add	%rax, %r10
	mov	%r11, -24(%rdi,%rbp,8)
	mov	-8(%rsi,%rbp,8), %rax
	mov	%rdx, %r11
	adc	$0, %r11
Lm1l0:mul	%r9
	mov	%r10, -16(%rdi,%rbp,8)
	add	%rax, %r11
	mov	%rdx, %r10
	mov	(%rsi,%rbp,8), %rax
	adc	$0, %r10
Lm1l3:mul	%r9
	mov	%r11, -8(%rdi,%rbp,8)
	mov	%rdx, %r11
	add	%rax, %r10
	mov	8(%rsi,%rbp,8), %rax
	adc	$0, %r11
Lm1l2:mul	%r9
	mov	%r10, (%rdi,%rbp,8)
	add	$4, %rbp
	jnc	Lm1tp

Lm1ed:add	%rax, %r11
	adc	$0, %rdx
	mov	%r11, -8(%rdi)
	mov	%rdx, (%rdi)

	dec	%r8d
	jz	Lret2

	lea	8(%rcx), %rcx
	lea	8(%rdi), %rdi
	push	%r12
	push	%r13
	push	%r14
	jmp	Ldo_addmul

Ldo_mul_2:

	push	%r12
	push	%r13
	push	%r14

	mov	8(%rcx), %r14

	test	$1, %bl
	jnz	Lm2b1

Lm2b0:lea	(%rbx), %rbp
	xor	%r10, %r10
	mov	%rax, %r12
	mov	%rdx, %r11
	jmp	Lm2l0

Lm2b1:lea	1(%rbx), %rbp
	xor	%r11, %r11
	xor	%r12, %r12
	mov	%rax, %r10
	mov	%rdx, %r13
	jmp	Lm2l1

	.align	32, 0x90
Lm2tp:mul	%r9
	add	%rax, %r10
	mov	%rdx, %r13
	adc	$0, %r13
Lm2l1:mov	-8(%rsi,%rbp,8), %rax
	mul	%r14
	add	%r11, %r10
	adc	$0, %r13
	add	%rax, %r12
	mov	%r10, -8(%rdi,%rbp,8)
	mov	%rdx, %r10
	adc	$0, %r10
	mov	(%rsi,%rbp,8), %rax
	mul	%r9
	add	%rax, %r12
	mov	%rdx, %r11
	adc	$0, %r11
	add	%r13, %r12
Lm2l0:mov	(%rsi,%rbp,8), %rax
	adc	$0, %r11
	mul	%r14
	mov	%r12, (%rdi,%rbp,8)
	add	%rax, %r10
	mov	%rdx, %r12
	mov	8(%rsi,%rbp,8), %rax
	adc	$0, %r12
	add	$2, %rbp
	jnc	Lm2tp

Lm2ed:mul	%r9
	add	%rax, %r10
	mov	%rdx, %r13
	adc	$0, %r13
	mov	-8(%rsi), %rax
	mul	%r14
	add	%r11, %r10
	adc	$0, %r13
	add	%rax, %r12
	mov	%r10, -8(%rdi)
	adc	$0, %rdx
	add	%r13, %r12
	mov	%r12, (%rdi)
	adc	$0, %rdx
	mov	%rdx, 8(%rdi)

	add	$-2, %r8d
	jz	Lret5
	lea	16(%rcx), %rcx
	lea	16(%rdi), %rdi


Ldo_addmul:
	push	%r15
	push	%r8			





Louter:
	mov	(%rcx), %r9
	mov	8(%rcx), %r8
	mov	(%rsi,%rbx,8), %rax
	mul	%r9
	test	$1, %bl
	jnz	La1x1

La1x0:mov	(%rdi,%rbx,8), %r14
	xor	%r10, %r10
	mov	%rdx, %r11
	test	$2, %bl
	jnz	La110

La100:lea	2(%rbx), %rbp		
	add	%rax, %r14
	adc	$0, %r11
	mov	(%rsi,%rbx,8), %rax
	mul	%r8
	mov	8(%rdi,%rbx,8), %r15
	jmp	Llo0

La110:lea	(%rbx), %rbp			
	xor	%r13, %r13
	jmp	Llo2

La1x1:mov	(%rdi,%rbx,8), %r15
	xor	%r12, %r12
	xor	%r11, %r11
	test	$2, %bl
	jz	La111

La101:lea	3(%rbx), %rbp		
	mov	%rdx, %r13
	add	%rax, %r15
	mov	(%rsi,%rbx,8), %rax
	mov	8(%rdi,%rbx,8), %r14
	adc	$0, %r13
	jmp	Ltop

La111:lea	1(%rbx), %rbp		
	jmp	Llo3

	.align	32, 0x90
Ltop:	mul	%r8
	mov	%rdx, %r10
	add	%rax, %r14
	adc	$0, %r10
	add	%r11, %r15
	adc	$0, %r13
	add	%r12, %r14
	adc	$0, %r10
	mov	-16(%rsi,%rbp,8), %rax
	mul	%r9
	add	%rax, %r14
	mov	%rdx, %r11
	adc	$0, %r11
	mov	-16(%rsi,%rbp,8), %rax
	mul	%r8
	mov	%r15, -24(%rdi,%rbp,8)
	mov	-8(%rdi,%rbp,8), %r15
	add	%r13, %r14
	adc	$0, %r11
Llo0:	mov	%rdx, %r12
	mov	%r14, -16(%rdi,%rbp,8)
	add	%rax, %r15
	adc	$0, %r12
	mov	-8(%rsi,%rbp,8), %rax
	add	%r10, %r15
	adc	$0, %r12
	mul	%r9
Llo3:	add	%rax, %r15
	mov	%rdx, %r13
	adc	$0, %r13
	mov	-8(%rsi,%rbp,8), %rax
	mul	%r8
	add	%r11, %r15
	mov	(%rdi,%rbp,8), %r14
	adc	$0, %r13
	mov	%rdx, %r10
	add	%rax, %r14
	adc	$0, %r10
	mov	(%rsi,%rbp,8), %rax
	mul	%r9
	add	%r12, %r14
	mov	%r15, -8(%rdi,%rbp,8)
	mov	%rdx, %r11
	adc	$0, %r10
Llo2:	add	%rax, %r14
	adc	$0, %r11
	mov	(%rsi,%rbp,8), %rax
	add	%r13, %r14
	adc	$0, %r11
	mul	%r8
	mov	8(%rdi,%rbp,8), %r15
	add	%rax, %r15
	mov	%rdx, %r12
	adc	$0, %r12
	mov	8(%rsi,%rbp,8), %rax
	mov	%r14, (%rdi,%rbp,8)
	mul	%r9
	add	%r10, %r15
	mov	%rdx, %r13
	adc	$0, %r12
	add	%rax, %r15
	mov	8(%rsi,%rbp,8), %rax
	mov	16(%rdi,%rbp,8), %r14		
	adc	$0, %r13
	add	$4, %rbp
	jnc	Ltop

Lend:	mul	%r8
	add	%r11, %r15
	adc	$0, %r13
	add	%r12, %rax
	adc	$0, %rdx
	mov	%r15, -8(%rdi)
	add	%r13, %rax
	adc	$0, %rdx
	mov	%rax, (%rdi)
	mov	%rdx, 8(%rdi)

	addl	$-2, (%rsp)
	lea	16(%rcx), %rcx
	lea	16(%rdi), %rdi
	jnz	Louter

	pop	%rax		
	pop	%r15
Lret5:pop	%r14
	pop	%r13
	pop	%r12
Lret2:pop	%rbp
	pop	%rbx
	pop	%rsi
	pop	%rdi
	ret
	
