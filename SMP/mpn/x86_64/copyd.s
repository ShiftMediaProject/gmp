
































































































	.text
	.align	64, 0x90
	.globl	__gmpn_copyd
	
	.def	__gmpn_copyd
	.scl	2
	.type	32
	.endef
__gmpn_copyd:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx


	lea	-8(%rsi,%rdx,8), %rsi
	lea	-8(%rdi,%rdx,8), %rdi

	cmp	$7, %rdx
	jbe	Lbc

	test	$8, %dil		
	jnz	Lrp_aligned		

	mov	(%rsi), %rax		
	mov	%rax, (%rdi)
	lea	-8(%rsi), %rsi
	lea	-8(%rdi), %rdi
	dec	%rdx

Lrp_aligned:
	test	$8, %sil
	jz	Luent

	jmp	Lam

	.align	16, 0x90
Latop:movaps	-8(%rsi), %xmm0
	movaps	-24(%rsi), %xmm1
	movaps	-40(%rsi), %xmm2
	movaps	-56(%rsi), %xmm3
	lea	-64(%rsi), %rsi
	movaps	%xmm0, -8(%rdi)
	movaps	%xmm1, -24(%rdi)
	movaps	%xmm2, -40(%rdi)
	movaps	%xmm3, -56(%rdi)
	lea	-64(%rdi), %rdi
Lam:	sub	$8, %rdx
	jnc	Latop

	test	$4, %dl
	jz	1f
	movaps	-8(%rsi), %xmm0
	movaps	-24(%rsi), %xmm1
	lea	-32(%rsi), %rsi
	movaps	%xmm0, -8(%rdi)
	movaps	%xmm1, -24(%rdi)
	lea	-32(%rdi), %rdi

1:	test	$2, %dl
	jz	1f
	movaps	-8(%rsi), %xmm0
	lea	-16(%rsi), %rsi
	movaps	%xmm0, -8(%rdi)
	lea	-16(%rdi), %rdi

1:	test	$1, %dl
	jz	1f
	mov	(%rsi), %r8
	mov	%r8, (%rdi)

1:	pop	%rsi
	pop	%rdi
	ret

Luent:sub	$16, %rdx
	movaps	(%rsi), %xmm0
	jc	Luend

	.align	16, 0x90
Lutop:sub	$16, %rdx
	movaps	-16(%rsi), %xmm1
	.byte	0x66,0x0f,0x3a,0x0f,193,8
	movaps	%xmm0, -8(%rdi)
	movaps	-32(%rsi), %xmm2
	.byte	0x66,0x0f,0x3a,0x0f,202,8
	movaps	%xmm1, -24(%rdi)
	movaps	-48(%rsi), %xmm3
	.byte	0x66,0x0f,0x3a,0x0f,211,8
	movaps	%xmm2, -40(%rdi)
	movaps	-64(%rsi), %xmm0
	.byte	0x66,0x0f,0x3a,0x0f,216,8
	movaps	%xmm3, -56(%rdi)
	movaps	-80(%rsi), %xmm1
	.byte	0x66,0x0f,0x3a,0x0f,193,8
	movaps	%xmm0, -72(%rdi)
	movaps	-96(%rsi), %xmm2
	.byte	0x66,0x0f,0x3a,0x0f,202,8
	movaps	%xmm1, -88(%rdi)
	movaps	-112(%rsi), %xmm3
	.byte	0x66,0x0f,0x3a,0x0f,211,8
	movaps	%xmm2, -104(%rdi)
	movaps	-128(%rsi), %xmm0
	.byte	0x66,0x0f,0x3a,0x0f,216,8
	movaps	%xmm3, -120(%rdi)
	lea	-128(%rsi), %rsi
	lea	-128(%rdi), %rdi
	jnc	Lutop

Luend:test	$8, %dl
	jz	1f
	movaps	-16(%rsi), %xmm1
	.byte	0x66,0x0f,0x3a,0x0f,193,8
	movaps	%xmm0, -8(%rdi)
	movaps	-32(%rsi), %xmm0
	.byte	0x66,0x0f,0x3a,0x0f,200,8
	movaps	%xmm1, -24(%rdi)
	movaps	-48(%rsi), %xmm1
	.byte	0x66,0x0f,0x3a,0x0f,193,8
	movaps	%xmm0, -40(%rdi)
	movaps	-64(%rsi), %xmm0
	.byte	0x66,0x0f,0x3a,0x0f,200,8
	movaps	%xmm1, -56(%rdi)
	lea	-64(%rsi), %rsi
	lea	-64(%rdi), %rdi

1:	test	$4, %dl
	jz	1f
	movaps	-16(%rsi), %xmm1
	.byte	0x66,0x0f,0x3a,0x0f,193,8
	movaps	%xmm0, -8(%rdi)
	movaps	-32(%rsi), %xmm0
	.byte	0x66,0x0f,0x3a,0x0f,200,8
	movaps	%xmm1, -24(%rdi)
	lea	-32(%rsi), %rsi
	lea	-32(%rdi), %rdi

1:	test	$2, %dl
	jz	1f
	movaps	-16(%rsi), %xmm1
	.byte	0x66,0x0f,0x3a,0x0f,193,8
	movaps	%xmm0, -8(%rdi)
	lea	-16(%rsi), %rsi
	lea	-16(%rdi), %rdi

1:	test	$1, %dl
	jz	1f
	mov	(%rsi), %r8
	mov	%r8, (%rdi)

1:	pop	%rsi
	pop	%rdi
	ret




Lbc:	sub	$4, %edx
	jc	Lend

	.align	16, 0x90
Ltop:	mov	(%rsi), %r8
	mov	-8(%rsi), %r9
	lea	-32(%rdi), %rdi
	mov	-16(%rsi), %r10
	mov	-24(%rsi), %r11
	lea	-32(%rsi), %rsi
	mov	%r8, 32(%rdi)
	mov	%r9, 24(%rdi)

	mov	%r10, 16(%rdi)
	mov	%r11, 8(%rdi)


Lend:	test	$1, %dl
	jz	1f
	mov	(%rsi), %r8
	mov	%r8, (%rdi)
	lea	-8(%rdi), %rdi
	lea	-8(%rsi), %rsi
1:	test	$2, %dl
	jz	1f
	mov	(%rsi), %r8
	mov	-8(%rsi), %r9
	mov	%r8, (%rdi)
	mov	%r9, -8(%rdi)
1:	pop	%rsi
	pop	%rdi
	ret
	

