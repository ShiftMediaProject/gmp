



































































		


















	.text
	.align	16, 0x90
	.globl	__gmpn_preinv_divrem_1
	
	.def	__gmpn_preinv_divrem_1
	.scl	2
	.type	32
	.endef
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

	mov	104(%rsp), %cl
	shl	%cl, %r8
	jmp	Lent
	

	.align	16, 0x90
	.globl	__gmpn_divrem_1
	
	.def	__gmpn_divrem_1
	.scl	2
	.type	32
	.endef
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
	sal	%cl, %r8
	sal	%cl, %rbp

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

Lent:	mov	-8(%rsi,%rbx,8), %rbp
	shr	%cl, %rax
	shld	%cl, %rbp, %rax
	sub	$2, %rbx
	js	Lend

	.align	16, 0x90
Ltop:	lea	1(%rax), %r11
	mul	%r9
	mov	(%rsi,%rbx,8), %r10
	shld	%cl, %r10, %rbp
	mov	%rbp, %r13
	add	%rax, %r13
	adc	%r11, %rdx
	mov	%rdx, %r11
	imul	%r8, %rdx
	sub	%rdx, %rbp
	lea	(%r8,%rbp), %rax
	sub	$8, %rdi
	cmp	%r13, %rbp
	cmovc	%rbp, %rax
	adc	$-1, %r11
	cmp	%r8, %rax
	jae	Lufx
Luok:	dec	%rbx
	mov	%r11, 8(%rdi)
	mov	%r10, %rbp
	jns	Ltop

Lend:	lea	1(%rax), %r11
	sal	%cl, %rbp
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
	inc	%r11
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
	
