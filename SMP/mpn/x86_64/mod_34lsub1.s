




































































	.text
	.align	32, 0x90
	.globl	__gmpn_mod_34lsub1
	
__gmpn_mod_34lsub1:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi


	mov	$0x0000FFFFFFFFFFFF, %r11

	sub	$2, %rsi
	ja	Lgt2

	mov	(%rdi), %rax
	nop
	jb	L1

	mov	8(%rdi), %rsi
	mov	%rax, %rdx
	shr	$48, %rax		

	and	%r11, %rdx		
	add	%rdx, %rax
	mov	%esi, %edx

	shr	$32, %rsi		
	add	%rsi, %rax

	shl	$16, %rdx		
	add	%rdx, %rax

L1:	pop	%rsi
	pop	%rdi
	ret


	.align	16, 0x90
Lgt2:	xor	%eax, %eax
	xor	%ecx, %ecx
	xor	%edx, %edx
	xor	%r8, %r8
	xor	%r9, %r9
	xor	%r10, %r10

Ltop:	add	(%rdi), %rax
	adc	$0, %r10
	add	8(%rdi), %rcx
	adc	$0, %r8
	add	16(%rdi), %rdx
	adc	$0, %r9

	sub	$3, %rsi
	jng	Lend

	add	24(%rdi), %rax
	adc	$0, %r10
	add	32(%rdi), %rcx
	adc	$0, %r8
	add	40(%rdi), %rdx
	lea	48(%rdi), %rdi
	adc	$0, %r9

	sub	$3, %rsi
	jg	Ltop


	add	$-24, %rdi
Lend:	add	%r9, %rax
	adc	%r10, %rcx
	adc	%r8, %rdx

	inc	%rsi
	mov	$0x1, %r10d
	js	Lcombine

	mov	$0x10000, %r10d
	adc	24(%rdi), %rax
	dec	%rsi
	js	Lcombine

	adc	32(%rdi), %rcx
	mov	$0x100000000, %r10

Lcombine:
	sbb	%rsi, %rsi		
	mov	%rax, %rdi		
	shr	$48, %rax		

	and	%r10, %rsi		
	and	%r11, %rdi		
	mov	%ecx, %r10d	

	add	%rsi, %rax		
	shr	$32, %rcx		

	add	%rdi, %rax		
	movzwl	%dx, %edi		
	shl	$16, %r10		

	add	%rcx, %rax		
	shr	$16, %rdx		

	add	%r10, %rax		
	shl	$32, %rdi		

	add	%rdx, %rax		
	add	%rdi, %rax		

	pop	%rsi
	pop	%rdi
	ret
	
