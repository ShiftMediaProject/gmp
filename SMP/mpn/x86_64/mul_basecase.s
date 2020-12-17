





































































































	





























	.text
	.align	32, 0x90
	.globl	__gmpn_mul_basecase
	
	.def	__gmpn_mul_basecase
	.scl	2
	.type	32
	.endef
__gmpn_mul_basecase:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	mov	56(%rsp), %r8d	
	mov	(%rsi), %rax		
	push	%rbx
	push	%rbp
	push	%r12
	push	%r13
	push	%r14

	mov	(%rcx), %r10		

	xor	%r9, %r9
	sub	%rdx, %r9		

	lea	(%rsi,%rdx,8), %rsi
	lea	(%rdi,%rdx,8), %rdi

	mul	%r10			

	test	$1, %r8b
	jz	Lm2

	lea	8(%rcx), %r14		

	test	$1, %r9b
	jnz	Lm1x1

Lm1x0:test	$2, %r9b
	jnz	Lm1s2

Lm1s0:
	lea	(%r9), %r13
	mov	%rax, (%rdi,%r9,8)
	mov	8(%rsi,%r9,8), %rax
	mov	%rdx, %rbx		
	lea	Ldo_am0(%rip), %rbp
	jmp	Lm1e0

Lm1s2:
	lea	2(%r9), %r13
	mov	%rax, (%rdi,%r9,8)
	mov	8(%rsi,%r9,8), %rax
	mov	%rdx, %rbx		
	mul	%r10
	lea	Ldo_am2(%rip), %rbp
	test	%r13, %r13
	jnz	Lm1e2
	add	%rax, %rbx
	adc	$0, %rdx
	mov	%rbx, -8(%rdi)
	mov	%rdx, (%rdi)
	jmp	Lret2

Lm1x1:test	$2, %r9b
	jz	Lm1s3

Lm1s1:
	lea	1(%r9), %r13
	mov	%rax, (%rdi,%r9,8)
	test	%r13, %r13
	jz	L1
	mov	8(%rsi,%r9,8), %rax
	mov	%rdx, %rcx		
	lea	Ldo_am1(%rip), %rbp
	jmp	Lm1e1
L1:	mov	%rdx, (%rdi)
	jmp	Lret2

Lm1s3:
	lea	-1(%r9), %r13
	mov	%rax, (%rdi,%r9,8)
	mov	8(%rsi,%r9,8), %rax
	mov	%rdx, %rcx		
	lea	Ldo_am3(%rip), %rbp
	jmp	Lm1e3

	.align	16, 0x90
Lm1top:
	mul	%r10
	mov	%rcx, -16(%rdi,%r13,8)
Lm1e2:xor	%ecx, %ecx
	add	%rax, %rbx
	mov	(%rsi,%r13,8), %rax
	adc	%rdx, %rcx
	mov	%rbx, -8(%rdi,%r13,8)
Lm1e1:xor	%ebx, %ebx
	mul	%r10
	add	%rax, %rcx
	mov	8(%rsi,%r13,8), %rax
	adc	%rdx, %rbx
	mov	%rcx, (%rdi,%r13,8)
Lm1e0:xor	%ecx, %ecx
	mul	%r10
	add	%rax, %rbx
	mov	16(%rsi,%r13,8), %rax
	adc	%rdx, %rcx
	mov	%rbx, 8(%rdi,%r13,8)
Lm1e3:xor	%ebx, %ebx
	mul	%r10
	add	%rax, %rcx
	mov	24(%rsi,%r13,8), %rax
	adc	%rdx, %rbx
	add	$4, %r13
	js	Lm1top

	mul	%r10
	mov	%rcx, -16(%rdi)
	add	%rax, %rbx
	adc	$0, %rdx
	mov	%rbx, -8(%rdi)
	mov	%rdx, (%rdi)

	dec	%r8
	jz	Lret2
	lea	-8(%rdi), %rdi
	jmp	*%rbp

Lm2:
	mov	8(%rcx), %r11
	lea	16(%rcx), %r14	

	test	$1, %r9b
	jnz	Lbx1

Lbx0:	test	$2, %r9b
	jnz	Lb10

Lb00:	lea	(%r9), %r13
	mov	%rax, (%rdi,%r9,8)
	mov	%rdx, %rcx		
	mov	(%rsi,%r9,8), %rax
	mov	$0, %ebp
	jmp	Lm2e0

Lb10:	lea	-2(%r9), %r13
	mov	%rax, %rbp		
	mov	(%rsi,%r9,8), %rax
	mov	%rdx, %r12		
	mov	$0, %ebx
	jmp	Lm2e2

Lbx1:	test	$2, %r9b
	jz	Lb11

Lb01:	lea	1(%r9), %r13
	mov	%rax, (%rdi,%r9,8)
	mov	(%rsi,%r9,8), %rax
	mov	%rdx, %rbx		
	mov	$0, %ecx
	jmp	Lm2e1

Lb11:	lea	-1(%r9), %r13
	mov	%rax, %rcx		
	mov	(%rsi,%r9,8), %rax
	mov	%rdx, %rbp		
	mov	$0, %r12d
	jmp	Lm2e3

	.align	16, 0x90
Lm2top0:
	mul	%r10
	add	%rax, %r12
	mov	-8(%rsi,%r13,8), %rax
	mov	%r12, -8(%rdi,%r13,8)
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
	mul	%r11
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
	mul	%r11
	mov	$0, %ecx
	add	%rax, %r12
	mov	24(%rsi,%r13,8), %rax
	mov	%rbp, 16(%rdi,%r13,8)
	adc	%rdx, %rbx
	add	$4, %r13
	js	Lm2top0

	mul	%r10
	add	%rax, %r12
	mov	-8(%rsi), %rax
	mov	%r12, -8(%rdi)
	adc	%rdx, %rbx
	adc	%ecx, %ecx
	mul	%r11
	add	%rax, %rbx
	adc	%rdx, %rcx
	mov	%rbx, (%rdi)
	mov	%rcx, 8(%rdi)

	add	$-2, %r8
	jz	Lret2

Ldo_am0:
	push	%r15
	push	%r8

Lolo0:
	mov	(%r14), %r10
	mov	8(%r14), %r11
	lea	16(%r14), %r14
	lea	16(%rdi), %rdi
	mov	(%rsi,%r9,8), %rax

	mov	%r9, %r13
	mul	%r10
	mov	%rax, %r8
	mov	(%rsi,%r9,8), %rax
	mov	%rdx, %r15
	mul	%r11
	lea	(%rdx), %rbx
	mov	(%rdi,%r9,8), %rbp
	mov	%rax, %r12
	jmp	Llo0

	.align	16, 0x90
Lam2top0:
	mul	%r11
	add	%rbx, %rcx
	adc	%rax, %rbp
	mov	(%rsi,%r13,8), %rax
	lea	(%rdx), %r12
	adc	$0, %r12
	mul	%r10
	add	%rcx, %r15
	mov	%r15, -8(%rdi,%r13,8)
	adc	%rax, %r8
	mov	%rdx, %r15
	adc	$0, %r15
	mov	(%rsi,%r13,8), %rax
	mul	%r11
	lea	(%rdx), %rbx
	mov	(%rdi,%r13,8), %rcx
	add	%rcx, %rbp
	adc	%rax, %r12
	adc	$0, %rbx
Llo0:	mov	8(%rsi,%r13,8), %rax
	mul	%r10
	add	%rbp, %r8
	adc	%rax, %r15
	mov	%r8, (%rdi,%r13,8)
	mov	%rdx, %r8
	adc	$0, %r8
	mov	8(%rsi,%r13,8), %rax
	mov	8(%rdi,%r13,8), %rbp
	mul	%r11
	add	%rbp, %r12
	adc	%rax, %rbx
	lea	(%rdx), %rcx
	adc	$0, %rcx
	mov	16(%rsi,%r13,8), %rax
	mul	%r10
	add	%r12, %r15
	mov	%r15, 8(%rdi,%r13,8)
	adc	%rax, %r8
	mov	%rdx, %r15
	mov	16(%rdi,%r13,8), %r12
	adc	$0, %r15
	mov	16(%rsi,%r13,8), %rax
	mul	%r11
	add	%r12, %rbx
	lea	(%rdx), %rbp
	adc	%rax, %rcx
	mov	24(%rsi,%r13,8), %rax
	adc	$0, %rbp
	mul	%r10
	add	%rbx, %r8
	mov	%r8, 16(%rdi,%r13,8)
	mov	%rdx, %r8
	adc	%rax, %r15
	mov	24(%rsi,%r13,8), %rax
	mov	24(%rdi,%r13,8), %rbx
	adc	$0, %r8
	add	$4, %r13
	jnc	Lam2top0

	mul	%r11
	add	%rbx, %rcx
	adc	%rax, %rbp
	adc	%r13, %rdx
	add	%rcx, %r15
	adc	%r13, %r8
	mov	%r15, -8(%rdi)
	add	%rbp, %r8
	mov	%r8, (%rdi)
	adc	%r13, %rdx
	mov	%rdx, 8(%rdi)

	addl	$-2, (%rsp)
	jnz	Lolo0

Lret:	pop	%rax
	pop	%r15
Lret2:pop	%r14
	pop	%r13
	pop	%r12
	pop	%rbp
	pop	%rbx
	pop	%rsi
	pop	%rdi
	ret


	.align	16, 0x90
Lm2top1:
	mul	%r10
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
	mul	%r11
	add	%rax, %rcx
	adc	%rdx, %rbp
	mov	8(%rsi,%r13,8), %rax
	mul	%r10
	mov	$0, %r12d
	add	%rax, %rcx
	adc	%rdx, %rbp
	adc	$0, %r12d
	mov	8(%rsi,%r13,8), %rax
	mul	%r11
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
	mul	%r11
	mov	$0, %ecx
	add	%rax, %r12
	mov	24(%rsi,%r13,8), %rax
	mov	%rbp, 16(%rdi,%r13,8)
	adc	%rdx, %rbx
	add	$4, %r13
	js	Lm2top1

	mul	%r10
	add	%rax, %r12
	mov	-8(%rsi), %rax
	mov	%r12, -8(%rdi)
	adc	%rdx, %rbx
	adc	%ecx, %ecx
	mul	%r11
	add	%rax, %rbx
	adc	%rdx, %rcx
	mov	%rbx, (%rdi)
	mov	%rcx, 8(%rdi)

	add	$-2, %r8
	jz	Lret2

Ldo_am1:
	push	%r15
	push	%r8

Lolo1:
	mov	(%r14), %r10
	mov	8(%r14), %r11
	lea	16(%r14), %r14
	lea	16(%rdi), %rdi
	mov	(%rsi,%r9,8), %rax
	lea	1(%r9), %r13
	mul	%r10
	mov	%rax, %r15
	mov	%rdx, %r8
	mov	(%rsi,%r9,8), %rax
	mov	(%rdi,%r9,8), %rcx
	mul	%r11
	mov	%rax, %rbp
	mov	8(%rsi,%r9,8), %rax
	lea	(%rdx), %r12
	jmp	Llo1

	.align	16, 0x90
Lam2top1:
	mul	%r11
	add	%rbx, %rcx
	adc	%rax, %rbp
	mov	(%rsi,%r13,8), %rax
	lea	(%rdx), %r12
	adc	$0, %r12
Llo1:	mul	%r10
	add	%rcx, %r15
	mov	%r15, -8(%rdi,%r13,8)
	adc	%rax, %r8
	mov	%rdx, %r15
	adc	$0, %r15
	mov	(%rsi,%r13,8), %rax
	mul	%r11
	lea	(%rdx), %rbx
	mov	(%rdi,%r13,8), %rcx
	add	%rcx, %rbp
	adc	%rax, %r12
	adc	$0, %rbx
	mov	8(%rsi,%r13,8), %rax
	mul	%r10
	add	%rbp, %r8
	adc	%rax, %r15
	mov	%r8, (%rdi,%r13,8)
	mov	%rdx, %r8
	adc	$0, %r8
	mov	8(%rsi,%r13,8), %rax
	mov	8(%rdi,%r13,8), %rbp
	mul	%r11
	add	%rbp, %r12
	adc	%rax, %rbx
	lea	(%rdx), %rcx
	adc	$0, %rcx
	mov	16(%rsi,%r13,8), %rax
	mul	%r10
	add	%r12, %r15
	mov	%r15, 8(%rdi,%r13,8)
	adc	%rax, %r8
	mov	%rdx, %r15
	mov	16(%rdi,%r13,8), %r12
	adc	$0, %r15
	mov	16(%rsi,%r13,8), %rax
	mul	%r11
	add	%r12, %rbx
	lea	(%rdx), %rbp
	adc	%rax, %rcx
	mov	24(%rsi,%r13,8), %rax
	adc	$0, %rbp
	mul	%r10
	add	%rbx, %r8
	mov	%r8, 16(%rdi,%r13,8)
	mov	%rdx, %r8
	adc	%rax, %r15
	mov	24(%rsi,%r13,8), %rax
	mov	24(%rdi,%r13,8), %rbx
	adc	$0, %r8
	add	$4, %r13
	jnc	Lam2top1

	mul	%r11
	add	%rbx, %rcx
	adc	%rax, %rbp
	adc	%r13, %rdx
	add	%rcx, %r15
	adc	%r13, %r8
	mov	%r15, -8(%rdi)
	add	%rbp, %r8
	mov	%r8, (%rdi)
	adc	%r13, %rdx
	mov	%rdx, 8(%rdi)

	addl	$-2, (%rsp)
	jnz	Lolo1

	pop	%rax
	pop	%r15
	pop	%r14
	pop	%r13
	pop	%r12
	pop	%rbp
	pop	%rbx
	pop	%rsi
	pop	%rdi
	ret


	.align	16, 0x90
Lm2top2:
	mul	%r10
	add	%rax, %r12
	mov	-8(%rsi,%r13,8), %rax
	mov	%r12, -8(%rdi,%r13,8)
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
	mov	$0, %r12d
	add	%rax, %rcx
	adc	%rdx, %rbp
	adc	$0, %r12d
	mov	8(%rsi,%r13,8), %rax
	mul	%r11
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
	js	Lm2top2

	mul	%r10
	add	%rax, %r12
	mov	-8(%rsi), %rax
	mov	%r12, -8(%rdi)
	adc	%rdx, %rbx
	adc	%ecx, %ecx
	mul	%r11
	add	%rax, %rbx
	adc	%rdx, %rcx
	mov	%rbx, (%rdi)
	mov	%rcx, 8(%rdi)

	add	$-2, %r8
	jz	Lret2

Ldo_am2:
	push	%r15
	push	%r8

Lolo2:
	mov	(%r14), %r10
	mov	8(%r14), %r11
	lea	16(%r14), %r14
	lea	16(%rdi), %rdi
	mov	(%rsi,%r9,8), %rax
	lea	-2(%r9), %r13
	mul	%r10
	mov	%rax, %r8
	mov	%rdx, %r15
	mov	(%rsi,%r9,8), %rax
	mov	(%rdi,%r9,8), %rbx
	mul	%r11
	mov	%rax, %rcx
	lea	(%rdx), %rbp
	mov	8(%rsi,%r9,8), %rax
	jmp	Llo2

	.align	16, 0x90
Lam2top2:
	mul	%r11
	add	%rbx, %rcx
	adc	%rax, %rbp
	mov	(%rsi,%r13,8), %rax
	lea	(%rdx), %r12
	adc	$0, %r12
	mul	%r10
	add	%rcx, %r15
	mov	%r15, -8(%rdi,%r13,8)
	adc	%rax, %r8
	mov	%rdx, %r15
	adc	$0, %r15
	mov	(%rsi,%r13,8), %rax
	mul	%r11
	lea	(%rdx), %rbx
	mov	(%rdi,%r13,8), %rcx
	add	%rcx, %rbp
	adc	%rax, %r12
	adc	$0, %rbx
	mov	8(%rsi,%r13,8), %rax
	mul	%r10
	add	%rbp, %r8
	adc	%rax, %r15
	mov	%r8, (%rdi,%r13,8)
	mov	%rdx, %r8
	adc	$0, %r8
	mov	8(%rsi,%r13,8), %rax
	mov	8(%rdi,%r13,8), %rbp
	mul	%r11
	add	%rbp, %r12
	adc	%rax, %rbx
	lea	(%rdx), %rcx
	adc	$0, %rcx
	mov	16(%rsi,%r13,8), %rax
	mul	%r10
	add	%r12, %r15
	mov	%r15, 8(%rdi,%r13,8)
	adc	%rax, %r8
	mov	%rdx, %r15
	mov	16(%rdi,%r13,8), %r12
	adc	$0, %r15
	mov	16(%rsi,%r13,8), %rax
	mul	%r11
	add	%r12, %rbx
	lea	(%rdx), %rbp
	adc	%rax, %rcx
	mov	24(%rsi,%r13,8), %rax
	adc	$0, %rbp
Llo2:	mul	%r10
	add	%rbx, %r8
	mov	%r8, 16(%rdi,%r13,8)
	mov	%rdx, %r8
	adc	%rax, %r15
	mov	24(%rsi,%r13,8), %rax
	mov	24(%rdi,%r13,8), %rbx
	adc	$0, %r8
	add	$4, %r13
	jnc	Lam2top2

	mul	%r11
	add	%rbx, %rcx
	adc	%rax, %rbp
	adc	%r13, %rdx
	add	%rcx, %r15
	adc	%r13, %r8
	mov	%r15, -8(%rdi)
	add	%rbp, %r8
	mov	%r8, (%rdi)
	adc	%r13, %rdx
	mov	%rdx, 8(%rdi)

	addl	$-2, (%rsp)
	jnz	Lolo2

	pop	%rax
	pop	%r15
	pop	%r14
	pop	%r13
	pop	%r12
	pop	%rbp
	pop	%rbx
	pop	%rsi
	pop	%rdi
	ret


	.align	16, 0x90
Lm2top3:
	mul	%r10
	add	%rax, %r12
	mov	-8(%rsi,%r13,8), %rax
	mov	%r12, -8(%rdi,%r13,8)
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
	mul	%r11
	mov	$0, %ecx
	add	%rax, %r12
	mov	24(%rsi,%r13,8), %rax
	mov	%rbp, 16(%rdi,%r13,8)
	adc	%rdx, %rbx
	add	$4, %r13
	js	Lm2top3

	mul	%r10
	add	%rax, %r12
	mov	-8(%rsi), %rax
	mov	%r12, -8(%rdi)
	adc	%rdx, %rbx
	adc	$0, %ecx
	mul	%r11
	add	%rax, %rbx
	adc	%rdx, %rcx
	mov	%rbx, (%rdi)
	mov	%rcx, 8(%rdi)

	add	$-2, %r8
	jz	Lret2

Ldo_am3:
	push	%r15
	push	%r8

Lolo3:
	mov	(%r14), %r10
	mov	8(%r14), %r11
	lea	16(%r14), %r14
	lea	16(%rdi), %rdi
	mov	(%rsi,%r9,8), %rax
	lea	-1(%r9), %r13
	mul	%r10
	mov	%rax, %r15
	mov	%rdx, %r8
	mov	(%rsi,%r9,8), %rax
	mov	(%rdi,%r9,8), %r12
	mul	%r11
	mov	%rax, %rbx
	lea	(%rdx), %rcx
	mov	8(%rsi,%r9,8), %rax
	jmp	Llo3

	.align	16, 0x90
Lam2top3:
	mul	%r11
	add	%rbx, %rcx
	adc	%rax, %rbp
	mov	(%rsi,%r13,8), %rax
	lea	(%rdx), %r12
	adc	$0, %r12
	mul	%r10
	add	%rcx, %r15
	mov	%r15, -8(%rdi,%r13,8)
	adc	%rax, %r8
	mov	%rdx, %r15
	adc	$0, %r15
	mov	(%rsi,%r13,8), %rax
	mul	%r11
	lea	(%rdx), %rbx
	mov	(%rdi,%r13,8), %rcx
	add	%rcx, %rbp
	adc	%rax, %r12
	adc	$0, %rbx
	mov	8(%rsi,%r13,8), %rax
	mul	%r10
	add	%rbp, %r8
	adc	%rax, %r15
	mov	%r8, (%rdi,%r13,8)
	mov	%rdx, %r8
	adc	$0, %r8
	mov	8(%rsi,%r13,8), %rax
	mov	8(%rdi,%r13,8), %rbp
	mul	%r11
	add	%rbp, %r12
	adc	%rax, %rbx
	lea	(%rdx), %rcx
	adc	$0, %rcx
	mov	16(%rsi,%r13,8), %rax
Llo3:	mul	%r10
	add	%r12, %r15
	mov	%r15, 8(%rdi,%r13,8)
	adc	%rax, %r8
	mov	%rdx, %r15
	mov	16(%rdi,%r13,8), %r12
	adc	$0, %r15
	mov	16(%rsi,%r13,8), %rax
	mul	%r11
	add	%r12, %rbx
	lea	(%rdx), %rbp
	adc	%rax, %rcx
	mov	24(%rsi,%r13,8), %rax
	adc	$0, %rbp
	mul	%r10
	add	%rbx, %r8
	mov	%r8, 16(%rdi,%r13,8)
	mov	%rdx, %r8
	adc	%rax, %r15
	mov	24(%rsi,%r13,8), %rax
	mov	24(%rdi,%r13,8), %rbx
	adc	$0, %r8
	add	$4, %r13
	jnc	Lam2top3

	mul	%r11
	add	%rbx, %rcx
	adc	%rax, %rbp
	adc	%r13, %rdx
	add	%rcx, %r15
	adc	%r13, %r8
	mov	%r15, -8(%rdi)
	add	%rbp, %r8
	mov	%r8, (%rdi)
	adc	%r13, %rdx
	mov	%rdx, 8(%rdi)

	addl	$-2, (%rsp)
	jnz	Lolo3

	pop	%rax
	pop	%r15
	pop	%r14
	pop	%r13
	pop	%r12
	pop	%rbp
	pop	%rbx
	pop	%rsi
	pop	%rdi
	ret
	
