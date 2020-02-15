













































































	.text
	.align	64, 0x90
	.globl	__gmpn_gcd_11
	
__gmpn_gcd_11:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi

	jmp	Lodd

	.align	16, 0x90
Ltop:	cmovc	%rdx, %rdi		
	cmovc	%rax, %rsi		
	shr	%cl, %rdi
Lodd:	mov	%rsi, %rdx
	sub	%rdi, %rdx		
	bsf	%rdx, %rcx
	mov	%rdi, %rax
	sub	%rsi, %rdi			
	jnz	Ltop

Lend:	
	
	pop	%rsi
	pop	%rdi
	ret
	
