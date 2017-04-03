
























































































	.text
	.align	64, 0x90
	.globl	__gmpn_lshift
	
__gmpn_lshift:

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
	mov	-8(%rsi,%rdx,8), %rax
	shr	%cl, %rax

	cmp	$3, %rdx
	jle	Lbc

	lea	(%rdi,%rdx,8), %ecx
	test	$8, %cl
	jz	Lrp_aligned


	movq	-8(%rsi,%rdx,8), %xmm0
	movq	-16(%rsi,%rdx,8), %xmm1
	psllq	%xmm4, %xmm0
	psrlq	%xmm5, %xmm1
	por	%xmm1, %xmm0
	movq	%xmm0, -8(%rdi,%rdx,8)
	dec	%rdx

Lrp_aligned:
	lea	1(%rdx), %r8d

	and	$6, %r8d
	jz	Lba0
	cmp	$4, %r8d
	jz	Lba4
	jc	Lba2
Lba6:	add	$-4, %rdx
	jmp	Li56
Lba0:	add	$-6, %rdx
	jmp	Li70
Lba4:	add	$-2, %rdx
	jmp	Li34
Lba2:	add	$-8, %rdx
	jle	Lend

	.align	16, 0x90
Ltop:	movdqu	40(%rsi,%rdx,8), %xmm1
	movdqu	48(%rsi,%rdx,8), %xmm0
	psllq	%xmm4, %xmm0
	psrlq	%xmm5, %xmm1
	por	%xmm1, %xmm0
	movdqa	%xmm0, 48(%rdi,%rdx,8)
Li70:
	movdqu	24(%rsi,%rdx,8), %xmm1
	movdqu	32(%rsi,%rdx,8), %xmm0
	psllq	%xmm4, %xmm0
	psrlq	%xmm5, %xmm1
	por	%xmm1, %xmm0
	movdqa	%xmm0, 32(%rdi,%rdx,8)
Li56:
	movdqu	8(%rsi,%rdx,8), %xmm1
	movdqu	16(%rsi,%rdx,8), %xmm0
	psllq	%xmm4, %xmm0
	psrlq	%xmm5, %xmm1
	por	%xmm1, %xmm0
	movdqa	%xmm0, 16(%rdi,%rdx,8)
Li34:
	movdqu	-8(%rsi,%rdx,8), %xmm1
	movdqu	(%rsi,%rdx,8), %xmm0
	psllq	%xmm4, %xmm0
	psrlq	%xmm5, %xmm1
	por	%xmm1, %xmm0
	movdqa	%xmm0, (%rdi,%rdx,8)
	sub	$8, %rdx
	jg	Ltop

Lend:	test	$1, %dl
	jnz	Lend8

	movdqu	(%rsi), %xmm1
	pxor	%xmm0, %xmm0
	punpcklqdq  %xmm1, %xmm0
	psllq	%xmm4, %xmm1
	psrlq	%xmm5, %xmm0
	por	%xmm1, %xmm0
	movdqa	%xmm0, (%rdi)
	pop	%rsi
	pop	%rdi
	ret


	.align	16, 0x90
Lbc:	dec	%edx
	jz	Lend8

	movq	(%rsi,%rdx,8), %xmm1
	movq	-8(%rsi,%rdx,8), %xmm0
	psllq	%xmm4, %xmm1
	psrlq	%xmm5, %xmm0
	por	%xmm1, %xmm0
	movq	%xmm0, (%rdi,%rdx,8)
	sub	$2, %edx
	jl	Lend8
	movq	8(%rsi), %xmm1
	movq	(%rsi), %xmm0
	psllq	%xmm4, %xmm1
	psrlq	%xmm5, %xmm0
	por	%xmm1, %xmm0
	movq	%xmm0, 8(%rdi)

Lend8:movq	(%rsi), %xmm0
	psllq	%xmm4, %xmm0
	movq	%xmm0, (%rdi)
	pop	%rsi
	pop	%rdi
	ret
	

