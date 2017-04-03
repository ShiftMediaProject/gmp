




























































	.text
	.align	32, 0x90
	.globl	__gmpn_com
	
__gmpn_com:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx

	movq	(%rsi), %r8
	movl	%edx, %eax
	leaq	(%rsi,%rdx,8), %rsi
	leaq	(%rdi,%rdx,8), %rdi
	negq	%rdx
	andl	$3, %eax
	je	Lb00
	cmpl	$2, %eax
	jc	Lb01
	je	Lb10

Lb11:	notq	%r8
	movq	%r8, (%rdi,%rdx,8)
	decq	%rdx
	jmp	Le11
Lb10:	addq	$-2, %rdx
	jmp	Le10
	.byte	0x90,0x90,0x90,0x90,0x90,0x90
Lb01:	notq	%r8
	movq	%r8, (%rdi,%rdx,8)
	incq	%rdx
	jz	Lret

Loop:	movq	(%rsi,%rdx,8), %r8
Lb00:	movq	8(%rsi,%rdx,8), %r9
	notq	%r8
	notq	%r9
	movq	%r8, (%rdi,%rdx,8)
	movq	%r9, 8(%rdi,%rdx,8)
Le11:	movq	16(%rsi,%rdx,8), %r8
Le10:	movq	24(%rsi,%rdx,8), %r9
	notq	%r8
	notq	%r9
	movq	%r8, 16(%rdi,%rdx,8)
	movq	%r9, 24(%rdi,%rdx,8)
	addq	$4, %rdx
	jnc	Loop
Lret:	pop	%rsi
	pop	%rdi
	ret
	
