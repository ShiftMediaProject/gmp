































































































	.text
	.align	32, 0x90
	.globl	__gmpn_mullo_basecase
	
__gmpn_mullo_basecase:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx


	mov	(%rsi), %rax
	mov	%rdx, %r8

	cmp	$4, %rcx
	jb	Lsmall

	mov	(%rdx), %r9
	push	%rbx
	lea	(%rdi,%rcx,8), %rdi		
	push	%rbp
	lea	(%rsi,%rcx,8), %rsi		
	push	%r12
	neg	%rcx
	push	%r13
	mul	%r9
	mov	8(%r8), %rbx

	test	$1, %cl
	jnz	Lm2b1

Lm2b0:lea	(%rcx), %rbp
	xor	%r10, %r10
	mov	%rax, %r12
	mov	%rdx, %r11
	jmp	Lm2l0

Lm2b1:lea	1(%rcx), %rbp
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
	mul	%rbx
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
	mul	%rbx
	mov	%r12, (%rdi,%rbp,8)
	add	%rax, %r10
	mov	%rdx, %r12		
	mov	8(%rsi,%rbp,8), %rax
	adc	$0, %r12			
	add	$2, %rbp
	jnc	Lm2tp

Lm2ed:imul	%r9, %rax
	add	%r10, %rax
	add	%r11, %rax
	mov	%rax, -8(%rdi)

	add	$2, %rcx
	lea	16(%r8), %r8
	lea	-16(%rsi), %rsi
	cmp	$-2, %rcx
	jge	Lcor1

	push	%r14
	push	%r15

Louter:
	mov	(%r8), %r9
	mov	8(%r8), %rbx
	mov	(%rsi,%rcx,8), %rax
	mul	%r9
	test	$1, %cl
	jnz	La1x1

La1x0:mov	(%rdi,%rcx,8), %r15
	xor	%r12, %r12
	xor	%r11, %r11
	test	$2, %cl
	jnz	La110

La100:lea	1(%rcx), %rbp
	jmp	Llo0

La110:lea	3(%rcx), %rbp
	mov	%rdx, %r13
	add	%rax, %r15
	mov	(%rsi,%rcx,8), %rax
	mov	8(%rdi,%rcx,8), %r14
	adc	$0, %r13
	jmp	Llo2

La1x1:mov	(%rdi,%rcx,8), %r14
	xor	%r10, %r10
	mov	%rdx, %r11
	test	$2, %cl
	jz	La111

La101:lea	2(%rcx), %rbp
	add	%rax, %r14
	adc	$0, %r11
	mov	(%rsi,%rcx,8), %rax
	mul	%rbx
	mov	8(%rdi,%rcx,8), %r15
	jmp	Llo1

La111:lea	(%rcx), %rbp
	xor	%r13, %r13
	jmp	Llo3

	.align	32, 0x90
Ltop:
Llo2:	mul	%rbx
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
	mul	%rbx
	mov	%r15, -24(%rdi,%rbp,8)
	mov	-8(%rdi,%rbp,8), %r15
	add	%r13, %r14
	adc	$0, %r11
Llo1:	mov	%rdx, %r12
	mov	%r14, -16(%rdi,%rbp,8)
	add	%rax, %r15
	adc	$0, %r12
	mov	-8(%rsi,%rbp,8), %rax
	add	%r10, %r15
	adc	$0, %r12
	mul	%r9
Llo0:	add	%rax, %r15
	mov	%rdx, %r13
	adc	$0, %r13
	mov	-8(%rsi,%rbp,8), %rax
	mul	%rbx
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
Llo3:	add	%rax, %r14
	adc	$0, %r11
	mov	(%rsi,%rbp,8), %rax
	add	%r13, %r14
	adc	$0, %r11
	mul	%rbx
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

Lend:	imul	%rbx, %rax
	add	%rax, %r14
	add	%r11, %r15
	adc	$0, %r13
	add	%r12, %r14
	mov	-8(%rsi), %rax
	imul	%r9, %rax
	add	%r14, %rax
	mov	%r15, -16(%rdi)
	add	%r13, %rax
	mov	%rax, -8(%rdi)

	add	$2, %rcx
	lea	16(%r8), %r8
	lea	-16(%rsi), %rsi
	cmp	$-2, %rcx
	jl	Louter

	pop	%r15
	pop	%r14

	jnz	Lcor0

Lcor1:mov	(%r8), %r9
	mov	8(%r8), %rbx
	mov	-16(%rsi), %rax
	mul	%r9			
	add	-16(%rdi), %rax		
	adc	-8(%rdi), %rdx		
	mov	-8(%rsi), %r10
	imul	%r9, %r10
	mov	-16(%rsi), %r11
	imul	%rbx, %r11
	mov	%rax, -16(%rdi)
	add	%r10, %r11
	add	%rdx, %r11
	mov	%r11, -8(%rdi)
	pop	%r13
	pop	%r12
	pop	%rbp
	pop	%rbx
	pop	%rsi
	pop	%rdi
	ret

Lcor0:mov	(%r8), %r11
	imul	-8(%rsi), %r11
	add	%rax, %r11
	mov	%r11, -8(%rdi)
	pop	%r13
	pop	%r12
	pop	%rbp
	pop	%rbx
	pop	%rsi
	pop	%rdi
	ret

	.align	16, 0x90
Lsmall:
	cmp	$2, %rcx
	jae	Lgt1
Ln1:	imul	(%rdx), %rax
	mov	%rax, (%rdi)
	pop	%rsi
	pop	%rdi
	ret
Lgt1:	ja	Lgt2
Ln2:	mov	(%rdx), %r9
	mul	%r9
	mov	%rax, (%rdi)
	mov	8(%rsi), %rax
	imul	%r9, %rax
	add	%rax, %rdx
	mov	8(%r8), %r9
	mov	(%rsi), %rcx
	imul	%r9, %rcx
	add	%rcx, %rdx
	mov	%rdx, 8(%rdi)
	pop	%rsi
	pop	%rdi
	ret
Lgt2:
Ln3:	mov	(%rdx), %r9
	mul	%r9		
	mov	%rax, (%rdi)
	mov	%rdx, %r10
	mov	8(%rsi), %rax
	mul	%r9		
	imul	16(%rsi), %r9	
	add	%rax, %r10
	adc	%rdx, %r9
	mov	8(%r8), %r11
	mov	(%rsi), %rax
	mul	%r11		
	add	%rax, %r10
	adc	%rdx, %r9
	imul	8(%rsi), %r11	
	add	%r11, %r9
	mov	%r10, 8(%rdi)
	mov	16(%r8), %r10
	mov	(%rsi), %rax
	imul	%rax, %r10	
	add	%r10, %r9
	mov	%r9, 16(%rdi)
	pop	%rsi
	pop	%rdi
	ret
	
