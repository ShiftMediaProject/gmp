












































































	.text
	.align	32, 0x90
	.globl	__gmpn_sqr_diag_addlsh1
	
	.def	__gmpn_sqr_diag_addlsh1
	.scl	2
	.type	32
	.endef
__gmpn_sqr_diag_addlsh1:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	push	%rbx

	dec	%rcx
	shl	%rcx

	mov	(%rdx), %rax

	lea	(%rdi,%rcx,8), %rdi
	lea	(%rsi,%rcx,8), %rsi
	lea	(%rdx,%rcx,4), %r11
	neg	%rcx

	mul	%rax
	mov	%rax, (%rdi,%rcx,8)

	xor	%ebx, %ebx
	jmp	Lmid

	.align	16, 0x90
Ltop:	add	%r10, %r8
	adc	%rax, %r9
	mov	%r8, -8(%rdi,%rcx,8)
	mov	%r9, (%rdi,%rcx,8)
Lmid:	mov	8(%r11,%rcx,4), %rax
	mov	(%rsi,%rcx,8), %r8
	mov	8(%rsi,%rcx,8), %r9
	adc	%r8, %r8
	adc	%r9, %r9
	lea	(%rdx,%rbx), %r10
	setc	%bl
	mul	%rax
	add	$2, %rcx
	js	Ltop

Lend:	add	%r10, %r8
	adc	%rax, %r9
	mov	%r8, -8(%rdi)
	mov	%r9, (%rdi)
	adc	%rbx, %rdx
	mov	%rdx, 8(%rdi)

	pop	%rbx
	pop	%rsi
	pop	%rdi
	ret
	
