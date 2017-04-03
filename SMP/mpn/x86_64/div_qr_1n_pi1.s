


































































	








	









	
	.text
	.align	16, 0x90
	.globl	__gmpn_div_qr_1n_pi1
	
__gmpn_div_qr_1n_pi1:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	mov	56(%rsp), %r8	
	mov	64(%rsp), %r9	
	dec	%rdx
	jnz	Lfirst

	
	
	lea	1(%rcx), %r10
	mov	%rcx, %rax
	mul	%r9
	mov	(%rsi), %r11
	add	%r11, %rax
	adc	%r10, %rdx
	mov	%rdx, %r10
	imul	%r8, %rdx
	sub	%rdx, %r11
	cmp	%r11, %rax
	lea	(%r11, %r8), %rax
	cmovnc	%r11, %rax
	sbb	$0, %r10
	cmp	%r8, %rax
	jc	Lsingle_div_done
	sub	%r8, %rax
	add	$1, %r10
Lsingle_div_done:
	mov	%r10, (%rdi)
	pop	%rsi
	pop	%rdi
	ret
Lfirst:
	
	push	%r15
	push	%r14
	push	%r13
	push	%r12
	push	%rbx
	push	%rbp

	mov	%r8, %rbp
	imul	%r9, %rbp
	neg	%rbp
	mov	%rbp, %rbx
	sub	%r8, %rbx

	
	push	%r8
	mov	%rdx, %r8	

	mov	%r9, %rax
	mul	%rcx
	mov	%rax, %r13
	add	%rcx, %rdx
	mov	%rdx, %r10

	mov	%rbp, %rax
	mul	%rcx
	mov	-8(%rsi, %r8, 8), %r11
	mov	(%rsi, %r8, 8), %rcx
	mov	%r10, (%rdi, %r8, 8)
	add	%rax, %r11
	adc	%rdx, %rcx
	sbb	%r12, %r12
	dec	%r8
	mov	%rcx, %rax
	jz	Lfinal

	.align	16, 0x90

	
	
Lloop:
	
	
	mov	%r9, %r14
	mov	%r12, %r15
	and	%r12, %r14
	neg	%r15
	mul	%r9
	add	%rdx, %r14
	adc	$0, %r15
	add	%r13, %r14
	mov	%rax, %r13
	mov	%rbp, %rax
	lea	(%rbx, %r11), %r10
	adc	$0, %r15

	
	mul	%rcx
	and	%rbp, %r12
	add	%r12, %r11
	cmovnc	%r11, %r10

	
	adc	%rcx, %r14
	mov	-8(%rsi, %r8, 8), %r11
	adc	%r15, 8(%rdi, %r8, 8)
	jc	Lq_incr
Lq_incr_done:
	add	%rax, %r11
	mov	%r10, %rax
	adc	%rdx, %rax
	mov	%r14, (%rdi, %r8, 8)
	sbb	%r12, %r12
	dec	%r8
	mov	%rax, %rcx
	jnz	Lloop

Lfinal:
	pop	%r8

	mov	%r12, %r14
	and	%r8, %r12
	sub	%r12, %rax
	neg	%r14

	mov	%rax, %rcx
	sub	%r8, %rax
	cmovc	%rcx, %rax
	sbb	$-1, %r14

	lea	1(%rax), %r10
	mul	%r9
	add	%r11, %rax
	adc	%r10, %rdx
	mov	%rdx, %r10
	imul	%r8, %rdx
	sub	%rdx, %r11
	cmp	%r11, %rax
	lea	(%r11, %r8), %rax
	cmovnc	%r11, %rax
	sbb	$0, %r10
	cmp	%r8, %rax
	jc	Ldiv_done
	sub	%r8, %rax
	add	$1, %r10
Ldiv_done:
	add	%r10, %r13
	mov	%r13, (%rdi)
	adc	%r14, 8(%rdi)
	jnc	Ldone
Lfinal_q_incr:
	addq	$1, 16(%rdi)
	lea	8(%rdi), %rdi
	jc	Lfinal_q_incr

Ldone:
	pop	%rbp
	pop	%rbx
	pop	%r12
	pop	%r13
	pop	%r14
	pop	%r15
	pop	%rsi
	pop	%rdi
	ret

Lq_incr:
	
	lea	16(%rdi, %r8, 8), %rcx
Lq_incr_loop:
	addq	$1, (%rcx)
	jnc	Lq_incr_done
	lea	8(%rcx), %rcx
	jmp	Lq_incr_loop
	
