










































































	.text
	.align	16, 0x90
	.globl	__gmpn_rshift
	
	.def	__gmpn_rshift
	.scl	2
	.type	32
	.endef
__gmpn_rshift:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx


	xor	%eax, %eax

	test	$1, %dl
	jnz	Lbx1
Lbx0:	test	$2, %dl
	jnz	Lb10

Lb00:	lea	8(%rsi), %rsi
	lea	-24(%rdi), %rdi
	mov	-8(%rsi), %r10
	mov	(%rsi), %r11
	shrd	%cl, %r10, %rax
	mov	8(%rsi), %r8
	shr	$2, %rdx
	jmp	L00

Lbx1:	test	$2, %dl
	jnz	Lb11

Lb01:	lea	16(%rsi), %rsi
	lea	-16(%rdi), %rdi
	mov	-16(%rsi), %r9
	shrd	%cl, %r9, %rax
	shr	$2, %rdx
	jz	L1
	mov	-8(%rsi), %r10
	mov	(%rsi), %r11
	jmp	L01

Lb10:	lea	24(%rsi), %rsi
	lea	-8(%rdi), %rdi
	mov	-24(%rsi), %r8
	mov	-16(%rsi), %r9
	shrd	%cl, %r8, %rax
	shr	$2, %rdx
	jz	L2
	mov	-8(%rsi), %r10
	jmp	L10

Lb11:	lea	32(%rsi), %rsi
	mov	-32(%rsi), %r11
	mov	-24(%rsi), %r8
	mov	-16(%rsi), %r9
	shrd	%cl, %r11, %rax
	shr	$2, %rdx
	jz	Lend

	.align	16, 0x90
Ltop:	shrd	%cl, %r8, %r11
	mov	-8(%rsi), %r10
	mov	%r11, (%rdi)
L10:	shrd	%cl, %r9, %r8
	mov	(%rsi), %r11
	mov	%r8, 8(%rdi)
L01:	shrd	%cl, %r10, %r9
	mov	8(%rsi), %r8
	mov	%r9, 16(%rdi)
L00:	shrd	%cl, %r11, %r10
	mov	16(%rsi), %r9
	add	$32, %rsi
	mov	%r10, 24(%rdi)
	add	$32, %rdi
	dec	%rdx
	jnz	Ltop

Lend:	shrd	%cl, %r8, %r11
	mov	%r11, (%rdi)
L2:	shrd	%cl, %r9, %r8
	mov	%r8, 8(%rdi)
L1:	shr	%cl, %r9
	mov	%r9, 16(%rdi)
	pop	%rsi
	pop	%rdi
	ret
	
