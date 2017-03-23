section .text
align 64
global	__gmpn_lshiftc
__gmpn_lshiftc:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	movd	xmm4, ecx
	mov	eax, 64
	sub	eax, ecx
	movd	xmm5, eax
	neg	ecx
	mov	rax, [rsi+rdx*8-8]
	shr	rax, cl
	pcmpeqb	xmm3, xmm3
	cmp	rdx, 3
	jle	Lbc
	lea	ecx, [rdi+rdx*8]
	test	cl, 8
	jz	Lrp_aligned
	movq	xmm0, [rsi+rdx*8-8]
	movq	xmm1, [rsi+rdx*8-16]
	psllq	xmm0, xmm4
	psrlq	xmm1, xmm5
	por	xmm0, xmm1
	pxor	xmm0, xmm3
	movq	 [rdi+rdx*8-8], xmm0
	dec	rdx
Lrp_aligned:
	lea	r8d, [rdx+1]
	and	r8d, 6
	jz	Lba0
	cmp	r8d, 4
	jz	Lba4
	jc	Lba2
Lba6:
	add	rdx, -4
	jmp	qword Li56
Lba0:
	add	rdx, -6
	jmp	qword Li70
Lba4:
	add	rdx, -2
	jmp	qword Li34
Lba2:
	add	rdx, -8
	jle	Lend
align 16
Ltop:
	movdqu	xmm1, [rsi+rdx*8+40]
	movdqu	xmm0, [rsi+rdx*8+48]
	psllq	xmm0, xmm4
	psrlq	xmm1, xmm5
	por	xmm0, xmm1
	pxor	xmm0, xmm3
	movdqa	[rdi+rdx*8+48], xmm0
Li70:
	movdqu	xmm1, [rsi+rdx*8+24]
	movdqu	xmm0, [rsi+rdx*8+32]
	psllq	xmm0, xmm4
	psrlq	xmm1, xmm5
	por	xmm0, xmm1
	pxor	xmm0, xmm3
	movdqa	[rdi+rdx*8+32], xmm0
Li56:
	movdqu	xmm1, [rsi+rdx*8+8]
	movdqu	xmm0, [rsi+rdx*8+16]
	psllq	xmm0, xmm4
	psrlq	xmm1, xmm5
	por	xmm0, xmm1
	pxor	xmm0, xmm3
	movdqa	[rdi+rdx*8+16], xmm0
Li34:
	movdqu	xmm1, [rsi+rdx*8-8]
	movdqu	xmm0, [rsi+rdx*8]
	psllq	xmm0, xmm4
	psrlq	xmm1, xmm5
	por	xmm0, xmm1
	pxor	xmm0, xmm3
	movdqa	[rdi+rdx*8], xmm0
	sub	rdx, 8
	jg	Ltop
Lend:
	test	dl, 1
	jnz	Lend8
	movdqu	xmm1, [rsi]
	pxor	xmm0, xmm0
	punpcklqdq	xmm0, xmm1
	psllq	xmm1, xmm4
	psrlq	xmm0, xmm5
	por	xmm0, xmm1
	pxor	xmm0, xmm3
	movdqa	[rdi], xmm0
	pop	rsi
	pop	rdi
	ret
align 16
Lbc:
	dec	edx
	jz	Lend8
	movq	xmm1, [rsi+rdx*8]
	movq	xmm0, [rsi+rdx*8-8]
	psllq	xmm1, xmm4
	psrlq	xmm0, xmm5
	por	xmm0, xmm1
	pxor	xmm0, xmm3
	movq	 [rdi+rdx*8], xmm0
	sub	edx, 2
	jl	Lend8
	movq	xmm1, [rsi+8]
	movq	xmm0, [rsi]
	psllq	xmm1, xmm4
	psrlq	xmm0, xmm5
	por	xmm0, xmm1
	pxor	xmm0, xmm3
	movq	 [rdi+8], xmm0
Lend8:
	movq	xmm0, [rsi]
	psllq	xmm0, xmm4
	pxor	xmm0, xmm3
	movq	 [rdi], xmm0
	pop	rsi
	pop	rdi
	ret
