%assign LABEL1 1
%define @1f ..@L11
%macro define1@... 1
%define @1f ..@L1%{1}
%endmacro
%macro @@1 1
..@L1%{1}:
%define @1b ..@L1%{1}
%assign LABEL1 LABEL1+1
define1@... LABEL1
%endmacro
section .text
align 64
global	__gmpn_rshift
__gmpn_rshift:
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
	mov	rax, [rsi]
	shl	rax, cl
	cmp	rdx, 3
	jle	Lbc
	test	dil, 8
	jz	Lrp_aligned
	movq	xmm0, [rsi]
	movq	xmm1, [rsi+8]
	psrlq	xmm0, xmm4
	psllq	xmm1, xmm5
	por	xmm0, xmm1
	movq	 [rdi], xmm0
	lea	rsi, [rsi+8]
	lea	rdi, [rdi+8]
	dec	rdx
Lrp_aligned:
	lea	r8d, [rdx+1]
	lea	rsi, [rsi+rdx*8]
	lea	rdi, [rdi+rdx*8]
	neg	rdx
	and	r8d, 6
	jz	Lbu0
	cmp	r8d, 4
	jz	Lbu4
	jc	Lbu2
Lbu6:
	add	rdx, 4
	jmp	qword Li56
Lbu0:
	add	rdx, 6
	jmp	qword Li70
Lbu4:
	add	rdx, 2
	jmp	qword Li34
Lbu2:
	add	rdx, 8
	jge	Lend
align 16
Ltop:
	movdqu	xmm1, [rsi+rdx*8-64]
	movdqu	xmm0, [rsi+rdx*8-56]
	psllq	xmm0, xmm5
	psrlq	xmm1, xmm4
	por	xmm0, xmm1
	movdqa	[rdi+rdx*8-64], xmm0
Li70:
	movdqu	xmm1, [rsi+rdx*8-48]
	movdqu	xmm0, [rsi+rdx*8-40]
	psllq	xmm0, xmm5
	psrlq	xmm1, xmm4
	por	xmm0, xmm1
	movdqa	[rdi+rdx*8-48], xmm0
Li56:
	movdqu	xmm1, [rsi+rdx*8-32]
	movdqu	xmm0, [rsi+rdx*8-24]
	psllq	xmm0, xmm5
	psrlq	xmm1, xmm4
	por	xmm0, xmm1
	movdqa	[rdi+rdx*8-32], xmm0
Li34:
	movdqu	xmm1, [rsi+rdx*8-16]
	movdqu	xmm0, [rsi+rdx*8-8]
	psllq	xmm0, xmm5
	psrlq	xmm1, xmm4
	por	xmm0, xmm1
	movdqa	[rdi+rdx*8-16], xmm0
	add	rdx, 8
	jl	Ltop
Lend:
	test	dl, 1
	jnz	Le1
	movdqu	xmm1, [rsi-16]
	movq	xmm0, [rsi-8]
	psrlq	xmm1, xmm4
	psllq	xmm0, xmm5
	por	xmm0, xmm1
	movdqa	[rdi-16], xmm0
	pop	rsi
	pop	rdi
	ret
Le1:
	movq	xmm0, [rsi-8]
	psrlq	xmm0, xmm4
	movq	 [rdi-8], xmm0
	pop	rsi
	pop	rdi
	ret
align 16
Lbc:
	dec	edx
	jnz	@1f
	movq	xmm0, [rsi]
	psrlq	xmm0, xmm4
	movq	 [rdi], xmm0
	pop	rsi
	pop	rdi
	ret
@@1 LABEL1
	movq	xmm1, [rsi]
	movq	xmm0, [rsi+8]
	psrlq	xmm1, xmm4
	psllq	xmm0, xmm5
	por	xmm0, xmm1
	movq	 [rdi], xmm0
	dec	edx
	jnz	@1f
	movq	xmm0, [rsi+8]
	psrlq	xmm0, xmm4
	movq	 [rdi+8], xmm0
	pop	rsi
	pop	rdi
	ret
@@1 LABEL1
	movq	xmm1, [rsi+8]
	movq	xmm0, [rsi+16]
	psrlq	xmm1, xmm4
	psllq	xmm0, xmm5
	por	xmm0, xmm1
	movq	 [rdi+8], xmm0
	movq	xmm0, [rsi+16]
	psrlq	xmm0, xmm4
	movq	 [rdi+16], xmm0
	pop	rsi
	pop	rdi
	ret
