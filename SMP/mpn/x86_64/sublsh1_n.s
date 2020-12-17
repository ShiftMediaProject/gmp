












































































	.text
	.align	8, 0x90
	.globl	__gmpn_sublsh1_n
	
	.def	__gmpn_sublsh1_n
	.scl	2
	.type	32
	.endef
__gmpn_sublsh1_n:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	push	%rbx
	push	%r12

	mov	%ecx, %eax
	lea	24(%rsi,%rcx,8), %rsi
	lea	24(%rdx,%rcx,8), %rdx
	lea	24(%rdi,%rcx,8), %rdi
	neg	%rcx

	xor	%r11d, %r11d

	mov	-24(%rdx,%rcx,8), %r8	
	shrd	$63, %r8, %r11

	and	$3, %eax
	je	Lb0
	cmp	$2, %eax
	jc	Lb1
	je	Lb2

Lb3:	mov	-16(%rdx,%rcx,8), %r9
	shrd	$63, %r9, %r8
	mov	-8(%rdx,%rcx,8), %r10
	shrd	$63, %r10, %r9
	mov	-24(%rsi,%rcx,8), %r12
	sub	%r11, %r12
	mov	%r12, -24(%rdi,%rcx,8)
	mov	-16(%rsi,%rcx,8), %r12
	sbb	%r8, %r12
	mov	%r12, -16(%rdi,%rcx,8)
	mov	-8(%rsi,%rcx,8), %r12
	sbb	%r9, %r12
	mov	%r12, -8(%rdi,%rcx,8)
	mov	%r10, %r11
	sbb	%eax, %eax	
	add	$3, %rcx
	js	Ltop
	jmp	Lend

Lb1:	mov	-24(%rsi,%rcx,8), %r12
	sub	%r11, %r12
	mov	%r12, -24(%rdi,%rcx,8)
	mov	%r8, %r11
	sbb	%eax, %eax	
	inc	%rcx
	js	Ltop
	jmp	Lend

Lb2:	mov	-16(%rdx,%rcx,8), %r9
	shrd	$63, %r9, %r8
	mov	-24(%rsi,%rcx,8), %r12
	sub	%r11, %r12
	mov	%r12, -24(%rdi,%rcx,8)
	mov	-16(%rsi,%rcx,8), %r12
	sbb	%r8, %r12
	mov	%r12, -16(%rdi,%rcx,8)
	mov	%r9, %r11
	sbb	%eax, %eax	
	add	$2, %rcx
	js	Ltop
	jmp	Lend

	.align	16, 0x90
Ltop:	mov	-24(%rdx,%rcx,8), %r8
	shrd	$63, %r8, %r11
Lb0:	mov	-16(%rdx,%rcx,8), %r9
	shrd	$63, %r9, %r8
	mov	-8(%rdx,%rcx,8), %r10
	shrd	$63, %r10, %r9
	mov	(%rdx,%rcx,8), %rbx
	shrd	$63, %rbx, %r10

	add	%eax, %eax	

	mov	-24(%rsi,%rcx,8), %r12
	sbb	%r11, %r12
	mov	%r12, -24(%rdi,%rcx,8)

	mov	-16(%rsi,%rcx,8), %r12
	sbb	%r8, %r12
	mov	%r12, -16(%rdi,%rcx,8)

	mov	-8(%rsi,%rcx,8), %r12
	sbb	%r9, %r12
	mov	%r12, -8(%rdi,%rcx,8)

	mov	(%rsi,%rcx,8), %r12
	sbb	%r10, %r12
	mov	%r12, (%rdi,%rcx,8)

	mov	%rbx, %r11
	sbb	%eax, %eax	

	add	$4, %rcx
	js	Ltop

Lend:	shr	$63, %r11
	pop	%r12
	pop	%rbx
	sub	%r11d, %eax
	neg	%eax
	pop	%rsi
	pop	%rdi
	ret
	

