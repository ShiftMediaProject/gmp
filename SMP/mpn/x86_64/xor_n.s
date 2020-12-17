









































































  
  
  


















	.text
	.align	32, 0x90
	.globl	__gmpn_xor_n
	
	.def	__gmpn_xor_n
	.scl	2
	.type	32
	.endef
__gmpn_xor_n:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	mov	(%rdx), %r8
	mov	%ecx, %eax
	and	$3, %eax
	je	Lb00
	cmp	$2, %eax
	jc	Lb01
	je	Lb10

Lb11:	xor	(%rsi), %r8
	mov	%r8, (%rdi)
	inc	%rcx
	lea	-8(%rsi), %rsi
	lea	-8(%rdx), %rdx
	lea	-8(%rdi), %rdi
	jmp	Le11
Lb10:	add	$2, %rcx
	lea	-16(%rsi), %rsi
	lea	-16(%rdx), %rdx
	lea	-16(%rdi), %rdi
	jmp	Le10
Lb01:	xor	(%rsi), %r8
	mov	%r8, (%rdi)
	dec	%rcx
	jz	Lret
	lea	8(%rsi), %rsi
	lea	8(%rdx), %rdx
	lea	8(%rdi), %rdi

	.align	16, 0x90
Ltop:	mov	(%rdx), %r8
Lb00:	mov	8(%rdx), %r9
	xor	(%rsi), %r8
	xor	8(%rsi), %r9
	mov	%r8, (%rdi)
	mov	%r9, 8(%rdi)
Le11:	mov	16(%rdx), %r8
Le10:	mov	24(%rdx), %r9
	lea	32(%rdx), %rdx
	xor	16(%rsi), %r8
	xor	24(%rsi), %r9
	lea	32(%rsi), %rsi
	mov	%r8, 16(%rdi)
	mov	%r9, 24(%rdi)
	lea	32(%rdi), %rdi
	sub	$4, %rcx
	jnz	Ltop

Lret:	pop	%rsi
	pop	%rdi
	ret
	





