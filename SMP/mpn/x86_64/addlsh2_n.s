















































	
	
	













































	.text
	.align	16, 0x90
	.globl	__gmpn_addlsh2_n
	
	.def	__gmpn_addlsh2_n
	.scl	2
	.type	32
	.endef
__gmpn_addlsh2_n:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	push	%r12
	push	%r13
	push	%r14
	push	%r15

	mov	(%rdx), %r8
	lea	(,%r8,4), %r12
	shr	$62, %r8

	mov	%ecx, %eax
	lea	(%rdi,%rcx,8), %rdi
	lea	(%rsi,%rcx,8), %rsi
	lea	(%rdx,%rcx,8), %rdx
	neg	%rcx
	and	$3, %al
	je	Lb00
	cmp	$2, %al
	jc	Lb01
	je	Lb10

Lb11:	mov	8(%rdx,%rcx,8), %r10
	lea	(%r8,%r10,4), %r14
	shr	$62, %r10
	mov	16(%rdx,%rcx,8), %r11
	lea	(%r10,%r11,4), %r15
	shr	$62, %r11
	add	(%rsi,%rcx,8), %r12
	adc	8(%rsi,%rcx,8), %r14
	adc	16(%rsi,%rcx,8), %r15
	sbb	%eax, %eax		  
	mov	%r12, (%rdi,%rcx,8)
	mov	%r14, 8(%rdi,%rcx,8)
	mov	%r15, 16(%rdi,%rcx,8)
	add	$3, %rcx
	js	Ltop
	jmp	Lend

Lb01:	mov	%r8, %r11
	add	(%rsi,%rcx,8), %r12
	sbb	%eax, %eax		  
	mov	%r12, (%rdi,%rcx,8)
	add	$1, %rcx
	js	Ltop
	jmp	Lend

Lb10:	mov	8(%rdx,%rcx,8), %r11
	lea	(%r8,%r11,4), %r15
	shr	$62, %r11
	add	(%rsi,%rcx,8), %r12
	adc	8(%rsi,%rcx,8), %r15
	sbb	%eax, %eax		  
	mov	%r12, (%rdi,%rcx,8)
	mov	%r15, 8(%rdi,%rcx,8)
	add	$2, %rcx
	js	Ltop
	jmp	Lend

Lb00:	mov	8(%rdx,%rcx,8), %r9
	mov	16(%rdx,%rcx,8), %r10
	jmp	Le00

	.align	16, 0x90
Ltop:	mov	16(%rdx,%rcx,8), %r10
	mov	(%rdx,%rcx,8), %r8
	mov	8(%rdx,%rcx,8), %r9
	lea	(%r11,%r8,4), %r12
	shr	$62, %r8
Le00:	lea	(%r8,%r9,4), %r13
	shr	$62, %r9
	mov	24(%rdx,%rcx,8), %r11
	lea	(%r9,%r10,4), %r14
	shr	$62, %r10
	lea	(%r10,%r11,4), %r15
	shr	$62, %r11
	add	%eax, %eax		  
	adc	(%rsi,%rcx,8), %r12
	adc	8(%rsi,%rcx,8), %r13
	adc	16(%rsi,%rcx,8), %r14
	adc	24(%rsi,%rcx,8), %r15
	mov	%r12, (%rdi,%rcx,8)
	mov	%r13, 8(%rdi,%rcx,8)
	mov	%r14, 16(%rdi,%rcx,8)
	sbb	%eax, %eax		  
	mov	%r15, 24(%rdi,%rcx,8)
	add	$4, %rcx
	js	Ltop
Lend:


	sub	%r11d, %eax
	neg	%eax

	pop	%r15
	pop	%r14
	pop	%r13
	pop	%r12
	pop	%rsi
	pop	%rdi
	ret
	

