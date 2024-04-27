



















































































	.text
	.align	16, 0x90
	.globl	__gmpn_com
	
	.def	__gmpn_com
	.scl	2
	.type	32
	.endef
__gmpn_com:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx


	add	$-56, %rsp	
	movdqa	%xmm6, (%rsp)	
	movdqa	%xmm7, 16(%rsp)	

	pcmpeqb	%xmm7, %xmm7		

	test	$8, %dil		
	jz	Lali			
	mov	(%rsi), %rax
	lea	8(%rsi), %rsi
	not	%rax
	mov	%rax, (%rdi)
	lea	8(%rdi), %rdi
	dec	%rdx

	sub	$14, %rdx
	jc	Lsma

	.align	16, 0x90
Ltop:	movdqu	(%rsi), %xmm0
	movdqu	16(%rsi), %xmm1
	movdqu	32(%rsi), %xmm2
	movdqu	48(%rsi), %xmm3
	movdqu	64(%rsi), %xmm4
	movdqu	80(%rsi), %xmm5
	movdqu	96(%rsi), %xmm6
	lea	112(%rsi), %rsi
	pxor	%xmm7, %xmm0
	pxor	%xmm7, %xmm1
	pxor	%xmm7, %xmm2
	pxor	%xmm7, %xmm3
	pxor	%xmm7, %xmm4
	pxor	%xmm7, %xmm5
	pxor	%xmm7, %xmm6
	movdqa	%xmm0, (%rdi)
	movdqa	%xmm1, 16(%rdi)
	movdqa	%xmm2, 32(%rdi)
	movdqa	%xmm3, 48(%rdi)
	movdqa	%xmm4, 64(%rdi)
	movdqa	%xmm5, 80(%rdi)
	movdqa	%xmm6, 96(%rdi)
	lea	112(%rdi), %rdi
Lali:	sub	$14, %rdx
	jnc	Ltop

Lsma:	add	$14, %rdx
	test	$8, %dl
	jz	1f
	movdqu	(%rsi), %xmm0
	movdqu	16(%rsi), %xmm1
	movdqu	32(%rsi), %xmm2
	movdqu	48(%rsi), %xmm3
	lea	64(%rsi), %rsi
	pxor	%xmm7, %xmm0
	pxor	%xmm7, %xmm1
	pxor	%xmm7, %xmm2
	pxor	%xmm7, %xmm3
	movdqa	%xmm0, (%rdi)
	movdqa	%xmm1, 16(%rdi)
	movdqa	%xmm2, 32(%rdi)
	movdqa	%xmm3, 48(%rdi)
	lea	64(%rdi), %rdi
1:
	test	$4, %dl
	jz	1f
	movdqu	(%rsi), %xmm0
	movdqu	16(%rsi), %xmm1
	lea	32(%rsi), %rsi
	pxor	%xmm7, %xmm0
	pxor	%xmm7, %xmm1
	movdqa	%xmm0, (%rdi)
	movdqa	%xmm1, 16(%rdi)
	lea	32(%rdi), %rdi
1:
	test	$2, %dl
	jz	1f
	movdqu	(%rsi), %xmm0
	lea	16(%rsi), %rsi
	pxor	%xmm7, %xmm0
	movdqa	%xmm0, (%rdi)
	lea	16(%rdi), %rdi
1:
	test	$1, %dl
	jz	1f
	mov	(%rsi), %rax
	not	%rax
	mov	%rax, (%rdi)
1:
Ldon:
	movdqa	(%rsp), %xmm6	
	movdqa	16(%rsp), %xmm7	
	add	$56, %rsp	
	pop	%rsi
	pop	%rdi
	ret
	
