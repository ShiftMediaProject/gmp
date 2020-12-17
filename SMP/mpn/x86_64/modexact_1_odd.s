














































































	.text
	.align	32, 0x90
	.globl	__gmpn_modexact_1_odd
	
	.def	__gmpn_modexact_1_odd
	.scl	2
	.type	32
	.endef
__gmpn_modexact_1_odd:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx

	mov	$0, %ecx
	jmp	Lent		

	.globl	__gmpn_modexact_1c_odd
	
	.def	__gmpn_modexact_1c_odd
	.scl	2
	.type	32
	.endef
__gmpn_modexact_1c_odd:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

Lent:
	
	
	
	

	mov	%rdx, %r8		
	shr	%edx		

	
	lea	__gmp_binvert_limb_table(%rip), %r9


	and	$127, %edx
	mov	%rcx, %r10		

	movzbl	(%r9,%rdx), %edx	

	mov	(%rdi), %rax		
	lea	(%rdi,%rsi,8), %r11	
	mov	%r8, %rdi		

	lea	(%rdx,%rdx), %ecx	
	imul	%edx, %edx	

	neg	%rsi			

	imul	%edi, %edx	

	sub	%edx, %ecx	

	lea	(%rcx,%rcx), %edx	
	imul	%ecx, %ecx	

	imul	%edi, %ecx	

	sub	%ecx, %edx	
	xor	%ecx, %ecx	

	lea	(%rdx,%rdx), %r9	
	imul	%rdx, %rdx		

	imul	%r8, %rdx		

	sub	%rdx, %r9		
	mov	%r10, %rdx		

	

	inc	%rsi
	jz	Lone


	.align	16, 0x90
Ltop:
	
	
	
	
	
	
	
	

	sub	%rdx, %rax		

	adc	$0, %rcx		
	imul	%r9, %rax		

	mul	%r8			

	mov	(%r11,%rsi,8), %rax	
	sub	%rcx, %rax		
	setc	%cl		

	inc	%rsi
	jnz	Ltop


Lone:
	sub	%rdx, %rax		

	adc	$0, %rcx		
	imul	%r9, %rax		

	mul	%r8			

	lea	(%rcx,%rdx), %rax	
	pop	%rsi
	pop	%rdi
	ret

	
	
