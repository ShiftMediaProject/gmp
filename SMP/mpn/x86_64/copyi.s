



































































































	.text
	.align	64, 0x90
	.globl	__gmpn_copyi
	
	.def	__gmpn_copyi
	.scl	2
	.type	32
	.endef
__gmpn_copyi:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx


	cmp	$7, %rdx
	jbe	Lbc

	test	$8, %dil		
	jz	Lrp_aligned		

	movsq				
	dec	%rdx

Lrp_aligned:
	test	$8, %sil
	jnz	Luent

	jmp	Lam

	.align	16, 0x90
Latop:movdqa	0(%rsi), %xmm0
	movdqa	16(%rsi), %xmm1
	movdqa	32(%rsi), %xmm2
	movdqa	48(%rsi), %xmm3
	lea	64(%rsi), %rsi
	movdqa	%xmm0, (%rdi)
	movdqa	%xmm1, 16(%rdi)
	movdqa	%xmm2, 32(%rdi)
	movdqa	%xmm3, 48(%rdi)
	lea	64(%rdi), %rdi
Lam:	sub	$8, %rdx
	jnc	Latop

	test	$4, %dl
	jz	1f
	movdqa	(%rsi), %xmm0
	movdqa	16(%rsi), %xmm1
	lea	32(%rsi), %rsi
	movdqa	%xmm0, (%rdi)
	movdqa	%xmm1, 16(%rdi)
	lea	32(%rdi), %rdi

1:	test	$2, %dl
	jz	1f
	movdqa	(%rsi), %xmm0
	lea	16(%rsi), %rsi
	movdqa	%xmm0, (%rdi)
	lea	16(%rdi), %rdi

1:	test	$1, %dl
	jz	1f
	mov	(%rsi), %r8
	mov	%r8, (%rdi)

1:	pop	%rsi
	pop	%rdi
	ret

Luent:


	cmp	$16, %rdx
	jc	Lued0

	add	$-56, %rsp	
	movdqa	%xmm6, (%rsp)	
	movdqa	%xmm7, 16(%rsp)	
	movdqa	%xmm8, 32(%rsp)	

	movaps	120(%rsi), %xmm7
	movaps	104(%rsi), %xmm6
	movaps	88(%rsi), %xmm5
	movaps	72(%rsi), %xmm4
	movaps	56(%rsi), %xmm3
	movaps	40(%rsi), %xmm2
	lea	128(%rsi), %rsi
	sub	$32, %rdx
	jc	Lued1

	.align	16, 0x90
Lutop:movaps	-104(%rsi), %xmm1
	sub	$16, %rdx
	movaps	-120(%rsi), %xmm0
	.byte	0x66,0x0f,0x3a,0x0f,254,8
	movaps	-136(%rsi), %xmm8
	movdqa	%xmm7, 112(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,245,8
	movaps	120(%rsi), %xmm7
	movdqa	%xmm6, 96(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,236,8
	movaps	104(%rsi), %xmm6
	movdqa	%xmm5, 80(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,227,8
	movaps	88(%rsi), %xmm5
	movdqa	%xmm4, 64(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,218,8
	movaps	72(%rsi), %xmm4
	movdqa	%xmm3, 48(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,209,8
	movaps	56(%rsi), %xmm3
	movdqa	%xmm2, 32(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,200,8
	movaps	40(%rsi), %xmm2
	movdqa	%xmm1, 16(%rdi)
	.byte	0x66,65,0x0f,0x3a,0x0f,192,8
	lea	128(%rsi), %rsi
	movdqa	%xmm0, (%rdi)
	lea	128(%rdi), %rdi
	jnc	Lutop

Lued1:movaps	-104(%rsi), %xmm1
	movaps	-120(%rsi), %xmm0
	movaps	-136(%rsi), %xmm8
	.byte	0x66,0x0f,0x3a,0x0f,254,8
	movdqa	%xmm7, 112(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,245,8
	movdqa	%xmm6, 96(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,236,8
	movdqa	%xmm5, 80(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,227,8
	movdqa	%xmm4, 64(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,218,8
	movdqa	%xmm3, 48(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,209,8
	movdqa	%xmm2, 32(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,200,8
	movdqa	%xmm1, 16(%rdi)
	.byte	0x66,65,0x0f,0x3a,0x0f,192,8
	movdqa	%xmm0, (%rdi)
	lea	128(%rdi), %rdi

	movdqa	(%rsp), %xmm6	
	movdqa	16(%rsp), %xmm7	
	movdqa	32(%rsp), %xmm8	
	add	$56, %rsp	

Lued0:test	$8, %dl
	jz	1f
	movaps	56(%rsi), %xmm3
	movaps	40(%rsi), %xmm2
	movaps	24(%rsi), %xmm1
	movaps	8(%rsi), %xmm0
	movaps	-8(%rsi), %xmm4
	.byte	0x66,0x0f,0x3a,0x0f,218,8
	movdqa	%xmm3, 48(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,209,8
	movdqa	%xmm2, 32(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,200,8
	movdqa	%xmm1, 16(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,196,8
	lea	64(%rsi), %rsi
	movdqa	%xmm0, (%rdi)
	lea	64(%rdi), %rdi

1:	test	$4, %dl
	jz	1f
	movaps	24(%rsi), %xmm1
	movaps	8(%rsi), %xmm0
	.byte	0x66,0x0f,0x3a,0x0f,200,8
	movaps	-8(%rsi), %xmm3
	movdqa	%xmm1, 16(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,195,8
	lea	32(%rsi), %rsi
	movdqa	%xmm0, (%rdi)
	lea	32(%rdi), %rdi

1:	test	$2, %dl
	jz	1f
	movdqa	8(%rsi), %xmm0
	movdqa	-8(%rsi), %xmm3
	.byte	0x66,0x0f,0x3a,0x0f,195,8
	lea	16(%rsi), %rsi
	movdqa	%xmm0, (%rdi)
	lea	16(%rdi), %rdi

1:	test	$1, %dl
	jz	1f
	mov	(%rsi), %r8
	mov	%r8, (%rdi)

1:	pop	%rsi
	pop	%rdi
	ret




Lbc:	lea	-8(%rdi), %rdi
	sub	$4, %edx
	jc	Lend

	.align	16, 0x90
Ltop:	mov	(%rsi), %r8
	mov	8(%rsi), %r9
	lea	32(%rdi), %rdi
	mov	16(%rsi), %r10
	mov	24(%rsi), %r11
	lea	32(%rsi), %rsi
	mov	%r8, -24(%rdi)
	mov	%r9, -16(%rdi)

	mov	%r10, -8(%rdi)
	mov	%r11, (%rdi)


Lend:	test	$1, %dl
	jz	1f
	mov	(%rsi), %r8
	mov	%r8, 8(%rdi)
	lea	8(%rdi), %rdi
	lea	8(%rsi), %rsi
1:	test	$2, %dl
	jz	1f
	mov	(%rsi), %r8
	mov	8(%rsi), %r9
	mov	%r8, 8(%rdi)
	mov	%r9, 16(%rdi)
1:	pop	%rsi
	pop	%rdi
	ret
	

