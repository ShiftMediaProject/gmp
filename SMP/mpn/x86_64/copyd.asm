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
global	__gmpn_copyd
__gmpn_copyd:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	lea	rsi, [rsi+rdx*8-8]
	lea	rdi, [rdi+rdx*8-8]
	cmp	rdx, 7
	jbe	Lbc
	test	dil, 8
	jnz	Lrp_aligned
	mov	rax, [rsi]
	mov	[rdi], rax
	lea	rsi, [rsi-8]
	lea	rdi, [rdi-8]
	dec	rdx
Lrp_aligned:
	test	sil, 8
	jz	Luent
	jmp	qword Lam
align 16
Latop:
	movaps	xmm0, [rsi-8]
	movaps	xmm1, [rsi-24]
	movaps	xmm2, [rsi-40]
	movaps	xmm3, [rsi-56]
	lea	rsi, [rsi-64]
	movaps	[rdi-8], xmm0
	movaps	[rdi-24], xmm1
	movaps	[rdi-40], xmm2
	movaps	[rdi-56], xmm3
	lea	rdi, [rdi-64]
Lam:
	sub	rdx, 8
	jnc	Latop
	test	dl, 4
	jz	@1f
	movaps	xmm0, [rsi-8]
	movaps	xmm1, [rsi-24]
	lea	rsi, [rsi-32]
	movaps	[rdi-8], xmm0
	movaps	[rdi-24], xmm1
	lea	rdi, [rdi-32]
@@1 LABEL1
	test	dl, 2
	jz	@1f
	movaps	xmm0, [rsi-8]
	lea	rsi, [rsi-16]
	movaps	[rdi-8], xmm0
	lea	rdi, [rdi-16]
@@1 LABEL1
	test	dl, 1
	jz	@1f
	mov	r8, [rsi]
	mov	[rdi], r8
@@1 LABEL1
	pop	rsi
	pop	rdi
	ret
Luent:
	sub	rdx, 16
	movaps	xmm0, [rsi]
	jc	Luend
align 16
Lutop:
	sub	rdx, 16
	movaps	xmm1, [rsi-16]
 db 0x66,0x0f,0x3a,0x0f,193,8
	movaps	[rdi-8], xmm0
	movaps	xmm2, [rsi-32]
 db 0x66,0x0f,0x3a,0x0f,202,8
	movaps	[rdi-24], xmm1
	movaps	xmm3, [rsi-48]
 db 0x66,0x0f,0x3a,0x0f,211,8
	movaps	[rdi-40], xmm2
	movaps	xmm0, [rsi-64]
 db 0x66,0x0f,0x3a,0x0f,216,8
	movaps	[rdi-56], xmm3
	movaps	xmm1, [rsi-80]
 db 0x66,0x0f,0x3a,0x0f,193,8
	movaps	[rdi-72], xmm0
	movaps	xmm2, [rsi-96]
 db 0x66,0x0f,0x3a,0x0f,202,8
	movaps	[rdi-88], xmm1
	movaps	xmm3, [rsi-112]
 db 0x66,0x0f,0x3a,0x0f,211,8
	movaps	[rdi-104], xmm2
	movaps	xmm0, [rsi-128]
 db 0x66,0x0f,0x3a,0x0f,216,8
	movaps	[rdi-120], xmm3
	lea	rsi, [rsi-128]
	lea	rdi, [rdi-128]
	jnc	Lutop
Luend:
	test	dl, 8
	jz	@1f
	movaps	xmm1, [rsi-16]
 db 0x66,0x0f,0x3a,0x0f,193,8
	movaps	[rdi-8], xmm0
	movaps	xmm0, [rsi-32]
 db 0x66,0x0f,0x3a,0x0f,200,8
	movaps	[rdi-24], xmm1
	movaps	xmm1, [rsi-48]
 db 0x66,0x0f,0x3a,0x0f,193,8
	movaps	[rdi-40], xmm0
	movaps	xmm0, [rsi-64]
 db 0x66,0x0f,0x3a,0x0f,200,8
	movaps	[rdi-56], xmm1
	lea	rsi, [rsi-64]
	lea	rdi, [rdi-64]
@@1 LABEL1
	test	dl, 4
	jz	@1f
	movaps	xmm1, [rsi-16]
 db 0x66,0x0f,0x3a,0x0f,193,8
	movaps	[rdi-8], xmm0
	movaps	xmm0, [rsi-32]
 db 0x66,0x0f,0x3a,0x0f,200,8
	movaps	[rdi-24], xmm1
	lea	rsi, [rsi-32]
	lea	rdi, [rdi-32]
@@1 LABEL1
	test	dl, 2
	jz	@1f
	movaps	xmm1, [rsi-16]
 db 0x66,0x0f,0x3a,0x0f,193,8
	movaps	[rdi-8], xmm0
	lea	rsi, [rsi-16]
	lea	rdi, [rdi-16]
@@1 LABEL1
	test	dl, 1
	jz	@1f
	mov	r8, [rsi]
	mov	[rdi], r8
@@1 LABEL1
	pop	rsi
	pop	rdi
	ret
Lbc:
	sub	edx, 4
	jc	Lend
align 16
Ltop:
	mov	r8, [rsi]
	mov	r9, [rsi-8]
	lea	rdi, [rdi-32]
	mov	r10, [rsi-16]
	mov	r11, [rsi-24]
	lea	rsi, [rsi-32]
	mov	[rdi+32], r8
	mov	[rdi+24], r9
	mov	[rdi+16], r10
	mov	[rdi+8], r11
Lend:
	test	dl, 1
	jz	@1f
	mov	r8, [rsi]
	mov	[rdi], r8
	lea	rdi, [rdi-8]
	lea	rsi, [rsi-8]
@@1 LABEL1
	test	dl, 2
	jz	@1f
	mov	r8, [rsi]
	mov	r9, [rsi-8]
	mov	[rdi], r8
	mov	[rdi-8], r9
@@1 LABEL1
	pop	rsi
	pop	rdi
	ret
