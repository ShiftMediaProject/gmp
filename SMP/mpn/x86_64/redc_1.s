








































































   
   
   
   
    














	.text
	.align	32, 0x90
	.globl	__gmpn_redc_1
	
__gmpn_redc_1:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	mov	56(%rsp), %r8	
	push	%rbx
	push	%rbp
	push	%r12
	push	%r13
	push	%r14
	push	%r15

	mov	(%rsi), %r13
	mov	%rcx, %r15			
	lea	8(%rdx,%rcx,8), %r12
	lea	8(%rsi,%rcx,8), %rsi
	neg	%rcx
	imul	%r8, %r13		

	test	$1, %cl
	jz	Lbx0

Lbx1:	test	$2, %cl
	jz	Lb3

Lb1:	cmp	$-1, %ecx
	jz	Ln1

Lotp1:lea	1(%rcx), %r14
	mov	-8(%r12,%rcx,8), %rax
	mul	%r13
	mov	-8(%rsi,%rcx,8), %r10
	mov	%rdx, %r11
	add	%rax, %r10
	mov	(%r12,%rcx,8), %rax
	adc	$0, %r11
	mul	%r13
	mov	%rdx, %r9
	mov	(%rsi,%rcx,8), %rbx
	add	%rax, %rbx
	adc	$0, %r9
	mov	(%r12,%r14,8), %rax
	mul	%r13
	mov	(%rsi,%r14,8), %r10
	add	%r11, %rbx
	mov	%rbx, -8(%rsi,%r14,8)	
	adc	$0, %r9
	imul	%r8, %rbx		
	jmp	Le1

	.align	16, 0x90
Ltp1:	mul	%r13
	mov	-16(%rsi,%r14,8), %r10
	add	%r11, %rbp
	mov	%rdx, %r11
	adc	$0, %r9
	mov	%rbp, -24(%rsi,%r14,8)
	add	%rax, %r10
	mov	-8(%r12,%r14,8), %rax
	adc	$0, %r11
	mul	%r13
	add	%r9, %r10
	mov	%rdx, %r9
	mov	-8(%rsi,%r14,8), %rbp
	adc	$0, %r11
	mov	%r10, -16(%rsi,%r14,8)
	add	%rax, %rbp
	adc	$0, %r9
	mov	(%r12,%r14,8), %rax
	mul	%r13
	mov	(%rsi,%r14,8), %r10
	add	%r11, %rbp
	mov	%rbp, -8(%rsi,%r14,8)
	adc	$0, %r9
Le1:	mov	%rdx, %r11
	add	%rax, %r10
	mov	8(%r12,%r14,8), %rax
	adc	$0, %r11
	mul	%r13
	mov	8(%rsi,%r14,8), %rbp
	add	%r9, %r10
	mov	%rdx, %r9
	mov	%r10, (%rsi,%r14,8)
	adc	$0, %r11
	add	%rax, %rbp
	adc	$0, %r9
	mov	16(%r12,%r14,8), %rax
	add	$4, %r14
	jnc	Ltp1

Led1:	mul	%r13
	mov	-16(%rsi), %r10
	add	%r11, %rbp
	adc	$0, %r9
	mov	%rbp, -24(%rsi)
	add	%rax, %r10
	adc	$0, %rdx
	add	%r9, %r10
	adc	$0, %rdx
	mov	%r10, -16(%rsi)
	mov	%rdx, -8(%rsi,%rcx,8)	
	mov	%rbx, %r13		
	lea	8(%rsi), %rsi		
	dec	%r15
	jnz	Lotp1
	jmp	Lcj

Lb3:	cmp	$-3, %ecx
	jz	Ln3

Lotp3:lea	3(%rcx), %r14
	mov	-8(%r12,%rcx,8), %rax
	mul	%r13
	mov	-8(%rsi,%rcx,8), %r10
	mov	%rdx, %r11
	add	%rax, %r10
	mov	(%r12,%rcx,8), %rax
	adc	$0, %r11
	mul	%r13
	mov	(%rsi,%rcx,8), %rbx
	mov	%rdx, %r9
	add	%rax, %rbx
	adc	$0, %r9
	mov	8(%r12,%rcx,8), %rax
	mul	%r13
	mov	8(%rsi,%rcx,8), %r10
	add	%r11, %rbx
	mov	%rdx, %r11
	adc	$0, %r9
	mov	%rbx, (%rsi,%rcx,8)
	imul	%r8, %rbx		
	jmp	Le3

	.align	16, 0x90
Ltp3:	mul	%r13
	mov	-16(%rsi,%r14,8), %r10
	add	%r11, %rbp
	mov	%rdx, %r11
	adc	$0, %r9
	mov	%rbp, -24(%rsi,%r14,8)
Le3:	add	%rax, %r10
	mov	-8(%r12,%r14,8), %rax
	adc	$0, %r11
	mul	%r13
	add	%r9, %r10
	mov	%rdx, %r9
	mov	-8(%rsi,%r14,8), %rbp
	adc	$0, %r11
	mov	%r10, -16(%rsi,%r14,8)
	add	%rax, %rbp
	adc	$0, %r9
	mov	(%r12,%r14,8), %rax
	mul	%r13
	mov	(%rsi,%r14,8), %r10
	add	%r11, %rbp
	mov	%rbp, -8(%rsi,%r14,8)
	adc	$0, %r9
	mov	%rdx, %r11
	add	%rax, %r10
	mov	8(%r12,%r14,8), %rax
	adc	$0, %r11
	mul	%r13
	mov	8(%rsi,%r14,8), %rbp
	add	%r9, %r10
	mov	%rdx, %r9
	mov	%r10, (%rsi,%r14,8)
	adc	$0, %r11
	add	%rax, %rbp
	adc	$0, %r9
	mov	16(%r12,%r14,8), %rax
	add	$4, %r14
	jnc	Ltp3

Led3:	mul	%r13
	mov	-16(%rsi), %r10
	add	%r11, %rbp
	adc	$0, %r9
	mov	%rbp, -24(%rsi)
	add	%rax, %r10
	adc	$0, %rdx
	add	%r9, %r10
	adc	$0, %rdx
	mov	%r10, -16(%rsi)
	mov	%rdx, -8(%rsi,%rcx,8)	
	mov	%rbx, %r13		
	lea	8(%rsi), %rsi		
	dec	%r15
	jnz	Lotp3


Lcj:
	

	lea	-8(%rsi,%rcx,8), %rdx	
	lea	(%rdx,%rcx,8), %r8		
	neg	%ecx
	mov	%rcx, %r9			
	mov	%rdi, %rcx		


	sub	$40, %rsp	
	
	call	__gmpn_add_n

	add	$40, %rsp	

Lret:	pop	%r15
	pop	%r14
	pop	%r13
	pop	%r12
	pop	%rbp
	pop	%rbx
	pop	%rsi
	pop	%rdi
	ret

Lbx0:	test	$2, %cl
	jnz	Lb2

Lb0:
Lotp0:lea	(%rcx), %r14
	mov	-8(%r12,%rcx,8), %rax
	mul	%r13
	mov	%rdx, %r9
	mov	-8(%rsi,%rcx,8), %rbp
	add	%rax, %rbp
	adc	$0, %r9
	mov	(%r12,%rcx,8), %rax
	mul	%r13
	mov	(%rsi,%rcx,8), %rbx
	mov	%rdx, %r11
	add	%rax, %rbx
	mov	8(%r12,%rcx,8), %rax
	adc	$0, %r11
	mul	%r13
	mov	8(%rsi,%rcx,8), %rbp
	add	%r9, %rbx
	mov	%rdx, %r9
	mov	%rbx, (%rsi,%rcx,8)
	adc	$0, %r11
	imul	%r8, %rbx		
	jmp	Le0

	.align	16, 0x90
Ltp0:	mul	%r13
	mov	-16(%rsi,%r14,8), %r10
	add	%r11, %rbp
	mov	%rdx, %r11
	adc	$0, %r9
	mov	%rbp, -24(%rsi,%r14,8)
	add	%rax, %r10
	mov	-8(%r12,%r14,8), %rax
	adc	$0, %r11
	mul	%r13
	add	%r9, %r10
	mov	%rdx, %r9
	mov	-8(%rsi,%r14,8), %rbp
	adc	$0, %r11
	mov	%r10, -16(%rsi,%r14,8)
	add	%rax, %rbp
	adc	$0, %r9
	mov	(%r12,%r14,8), %rax
	mul	%r13
	mov	(%rsi,%r14,8), %r10
	add	%r11, %rbp
	mov	%rbp, -8(%rsi,%r14,8)
	adc	$0, %r9
	mov	%rdx, %r11
	add	%rax, %r10
	mov	8(%r12,%r14,8), %rax
	adc	$0, %r11
	mul	%r13
	mov	8(%rsi,%r14,8), %rbp
	add	%r9, %r10
	mov	%rdx, %r9
	mov	%r10, (%rsi,%r14,8)
	adc	$0, %r11
Le0:	add	%rax, %rbp
	adc	$0, %r9
	mov	16(%r12,%r14,8), %rax
	add	$4, %r14
	jnc	Ltp0

Led0:	mul	%r13
	mov	-16(%rsi), %r10
	add	%r11, %rbp
	adc	$0, %r9
	mov	%rbp, -24(%rsi)
	add	%rax, %r10
	adc	$0, %rdx
	add	%r9, %r10
	adc	$0, %rdx
	mov	%r10, -16(%rsi)
	mov	%rdx, -8(%rsi,%rcx,8)	
	mov	%rbx, %r13		
	lea	8(%rsi), %rsi		
	dec	%r15
	jnz	Lotp0
	jmp	Lcj

Lb2:	cmp	$-2, %ecx
	jz	Ln2

Lotp2:lea	2(%rcx), %r14
	mov	-8(%r12,%rcx,8), %rax
	mul	%r13
	mov	-8(%rsi,%rcx,8), %rbp
	mov	%rdx, %r9
	add	%rax, %rbp
	adc	$0, %r9
	mov	(%r12,%rcx,8), %rax
	mul	%r13
	mov	(%rsi,%rcx,8), %rbx
	mov	%rdx, %r11
	add	%rax, %rbx
	mov	8(%r12,%rcx,8), %rax
	adc	$0, %r11
	mul	%r13
	add	%r9, %rbx
	mov	%rdx, %r9
	mov	8(%rsi,%rcx,8), %rbp
	adc	$0, %r11
	mov	%rbx, (%rsi,%rcx,8)
	imul	%r8, %rbx		
	jmp	Le2

	.align	16, 0x90
Ltp2:	mul	%r13
	mov	-16(%rsi,%r14,8), %r10
	add	%r11, %rbp
	mov	%rdx, %r11
	adc	$0, %r9
	mov	%rbp, -24(%rsi,%r14,8)
	add	%rax, %r10
	mov	-8(%r12,%r14,8), %rax
	adc	$0, %r11
	mul	%r13
	add	%r9, %r10
	mov	%rdx, %r9
	mov	-8(%rsi,%r14,8), %rbp
	adc	$0, %r11
	mov	%r10, -16(%rsi,%r14,8)
Le2:	add	%rax, %rbp
	adc	$0, %r9
	mov	(%r12,%r14,8), %rax
	mul	%r13
	mov	(%rsi,%r14,8), %r10
	add	%r11, %rbp
	mov	%rbp, -8(%rsi,%r14,8)
	adc	$0, %r9
	mov	%rdx, %r11
	add	%rax, %r10
	mov	8(%r12,%r14,8), %rax
	adc	$0, %r11
	mul	%r13
	mov	8(%rsi,%r14,8), %rbp
	add	%r9, %r10
	mov	%rdx, %r9
	mov	%r10, (%rsi,%r14,8)
	adc	$0, %r11
	add	%rax, %rbp
	adc	$0, %r9
	mov	16(%r12,%r14,8), %rax
	add	$4, %r14
	jnc	Ltp2

Led2:	mul	%r13
	mov	-16(%rsi), %r10
	add	%r11, %rbp
	adc	$0, %r9
	mov	%rbp, -24(%rsi)
	add	%rax, %r10
	adc	$0, %rdx
	add	%r9, %r10
	adc	$0, %rdx
	mov	%r10, -16(%rsi)
	mov	%rdx, -8(%rsi,%rcx,8)	
	mov	%rbx, %r13		
	lea	8(%rsi), %rsi		
	dec	%r15
	jnz	Lotp2
	jmp	Lcj

Ln1:	mov	(%rdx), %rax
	mul	%r13
	add	-16(%rsi), %rax
	adc	-8(%rsi), %rdx
	mov	%rdx, (%rdi)
	mov	$0, %eax
	adc	%eax, %eax
	jmp	Lret

Ln2:	mov	(%rdx), %rax
	mov	-24(%rsi), %rbp
	mul	%r13
	add	%rax, %rbp
	mov	%rdx, %r9
	adc	$0, %r9
	mov	-16(%r12), %rax
	mov	-16(%rsi), %r10
	mul	%r13
	add	%rax, %r10
	mov	%rdx, %r11
	adc	$0, %r11
	add	%r9, %r10
	adc	$0, %r11
	mov	%r10, %r13
	imul	%r8, %r13		
	mov	-24(%r12), %rax
	mul	%r13
	add	%rax, %r10
	mov	%rdx, %r9
	adc	$0, %r9
	mov	-16(%r12), %rax
	mov	-8(%rsi), %r14
	mul	%r13
	add	%rax, %r14
	adc	$0, %rdx
	add	%r9, %r14
	adc	$0, %rdx
	xor	%eax, %eax
	add	%r11, %r14
	adc	(%rsi), %rdx
	mov	%r14, (%rdi)
	mov	%rdx, 8(%rdi)
	adc	%eax, %eax
	jmp	Lret

	.align	16, 0x90
Ln3:	mov	-32(%r12), %rax
	mov	-32(%rsi), %r10
	mul	%r13
	add	%rax, %r10
	mov	-24(%r12), %rax
	mov	%rdx, %r11
	adc	$0, %r11
	mov	-24(%rsi), %rbp
	mul	%r13
	add	%rax, %rbp
	mov	%rdx, %r9
	adc	$0, %r9
	mov	-16(%r12), %rax
	add	%r11, %rbp
	mov	-16(%rsi), %r10
	adc	$0, %r9
	mul	%r13
	mov	%rbp, %r13
	imul	%r8, %r13		
	add	%rax, %r10
	mov	%rdx, %r11
	adc	$0, %r11
	mov	%rbp, -24(%rsi)
	add	%r9, %r10
	adc	$0, %r11
	mov	%r10, -16(%rsi)
	mov	%r11, -32(%rsi)		
	lea	8(%rsi), %rsi		
	dec	%r15
	jnz	Ln3
	jmp	Lcj
	

