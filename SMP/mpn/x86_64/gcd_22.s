






















































































	.text
	.align	64, 0x90
	.globl	__gmpn_gcd_22
	
__gmpn_gcd_22:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	mov	%rcx, %rax

	.align	16, 0x90
Ltop:	mov	%rax, %r10
	sub	%rsi, %r10
	jz	Llowz		
	mov	%rdx, %r11
	sbb	%rdi, %r11

	mov	%rsi, %r8
	mov	%rdi, %r9

	bsf	%r10, %rcx

	sub	%rax, %rsi
	sbb	%rdx, %rdi

Lbck:	cmovc	%r10, %rsi		
	cmovc	%r11, %rdi		
	cmovc	%r8, %rax		
	cmovc	%r9, %rdx		

	shrd	%cl, %rdi, %rsi
	shr	%cl, %rdi

	mov	%rdx, %r11
	or	%rdi, %r11
	jnz	Ltop

Lgcd_11:
	mov	%rax, %rdi

	jmp	__gmpn_gcd_11

Llowz:
	
	
	mov	%rdx, %r10
	sub	%rdi, %r10
	je	Lend

	xor	%r11, %r11
	mov	%rsi, %r8
	mov	%rdi, %r9
	bsf	%r10, %rcx
	mov	%rdi, %rsi
	xor	%rdi, %rdi
	sub	%rdx, %rsi
	jmp	Lbck

Lend:	
	
	pop	%rsi
	pop	%rdi
	ret
	
