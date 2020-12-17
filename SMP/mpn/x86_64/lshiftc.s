














































































	.text
	.align	16, 0x90
	.globl	__gmpn_lshiftc
	
	.def	__gmpn_lshiftc
	.scl	2
	.type	32
	.endef
__gmpn_lshiftc:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx


	xor	%eax, %eax

	test	$1, %dl
	jnz	Lbx1
Lbx0:	test	$2, %dl
	jnz	Lb10

Lb00:	lea	-8(%rsi,%rdx,8), %rsi
	lea	16(%rdi,%rdx,8), %rdi
	mov	(%rsi), %r10
	mov	-8(%rsi), %r11
	shld	%cl, %r10, %rax
	mov	-16(%rsi), %r8
	shr	$2, %rdx
	shld	%cl, %r11, %r10
	jmp	L00

Lbx1:	test	$2, %dl
	jnz	Lb11

Lb01:	lea	-16(%rsi,%rdx,8), %rsi
	lea	8(%rdi,%rdx,8), %rdi
	mov	8(%rsi), %r9
	shld	%cl, %r9, %rax
	shr	$2, %rdx
	jz	L1
	mov	(%rsi), %r10
	mov	-8(%rsi), %r11
	shld	%cl, %r10, %r9
	jmp	L01

Lb10:	lea	-24(%rsi,%rdx,8), %rsi
	lea	(%rdi,%rdx,8), %rdi
	mov	16(%rsi), %r8
	mov	8(%rsi), %r9
	shld	%cl, %r8, %rax
	shr	$2, %rdx
	jz	L2
	mov	(%rsi), %r10
	shld	%cl, %r9, %r8
	jmp	L10

	.align	16, 0x90
Lb11:	lea	-32(%rsi,%rdx,8), %rsi
	lea	-8(%rdi,%rdx,8), %rdi
	mov	24(%rsi), %r11
	mov	16(%rsi), %r8
	mov	8(%rsi), %r9
	shld	%cl, %r11, %rax
	shr	$2, %rdx
	jz	Lend

	.align	16, 0x90
Ltop:	shld	%cl, %r8, %r11
	mov	(%rsi), %r10
	not	%r11
	shld	%cl, %r9, %r8
	mov	%r11, (%rdi)
L10:	mov	-8(%rsi), %r11
	not	%r8
	shld	%cl, %r10, %r9
	mov	%r8, -8(%rdi)
L01:	mov	-16(%rsi), %r8
	not	%r9
	shld	%cl, %r11, %r10
	mov	%r9, -16(%rdi)
L00:	mov	-24(%rsi), %r9
	not	%r10
	add	$-32, %rsi
	mov	%r10, -24(%rdi)
	add	$-32, %rdi
	dec	%rdx
	jnz	Ltop

Lend:	shld	%cl, %r8, %r11
	not	%r11
	mov	%r11, (%rdi)
L2:	shld	%cl, %r9, %r8
	not	%r8
	mov	%r8, -8(%rdi)
L1:	shl	%cl, %r9
	not	%r9
	mov	%r9, -16(%rdi)
	pop	%rsi
	pop	%rdi
	ret
	
