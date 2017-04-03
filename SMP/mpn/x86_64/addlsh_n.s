






























































			


	
	
	
	
	










	.text
	.align	32, 0x90
	.globl	__gmpn_addlsh_n
	
__gmpn_addlsh_n:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	mov	56(%rsp), %r8d		
	push	%rbx
	xor	%ebx, %ebx	
Lent:	push	%rbp
	mov	%ecx, %ebp
	mov	%rcx, %rax
	mov	%r8d, %ecx
	neg	%ecx
	and	$3, %ebp
	jz	Lb0
	lea	-32(%rdx,%rbp,8), %rdx
	lea	-32(%rsi,%rbp,8), %rsi
	lea	-32(%rdi,%rbp,8), %rdi
	cmp	$2, %ebp
	jc	Lb1
	jz	Lb2

Lb3:	xor	%r8, %r8
	mov	8(%rdx), %r9
	mov	16(%rdx), %r10
	shrd	%cl, %r9, %r8
	shrd	%cl, %r10, %r9
	mov	24(%rdx), %r11
	shrd	%cl, %r11, %r10
	sub	$3, %rax
	jz	L3
	add	%ebx, %ebx
	lea	32(%rdx), %rdx
	adc	8(%rsi), %r8
	adc	16(%rsi), %r9
	adc	24(%rsi), %r10
	lea	32(%rsi), %rsi
	jmp	Llo3
L3:	add	%ebx, %ebx
	lea	32(%rdx), %rdx
	adc	8(%rsi), %r8
	adc	16(%rsi), %r9
	adc	24(%rsi), %r10
	jmp	Lwd3

Lb0:	mov	(%rdx), %r8
	mov	8(%rdx), %r9
	xor	%ebp, %ebp
	jmp	Llo0

Lb1:	xor	%r10, %r10
	mov	24(%rdx), %r11
	shrd	%cl, %r11, %r10
	sub	$1, %rax
	jz	L1
	add	%ebx, %ebx
	lea	32(%rdx), %rdx
	adc	24(%rsi), %r10
	lea	32(%rsi), %rsi
	mov	(%rdx), %r8
	jmp	Llo1
L1:	add	%ebx, %ebx
	adc	24(%rsi), %r10
	jmp	Lwd1

Lb2:	xor	%r9, %r9
	mov	16(%rdx), %r10
	shrd	%cl, %r10, %r9
	mov	24(%rdx), %r11
	shrd	%cl, %r11, %r10
	sub	$2, %rax
	jz	L2
	add	%ebx, %ebx
	lea	32(%rdx), %rdx
	adc	16(%rsi), %r9
	adc	24(%rsi), %r10
	lea	32(%rsi), %rsi
	jmp	Llo2
L2:	add	%ebx, %ebx
	adc	16(%rsi), %r9
	adc	24(%rsi), %r10
	jmp	Lwd2

	.align	32, 0x90			
Ltop:	shrd	%cl, %r11, %r10
	add	%ebx, %ebx
	lea	32(%rdx), %rdx
	adc	(%rsi), %rbp
	adc	8(%rsi), %r8
	adc	16(%rsi), %r9
	adc	24(%rsi), %r10
	mov	%rbp, (%rdi)
	lea	32(%rsi), %rsi
Llo3:	mov	%r8, 8(%rdi)
Llo2:	mov	%r9, 16(%rdi)
	mov	(%rdx), %r8
Llo1:	mov	%r10, 24(%rdi)
	mov	8(%rdx), %r9
	mov	%r11, %rbp
	lea	32(%rdi), %rdi
	sbb	%ebx, %ebx
Llo0:	shrd	%cl, %r8, %rbp
	mov	16(%rdx), %r10
	shrd	%cl, %r9, %r8
	shrd	%cl, %r10, %r9
	mov	24(%rdx), %r11
	sub	$4, %rax
	jg	Ltop

	shrd	%cl, %r11, %r10
	add	%ebx, %ebx
	adc	(%rsi), %rbp
	adc	8(%rsi), %r8
	adc	16(%rsi), %r9
	adc	24(%rsi), %r10
	mov	%rbp, (%rdi)
Lwd3:	mov	%r8, 8(%rdi)
Lwd2:	mov	%r9, 16(%rdi)
Lwd1:	mov	%r10, 24(%rdi)
	adc	%eax, %eax	
	shr	%cl, %r11
	add	%r11, %rax

	pop	%rbp
	pop	%rbx
	pop	%rsi
	pop	%rdi
	ret
	
	.globl	__gmpn_addlsh_nc
	
__gmpn_addlsh_nc:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	mov	56(%rsp), %r8d		
	mov	64(%rsp), %r9		
	push	%rbx
	neg	%r9
	sbb	%ebx, %ebx	
	jmp	Lent
	
