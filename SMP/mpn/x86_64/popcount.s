










































































	.text
	.align	32, 0x90
	.globl	__gmpn_popcount
	
__gmpn_popcount:

	lea	Lcnsts(%rip), %r9

 
	movdqa	32(%r9), %xmm7
	movdqa	48(%r9), %xmm6
	pxor	%xmm4, %xmm4
	pxor	%xmm5, %xmm5
	pxor	%xmm8, %xmm8

	mov	%esi, %eax
	and	$7, %eax

	movslq	(%r9,%rax,4), %rax
	add	%r9, %rax
	jmp	*%rax


L1:	movq	(%rdi), %xmm1
	add	$8, %rdi
	jmp	Le1

L2:	add	$-48, %rdi
	jmp	Le2

L3:	movq	(%rdi), %xmm1
	add	$-40, %rdi
	jmp	Le3

L4:	add	$-32, %rdi
	jmp	Le4

L5:	movq	(%rdi), %xmm1
	add	$-24, %rdi
	jmp	Le5

L6:	add	$-16, %rdi
	jmp	Le6

L7:	movq	(%rdi), %xmm1
	add	$-8, %rdi
	jmp	Le7

	.align	32, 0x90
Ltop:	lddqu	(%rdi), %xmm1
Le7:	movdqa	%xmm6, %xmm0		
	movdqa	%xmm7, %xmm2		
	movdqa	%xmm7, %xmm3		
	pand	%xmm1, %xmm0
	psrlw	$4, %xmm1
	pand	%xmm6, %xmm1
	pshufb	%xmm0, %xmm2
	pshufb	%xmm1, %xmm3
	paddb	%xmm2, %xmm3
	paddb	%xmm3, %xmm4
Le6:	lddqu	16(%rdi), %xmm1
Le5:	movdqa	%xmm6, %xmm0
	movdqa	%xmm7, %xmm2
	movdqa	%xmm7, %xmm3
	pand	%xmm1, %xmm0
	psrlw	$4, %xmm1
	pand	%xmm6, %xmm1
	pshufb	%xmm0, %xmm2
	pshufb	%xmm1, %xmm3
	paddb	%xmm2, %xmm3
	paddb	%xmm3, %xmm4
Le4:	lddqu	32(%rdi), %xmm1
Le3:	movdqa	%xmm6, %xmm0
	movdqa	%xmm7, %xmm2
	movdqa	%xmm7, %xmm3
	pand	%xmm1, %xmm0
	psrlw	$4, %xmm1
	pand	%xmm6, %xmm1
	pshufb	%xmm0, %xmm2
	pshufb	%xmm1, %xmm3
	paddb	%xmm2, %xmm3
	paddb	%xmm3, %xmm4
Le2:	lddqu	48(%rdi), %xmm1
	add	$64, %rdi
Le1:	movdqa	%xmm6, %xmm0
	movdqa	%xmm7, %xmm2
	movdqa	%xmm7, %xmm3
	pand	%xmm1, %xmm0
	psrlw	$4, %xmm1
	pand	%xmm6, %xmm1
	pshufb	%xmm0, %xmm2
	pshufb	%xmm1, %xmm3
	psadbw	%xmm5, %xmm4		
	paddb	%xmm2, %xmm3
	paddq	%xmm4, %xmm8		
	movdqa	%xmm3, %xmm4
	sub	$8, %rsi
	jg	Ltop

	psadbw	%xmm5, %xmm4
	paddq	%xmm4, %xmm8
	pshufd	$14, %xmm8, %xmm0
	paddq	%xmm8, %xmm0
	movq	%xmm0, %rax
	ret
	
	.section .rdata,"dr"
	.align	16, 0x90
Lcnsts:

	.long	Ltop-Lcnsts
	.long	L1-Lcnsts
	.long	L2-Lcnsts
	.long	L3-Lcnsts
	.long	L4-Lcnsts
	.long	L5-Lcnsts
	.long	L6-Lcnsts
	.long	L7-Lcnsts
	.byte	0x00,0x01,0x01,0x02,0x01,0x02,0x02,0x03
	.byte	0x01,0x02,0x02,0x03,0x02,0x03,0x03,0x04
	.byte	0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f
	.byte	0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f
	
