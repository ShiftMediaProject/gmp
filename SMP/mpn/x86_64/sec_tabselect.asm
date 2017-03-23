section .text
align 16
global	__gmpn_sec_tabselect
__gmpn_sec_tabselect:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	mov	r8d, [rsp+56]
	movq	xmm8, r8
	pshufd	xmm8, xmm8, 0
	mov	eax, 1
	movq	xmm9, rax
	pshufd	xmm9, xmm9, 0
	mov	r9, rdx
	add	r9, -8
	js	Louter_end
Louter_top:
	mov	r10, rcx
	mov	r11, rsi
	pxor	xmm13, xmm13
	pxor	xmm4, xmm4
	pxor	xmm5, xmm5
	pxor	xmm6, xmm6
	pxor	xmm7, xmm7
align 16
Ltop:
	movdqa	xmm0, xmm8
	pcmpeqd	xmm0, xmm13
	paddd	xmm13, xmm9
	movdqu	xmm2, [rsi+0]
	movdqu	xmm3, [rsi+16]
	pand	xmm2, xmm0
	pand	xmm3, xmm0
	por	xmm4, xmm2
	por	xmm5, xmm3
	movdqu	xmm2, [rsi+32]
	movdqu	xmm3, [rsi+48]
	pand	xmm2, xmm0
	pand	xmm3, xmm0
	por	xmm6, xmm2
	por	xmm7, xmm3
	lea	rsi, [rsi+rdx*8]
	add	r10, -1
	jne	Ltop
	movdqu	[rdi+0], xmm4
	movdqu	[rdi+16], xmm5
	movdqu	[rdi+32], xmm6
	movdqu	[rdi+48], xmm7
	lea	rsi, [r11+64]
	lea	rdi, [rdi+64]
	add	r9, -8
	jns	Louter_top
Louter_end:
	test	dl, 4
	je	Lb0xx
Lb1xx:
	mov	r10, rcx
	mov	r11, rsi
	pxor	xmm13, xmm13
	pxor	xmm4, xmm4
	pxor	xmm5, xmm5
align 16
Ltp4:
	movdqa	xmm0, xmm8
	pcmpeqd	xmm0, xmm13
	paddd	xmm13, xmm9
	movdqu	xmm2, [rsi+0]
	movdqu	xmm3, [rsi+16]
	pand	xmm2, xmm0
	pand	xmm3, xmm0
	por	xmm4, xmm2
	por	xmm5, xmm3
	lea	rsi, [rsi+rdx*8]
	add	r10, -1
	jne	Ltp4
	movdqu	[rdi+0], xmm4
	movdqu	[rdi+16], xmm5
	lea	rsi, [r11+32]
	lea	rdi, [rdi+32]
Lb0xx:
	test	dl, 2
	je	Lb00x
Lb01x:
	mov	r10, rcx
	mov	r11, rsi
	pxor	xmm13, xmm13
	pxor	xmm4, xmm4
align 16
Ltp2:
	movdqa	xmm0, xmm8
	pcmpeqd	xmm0, xmm13
	paddd	xmm13, xmm9
	movdqu	xmm2, [rsi+0]
	pand	xmm2, xmm0
	por	xmm4, xmm2
	lea	rsi, [rsi+rdx*8]
	add	r10, -1
	jne	Ltp2
	movdqu	[rdi+0], xmm4
	lea	rsi, [r11+16]
	lea	rdi, [rdi+16]
Lb00x:
	test	dl, 1
	je	Lb000
Lb001:
	mov	r10, rcx
	mov	r11, rsi
	pxor	xmm13, xmm13
	pxor	xmm4, xmm4
align 16
Ltp1:
	movdqa	xmm0, xmm8
	pcmpeqd	xmm0, xmm13
	paddd	xmm13, xmm9
	movq	xmm2, [rsi+0]
	pand	xmm2, xmm0
	por	xmm4, xmm2
	lea	rsi, [rsi+rdx*8]
	add	r10, -1
	jne	Ltp1
	movq	 [rdi+0], xmm4
Lb000:
	pop	rsi
	pop	rdi
	ret
