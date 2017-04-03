
















































































		


















	.text
	.align	16, 0x90
	.globl	__gmpn_preinv_divrem_1
	
__gmpn_preinv_divrem_1:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	mov	56(%rsp), %r8	
	mov	64(%rsp), %r9	
	xor	%eax, %eax
	push	%r13
	push	%r12
	push	%rbp
	push	%rbx

	mov	%rsi, %r12
	mov	%rcx, %rbx
	add	%rsi, %rcx
	mov	%rdx, %rsi

	lea	-8(%rdi,%rcx,8), %rdi

	test	%r8, %r8
	js	Lnent

	mov	104(%rsp), %cl
	shl	%cl, %r8
	jmp	Luent
	

	.align	16, 0x90
	.globl	__gmpn_divrem_1
	
__gmpn_divrem_1:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	mov	56(%rsp), %r8	
	xor	%eax, %eax
	push	%r13
	push	%r12
	push	%rbp
	push	%rbx

	mov	%rsi, %r12
	mov	%rcx, %rbx
	add	%rsi, %rcx
	mov	%rdx, %rsi
	je	Lret

	lea	-8(%rdi,%rcx,8), %rdi
	xor	%ebp, %ebp

	test	%r8, %r8
	jns	Lunnormalized

Lnormalized:
	test	%rbx, %rbx
	je	L8			
	mov	-8(%rsi,%rbx,8), %rbp
	dec	%rbx
	mov	%rbp, %rax
	sub	%r8, %rbp
	cmovc	%rax, %rbp
	sbb	%eax, %eax
	inc	%eax
	mov	%rax, (%rdi)
	lea	-8(%rdi), %rdi
L8:


	push	%r8

	sub	$32, %rsp	
	mov	%r8, %rcx		
	
	call	__gmpn_invert_limb
	add	$32, %rsp	
	pop	%r8



	mov	%rax, %r9
	mov	%rbp, %rax
	jmp	Lnent

	.align	16, 0x90
Lntop:mov	(%rsi,%rbx,8), %r10		
	mul	%r9			
	add	%r10, %rax		
	adc	%rbp, %rdx		
	mov	%rax, %rbp		
	mov	%rdx, %r13		
	imul	%r8, %rdx			
	sub	%rdx, %r10		
	mov	%r8, %rax			
	add	%r10, %rax		
	cmp	%rbp, %r10		
	cmovc	%r10, %rax		
	adc	$-1, %r13		
	cmp	%r8, %rax			
	jae	Lnfx			
Lnok:	mov	%r13, (%rdi)		
	sub	$8, %rdi			
Lnent:lea	1(%rax), %rbp		
	dec	%rbx			
	jns	Lntop			

	xor	%ecx, %ecx
	jmp	Lfrac

Lnfx:	sub	%r8, %rax
	inc	%r13
	jmp	Lnok

Lunnormalized:
	test	%rbx, %rbx
	je	L44
	mov	-8(%rsi,%rbx,8), %rax
	cmp	%r8, %rax
	jae	L44
	mov	%rbp, (%rdi)
	mov	%rax, %rbp
	lea	-8(%rdi), %rdi
	je	Lret
	dec	%rbx
L44:
	bsr	%r8, %rcx
	not	%ecx
	shl	%cl, %r8
	shl	%cl, %rbp

	push	%rcx


	push	%r8


	sub	$40, %rsp	
	mov	%r8, %rcx		
	
	call	__gmpn_invert_limb

	add	$40, %rsp	
	pop	%r8


	pop	%rcx

	mov	%rax, %r9
	mov	%rbp, %rax
	test	%rbx, %rbx
	je	Lfrac

Luent:dec	%rbx
	mov	(%rsi,%rbx,8), %rbp
	neg	%ecx
	shr	%cl, %rbp
	neg	%ecx
	or	%rbp, %rax
	jmp	Lent

	.align	16, 0x90
Lutop:mov	(%rsi,%rbx,8), %r10
	shl	%cl, %rbp
	neg	%ecx
	shr	%cl, %r10
	neg	%ecx
	or	%r10, %rbp
	mul	%r9
	add	%rbp, %rax
	adc	%r11, %rdx
	mov	%rax, %r11
	mov	%rdx, %r13
	imul	%r8, %rdx
	sub	%rdx, %rbp
	mov	%r8, %rax
	add	%rbp, %rax
	cmp	%r11, %rbp
	cmovc	%rbp, %rax
	adc	$-1, %r13
	cmp	%r8, %rax
	jae	Lufx
Luok:	mov	%r13, (%rdi)
	sub	$8, %rdi
Lent:	mov	(%rsi,%rbx,8), %rbp
	dec	%rbx
	lea	1(%rax), %r11
	jns	Lutop

Luend:shl	%cl, %rbp
	mul	%r9
	add	%rbp, %rax
	adc	%r11, %rdx
	mov	%rax, %r11
	mov	%rdx, %r13
	imul	%r8, %rdx
	sub	%rdx, %rbp
	mov	%r8, %rax
	add	%rbp, %rax
	cmp	%r11, %rbp
	cmovc	%rbp, %rax
	adc	$-1, %r13
	cmp	%r8, %rax
	jae	Lefx
Leok:	mov	%r13, (%rdi)
	sub	$8, %rdi
	jmp	Lfrac

Lufx:	sub	%r8, %rax
	inc	%r13
	jmp	Luok
Lefx:	sub	%r8, %rax
	inc	%r13
	jmp	Leok

Lfrac:mov	%r8, %rbp
	neg	%rbp
	jmp	Lfent

	.align	16, 0x90			
Lftop:mul	%r9			
	add	%r11, %rdx		
	mov	%rax, %r11		
	mov	%rdx, %r13		
	imul	%rbp, %rdx		
	mov	%r8, %rax			
	add	%rdx, %rax		
	cmp	%r11, %rdx		
	cmovc	%rdx, %rax		
	adc	$-1, %r13		
	mov	%r13, (%rdi)		
	sub	$8, %rdi			
Lfent:lea	1(%rax), %r11		
	dec	%r12			
	jns	Lftop			

	shr	%cl, %rax
Lret:	pop	%rbx
	pop	%rbp
	pop	%r12
	pop	%r13
	pop	%rsi
	pop	%rdi
	ret
	

