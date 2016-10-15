
























































































	.text
	.align	64, 0x90
	.globl	__gmpn_rshift
	
__gmpn_rshift:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	movd	%ecx, %xmm4
	mov	$64, %eax
	sub	%ecx, %eax
	movd	%eax, %xmm5

	neg	%ecx
	mov	(%rsi), %rax
	shl	%cl, %rax

	cmp	$3, %rdx
	jle	Lbc

	test	$8, %dil
	jz	Lrp_aligned


	movq	(%rsi), %xmm0
	movq	8(%rsi), %xmm1
	psrlq	%xmm4, %xmm0
	psllq	%xmm5, %xmm1
	por	%xmm1, %xmm0
	movq	%xmm0, (%rdi)
	lea	8(%rsi), %rsi
	lea	8(%rdi), %rdi
	dec	%rdx

Lrp_aligned:
	lea	1(%rdx), %r8d
	lea	(%rsi,%rdx,8), %rsi
	lea	(%rdi,%rdx,8), %rdi
	neg	%rdx

	and	$6, %r8d
	jz	Lbu0
	cmp	$4, %r8d
	jz	Lbu4
	jc	Lbu2
Lbu6:	add	$4, %rdx
	jmp	Li56
Lbu0:	add	$6, %rdx
	jmp	Li70
Lbu4:	add	$2, %rdx
	jmp	Li34
Lbu2:	add	$8, %rdx
	jge	Lend

	.align	16, 0x90
Ltop:	movdqu	-64(%rsi,%rdx,8), %xmm1
	movdqu	-56(%rsi,%rdx,8), %xmm0
	psllq	%xmm5, %xmm0
	psrlq	%xmm4, %xmm1
	por	%xmm1, %xmm0
	movdqa	%xmm0, -64(%rdi,%rdx,8)
Li70:
	movdqu	-48(%rsi,%rdx,8), %xmm1
	movdqu	-40(%rsi,%rdx,8), %xmm0
	psllq	%xmm5, %xmm0
	psrlq	%xmm4, %xmm1
	por	%xmm1, %xmm0
	movdqa	%xmm0, -48(%rdi,%rdx,8)
Li56:
	movdqu	-32(%rsi,%rdx,8), %xmm1
	movdqu	-24(%rsi,%rdx,8), %xmm0
	psllq	%xmm5, %xmm0
	psrlq	%xmm4, %xmm1
	por	%xmm1, %xmm0
	movdqa	%xmm0, -32(%rdi,%rdx,8)
Li34:
	movdqu	-16(%rsi,%rdx,8), %xmm1
	movdqu	-8(%rsi,%rdx,8), %xmm0
	psllq	%xmm5, %xmm0
	psrlq	%xmm4, %xmm1
	por	%xmm1, %xmm0
	movdqa	%xmm0, -16(%rdi,%rdx,8)
	add	$8, %rdx
	jl	Ltop

Lend:	test	$1, %dl
	jnz	Le1

	movdqu	-16(%rsi), %xmm1
	movq	-8(%rsi), %xmm0
	psrlq	%xmm4, %xmm1
	psllq	%xmm5, %xmm0
	por	%xmm1, %xmm0
	movdqa	%xmm0, -16(%rdi)
	pop	%rsi
	pop	%rdi
	ret

Le1:	movq	-8(%rsi), %xmm0
	psrlq	%xmm4, %xmm0
	movq	%xmm0, -8(%rdi)
	pop	%rsi
	pop	%rdi
	ret


	.align	16, 0x90
Lbc:	dec	%edx
	jnz	1f
	movq	(%rsi), %xmm0
	psrlq	%xmm4, %xmm0
	movq	%xmm0, (%rdi)
	pop	%rsi
	pop	%rdi
	ret

1:	movq	(%rsi), %xmm1
	movq	8(%rsi), %xmm0
	psrlq	%xmm4, %xmm1
	psllq	%xmm5, %xmm0
	por	%xmm1, %xmm0
	movq	%xmm0, (%rdi)
	dec	%edx
	jnz	1f
	movq	8(%rsi), %xmm0
	psrlq	%xmm4, %xmm0
	movq	%xmm0, 8(%rdi)
	pop	%rsi
	pop	%rdi
	ret

1:	movq	8(%rsi), %xmm1
	movq	16(%rsi), %xmm0
	psrlq	%xmm4, %xmm1
	psllq	%xmm5, %xmm0
	por	%xmm1, %xmm0
	movq	%xmm0,	8(%rdi)
	movq	16(%rsi), %xmm0
	psrlq	%xmm4, %xmm0
	movq	%xmm0, 16(%rdi)
	pop	%rsi
	pop	%rdi
	ret
	

