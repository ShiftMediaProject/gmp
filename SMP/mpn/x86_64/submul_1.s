































































   
   
   
   







      
      







	 	 	 	 	 

	.text
	.align	32, 0x90
	.globl	__gmpn_submul_1
	
__gmpn_submul_1:


push	%rsi		
push	%rdi		
mov	%rdx, %rsi	

	mov	(%rsi), %rax
	push	%rbx
	lea	(%rsi,%r8,8), %rsi
	lea	(%rcx,%r8,8), %rcx

	test	$1, %r8b
	jnz	Lb13

Lb02:	xor	%r11d, %r11d
	test	$2, %r8b
	jnz	Lb2

Lb0:	mov	$1, %ebx
	sub	%r8, %rbx
	mul	%r9
	mov	%rdx, %rdi
	mov	-8(%rcx,%rbx,8), %r8
	jmp	Le0

	.align	16, 0x90
Lb2:	mov	$-1, %rbx
	sub	%r8, %rbx
	mul	%r9
	mov	8(%rcx,%rbx,8), %r8
	mov	%rdx, %rdi
	jmp	Le2

	.align	16, 0x90
Lb13:	xor	%edi, %edi
	test	$2, %r8b
	jnz	Lb3

Lb1:	mov	$2, %ebx
	sub	%r8, %rbx
	jns	L1
	mul	%r9
	mov	-16(%rcx,%rbx,8), %r10
	mov	%rdx, %r11
	jmp	Le1

	.align	16, 0x90
Lb3:	xor	%ebx, %ebx
	sub	%r8, %rbx
	mul	%r9
	mov	(%rcx,%rbx,8), %r10
	jmp	Le3

	.align	32, 0x90
Ltop:	mul	%r9
	mov	-16(%rcx,%rbx,8), %r10
	sub	%r11, %r8
	mov	%rdx, %r11
	adc	$0, %rdi
	mov	%r8, -24(%rcx,%rbx,8)
Le1:	sub	%rax, %r10
	mov	-8(%rsi,%rbx,8), %rax
	adc	$0, %r11
	mul	%r9
	sub	%rdi, %r10
	mov	%rdx, %rdi
	mov	-8(%rcx,%rbx,8), %r8
	adc	$0, %r11
	mov	%r10, -16(%rcx,%rbx,8)
Le0:	sub	%rax, %r8
	adc	$0, %rdi
	mov	(%rsi,%rbx,8), %rax
	mul	%r9
	mov	(%rcx,%rbx,8), %r10
	sub	%r11, %r8
	mov	%r8, -8(%rcx,%rbx,8)
	adc	$0, %rdi
Le3:	mov	%rdx, %r11
	sub	%rax, %r10
	mov	8(%rsi,%rbx,8), %rax
	adc	$0, %r11
	mul	%r9
	mov	8(%rcx,%rbx,8), %r8
	sub	%rdi, %r10
	mov	%rdx, %rdi
	mov	%r10, (%rcx,%rbx,8)
	adc	$0, %r11
Le2:	sub	%rax, %r8
	adc	$0, %rdi
	mov	16(%rsi,%rbx,8), %rax
	add	$4, %rbx
	jnc	Ltop

Lend:	mul	%r9
	mov	-8(%rcx), %r10
	sub	%r11, %r8
	mov	%rdx, %r11
	adc	$0, %rdi
	mov	%r8, -16(%rcx)
	sub	%rax, %r10
	adc	$0, %r11
	sub	%rdi, %r10
	adc	$0, %r11
	mov	%r10, -8(%rcx)
	mov	%r11, %rax

	pop	%rbx
pop	%rdi		
pop	%rsi		
	ret

	.align	16, 0x90
L1:	mul	%r9
	sub	%rax, -8(%rcx)
	mov	%rdx, %rax
	adc	$0, %rax
	pop	%rbx
pop	%rdi		
pop	%rsi		
	ret
	

