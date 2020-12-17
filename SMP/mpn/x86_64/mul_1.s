






































































   
   
   
   






		 		 		 		 		 		 

	.text
	.align	16, 0x90
	.globl	__gmpn_mul_1c
	
	.def	__gmpn_mul_1c
	.scl	2
	.type	32
	.endef
__gmpn_mul_1c:

push	%rsi		
push	%rdi		
mov	%rdx, %rsi	
	push	%rbx

	mov	64(%rsp), %r10	
	jmp	Lcommon
	

	.globl	__gmpn_mul_1
	
	.def	__gmpn_mul_1
	.scl	2
	.type	32
	.endef
__gmpn_mul_1:

push	%rsi		
push	%rdi		
mov	%rdx, %rsi	

	push	%rbx
	xor	%r10, %r10
Lcommon:
	mov	(%rsi), %rax		
	
	mov	%r8, %rbx         
	mul	%r9


	add	%r10, %rax
	adc	$0, %rdx

	and	$3, %ebx
	jz	Lb0
	cmp	$2, %ebx
	jz	Lb2
	jg	Lb3

Lb1:	dec	%r8
	jne	Lgt1
	mov	%rax, (%rcx)
	jmp	Lret
Lgt1:	lea	8(%rsi,%r8,8), %rsi
	lea	-8(%rcx,%r8,8), %rcx
	neg	%r8
	xor	%r10, %r10
	xor	%ebx, %ebx
	mov	%rax, %rdi
	mov	(%rsi,%r8,8), %rax
	mov	%rdx, %r11
	jmp	LL1

Lb0:	lea	(%rsi,%r8,8), %rsi
	lea	-16(%rcx,%r8,8), %rcx
	neg	%r8
	xor	%r10, %r10
	mov	%rax, %r11
	mov	%rdx, %rbx
	jmp	 LL0

Lb3:	lea	-8(%rsi,%r8,8), %rsi
	lea	-24(%rcx,%r8,8), %rcx
	neg	%r8
	mov	%rax, %rbx
	mov	%rdx, %r10
	jmp	LL3

Lb2:	lea	-16(%rsi,%r8,8), %rsi
	lea	-32(%rcx,%r8,8), %rcx
	neg	%r8
	xor	%r11, %r11
	xor	%ebx, %ebx
	mov	%rax, %r10
	mov	24(%rsi,%r8,8), %rax
	mov	%rdx, %rdi
	jmp	LL2

	.align	16, 0x90
Ltop:	mov	%r10, (%rcx,%r8,8)
	add	%rax, %rdi
	mov	(%rsi,%r8,8), %rax
	adc	%rdx, %r11
	mov	$0, %r10d
LL1:	mul	%r9
	mov	%rdi, 8(%rcx,%r8,8)
	add	%rax, %r11
	adc	%rdx, %rbx
LL0:	mov	8(%rsi,%r8,8), %rax
	mul	%r9
	mov	%r11, 16(%rcx,%r8,8)
	add	%rax, %rbx
	adc	%rdx, %r10
LL3:	mov	16(%rsi,%r8,8), %rax
	mul	%r9
	mov	%rbx, 24(%rcx,%r8,8)
	mov	$0, %r11d		
	mov	%r11, %rbx		
	add	%rax, %r10
	mov	24(%rsi,%r8,8), %rax
	mov	%r11, %rdi		
	adc	%rdx, %rdi
LL2:	mul	%r9
	add	$4, %r8
	js	 Ltop

	mov	%r10, (%rcx,%r8,8)
	add	%rax, %rdi
	adc	%r11, %rdx
	mov	%rdi, 8(%rcx,%r8,8)
	add	%r11, %rdx
Lret:	mov	%rdx, %rax

	pop	%rbx
pop	%rdi		
pop	%rsi		
	ret
	
