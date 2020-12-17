















































































































































	.text
	.align	32, 0x90
	.globl	__gmpn_sqr_basecase
	
	.def	__gmpn_sqr_basecase
	.scl	2
	.type	32
	.endef
__gmpn_sqr_basecase:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx


	cmp	$4, %rdx
	jl	Lsmall

	push	%rbx
	push	%rbp
	push	%r12
	push	%r13
	push	%r14

	mov	(%rsi), %r10
	mov	8(%rsi), %rax
	mov	%rax, %r11

	mov	$1, %r8d
	sub	%rdx, %r8		
	push	%r8

	lea	(%rsi,%rdx,8), %rsi
	lea	(%rdi,%rdx,8), %rdi

	mul	%r10

	test	$1, %r8b
	jnz	Lbx1

Lbx0:	test	$2, %r8b
	mov	%rax, (%rdi,%r8,8)
	jnz	Lb10

Lb00:	lea	(%r8), %r13			
	mov	%rdx, %rcx		
	xor	%ebp, %ebp
	jmp	Lm2e0

Lb10:	lea	2(%r8), %r13			
	mov	8(%rsi,%r8,8), %rax
	mov	%rdx, %r9		
	xor	%ebx, %ebx
	xor	%ecx, %ecx
	jmp	Lm2e2

Lbx1:	test	$2, %r8b
	mov	%rax, (%rdi,%r8,8)
	jz	Lb11

Lb01:	lea	1(%r8), %r13			
	mov	%rdx, %rbx		
	xor	%ecx, %ecx
	jmp	Lm2e1

Lb11:	lea	-1(%r8), %r13		
	mov	%rdx, %rbp		
	xor	%r9d, %r9d
	jmp	Lm2e3


	.align	16, 0x90
Lm2top1:
	mul	%r10
	add	%rax, %r9
	mov	-8(%rsi,%r13,8), %rax
	mov	%r9, -8(%rdi,%r13,8)
	adc	%rdx, %rbx
	adc	$0, %ecx
	mul	%r11
	add	%rax, %rbx
	adc	%rdx, %rcx
Lm2e1:mov	$0, %ebp
	mov	(%rsi,%r13,8), %rax
	mul	%r10
	add	%rax, %rbx
	mov	%rbx, (%rdi,%r13,8)
	adc	%rdx, %rcx
	mov	(%rsi,%r13,8), %rax
	adc	$0, %ebp
	mul	%r11
	add	%rax, %rcx
	adc	%rdx, %rbp
	mov	8(%rsi,%r13,8), %rax
	mul	%r10
	mov	$0, %r9d
	add	%rax, %rcx
	adc	%rdx, %rbp
	adc	$0, %r9d
	mov	8(%rsi,%r13,8), %rax
	mul	%r11
	add	%rax, %rbp
	mov	%rcx, 8(%rdi,%r13,8)
	adc	%rdx, %r9
	mov	$0, %ebx
	mov	16(%rsi,%r13,8), %rax
	mul	%r10
	add	%rax, %rbp
	mov	16(%rsi,%r13,8), %rax
	adc	%rdx, %r9
	adc	$0, %ebx
	mul	%r11
	mov	$0, %ecx
	add	%rax, %r9
	mov	24(%rsi,%r13,8), %rax
	mov	%rbp, 16(%rdi,%r13,8)
	adc	%rdx, %rbx
	add	$4, %r13
	js	Lm2top1

	mul	%r10
	add	%rax, %r9
	mov	-8(%rsi), %rax
	mov	%r9, -8(%rdi)
	adc	%rdx, %rbx
	adc	%ecx, %ecx
	mul	%r11
	add	%rbx, %rax
	adc	%rcx, %rdx
	mov	%rax, (%rdi)
	mov	%rdx, 8(%rdi)

	lea	16(%rdi), %rdi
	add	$2, %r8			
	jmp	Lam2o3

	.align	16, 0x90
Lm2top3:
	mul	%r10
	add	%rax, %r9
	mov	-8(%rsi,%r13,8), %rax
	mov	%r9, -8(%rdi,%r13,8)
	adc	%rdx, %rbx
	adc	$0, %ecx
	mul	%r11
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
	mul	%r11
	add	%rax, %rcx
	adc	%rdx, %rbp
	mov	8(%rsi,%r13,8), %rax
	mul	%r10
	mov	$0, %r9d
	add	%rax, %rcx
	adc	%rdx, %rbp
	adc	$0, %r9d
	mov	8(%rsi,%r13,8), %rax
	mul	%r11
	add	%rax, %rbp
	mov	%rcx, 8(%rdi,%r13,8)
	adc	%rdx, %r9
Lm2e3:mov	$0, %ebx
	mov	16(%rsi,%r13,8), %rax
	mul	%r10
	add	%rax, %rbp
	mov	16(%rsi,%r13,8), %rax
	adc	%rdx, %r9
	adc	$0, %ebx
	mul	%r11
	mov	$0, %ecx
	add	%rax, %r9
	mov	24(%rsi,%r13,8), %rax
	mov	%rbp, 16(%rdi,%r13,8)
	adc	%rdx, %rbx
	add	$4, %r13
	js	Lm2top3

	mul	%r10
	add	%rax, %r9
	mov	-8(%rsi), %rax
	mov	%r9, -8(%rdi)
	adc	%rdx, %rbx
	adc	%ecx, %ecx
	mul	%r11
	add	%rbx, %rax
	adc	%rcx, %rdx
	mov	%rax, (%rdi)
	mov	%rdx, 8(%rdi)

	lea	16(%rdi), %rdi
	add	$2, %r8			
	cmp	$-1, %r8
	jz	Lcor1			

Lam2o1:
	mov	-8(%rsi,%r8,8), %r10
	mov	(%rsi,%r8,8), %rax
	mov	%rax, %r11
	lea	1(%r8), %r13
	mul	%r10
	mov	%rax, %r14
	mov	%rdx, %r12
	mov	(%rdi,%r8,8), %rcx
	xor	%ebp, %ebp
	mov	8(%rsi,%r8,8), %rax
	xor	%r9d, %r9d
	jmp	Llo1

	.align	16, 0x90
Lam2top1:
	mul	%r11
	add	%rbx, %rcx
	adc	%rax, %rbp
	mov	(%rsi,%r13,8), %rax
	lea	(%rdx), %r9
	adc	$0, %r9
Llo1:	mul	%r10
	add	%rcx, %r14
	mov	%r14, -8(%rdi,%r13,8)
	adc	%rax, %r12
	mov	%rdx, %r14
	adc	$0, %r14
	mov	(%rsi,%r13,8), %rax
	mul	%r11
	lea	(%rdx), %rbx
	mov	(%rdi,%r13,8), %rcx
	add	%rcx, %rbp
	adc	%rax, %r9
	adc	$0, %rbx
	mov	8(%rsi,%r13,8), %rax
	mul	%r10
	add	%rbp, %r12
	adc	%rax, %r14
	mov	%r12, (%rdi,%r13,8)
	mov	%rdx, %r12
	adc	$0, %r12
	mov	8(%rsi,%r13,8), %rax
	mov	8(%rdi,%r13,8), %rbp
	mul	%r11
	add	%rbp, %r9
	adc	%rax, %rbx
	lea	(%rdx), %rcx
	adc	$0, %rcx
	mov	16(%rsi,%r13,8), %rax
	mul	%r10
	add	%r9, %r14
	mov	%r14, 8(%rdi,%r13,8)
	adc	%rax, %r12
	mov	%rdx, %r14
	mov	16(%rdi,%r13,8), %r9
	adc	$0, %r14
	mov	16(%rsi,%r13,8), %rax
	mul	%r11
	add	%r9, %rbx
	lea	(%rdx), %rbp
	adc	%rax, %rcx
	mov	24(%rsi,%r13,8), %rax
	adc	$0, %rbp
	mul	%r10
	add	%rbx, %r12
	mov	%r12, 16(%rdi,%r13,8)
	mov	%rdx, %r12
	adc	%rax, %r14
	mov	24(%rsi,%r13,8), %rax
	mov	24(%rdi,%r13,8), %rbx
	adc	$0, %r12
	add	$4, %r13
	jnc	Lam2top1

	mul	%r11
	add	%rbx, %rcx
	adc	%rbp, %rax
	adc	%r13, %rdx
	add	%rcx, %r14
	adc	%r13, %r12
	mov	%r14, -8(%rdi)
	add	%r12, %rax
	mov	%rax, (%rdi)
	adc	%r13, %rdx
	mov	%rdx, 8(%rdi)

	lea	16(%rdi), %rdi
	add	$2, %r8

Lam2o3:
	mov	-8(%rsi,%r8,8), %r10
	mov	(%rsi,%r8,8), %rax
	mov	%rax, %r11
	lea	-1(%r8), %r13
	mul	%r10
	mov	%rax, %r14
	mov	%rdx, %r12
	mov	(%rdi,%r8,8), %r9
	xor	%ebx, %ebx
	xor	%ecx, %ecx
	mov	8(%rsi,%r8,8), %rax
	jmp	Llo3

	.align	16, 0x90
Lam2top3:
	mul	%r11
	add	%rbx, %rcx
	adc	%rax, %rbp
	mov	(%rsi,%r13,8), %rax
	lea	(%rdx), %r9
	adc	$0, %r9
	mul	%r10
	add	%rcx, %r14
	mov	%r14, -8(%rdi,%r13,8)
	adc	%rax, %r12
	mov	%rdx, %r14
	adc	$0, %r14
	mov	(%rsi,%r13,8), %rax
	mul	%r11
	lea	(%rdx), %rbx
	mov	(%rdi,%r13,8), %rcx
	add	%rcx, %rbp
	adc	%rax, %r9
	adc	$0, %rbx
	mov	8(%rsi,%r13,8), %rax
	mul	%r10
	add	%rbp, %r12
	adc	%rax, %r14
	mov	%r12, (%rdi,%r13,8)
	mov	%rdx, %r12
	adc	$0, %r12
	mov	8(%rsi,%r13,8), %rax
	mov	8(%rdi,%r13,8), %rbp
	mul	%r11
	add	%rbp, %r9
	adc	%rax, %rbx
	lea	(%rdx), %rcx
	adc	$0, %rcx
	mov	16(%rsi,%r13,8), %rax
Llo3:	mul	%r10
	add	%r9, %r14
	mov	%r14, 8(%rdi,%r13,8)
	adc	%rax, %r12
	mov	%rdx, %r14
	mov	16(%rdi,%r13,8), %r9
	adc	$0, %r14
	mov	16(%rsi,%r13,8), %rax
	mul	%r11
	add	%r9, %rbx
	lea	(%rdx), %rbp
	adc	%rax, %rcx
	mov	24(%rsi,%r13,8), %rax
	adc	$0, %rbp
	mul	%r10
	add	%rbx, %r12
	mov	%r12, 16(%rdi,%r13,8)
	mov	%rdx, %r12
	adc	%rax, %r14
	mov	24(%rsi,%r13,8), %rax
	mov	24(%rdi,%r13,8), %rbx
	adc	$0, %r12
	add	$4, %r13
	jnc	Lam2top3

	mul	%r11
	add	%rbx, %rcx
	adc	%rbp, %rax
	adc	%r13, %rdx
	add	%rcx, %r14
	adc	%r13, %r12
	mov	%r14, -8(%rdi)
	add	%r12, %rax
	mov	%rax, (%rdi)
	adc	%r13, %rdx
	mov	%rdx, 8(%rdi)

	lea	16(%rdi), %rdi
	add	$2, %r8
	cmp	$-1, %r8
	jnz	Lam2o1

Lcor1:pop	%r8
	mov	%rdx, %r9
	mov	-16(%rsi), %r10
	mov	-8(%rsi), %rax
	mul	%r10
	add	%r9, %rax
	adc	$0, %rdx
	mov	%rax, -8(%rdi)
	mov	%rdx, (%rdi)
	jmp	Lsqr_diag_addlsh1

	.align	16, 0x90
Lm2top2:
Lm2e2:mul	%r10
	add	%rax, %r9
	mov	-8(%rsi,%r13,8), %rax
	mov	%r9, -8(%rdi,%r13,8)
	adc	%rdx, %rbx
	adc	$0, %ecx
	mul	%r11
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
	mul	%r11
	add	%rax, %rcx
	adc	%rdx, %rbp
	mov	8(%rsi,%r13,8), %rax
	mul	%r10
	mov	$0, %r9d
	add	%rax, %rcx
	adc	%rdx, %rbp
	adc	$0, %r9d
	mov	8(%rsi,%r13,8), %rax
	mul	%r11
	add	%rax, %rbp
	mov	%rcx, 8(%rdi,%r13,8)
	adc	%rdx, %r9
	mov	$0, %ebx
	mov	16(%rsi,%r13,8), %rax
	mul	%r10
	add	%rax, %rbp
	mov	16(%rsi,%r13,8), %rax
	adc	%rdx, %r9
	adc	$0, %ebx
	mul	%r11
	mov	$0, %ecx
	add	%rax, %r9
	mov	24(%rsi,%r13,8), %rax
	mov	%rbp, 16(%rdi,%r13,8)
	adc	%rdx, %rbx
	add	$4, %r13
	js	Lm2top2

	mul	%r10
	add	%rax, %r9
	mov	-8(%rsi), %rax
	mov	%r9, -8(%rdi)
	adc	%rdx, %rbx
	adc	%ecx, %ecx
	mul	%r11
	add	%rbx, %rax
	adc	%rcx, %rdx
	mov	%rax, (%rdi)
	mov	%rdx, 8(%rdi)

	lea	16(%rdi), %rdi
	add	$2, %r8			
	jmp	Lam2o0

	.align	16, 0x90
Lm2top0:
	mul	%r10
	add	%rax, %r9
	mov	-8(%rsi,%r13,8), %rax
	mov	%r9, -8(%rdi,%r13,8)
	adc	%rdx, %rbx
	adc	$0, %ecx
	mul	%r11
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
	mul	%r11
	add	%rax, %rcx
	adc	%rdx, %rbp
Lm2e0:mov	8(%rsi,%r13,8), %rax
	mul	%r10
	mov	$0, %r9d
	add	%rax, %rcx
	adc	%rdx, %rbp
	adc	$0, %r9d
	mov	8(%rsi,%r13,8), %rax
	mul	%r11
	add	%rax, %rbp
	mov	%rcx, 8(%rdi,%r13,8)
	adc	%rdx, %r9
	mov	$0, %ebx
	mov	16(%rsi,%r13,8), %rax
	mul	%r10
	add	%rax, %rbp
	mov	16(%rsi,%r13,8), %rax
	adc	%rdx, %r9
	adc	$0, %ebx
	mul	%r11
	mov	$0, %ecx
	add	%rax, %r9
	mov	24(%rsi,%r13,8), %rax
	mov	%rbp, 16(%rdi,%r13,8)
	adc	%rdx, %rbx
	add	$4, %r13
	js	Lm2top0

	mul	%r10
	add	%rax, %r9
	mov	-8(%rsi), %rax
	mov	%r9, -8(%rdi)
	adc	%rdx, %rbx
	adc	%ecx, %ecx
	mul	%r11
	add	%rbx, %rax
	adc	%rcx, %rdx
	mov	%rax, (%rdi)
	mov	%rdx, 8(%rdi)

	lea	16(%rdi), %rdi
	add	$2, %r8			
	cmp	$-2, %r8
	jz	Lcor2			

Lam2o2:
	mov	-8(%rsi,%r8,8), %r10
	mov	(%rsi,%r8,8), %rax
	mov	%rax, %r11
	lea	-2(%r8), %r13
	mul	%r10
	mov	%rax, %r12
	mov	%rdx, %r14
	mov	(%rdi,%r8,8), %rbx
	xor	%ecx, %ecx
	xor	%ebp, %ebp
	mov	8(%rsi,%r8,8), %rax
	jmp	Llo2

	.align	16, 0x90
Lam2top2:
	mul	%r11
	add	%rbx, %rcx
	adc	%rax, %rbp
	mov	(%rsi,%r13,8), %rax
	lea	(%rdx), %r9
	adc	$0, %r9
	mul	%r10
	add	%rcx, %r14
	mov	%r14, -8(%rdi,%r13,8)
	adc	%rax, %r12
	mov	%rdx, %r14
	adc	$0, %r14
	mov	(%rsi,%r13,8), %rax
	mul	%r11
	lea	(%rdx), %rbx
	mov	(%rdi,%r13,8), %rcx
	add	%rcx, %rbp
	adc	%rax, %r9
	adc	$0, %rbx
	mov	8(%rsi,%r13,8), %rax
	mul	%r10
	add	%rbp, %r12
	adc	%rax, %r14
	mov	%r12, (%rdi,%r13,8)
	mov	%rdx, %r12
	adc	$0, %r12
	mov	8(%rsi,%r13,8), %rax
	mov	8(%rdi,%r13,8), %rbp
	mul	%r11
	add	%rbp, %r9
	adc	%rax, %rbx
	lea	(%rdx), %rcx
	adc	$0, %rcx
	mov	16(%rsi,%r13,8), %rax
	mul	%r10
	add	%r9, %r14
	mov	%r14, 8(%rdi,%r13,8)
	adc	%rax, %r12
	mov	%rdx, %r14
	mov	16(%rdi,%r13,8), %r9
	adc	$0, %r14
	mov	16(%rsi,%r13,8), %rax
	mul	%r11
	add	%r9, %rbx
	lea	(%rdx), %rbp
	adc	%rax, %rcx
	mov	24(%rsi,%r13,8), %rax
	adc	$0, %rbp
Llo2:	mul	%r10
	add	%rbx, %r12
	mov	%r12, 16(%rdi,%r13,8)
	mov	%rdx, %r12
	adc	%rax, %r14
	mov	24(%rsi,%r13,8), %rax
	mov	24(%rdi,%r13,8), %rbx
	adc	$0, %r12
	add	$4, %r13
	jnc	Lam2top2

	mul	%r11
	add	%rbx, %rcx
	adc	%rbp, %rax
	adc	%r13, %rdx
	add	%rcx, %r14
	adc	%r13, %r12
	mov	%r14, -8(%rdi)
	add	%r12, %rax
	mov	%rax, (%rdi)
	adc	%r13, %rdx
	mov	%rdx, 8(%rdi)

	lea	16(%rdi), %rdi
	add	$2, %r8

Lam2o0:
	mov	-8(%rsi,%r8,8), %r10
	mov	(%rsi,%r8,8), %rax
	mov	%rax, %r11
	lea	0(%r8), %r13
	mul	%r10
	mov	%rax, %r12
	mov	%rdx, %r14
	xor	%ebx, %ebx
	mov	(%rdi,%r8,8), %rbp
	xor	%r9d, %r9d
	jmp	Llo0

	.align	16, 0x90
Lam2top0:
	mul	%r11
	add	%rbx, %rcx
	adc	%rax, %rbp
	mov	(%rsi,%r13,8), %rax
	lea	(%rdx), %r9
	adc	$0, %r9
	mul	%r10
	add	%rcx, %r14
	mov	%r14, -8(%rdi,%r13,8)
	adc	%rax, %r12
	mov	%rdx, %r14
	adc	$0, %r14
	mov	(%rsi,%r13,8), %rax
	mul	%r11
	lea	(%rdx), %rbx
	mov	(%rdi,%r13,8), %rcx
	add	%rcx, %rbp
	adc	%rax, %r9
	adc	$0, %rbx
Llo0:	mov	8(%rsi,%r13,8), %rax
	mul	%r10
	add	%rbp, %r12
	adc	%rax, %r14
	mov	%r12, (%rdi,%r13,8)
	mov	%rdx, %r12
	adc	$0, %r12
	mov	8(%rsi,%r13,8), %rax
	mov	8(%rdi,%r13,8), %rbp
	mul	%r11
	add	%rbp, %r9
	adc	%rax, %rbx
	lea	(%rdx), %rcx
	adc	$0, %rcx
	mov	16(%rsi,%r13,8), %rax
	mul	%r10
	add	%r9, %r14
	mov	%r14, 8(%rdi,%r13,8)
	adc	%rax, %r12
	mov	%rdx, %r14
	mov	16(%rdi,%r13,8), %r9
	adc	$0, %r14
	mov	16(%rsi,%r13,8), %rax
	mul	%r11
	add	%r9, %rbx
	lea	(%rdx), %rbp
	adc	%rax, %rcx
	mov	24(%rsi,%r13,8), %rax
	adc	$0, %rbp
	mul	%r10
	add	%rbx, %r12
	mov	%r12, 16(%rdi,%r13,8)
	mov	%rdx, %r12
	adc	%rax, %r14
	mov	24(%rsi,%r13,8), %rax
	mov	24(%rdi,%r13,8), %rbx
	adc	$0, %r12
	add	$4, %r13
	jnc	Lam2top0

	mul	%r11
	add	%rbx, %rcx
	adc	%rbp, %rax
	adc	%r13, %rdx
	add	%rcx, %r14
	adc	%r13, %r12
	mov	%r14, -8(%rdi)
	add	%r12, %rax
	mov	%rax, (%rdi)
	adc	%r13, %rdx
	mov	%rdx, 8(%rdi)

	lea	16(%rdi), %rdi
	add	$2, %r8
	cmp	$-2, %r8
	jnz	Lam2o2

Lcor2:pop	%r8
	mov	-24(%rsi), %r10
	mov	%rax, %rbp
	mov	%rdx, %rbx
	mov	-16(%rsi), %rax
	mov	%rax, %r11
	mul	%r10
	mov	%rax, %r12
	mov	%rdx, %r14
	mov	-8(%rsi), %rax
	mul	%r10
	add	%rbp, %r12
	mov	%r12, -16(%rdi)
	mov	%rdx, %r12
	adc	%rax, %r14
	mov	-8(%rsi), %rax
	adc	$0, %r12
	mul	%r11
	add	%rbx, %r14
	adc	$0, %r12
	mov	%r14, -8(%rdi)
	add	%r12, %rax
	mov	%rax, (%rdi)
	adc	$0, %rdx
	mov	%rdx, 8(%rdi)
	lea	8(%rdi), %rdi

Lsqr_diag_addlsh1:
	mov	-8(%rsi,%r8,8), %rax
	shl	%r8
	xor	%ebx, %ebx
	mul	%rax
	mov	8(%rdi,%r8,8), %r11
	lea	(%rdx), %r10
	mov	16(%rdi,%r8,8), %r9
	add	%r11, %r11
	jmp	Ldm

	.align	16, 0x90
Ldtop:mul	%rax
	add	%r11, %r10
	mov	8(%rdi,%r8,8), %r11
	mov	%r10, -8(%rdi,%r8,8)
	adc	%r9, %rax
	lea	(%rdx,%rbx), %r10
	mov	16(%rdi,%r8,8), %r9
	adc	%r11, %r11
Ldm:	mov	%rax, (%rdi,%r8,8)
	mov	(%rsi,%r8,4), %rax
	adc	%r9, %r9
	setc	%bl
	add	$2, %r8
	js	Ldtop

	mul	%rax
	add	%r11, %r10
	mov	%r10, -8(%rdi)
	adc	%r9, %rax
	lea	(%rdx,%rbx), %r10
	mov	%rax, (%rdi)
	adc	$0, %r10
	mov	%r10, 8(%rdi)

	pop	%r14
	pop	%r13
	pop	%r12
	pop	%rbp
	pop	%rbx
	pop	%rsi
	pop	%rdi
	ret

	.align	16, 0x90
Lsmall:
	mov	(%rsi), %rax
	cmp	$2, %rdx
	jae	Lgt1
Ln1:
	mul	%rax
	mov	%rax, (%rdi)
	mov	%rdx, 8(%rdi)
	pop	%rsi
	pop	%rdi
	ret

Lgt1:	jne	Lgt2
Ln2:	mov	%rax, %r8
	mul	%rax
	mov	8(%rsi), %r11
	mov	%rax, (%rdi)
	mov	%r11, %rax
	mov	%rdx, %r9
	mul	%rax
	mov	%rax, %r10
	mov	%r11, %rax
	mov	%rdx, %r11
	mul	%r8
	xor	%r8, %r8
	add	%rax, %r9
	adc	%rdx, %r10
	adc	%r8, %r11
	add	%rax, %r9
	mov	%r9, 8(%rdi)
	adc	%rdx, %r10
	mov	%r10, 16(%rdi)
	adc	%r8, %r11
	mov	%r11, 24(%rdi)
	pop	%rsi
	pop	%rdi
	ret

Lgt2:
Ln3:	mov	%rax, %r10
	mul	%rax
	mov	8(%rsi), %r11
	mov	%rax, (%rdi)
	mov	%r11, %rax
	mov	%rdx, 8(%rdi)
	mul	%rax
	mov	16(%rsi), %rcx
	mov	%rax, 16(%rdi)
	mov	%rcx, %rax
	mov	%rdx, 24(%rdi)
	mul	%rax
	mov	%rax, 32(%rdi)
	mov	%rdx, 40(%rdi)

	mov	%r11, %rax
	mul	%r10
	mov	%rax, %r8
	mov	%rcx, %rax
	mov	%rdx, %r9
	mul	%r10
	xor	%r10, %r10
	add	%rax, %r9
	mov	%r11, %rax
	mov	%r10, %r11
	adc	%rdx, %r10

	mul	%rcx
	add	%rax, %r10
	adc	%r11, %rdx
	add	%r8, %r8
	adc	%r9, %r9
	adc	%r10, %r10
	adc	%rdx, %rdx
	adc	%r11, %r11
	add	%r8, 8(%rdi)
	adc	%r9, 16(%rdi)
	adc	%r10, 24(%rdi)
	adc	%rdx, 32(%rdi)
	adc	%r11, 40(%rdi)
	pop	%rsi
	pop	%rdi
	ret
	
