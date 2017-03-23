%assign LABEL2 1
%define @2f ..@L21
%macro define2@... 1
%define @2f ..@L2%{1}
%endmacro
%macro @@2 1
..@L2%{1}:
%define @2b ..@L2%{1}
%assign LABEL2 LABEL2+1
define2@... LABEL2
%endmacro
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
align 16
global	__gmpn_addaddmul_1msb0
__gmpn_addaddmul_1msb0:
	push	r12
	push	rbp
	lea	rsi, [rsi+rcx*8]
	lea	rbp, [rdx+rcx*8]
	lea	rdi, [rdi+rcx*8]
	neg	rcx
	mov	rax, [rsi+rcx*8]
	mul	r8
	mov	r12, rax
	mov	rax, [rbp+rcx*8]
	mov	r10, rdx
	add	rcx, 3
	jns	Lend
align 16
Ltop:
	mul	r9
	add	r12, rax
	mov	rax, [rsi+rcx*8-16]
	adc	r10, rdx
	mov	[rdi+rcx*8-24], r12
	mul	r8
	add	r10, rax
	mov	rax, [rbp+rcx*8-16]
	mov	r11d, 0
	adc	r11, rdx
	mul	r9
	add	r10, rax
	mov	rax, [rsi+rcx*8-8]
	adc	r11, rdx
	mov	[rdi+rcx*8-16], r10
	mul	r8
	add	r11, rax
	mov	rax, [rbp+rcx*8-8]
	mov	r12d, 0
	adc	r12, rdx
	mul	r9
	add	r11, rax
	adc	r12, rdx
	mov	rax, [rsi+rcx*8]
	mul	r8
	add	r12, rax
	mov	[rdi+rcx*8-8], r11
	mov	rax, [rbp+rcx*8]
	mov	r10d, 0
	adc	r10, rdx
	add	rcx, 3
	js	Ltop
Lend:
	cmp	ecx, 1
	ja	@2f
	jz	@1f
	mul	r9
	add	r12, rax
	mov	rax, [rsi-16]
	adc	r10, rdx
	mov	[rdi-24], r12
	mul	r8
	add	r10, rax
	mov	rax, [rbp-16]
	mov	r11d, 0
	adc	r11, rdx
	mul	r9
	add	r10, rax
	mov	rax, [rsi-8]
	adc	r11, rdx
	mov	[rdi-16], r10
	mul	r8
	add	r11, rax
	mov	rax, [rbp-8]
	mov	r12d, 0
	adc	r12, rdx
	mul	r9
	add	r11, rax
	adc	r12, rdx
	mov	[rdi-8], r11
	mov	rax, r12
	pop	rbp
	pop	r12
	ret
@@1 LABEL1
	mul	r9
	add	r12, rax
	mov	rax, [rsi-8]
	adc	r10, rdx
	mov	[rdi-16], r12
	mul	r8
	add	r10, rax
	mov	rax, [rbp-8]
	mov	r11d, 0
	adc	r11, rdx
	mul	r9
	add	r10, rax
	adc	r11, rdx
	mov	[rdi-8], r10
	mov	rax, r11
	pop	rbp
	pop	r12
	ret
@@2 LABEL2
	mul	r9
	add	r12, rax
	mov	[rdi-8], r12
	adc	r10, rdx
	mov	rax, r10
	pop	rbp
	pop	r12
	ret
