















































































	.text
	.align	32, 0x90
	.globl	__gmpn_hamdist
	
__gmpn_hamdist:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx

	mov	(%rdi), %r8
	xor	(%rsi), %r8

	lea	(%rdi,%rdx,8), %rdi			
	lea	(%rsi,%rdx,8), %rsi			
	neg	%rdx

	bt	$0, %edx
	jnc	L2

L1:	.byte	0xf3,0x49,0x0f,0xb8,0xc0	
	xor	%r10d, %r10d
	add	$1, %rdx
	js	Ltop
	pop	%rsi
	pop	%rdi
	ret

	.align	16, 0x90
L2:	mov	8(%rdi,%rdx,8), %r9
	.byte	0xf3,0x49,0x0f,0xb8,0xc0	
	xor	8(%rsi,%rdx,8), %r9
	.byte	0xf3,0x4d,0x0f,0xb8,0xd1	
	add	$2, %rdx
	js	Ltop
	lea	(%r10, %rax), %rax
	pop	%rsi
	pop	%rdi
	ret

	.align	16, 0x90
Ltop:	mov	(%rdi,%rdx,8), %r8
	lea	(%r10, %rax), %rax
	mov	8(%rdi,%rdx,8), %r9
	xor	(%rsi,%rdx,8), %r8
	xor	8(%rsi,%rdx,8), %r9
	.byte	0xf3,0x49,0x0f,0xb8,0xc8	
	lea	(%rcx, %rax), %rax
	.byte	0xf3,0x4d,0x0f,0xb8,0xd1	
	add	$2, %rdx
	js	Ltop

	lea	(%r10, %rax), %rax
	pop	%rsi
	pop	%rdi
	ret
	

