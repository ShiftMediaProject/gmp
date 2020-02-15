









































































   
   
   
   
    















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
	lea	(%rdx,%rcx,8), %r12
	lea	-16(%rsi,%rcx,8), %rsi
	neg	%rcx
	imul	%r8, %r13		

	test	$1, %cl
	jz	Lb0

Lb1:	cmp	$-1, %ecx
	jz	Ln1
	cmp	$-3, %ecx
	jz	Ln3

	push	%rdi

Lotp1:lea	3(%rcx), %r14
	mov	(%r12,%rcx,8), %rax
	mul	%r13
	lea	(%rax), %rbp
	mov	8(%r12,%rcx,8), %rax
	lea	(%rdx), %r9
	mul	%r13
	lea	(%rax), %r11
	mov	16(%r12,%rcx,8), %rax
	mov	16(%rsi,%rcx,8), %r10
	lea	(%rdx), %rdi
	mul	%r13
	add	%rbp, %r10
	lea	(%rax), %rbp
	mov	24(%r12,%rcx,8), %rax
	adc	%r9, %r11
	mov	24(%rsi,%rcx,8), %rbx
	lea	(%rdx), %r9
	adc	$0, %rdi
	mul	%r13
	add	%r11, %rbx
	lea	(%rax), %r11
	mov	32(%r12,%rcx,8), %rax
	adc	%rdi, %rbp
	mov	%rbx, 24(%rsi,%rcx,8)
	mov	32(%rsi,%rcx,8), %r10
	lea	(%rdx), %rdi
	adc	$0, %r9
	imul	%r8, %rbx		
	add	$2, %r14
	jns	Led1

	.align	16, 0x90
Ltp1:	mul	%r13
	add	%rbp, %r10
	lea	(%rax), %rbp
	mov	(%r12,%r14,8), %rax
	adc	%r9, %r11
	mov	%r10, -8(%rsi,%r14,8)
	mov	(%rsi,%r14,8), %r10
	lea	(%rdx), %r9
	adc	$0, %rdi
	mul	%r13
	add	%r11, %r10
	lea	(%rax), %r11
	mov	8(%r12,%r14,8), %rax
	adc	%rdi, %rbp
	mov	%r10, (%rsi,%r14,8)
	mov	8(%rsi,%r14,8), %r10
	lea	(%rdx), %rdi
	adc	$0, %r9
	add	$2, %r14
	js	Ltp1

Led1:	mul	%r13
	add	%rbp, %r10
	adc	%r9, %r11
	mov	%r10, -8(%rsi)
	mov	(%rsi), %r10
	adc	$0, %rdi
	add	%r11, %r10
	adc	%rdi, %rax
	mov	%r10, (%rsi)
	mov	8(%rsi), %r10
	adc	$0, %rdx
	add	%rax, %r10
	mov	%r10, 8(%rsi)
	adc	$0, %rdx
	mov	%rdx, 16(%rsi,%rcx,8)	
	mov	%rbx, %r13		
	lea	8(%rsi), %rsi		
	dec	%r15
	jnz	Lotp1
	jmp	Lcj

Lb0:	cmp	$-2, %ecx
	jz	Ln2
	cmp	$-4, %ecx
	jz	Ln4

	push	%rdi

Lotp0:lea	4(%rcx), %r14
	mov	(%r12,%rcx,8), %rax
	mul	%r13
	lea	(%rax), %r11
	mov	8(%r12,%rcx,8), %rax
	lea	(%rdx), %rdi
	mul	%r13
	lea	(%rax), %rbp
	mov	16(%r12,%rcx,8), %rax
	mov	16(%rsi,%rcx,8), %r10
	lea	(%rdx), %r9
	mul	%r13
	add	%r11, %r10
	lea	(%rax), %r11
	mov	24(%r12,%rcx,8), %rax
	adc	%rdi, %rbp
	mov	24(%rsi,%rcx,8), %rbx
	lea	(%rdx), %rdi
	adc	$0, %r9
	mul	%r13
	add	%rbp, %rbx
	lea	(%rax), %rbp
	mov	32(%r12,%rcx,8), %rax
	adc	%r9, %r11
	mov	%rbx, 24(%rsi,%rcx,8)
	mov	32(%rsi,%rcx,8), %r10
	lea	(%rdx), %r9
	adc	$0, %rdi
	imul	%r8, %rbx		
	jmp	Le0

	.align	16, 0x90
Ltp0:	mul	%r13
	add	%rbp, %r10
	lea	(%rax), %rbp
	mov	(%r12,%r14,8), %rax
	adc	%r9, %r11
	mov	%r10, -8(%rsi,%r14,8)
	mov	(%rsi,%r14,8), %r10
	lea	(%rdx), %r9
	adc	$0, %rdi
Le0:	mul	%r13
	add	%r11, %r10
	lea	(%rax), %r11
	mov	8(%r12,%r14,8), %rax
	adc	%rdi, %rbp
	mov	%r10, (%rsi,%r14,8)
	mov	8(%rsi,%r14,8), %r10
	lea	(%rdx), %rdi
	adc	$0, %r9
	add	$2, %r14
	js	Ltp0

Led0:	mul	%r13
	add	%rbp, %r10
	adc	%r9, %r11
	mov	%r10, -8(%rsi)
	mov	(%rsi), %r10
	adc	$0, %rdi
	add	%r11, %r10
	adc	%rdi, %rax
	mov	%r10, (%rsi)
	mov	8(%rsi), %r10
	adc	$0, %rdx
	add	%rax, %r10
	mov	%r10, 8(%rsi)
	adc	$0, %rdx
	mov	%rdx, 16(%rsi,%rcx,8)	
	mov	%rbx, %r13		
	lea	8(%rsi), %rsi		
	dec	%r15
	jnz	Lotp0

Lcj:	lea	16(%rsi), %rsi		
	pop	%rdi
Ladd_n:
	

	lea	(%rsi,%rcx,8), %rdx		
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

Ln1:	mov	(%rdx), %rax
	mul	%r13
	add	8(%rsi), %rax
	adc	16(%rsi), %rdx
	mov	%rdx, (%rdi)
	mov	$0, %eax
	adc	%eax, %eax
	jmp	Lret

Ln2:	mov	(%rdx), %rax
	mov	(%rsi), %rbp
	mul	%r13
	add	%rax, %rbp
	mov	%rdx, %r9
	adc	$0, %r9
	mov	-8(%r12), %rax
	mov	8(%rsi), %r10
	mul	%r13
	add	%rax, %r10
	mov	%rdx, %r11
	adc	$0, %r11
	add	%r9, %r10
	adc	$0, %r11
	mov	%r10, %r13
	imul	%r8, %r13		
	mov	-16(%r12), %rax
	mul	%r13
	add	%rax, %r10
	mov	%rdx, %r9
	adc	$0, %r9
	mov	-8(%r12), %rax
	mov	16(%rsi), %r14
	mul	%r13
	add	%rax, %r14
	adc	$0, %rdx
	add	%r9, %r14
	adc	$0, %rdx
	xor	%eax, %eax
	add	%r11, %r14
	adc	24(%rsi), %rdx
	mov	%r14, (%rdi)
	mov	%rdx, 8(%rdi)
	adc	%eax, %eax
	jmp	Lret

	.align	16, 0x90
Ln3:	mov	-24(%r12), %rax
	mov	-8(%rsi), %r10
	mul	%r13
	add	%rax, %r10
	mov	-16(%r12), %rax
	mov	%rdx, %r11
	adc	$0, %r11
	mov	(%rsi), %rbp
	mul	%r13
	add	%rax, %rbp
	mov	%rdx, %r9
	adc	$0, %r9
	mov	-8(%r12), %rax
	add	%r11, %rbp
	mov	8(%rsi), %r10
	adc	$0, %r9
	mul	%r13
	mov	%rbp, %r13
	imul	%r8, %r13		
	add	%rax, %r10
	mov	%rdx, %r11
	adc	$0, %r11
	mov	%rbp, (%rsi)
	add	%r9, %r10
	adc	$0, %r11
	mov	%r10, 8(%rsi)
	mov	%r11, -8(%rsi)		
	lea	8(%rsi), %rsi		
	dec	%r15
	jnz	Ln3

	mov	-32(%rsi), %rdx
	mov	-24(%rsi), %rbx
	xor	%eax, %eax
	add	%rbp, %rdx
	adc	%r10, %rbx
	adc	8(%rsi), %r11
	mov	%rdx, (%rdi)
	mov	%rbx, 8(%rdi)
	mov	%r11, 16(%rdi)
	adc	%eax, %eax
	jmp	Lret

	.align	16, 0x90
Ln4:	mov	-32(%r12), %rax
	mul	%r13
	lea	(%rax), %r11
	mov	-24(%r12), %rax
	lea	(%rdx), %r14
	mul	%r13
	lea	(%rax), %rbp
	mov	-16(%r12), %rax
	mov	-16(%rsi), %r10
	lea	(%rdx), %r9
	mul	%r13
	add	%r11, %r10
	lea	(%rax), %r11
	mov	-8(%r12), %rax
	adc	%r14, %rbp
	mov	-8(%rsi), %rbx
	lea	(%rdx), %r14
	adc	$0, %r9
	mul	%r13
	add	%rbp, %rbx
	adc	%r9, %r11
	mov	%rbx, -8(%rsi)
	mov	(%rsi), %r10
	adc	$0, %r14
	imul	%r8, %rbx		
	add	%r11, %r10
	adc	%r14, %rax
	mov	%r10, (%rsi)
	mov	8(%rsi), %r10
	adc	$0, %rdx
	add	%rax, %r10
	mov	%r10, 8(%rsi)
	adc	$0, %rdx
	mov	%rdx, -16(%rsi)		
	mov	%rbx, %r13		
	lea	8(%rsi), %rsi		
	dec	%r15
	jnz	Ln4
	lea	16(%rsi), %rsi
	jmp	Ladd_n
	

