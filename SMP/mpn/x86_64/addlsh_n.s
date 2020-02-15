













































































			


	
	
	
	
	










	.text
	.align	32, 0x90
	.globl	__gmpn_addlsh_n
	
__gmpn_addlsh_n:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	mov	56(%rsp), %r8d		
	push	%rbx
	xor	%ebx, %ebx	
Lent:	push	%rbp
	mov	%ecx, %ebp
	mov	%rcx, %rax

	mov	%r8d, %ecx
	neg	%ecx

	lea	-8(%rsi,%rax,8), %rsi
	lea	-8(%rdx,%rax,8), %rdx
	lea	-40(%rdi,%rax,8), %rdi
	neg	%rax

	and	$3, %ebp
	jz	Lb0
	cmp	$2, %ebp
	jc	Lb1
	jz	Lb2

Lb3:	xor	%r9d, %r9d
	mov	8(%rdx,%rax,8), %r10
	mov	16(%rdx,%rax,8), %r11
	shrd	%cl, %r10, %r9
	shrd	%cl, %r11, %r10
	add	%ebx, %ebx
	adc	8(%rsi,%rax,8), %r9
	mov	24(%rdx,%rax,8), %r8
	adc	16(%rsi,%rax,8), %r10
	sbb	%ebx, %ebx
	add	$3, %rax
	jmp	Llo3

Lb0:	mov	8(%rdx,%rax,8), %r9
	xor	%r8d, %r8d
	shrd	%cl, %r9, %r8
	mov	16(%rdx,%rax,8), %r10
	mov	24(%rdx,%rax,8), %r11
	shrd	%cl, %r10, %r9
	shrd	%cl, %r11, %r10
	add	%ebx, %ebx
	adc	8(%rsi,%rax,8), %r8
	mov	%r8, 40(%rdi,%rax,8)	
	adc	16(%rsi,%rax,8), %r9
	mov	32(%rdx,%rax,8), %r8
	adc	24(%rsi,%rax,8), %r10
	sbb	%ebx, %ebx
	add	$4, %rax
	jmp	Llo0

Lb1:	mov	8(%rdx,%rax,8), %r8
	add	$1, %rax
	jz	L1
	mov	8(%rdx,%rax,8), %r9
	xor	%ebp, %ebp
	jmp	Llo1
L1:	xor	%r11d, %r11d
	jmp	Lwd1

Lb2:	xor	%r10, %r10
	mov	8(%rdx,%rax,8), %r11
	shrd	%cl, %r11, %r10
	add	%ebx, %ebx
	mov	16(%rdx,%rax,8), %r8
	adc	8(%rsi,%rax,8), %r10
	sbb	%ebx, %ebx
	add	$2, %rax
	jz	Lend

	.align	16, 0x90
Ltop:	mov	8(%rdx,%rax,8), %r9
	mov	%r11, %rbp
Llo2:	mov	%r10, 24(%rdi,%rax,8)	
Llo1:	shrd	%cl, %r8, %rbp
	shrd	%cl, %r9, %r8
	mov	16(%rdx,%rax,8), %r10
	mov	24(%rdx,%rax,8), %r11
	shrd	%cl, %r10, %r9
	shrd	%cl, %r11, %r10
	add	%ebx, %ebx
	adc	(%rsi,%rax,8), %rbp
	adc	8(%rsi,%rax,8), %r8
	mov	%r8, 40(%rdi,%rax,8)	
	adc	16(%rsi,%rax,8), %r9
	mov	32(%rdx,%rax,8), %r8
	adc	24(%rsi,%rax,8), %r10
	sbb	%ebx, %ebx
	add	$4, %rax
	mov	%rbp, (%rdi,%rax,8)	
Llo0:
Llo3:	mov	%r9, 16(%rdi,%rax,8)	
	jnz	Ltop

Lend:	mov	%r10, 24(%rdi,%rax,8)
Lwd1:	shrd	%cl, %r8, %r11
	add	%ebx, %ebx
	adc	(%rsi,%rax,8), %r11
	mov	%r11, 32(%rdi,%rax,8)	
	adc	%eax, %eax	
	shr	%cl, %r8
	add	%r8, %rax

	pop	%rbp
	pop	%rbx
	pop	%rsi
	pop	%rdi
	ret
	
	.globl	__gmpn_addlsh_nc
	
__gmpn_addlsh_nc:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	mov	56(%rsp), %r8d		
	mov	64(%rsp), %r9		
	push	%rbx
	neg	%r9
	sbb	%ebx, %ebx	
	jmp	Lent
	

