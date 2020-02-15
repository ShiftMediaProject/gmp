




















































































	.text
	.align	64, 0x90
	.globl	__gmpn_com
	
__gmpn_com:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx


	cmp	$7, %rdx
	jbe	Lbc

	pcmpeqb	%xmm5, %xmm5		

	test	$8, %dil		
	jz	Lrp_aligned		

	mov	(%rsi), %r8
	lea	8(%rsi), %rsi
	not	%r8
	mov	%r8, (%rdi)
	lea	8(%rdi), %rdi
	dec	%rdx

Lrp_aligned:
	test	$8, %sil
	jnz	Luent

	jmp	Lam

	.align	16, 0x90
Latop:movaps	0(%rsi), %xmm0
	movaps	16(%rsi), %xmm1
	movaps	32(%rsi), %xmm2
	movaps	48(%rsi), %xmm3
	lea	64(%rsi), %rsi
	pxor	%xmm5, %xmm0
	pxor	%xmm5, %xmm1
	pxor	%xmm5, %xmm2
	pxor	%xmm5, %xmm3
	movaps	%xmm0, (%rdi)
	movaps	%xmm1, 16(%rdi)
	movaps	%xmm2, 32(%rdi)
	movaps	%xmm3, 48(%rdi)
	lea	64(%rdi), %rdi
Lam:	sub	$8, %rdx
	jnc	Latop

	test	$4, %dl
	jz	1f
	movaps	(%rsi), %xmm0
	movaps	16(%rsi), %xmm1
	lea	32(%rsi), %rsi
	pxor	%xmm5, %xmm0
	pxor	%xmm5, %xmm1
	movaps	%xmm0, (%rdi)
	movaps	%xmm1, 16(%rdi)
	lea	32(%rdi), %rdi

1:	test	$2, %dl
	jz	1f
	movaps	(%rsi), %xmm0
	lea	16(%rsi), %rsi
	pxor	%xmm5, %xmm0
	movaps	%xmm0, (%rdi)
	lea	16(%rdi), %rdi

1:	test	$1, %dl
	jz	1f
	mov	(%rsi), %r8
	not	%r8
	mov	%r8, (%rdi)

1:	pop	%rsi
	pop	%rdi
	ret

Luent:




	lea	-40(%rsi), %rax		
	sub	%rdi, %rax
	cmp	$80, %rax		
	jbe	Lbc			

	sub	$16, %rdx
	jc	Luend

	movaps	120(%rsi), %xmm3

	sub	$16, %rdx
	jmp	Lum

	.align	16, 0x90
Lutop:movaps	120(%rsi), %xmm3
	pxor	%xmm5, %xmm0
	movaps	%xmm0, -128(%rdi)
	sub	$16, %rdx
Lum:	movaps	104(%rsi), %xmm2
	.byte	0x66,0x0f,0x3a,0x0f,218,8
	movaps	88(%rsi), %xmm1
	pxor	%xmm5, %xmm3
	movaps	%xmm3, 112(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,209,8
	movaps	72(%rsi), %xmm0
	pxor	%xmm5, %xmm2
	movaps	%xmm2, 96(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,200,8
	movaps	56(%rsi), %xmm3
	pxor	%xmm5, %xmm1
	movaps	%xmm1, 80(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,195,8
	movaps	40(%rsi), %xmm2
	pxor	%xmm5, %xmm0
	movaps	%xmm0, 64(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,218,8
	movaps	24(%rsi), %xmm1
	pxor	%xmm5, %xmm3
	movaps	%xmm3, 48(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,209,8
	movaps	8(%rsi), %xmm0
	pxor	%xmm5, %xmm2
	movaps	%xmm2, 32(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,200,8
	movaps	-8(%rsi), %xmm3
	pxor	%xmm5, %xmm1
	movaps	%xmm1, 16(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,195,8
	lea	128(%rsi), %rsi
	lea	128(%rdi), %rdi
	jnc	Lutop

	pxor	%xmm5, %xmm0
	movaps	%xmm0, -128(%rdi)

Luend:test	$8, %dl
	jz	1f
	movaps	56(%rsi), %xmm3
	movaps	40(%rsi), %xmm2
	.byte	0x66,0x0f,0x3a,0x0f,218,8
	movaps	24(%rsi), %xmm1
	pxor	%xmm5, %xmm3
	movaps	%xmm3, 48(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,209,8
	movaps	8(%rsi), %xmm0
	pxor	%xmm5, %xmm2
	movaps	%xmm2, 32(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,200,8
	movaps	-8(%rsi), %xmm3
	pxor	%xmm5, %xmm1
	movaps	%xmm1, 16(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,195,8
	lea	64(%rsi), %rsi
	pxor	%xmm5, %xmm0
	movaps	%xmm0, (%rdi)
	lea	64(%rdi), %rdi

1:	test	$4, %dl
	jz	1f
	movaps	24(%rsi), %xmm1
	movaps	8(%rsi), %xmm0
	.byte	0x66,0x0f,0x3a,0x0f,200,8
	movaps	-8(%rsi), %xmm3
	pxor	%xmm5, %xmm1
	movaps	%xmm1, 16(%rdi)
	.byte	0x66,0x0f,0x3a,0x0f,195,8
	lea	32(%rsi), %rsi
	pxor	%xmm5, %xmm0
	movaps	%xmm0, (%rdi)
	lea	32(%rdi), %rdi

1:	test	$2, %dl
	jz	1f
	movaps	8(%rsi), %xmm0
	movaps	-8(%rsi), %xmm3
	.byte	0x66,0x0f,0x3a,0x0f,195,8
	lea	16(%rsi), %rsi
	pxor	%xmm5, %xmm0
	movaps	%xmm0, (%rdi)
	lea	16(%rdi), %rdi

1:	test	$1, %dl
	jz	1f
	mov	(%rsi), %r8
	not	%r8
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
	not	%r8
	not	%r9
	not	%r10
	not	%r11
	mov	%r8, -24(%rdi)
	mov	%r9, -16(%rdi)
	sub	$4, %edx
	mov	%r10, -8(%rdi)
	mov	%r11, (%rdi)
	jnc	Ltop

Lend:	test	$1, %dl
	jz	1f
	mov	(%rsi), %r8
	not	%r8
	mov	%r8, 8(%rdi)
	lea	8(%rdi), %rdi
	lea	8(%rsi), %rsi
1:	test	$2, %dl
	jz	1f
	mov	(%rsi), %r8
	mov	8(%rsi), %r9
	not	%r8
	not	%r9
	mov	%r8, 8(%rdi)
	mov	%r9, 16(%rdi)
1:	pop	%rsi
	pop	%rdi
	ret
	
