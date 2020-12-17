




































































	.text
	.align	16, 0x90
	.globl	__gmpn_bdiv_dbm1c
	
	.def	__gmpn_bdiv_dbm1c
	.scl	2
	.type	32
	.endef
__gmpn_bdiv_dbm1c:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	mov	56(%rsp), %r8	
	mov	(%rsi), %rax
	mov	%rdx, %r9
	mov	%edx, %r11d
	mul	%rcx
	lea	(%rsi,%r9,8), %rsi
	lea	(%rdi,%r9,8), %rdi
	neg	%r9
	and	$3, %r11d
	jz	Llo0
	lea	-4(%r9,%r11), %r9
	cmp	$2, %r11d
	jc	Llo1
	jz	Llo2
	jmp	Llo3

	.align	16, 0x90
Ltop:	mov	(%rsi,%r9,8), %rax
	mul	%rcx
Llo0:	sub	%rax, %r8
	mov	%r8, (%rdi,%r9,8)
	sbb	%rdx, %r8
	mov	8(%rsi,%r9,8), %rax
	mul	%rcx
Llo3:	sub	%rax, %r8
	mov	%r8, 8(%rdi,%r9,8)
	sbb	%rdx, %r8
	mov	16(%rsi,%r9,8), %rax
	mul	%rcx
Llo2:	sub	%rax, %r8
	mov	%r8, 16(%rdi,%r9,8)
	sbb	%rdx, %r8
	mov	24(%rsi,%r9,8), %rax
	mul	%rcx
Llo1:	sub	%rax, %r8
	mov	%r8, 24(%rdi,%r9,8)
	sbb	%rdx, %r8
	add	$4, %r9
	jnz	Ltop

	mov	%r8, %rax
	pop	%rsi
	pop	%rdi
	ret
	
