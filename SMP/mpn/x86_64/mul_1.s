
































































   
   
   
   






	 	 	 	 	 	 	 

	.text
	.align	16, 0x90
	.globl	__gmpn_mul_1
	
__gmpn_mul_1:


push	%rsi		
push	%rdi		
mov	%rdx, %rsi	

	mov	(%rsi), %rax
	mov	%r8d, %r10d


	lea	(%rsi,%r8,8), %rsi
	lea	-8(%rcx,%r8,8), %rcx
	neg	%r8
	mul	%r9
	and	$3, %r10d
	jz	Lb0
	cmp	$2, %r10d
	jb	Lb1
	jz	Lb2

Lb3:	add	$-1, %r8
	mov	%rax, %rdi
	mov	%rdx, %r11
	mov	16(%rsi,%r8,8), %rax
	jmp	LL3

Lb1:	mov	%rax, %rdi
	mov	%rdx, %r11
	add	$1, %r8
	jnc	LL1
	mov	%rax, (%rcx)
	mov	%rdx, %rax
pop	%rdi		
pop	%rsi		
	ret

Lb2:	add	$-2, %r8
	mov	%rax, %r11
	mov	%rdx, %rdi
	mov	24(%rsi,%r8,8), %rax
	jmp	LL2

Lb0:	mov	%rax, %r11
	mov	%rdx, %rdi
	mov	8(%rsi,%r8,8), %rax
	jmp	LL0

	.align	8, 0x90
Ltop:	mov	%rdx, %r11
	add	%rax, %rdi
LL1:	mov	0(%rsi,%r8,8), %rax
	adc	$0, %r11
	mul	%r9
	add	%rax, %r11
	mov	%rdi, 0(%rcx,%r8,8)
	mov	8(%rsi,%r8,8), %rax
	mov	%rdx, %rdi
	adc	$0, %rdi
LL0:	mul	%r9
	mov	%r11, 8(%rcx,%r8,8)
	add	%rax, %rdi
	mov	%rdx, %r11
	mov	16(%rsi,%r8,8), %rax
	adc	$0, %r11
LL3:	mul	%r9
	mov	%rdi, 16(%rcx,%r8,8)
	mov	%rdx, %rdi
	add	%rax, %r11
	mov	24(%rsi,%r8,8), %rax
	adc	$0, %rdi
LL2:	mul	%r9
	mov	%r11, 24(%rcx,%r8,8)
	add	$4, %r8
	jnc	Ltop

Lend:	add	%rax, %rdi
	mov	%rdx, %rax
	adc	$0, %rax
	mov	%rdi, (%rcx)

pop	%rdi		
pop	%rsi		
	ret
	
