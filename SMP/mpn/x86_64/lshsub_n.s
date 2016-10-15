






































































	.text
	.align	16, 0x90
	.globl	mpn_lshsub_n
	
mpn_lshsub_n:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	mov	56(%rsp), %r8d	

	push	%r12
	push	%r13
	push	%r14
	push	%r15
	push	%rbx

	mov	%rcx, %rax
	xor	%ebx, %ebx	
	mov	%r8d, %ecx	
	xor	%r15d, %r15d	

	mov	%eax, %r11d
	and	$3, %r11d
	je	L4
	sub	$1, %r11d

Loopette:
	add	%ebx, %ebx	
	mov	0(%rsi), %r8
	lea	8(%rsi), %rsi
	sbb	0(%rdx), %r8
	mov	%r8, %r12
	sbb	%ebx, %ebx	
	shl	%cl, %r8
	or	%r15, %r8
	mov	%r12, %r15
	lea	8(%rdx), %rdx
	neg	%cl
	shr	%cl, %r15
	neg	%cl
	mov	%r8, 0(%rdi)
	lea	8(%rdi), %rdi
	sub	$1, %r11d
	jnc	Loopette

L4:
	sub	$4, %rax
	jc	Lend

	.align	16, 0x90
Loop:
	add	%ebx, %ebx	

	mov	0(%rsi), %r8
	mov	8(%rsi), %r9
	mov	16(%rsi), %r10
	mov	24(%rsi), %r11

	lea	32(%rsi), %rsi

	sbb	0(%rdx), %r8
	mov	%r8, %r12
	sbb	8(%rdx), %r9
	mov	%r9, %r13
	sbb	16(%rdx), %r10
	mov	%r10, %r14
	sbb	24(%rdx), %r11

	sbb	%ebx, %ebx	

	shl	%cl, %r8
	shl	%cl, %r9
	shl	%cl, %r10
	or	%r15, %r8
	mov	%r11, %r15
	shl	%cl, %r11

	lea	32(%rdx), %rdx

	neg	%cl

	shr	%cl, %r12
	shr	%cl, %r13
	shr	%cl, %r14
	shr	%cl, %r15		

	or	%r12, %r9
	or	%r13, %r10
	or	%r14, %r11

	neg	%cl

	mov	%r8, 0(%rdi)
	mov	%r9, 8(%rdi)
	mov	%r10, 16(%rdi)
	mov	%r11, 24(%rdi)

	lea	32(%rdi), %rdi

	sub	$4, %rax
	jnc	Loop
Lend:
	neg	%ebx
	shl	%cl, %rbx
	adc	%r15, %rbx
	mov	%rbx, %rax
	pop	%rbx
	pop	%r15
	pop	%r14
	pop	%r13
	pop	%r12

	pop	%rsi
	pop	%rdi
	ret
	
