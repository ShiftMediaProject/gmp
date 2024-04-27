





















































































	.text
	.align	32, 0x90
	.globl	__gmpn_hamdist
	
	.def	__gmpn_hamdist
	.scl	2
	.type	32
	.endef
__gmpn_hamdist:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx

	push	%rbx
	push	%rbp

	mov	(%rdi), %r10
	xor	(%rsi), %r10

	mov	%edx, %r8d
	and	$3, %r8d

	xor	%ecx, %ecx
	.byte	0xf3,0x49,0x0f,0xb8,0xc2	

	lea	Ltab(%rip), %r9

	movslq	(%r9,%r8,4), %r8
	add	%r9, %r8
	jmp	*%r8


L3:	mov	8(%rdi), %r10
	mov	16(%rdi), %r11
	xor	8(%rsi), %r10
	xor	16(%rsi), %r11
	xor	%ebp, %ebp
	sub	$4, %rdx
	jle	Lx3
	mov	24(%rdi), %r8
	mov	32(%rdi), %r9
	add	$24, %rdi
	add	$24, %rsi
	jmp	Le3

L0:	mov	8(%rdi), %r9
	xor	8(%rsi), %r9
	mov	16(%rdi), %r10
	mov	24(%rdi), %r11
	xor	%ebx, %ebx
	xor	16(%rsi), %r10
	xor	24(%rsi), %r11
	add	$32, %rdi
	add	$32, %rsi
	sub	$4, %rdx
	jle	Lx4

	.align	16, 0x90
Ltop:
Le0:	.byte	0xf3,0x49,0x0f,0xb8,0xe9	
	mov	(%rdi), %r8
	mov	8(%rdi), %r9
	add	%rbx, %rax
Le3:	.byte	0xf3,0x49,0x0f,0xb8,0xda	
	xor	(%rsi), %r8
	xor	8(%rsi), %r9
	add	%rbp, %rcx
Le2:	.byte	0xf3,0x49,0x0f,0xb8,0xeb	
	mov	16(%rdi), %r10
	mov	24(%rdi), %r11
	add	$32, %rdi
	add	%rbx, %rax
Le1:	.byte	0xf3,0x49,0x0f,0xb8,0xd8	
	xor	16(%rsi), %r10
	xor	24(%rsi), %r11
	add	$32, %rsi
	add	%rbp, %rcx
	sub	$4, %rdx
	jg	Ltop

Lx4:	.byte	0xf3,0x49,0x0f,0xb8,0xe9	
	add	%rbx, %rax
Lx3:	.byte	0xf3,0x49,0x0f,0xb8,0xda	
	add	%rbp, %rcx
	.byte	0xf3,0x49,0x0f,0xb8,0xeb	
	add	%rbx, %rax
	add	%rbp, %rcx
Lx2:	add	%rcx, %rax
Lx1:	pop	%rbp
	pop	%rbx
	pop	%rsi
	pop	%rdi
	ret

L2:	mov	8(%rdi), %r11
	xor	8(%rsi), %r11
	sub	$2, %rdx
	jle	Ln2
	mov	16(%rdi), %r8
	mov	24(%rdi), %r9
	xor	%ebx, %ebx
	xor	16(%rsi), %r8
	xor	24(%rsi), %r9
	add	$16, %rdi
	add	$16, %rsi
	jmp	Le2
Ln2:	.byte	0xf3,0x49,0x0f,0xb8,0xcb	
	jmp	Lx2

L1:	dec	%rdx
	jle	Lx1
	mov	8(%rdi), %r8
	mov	16(%rdi), %r9
	xor	8(%rsi), %r8
	xor	16(%rsi), %r9
	xor	%ebp, %ebp
	mov	24(%rdi), %r10
	mov	32(%rdi), %r11
	add	$40, %rdi
	add	$8, %rsi
	jmp	Le1

	
		.section .rdata,"dr"
	.align	8, 0x90
Ltab:	.long	L0-Ltab
	.long	L1-Ltab
	.long	L2-Ltab
	.long	L3-Ltab
