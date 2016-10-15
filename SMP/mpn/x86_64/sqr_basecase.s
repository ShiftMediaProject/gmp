


























































































	.text
	.align	32, 0x90
	.globl	__gmpn_sqr_basecase
	
__gmpn_sqr_basecase:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx


	cmp	$2, %rdx
	jae	Lgt1

	mov	(%rsi), %rax
	mul	%rax
	mov	%rax, (%rdi)
	mov	%rdx, 8(%rdi)
	pop	%rsi
	pop	%rdi
	ret

Lgt1:	jne	Lgt2

	mov	(%rsi), %rax
	mov	%rax, %r8
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

Lgt2:	cmp	$4, %rdx
	jae	Lgt3





	mov	(%rsi), %rax
	mov	%rax, %r10
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

Lgt3:













Ldo_mul_2:
	mov	(%rsi), %r8
	push	%rbx
	lea	(%rdi,%rdx,8), %rdi	
	mov	8(%rsi), %rax
	push	%rbp
	lea	(%rsi,%rdx,8), %rsi	
	mov	%rax, %r9
	push	%r12
	mov	$1, %r12d		
	push	%r13
	sub	%rdx, %r12
	push	%r14
	push	%r12
	mul	%r8
	mov	%rax, (%rdi,%r12,8)
	mov	8(%rsi,%r12,8), %rax
	test	$1, %r12b
	jnz	Lm2b1

Lm2b0:lea	2(%r12), %rcx
	xor	%r11d, %r11d	
	xor	%ebx, %ebx	
	mov	%rdx, %r10
	jmp	Lm2l0

Lm2b1:lea	1(%r12), %rcx
	xor	%ebp, %ebp	
	xor	%r10d, %r10d	
	mov	%rdx, %rbx
	jmp	Lm2l1

	.align	32, 0x90
Lm2tp:
Lm2l0:mul	%r8
	add	%rax, %r10
	mov	%rdx, %rbp
	adc	$0, %rbp
	mov	-8(%rsi,%rcx,8), %rax
	mul	%r9
	add	%r11, %r10
	adc	$0, %rbp
	add	%rax, %rbx
	mov	%r10, -8(%rdi,%rcx,8)
	mov	%rdx, %r10
	adc	$0, %r10
	mov	(%rsi,%rcx,8), %rax
Lm2l1:mul	%r8
	add	%rax, %rbx
	mov	%rdx, %r11
	adc	$0, %r11
	add	%rbp, %rbx
	mov	(%rsi,%rcx,8), %rax
	adc	$0, %r11
	mul	%r9
	mov	%rbx, (%rdi,%rcx,8)
	add	%rax, %r10
	mov	%rdx, %rbx
	mov	8(%rsi,%rcx,8), %rax
	adc	$0, %rbx
	add	$2, %rcx
	jnc	Lm2tp

Lm2ed:mul	%r8
	add	%rax, %r10
	mov	%rdx, %rbp
	adc	$0, %rbp
	mov	-8(%rsi), %rax
	mul	%r9
	add	%r11, %r10
	adc	$0, %rbp
	add	%rax, %rbx
	mov	%r10, -8(%rdi)
	adc	$0, %rdx
	add	%rbp, %rbx
	mov	%rbx, (%rdi)
	adc	$0, %rdx
	mov	%rdx, 8(%rdi)

	add	$2, %r12			

Ldo_addmul_2:
Louter:
	lea	16(%rdi), %rdi
	cmp	$-2, %r12d		
	jge	Lcorner		

	mov	-8(%rsi,%r12,8), %r8
	mov	(%rsi,%r12,8), %rax
	mov	%rax, %r9
	mul	%r8
	test	$1, %r12b
	jnz	La1x1

La1x0:mov	(%rdi,%r12,8), %r13
	xor	%r10, %r10
	mov	8(%rdi,%r12,8), %r14
	add	%rax, %r13
	mov	%rdx, %r11
	adc	$0, %r11
	xor	%rbx, %rbx
	mov	%r13, (%rdi,%r12,8)
	mov	8(%rsi,%r12,8), %rax
	test	$2, %r12b
	jnz	La110

La100:lea	2(%r12), %rcx		
	jmp	Llo0

La110:lea	(%r12), %rcx			
	jmp	Llo2

La1x1:mov	(%rdi,%r12,8), %r14
	xor	%rbx, %rbx
	mov	8(%rdi,%r12,8), %r13
	add	%rax, %r14
	mov	%rdx, %rbp
	adc	$0, %rbp
	xor	%r10, %r10
	mov	8(%rsi,%r12,8), %rax
	test	$2, %r12b
	jz	La111

La101:lea	3(%r12), %rcx		
	jmp	Llo1

La111:lea	1(%r12), %rcx		
	jmp	Llo3

	.align	32, 0x90
Ltop:	mul	%r9
	mov	%rdx, %r10
	add	%rax, %r13
	adc	$0, %r10
	add	%r11, %r14
	adc	$0, %rbp
	add	%rbx, %r13
	adc	$0, %r10
	mov	-16(%rsi,%rcx,8), %rax
Llo1:	mul	%r8
	add	%rax, %r13
	mov	%rdx, %r11
	adc	$0, %r11
	mov	-16(%rsi,%rcx,8), %rax
	mul	%r9
	mov	%r14, -24(%rdi,%rcx,8)
	mov	-8(%rdi,%rcx,8), %r14
	add	%rbp, %r13
	adc	$0, %r11
	mov	%rdx, %rbx
	mov	%r13, -16(%rdi,%rcx,8)
	add	%rax, %r14
	adc	$0, %rbx
	mov	-8(%rsi,%rcx,8), %rax
	add	%r10, %r14
	adc	$0, %rbx
Llo0:	mul	%r8
	add	%rax, %r14
	mov	%rdx, %rbp
	adc	$0, %rbp
	mov	-8(%rsi,%rcx,8), %rax
	mul	%r9
	add	%r11, %r14
	mov	(%rdi,%rcx,8), %r13
	adc	$0, %rbp
	mov	%rdx, %r10
	add	%rax, %r13
	adc	$0, %r10
	mov	(%rsi,%rcx,8), %rax
Llo3:	mul	%r8
	add	%rbx, %r13
	mov	%r14, -8(%rdi,%rcx,8)
	mov	%rdx, %r11
	adc	$0, %r10
	add	%rax, %r13
	adc	$0, %r11
	mov	(%rsi,%rcx,8), %rax
	add	%rbp, %r13
	adc	$0, %r11
	mul	%r9
	mov	8(%rdi,%rcx,8), %r14
	add	%rax, %r14
	mov	%rdx, %rbx
	adc	$0, %rbx
	mov	8(%rsi,%rcx,8), %rax
	mov	%r13, (%rdi,%rcx,8)
Llo2:	mul	%r8
	add	%r10, %r14
	mov	%rdx, %rbp
	adc	$0, %rbx
	add	%rax, %r14
	mov	8(%rsi,%rcx,8), %rax
	mov	16(%rdi,%rcx,8), %r13
	adc	$0, %rbp
	add	$4, %rcx
	jnc	Ltop

Lend:	mul	%r9
	add	%r11, %r14
	adc	$0, %rbp
	add	%rbx, %rax
	adc	$0, %rdx
	mov	%r14, -8(%rdi)
	add	%rbp, %rax
	adc	$0, %rdx
	mov	%rax, (%rdi)
	mov	%rdx, 8(%rdi)

	add	$2, %r12			
	jmp	Louter		

Lcorner:
	pop	%rcx
	jg	Lsmall_corner

	lea	8(%rdi), %rdi
	mov	-24(%rsi), %r8
	mov	-16(%rsi), %rax
	mov	%rax, %r9
	mul	%r8
	mov	-24(%rdi), %r13
	mov	-16(%rdi), %r14
	add	%rax, %r13
	mov	%rdx, %r11
	adc	$0, %r11
	xor	%rbx, %rbx
	mov	%r13, -24(%rdi)
	mov	-8(%rsi), %rax
	mul	%r8
	add	$0, %r14
	mov	%rdx, %rbp
	adc	$0, %rbx
	add	%rax, %r14
	mov	-8(%rsi), %rax
	adc	$0, %rbp
	mul	%r9
	add	%r11, %r14
	adc	$0, %rbp
	add	%rbx, %rax
	adc	$0, %rdx
	mov	%r14, -16(%rdi)
	jmp	Lcom

Lsmall_corner:
	mov	-8(%rdi), %rbp
	mov	-16(%rsi), %r8
	mov	-8(%rsi), %rax
	mul	%r8
Lcom:	add	%rbp, %rax
	adc	$0, %rdx
	mov	%rax, -8(%rdi)
	mov	%rdx, (%rdi)

Lsqr_diag_addlsh1:
	mov	-8(%rsi,%rcx,8), %rax
	shl	%rcx
	mul	%rax
	mov	%rax, (%rdi,%rcx,8)

	xor	%ebx, %ebx
	mov	8(%rdi,%rcx,8), %r8
	mov	16(%rdi,%rcx,8), %r9
	jmp	Ldm

	.align	32, 0x90
Ldtop:add	%r8, %r10
	adc	%r9, %rax
	mov	8(%rdi,%rcx,8), %r8
	mov	16(%rdi,%rcx,8), %r9
	mov	%r10, -8(%rdi,%rcx,8)
	mov	%rax, (%rdi,%rcx,8)
Ldm:	adc	%r8, %r8
	adc	%r9, %r9
	mov	(%rsi,%rcx,4), %rax
	lea	(%rdx,%rbx), %r10
	setc	%bl
	mul	%rax
	add	$2, %rcx
	js	Ldtop

Ldend:add	%r8, %r10
	adc	%r9, %rax
	mov	%r10, -8(%rdi)
	mov	%rax, (%rdi)
	adc	%rbx, %rdx
	mov	%rdx, 8(%rdi)

	pop	%r14
	pop	%r13
	pop	%r12
	pop	%rbp
	pop	%rbx
	pop	%rsi
	pop	%rdi
	ret
	
