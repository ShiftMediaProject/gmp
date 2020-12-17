







































































































	.text
	.align	32, 0x90
	.globl	__gmpn_mullo_basecase
	
	.def	__gmpn_mullo_basecase
	.scl	2
	.type	32
	.endef
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

	mov	(%rdx), %r10
	push	%rbx
	lea	(%rdi,%rcx,8), %rdi	
	push	%rbp
	lea	(%rsi,%rcx,8), %rsi	
	push	%r12
	mov	$0, %r9d		
	sub	%rcx, %r9
	push	%r13
	mul	%r10
	mov	8(%r8), %r11

	test	$1, %cl
	jnz	Lm2x1

Lm2x0:test	$2, %cl
	jnz	Lm2b2

Lm2b0:lea	(%r9), %r13
	mov	%rax, (%rdi,%r9,8)
	mov	%rdx, %rcx
	mov	(%rsi,%r9,8), %rax
	xor	%ebp, %ebp
	jmp	Lm2e0

Lm2b2:lea	-2(%r9), %r13
	mov	%rax, %rbp
	mov	(%rsi,%r9,8), %rax
	mov	%rdx, %r12
	xor	%ebx, %ebx
	jmp	Lm2e2

Lm2x1:test	$2, %cl
	jnz	Lm2b3

Lm2b1:lea	1(%r9), %r13
	mov	%rax, (%rdi,%r9,8)
	mov	(%rsi,%r9,8), %rax
	mov	%rdx, %rbx
	xor	%ecx, %ecx
	jmp	Lm2e1

Lm2b3:lea	-1(%r9), %r13
	xor	%r12d, %r12d
	mov	%rax, %rcx
	mov	%rdx, %rbp
	mov	(%rsi,%r9,8), %rax
	jmp	Lm2e3

	.align	16, 0x90
Lm2tp:mul	%r10
	add	%rax, %r12
	mov	-8(%rsi,%r13,8), %rax
	mov	%r12, -8(%rdi,%r13,8)
	adc	%rdx, %rbx
	adc	$0, %ecx
Lm2e1:mul	%r11
	add	%rax, %rbx
	adc	%rdx, %rcx
	mov	$0, %ebp
	mov	(%rsi,%r13,8), %rax
	mul	%r10
	add	%rax, %rbx
	mov	%rbx, (%rdi,%r13,8)
	adc	%rdx, %rcx
	mov	(%rsi,%r13,8), %rax
	adc	$0, %ebp
Lm2e0:mul	%r11
	add	%rax, %rcx
	adc	%rdx, %rbp
	mov	8(%rsi,%r13,8), %rax
	mul	%r10
	mov	$0, %r12d
	add	%rax, %rcx
	adc	%rdx, %rbp
	adc	$0, %r12d
	mov	8(%rsi,%r13,8), %rax
Lm2e3:mul	%r11
	add	%rax, %rbp
	mov	%rcx, 8(%rdi,%r13,8)
	adc	%rdx, %r12
	mov	$0, %ebx
	mov	16(%rsi,%r13,8), %rax
	mul	%r10
	add	%rax, %rbp
	mov	16(%rsi,%r13,8), %rax
	adc	%rdx, %r12
	adc	$0, %ebx
Lm2e2:mul	%r11
	mov	$0, %ecx		
	add	%rax, %r12
	mov	24(%rsi,%r13,8), %rax
	mov	%rbp, 16(%rdi,%r13,8)
	adc	%rdx, %rbx		
	add	$4, %r13
	js	Lm2tp

Lm2ed:imul	%r10, %rax
	add	%r12, %rax
	mov	%rax, -8(%rdi)

	add	$2, %r9
	lea	16(%r8), %r8
	lea	-16(%rsi), %rsi
	cmp	$-2, %r9
	jge	Lcor1

	push	%r14
	push	%r15

Louter:
	mov	(%r8), %r10
	mov	8(%r8), %r11
	mov	(%rsi,%r9,8), %rax
	mul	%r10
	test	$1, %r9b
	jnz	La1x1

La1x0:mov	%rax, %r15
	mov	%rdx, %r14
	mov	(%rsi,%r9,8), %rax
	mul	%r11
	test	$2, %r9b
	jnz	La110

La100:lea	(%r9), %r13
	mov	(%rdi,%r9,8), %r12
	mov	%rax, %rbx
	lea	(%rdx), %rcx
	jmp	Llo0

La110:lea	2(%r9), %r13
	mov	(%rdi,%r9,8), %rcx
	mov	%rax, %rbp
	mov	8(%rsi,%r9,8), %rax
	lea	(%rdx), %r12
	jmp	Llo2

La1x1:mov	%rax, %r14
	mov	%rdx, %r15
	mov	(%rsi,%r9,8), %rax
	mul	%r11
	test	$2, %r9b
	jz	La111

La101:lea	1(%r9), %r13
	lea	(%rdx), %rbx
	mov	(%rdi,%r9,8), %rbp
	mov	%rax, %r12
	jmp	Llo1

La111:lea	-1(%r9), %r13
	lea	(%rdx), %rbp
	mov	%rax, %rcx
	mov	(%rdi,%r9,8), %rbx
	mov	8(%rsi,%r9,8), %rax
	jmp	Llo3

	.align	16, 0x90
Ltop:	mul	%r11
	add	%rbx, %rcx
	adc	%rax, %rbp
	mov	-8(%rsi,%r13,8), %rax
	lea	(%rdx), %r12
	adc	$0, %r12
Llo2:	mul	%r10
	add	%rcx, %r15
	mov	%r15, -16(%rdi,%r13,8)
	adc	%rax, %r14
	mov	%rdx, %r15
	adc	$0, %r15
	mov	-8(%rsi,%r13,8), %rax
	mul	%r11
	lea	(%rdx), %rbx
	mov	-8(%rdi,%r13,8), %rcx
	add	%rcx, %rbp
	adc	%rax, %r12
	adc	$0, %rbx
Llo1:	mov	(%rsi,%r13,8), %rax
	mul	%r10
	add	%rbp, %r14
	adc	%rax, %r15
	mov	%r14, -8(%rdi,%r13,8)
	mov	%rdx, %r14
	adc	$0, %r14
	mov	(%rsi,%r13,8), %rax
	mov	(%rdi,%r13,8), %rbp
	mul	%r11
	add	%rbp, %r12
	adc	%rax, %rbx
	lea	(%rdx), %rcx
	adc	$0, %rcx
Llo0:	mov	8(%rsi,%r13,8), %rax
	mul	%r10
	add	%r12, %r15
	mov	%r15, (%rdi,%r13,8)
	adc	%rax, %r14
	mov	%rdx, %r15
	mov	8(%rdi,%r13,8), %r12
	adc	$0, %r15
	mov	8(%rsi,%r13,8), %rax
	mul	%r11
	add	%r12, %rbx
	lea	(%rdx), %rbp
	adc	%rax, %rcx
	mov	16(%rsi,%r13,8), %rax
	adc	$0, %rbp
Llo3:	mul	%r10
	add	%rbx, %r14
	mov	%r14, 8(%rdi,%r13,8)
	mov	%rdx, %r14
	adc	%rax, %r15
	mov	16(%rsi,%r13,8), %rax
	mov	16(%rdi,%r13,8), %rbx
	adc	$0, %r14
	add	$4, %r13
	jnc	Ltop

Lend:	imul	%r11, %rax
	add	%rbx, %rcx
	adc	%rax, %rbp
	mov	-8(%rsi), %rax
	imul	%r10, %rax
	add	%rcx, %r15
	mov	%r15, -16(%rdi)
	adc	%r14, %rax
	mov	-8(%rdi), %rcx
	add	%rcx, %rbp
	add	%rbp, %rax
	mov	%rax, -8(%rdi)

	add	$2, %r9
	lea	16(%r8), %r8
	lea	-16(%rsi), %rsi
	cmp	$-2, %r9
	jl	Louter

	pop	%r15
	pop	%r14

	jnz	Lcor0

Lcor1:mov	(%r8), %r10
	mov	8(%r8), %r11
	mov	-16(%rsi), %rax
	mul	%r10			
	add	-16(%rdi), %rax		
	adc	-8(%rdi), %rdx		
	mov	-8(%rsi), %rbx
	imul	%r10, %rbx
	mov	-16(%rsi), %rcx
	imul	%r11, %rcx
	mov	%rax, -16(%rdi)
	add	%rbx, %rcx
	add	%rdx, %rcx
	mov	%rcx, -8(%rdi)
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
	
