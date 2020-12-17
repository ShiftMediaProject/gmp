































































	
	
	
	







	.text
	.align	16, 0x90
	.globl	__gmpn_rsh1sub_nc
	
	.def	__gmpn_rsh1sub_nc
	.scl	2
	.type	32
	.endef
__gmpn_rsh1sub_nc:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	mov	56(%rsp), %r8	
	push	%rbx
	push	%rbp

	neg	%r8			
	mov	(%rsi), %r8
	sbb	(%rdx), %r8
	jmp	Lent
	

	.align	16, 0x90
	.globl	__gmpn_rsh1sub_n
	
	.def	__gmpn_rsh1sub_n
	.scl	2
	.type	32
	.endef
__gmpn_rsh1sub_n:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	push	%rbx
	push	%rbp

	mov	(%rsi), %r8
	sub	(%rdx), %r8
Lent:	sbb	%ebx, %ebx	
	mov	%r8, %rax
	and	$1, %eax		

	lea	(%rsi,%rcx,8), %rsi
	lea	(%rdx,%rcx,8), %rdx
	lea	(%rdi,%rcx,8), %rdi
	mov	%ecx, %ebp
	neg	%rcx
	and	$3, %ebp
	jz	Lb0
	cmp	$2, %ebp
	jae	Ln1

Lb1:	mov	%r8, %rbp
	inc	%rcx
	js	Ltop
	jmp	Lend

Ln1:	jnz	Lb3
	add	%ebx, %ebx	
	mov	8(%rsi,%rcx,8), %r11
	sbb	8(%rdx,%rcx,8), %r11
	sbb	%ebx, %ebx	
	mov	%r8, %r10
	add	$-2, %rcx
	jmp	L2

Lb3:	add	%ebx, %ebx	
	mov	8(%rsi,%rcx,8), %r10
	mov	16(%rsi,%rcx,8), %r11
	sbb	8(%rdx,%rcx,8), %r10
	sbb	16(%rdx,%rcx,8), %r11
	sbb	%ebx, %ebx	
	mov	%r8, %r9
	dec	%rcx
	jmp	L3

Lb0:	add	%ebx, %ebx	
	mov	8(%rsi,%rcx,8), %r9
	mov	16(%rsi,%rcx,8), %r10
	mov	24(%rsi,%rcx,8), %r11
	sbb	8(%rdx,%rcx,8), %r9
	sbb	16(%rdx,%rcx,8), %r10
	sbb	24(%rdx,%rcx,8), %r11
	sbb	%ebx, %ebx	
	jmp	L4

	.align	16, 0x90

Ltop:	add	%ebx, %ebx	
	mov	(%rsi,%rcx,8), %r8
	mov	8(%rsi,%rcx,8), %r9
	mov	16(%rsi,%rcx,8), %r10
	mov	24(%rsi,%rcx,8), %r11
	sbb	(%rdx,%rcx,8), %r8
	sbb	8(%rdx,%rcx,8), %r9
	sbb	16(%rdx,%rcx,8), %r10
	sbb	24(%rdx,%rcx,8), %r11
	sbb	%ebx, %ebx	
	shrd	$1, %r8, %rbp
	mov	%rbp, -8(%rdi,%rcx,8)
L4:	shrd	$1, %r9, %r8
	mov	%r8, (%rdi,%rcx,8)
L3:	shrd	$1, %r10, %r9
	mov	%r9, 8(%rdi,%rcx,8)
L2:	shrd	$1, %r11, %r10
	mov	%r10, 16(%rdi,%rcx,8)
L1:	add	$4, %rcx
	mov	%r11, %rbp
	js	Ltop

Lend:	shrd	$1, %rbx, %rbp
	mov	%rbp, -8(%rdi)
	pop	%rbp
	pop	%rbx
	pop	%rsi
	pop	%rdi
	ret
	
