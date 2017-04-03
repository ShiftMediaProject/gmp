




















































































	.text
	.align	16, 0x90
	.globl	__gmpn_gcd_1
	
__gmpn_gcd_1:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx

	mov	(%rdi), %rax	
	or	%rdx, %rax
	bsf	%rax, %rax	

	bsf	%rdx, %rcx
	shr	%cl, %rdx

	push	%rax		
	push	%rdx		
	sub	$40, %rsp	

	cmp	$1, %rsi
	jnz	Lreduce_nby1


	mov	(%rdi), %r8
	mov	%r8, %rax
	shr	$6, %r8
	cmp	%r8, %rdx
	ja	Lreduced
	jmp	Lbmod

Lreduce_nby1:
	cmp	$16, %rsi
	jl	Lbmod
	mov	%rdx, %r8	
	mov	%rsi, %rdx	
	mov	%rdi, %rcx	
	
	call	__gmpn_mod_1
	jmp	Lreduced
Lbmod:
	mov	%rdx, %r8	
	mov	%rsi, %rdx	
	mov	%rdi, %rcx	
	
	call	__gmpn_modexact_1_odd
Lreduced:

	add	$40, %rsp
	pop	%rdx

	bsf	%rax, %rcx

	jnz	Lmid
	jmp	Lend

	.align	16, 0x90		
Ltop:	cmovc	%r10, %rax	
	cmovc	%r9, %rdx	
Lmid:	shr	%cl, %rax	
	mov	%rdx, %r10	
	sub	%rax, %r10	
	bsf	%r10, %rcx	
	mov	%rax, %r9	
	sub	%rdx, %rax	
	jnz	Ltop		

Lend:	pop	%rcx
	mov	%rdx, %rax
	shl	%cl, %rax
	pop	%rsi
	pop	%rdi
	ret
	
