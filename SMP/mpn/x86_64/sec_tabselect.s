






























































































	.text
	.align	16, 0x90
	.globl	__gmpn_sec_tabselect
	
	.def	__gmpn_sec_tabselect
	.scl	2
	.type	32
	.endef
__gmpn_sec_tabselect:

	push	%rdi
	push	%rsi
	mov	%rcx, %rdi
	mov	%rdx, %rsi
	mov	%r8, %rdx
	mov	%r9, %rcx

	mov	56(%rsp), %r8d	

	add	$-88, %rsp	
	movdqu	%xmm6, (%rsp)	
	movdqu	%xmm7, 16(%rsp)	
	movdqu	%xmm8, 32(%rsp)	
	movdqu	%xmm9, 48(%rsp)	

	movd	%r8, %xmm8
	pshufd	$0, %xmm8, %xmm8	
	mov	$1, %eax
	movd	%rax, %xmm9
	pshufd	$0, %xmm9, %xmm9	

	mov	%rdx, %r9
	add	$-8, %r9
	js	Louter_end

Louter_top:
	mov	%rcx, %r10
	mov	%rsi, %r11
	pxor	%xmm1, %xmm1
	pxor	%xmm4, %xmm4
	pxor	%xmm5, %xmm5
	pxor	%xmm6, %xmm6
	pxor	%xmm7, %xmm7
	.align	16, 0x90
Ltop:	movdqa	%xmm8, %xmm0
	pcmpeqd	%xmm1, %xmm0
	paddd	%xmm9, %xmm1
	movdqu	0(%rsi), %xmm2
	movdqu	16(%rsi), %xmm3
	pand	%xmm0, %xmm2
	pand	%xmm0, %xmm3
	por	%xmm2, %xmm4
	por	%xmm3, %xmm5
	movdqu	32(%rsi), %xmm2
	movdqu	48(%rsi), %xmm3
	pand	%xmm0, %xmm2
	pand	%xmm0, %xmm3
	por	%xmm2, %xmm6
	por	%xmm3, %xmm7
	lea	(%rsi,%rdx,8), %rsi
	add	$-1, %r10
	jne	Ltop

	movdqu	%xmm4, 0(%rdi)
	movdqu	%xmm5, 16(%rdi)
	movdqu	%xmm6, 32(%rdi)
	movdqu	%xmm7, 48(%rdi)

	lea	64(%r11), %rsi
	lea	64(%rdi), %rdi
	add	$-8, %r9
	jns	Louter_top
Louter_end:

	test	$4, %dl
	je	Lb0xx
Lb1xx:mov	%rcx, %r10
	mov	%rsi, %r11
	pxor	%xmm1, %xmm1
	pxor	%xmm4, %xmm4
	pxor	%xmm5, %xmm5
	.align	16, 0x90
Ltp4:	movdqa	%xmm8, %xmm0
	pcmpeqd	%xmm1, %xmm0
	paddd	%xmm9, %xmm1
	movdqu	0(%rsi), %xmm2
	movdqu	16(%rsi), %xmm3
	pand	%xmm0, %xmm2
	pand	%xmm0, %xmm3
	por	%xmm2, %xmm4
	por	%xmm3, %xmm5
	lea	(%rsi,%rdx,8), %rsi
	add	$-1, %r10
	jne	Ltp4
	movdqu	%xmm4, 0(%rdi)
	movdqu	%xmm5, 16(%rdi)
	lea	32(%r11), %rsi
	lea	32(%rdi), %rdi

Lb0xx:test	$2, %dl
	je	Lb00x
Lb01x:mov	%rcx, %r10
	mov	%rsi, %r11
	pxor	%xmm1, %xmm1
	pxor	%xmm4, %xmm4
	.align	16, 0x90
Ltp2:	movdqa	%xmm8, %xmm0
	pcmpeqd	%xmm1, %xmm0
	paddd	%xmm9, %xmm1
	movdqu	0(%rsi), %xmm2
	pand	%xmm0, %xmm2
	por	%xmm2, %xmm4
	lea	(%rsi,%rdx,8), %rsi
	add	$-1, %r10
	jne	Ltp2
	movdqu	%xmm4, 0(%rdi)
	lea	16(%r11), %rsi
	lea	16(%rdi), %rdi

Lb00x:test	$1, %dl
	je	Lb000
Lb001:mov	%rcx, %r10
	mov	%rsi, %r11
	pxor	%xmm1, %xmm1
	pxor	%xmm4, %xmm4
	.align	16, 0x90
Ltp1:	movdqa	%xmm8, %xmm0
	pcmpeqd	%xmm1, %xmm0
	paddd	%xmm9, %xmm1
	movq	0(%rsi), %xmm2
	pand	%xmm0, %xmm2
	por	%xmm2, %xmm4
	lea	(%rsi,%rdx,8), %rsi
	add	$-1, %r10
	jne	Ltp1
	movq	%xmm4, 0(%rdi)

Lb000:
	movdqu	(%rsp), %xmm6	
	movdqu	16(%rsp), %xmm7	
	movdqu	32(%rsp), %xmm8	
	movdqu	48(%rsp), %xmm9	
	add	$88, %rsp	
	pop	%rsi
	pop	%rdi
	ret
	

