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
extern __gmpn_invert_limb
section .text
align 16
global	__gmpn_divrem_2
__gmpn_divrem_2:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	mov	r8, [rsp+56]
	push	r15
	push	r14
	push	r13
	push	r12
	lea	r12, [rdx+rcx*8-24]
	mov	r13, rsi
	push	rbp
	mov	rbp, rdi
	push	rbx
	mov	r11, [r8+8]
	mov	rbx, [r12+16]
	mov	r8, [r8]
	mov	r10, [r12+8]
	xor	r15d, r15d
	cmp	r11, rbx
	ja	L2
	setb	dl
	cmp	r8, r10
	setbe	al
	or	dl, al
	je	L2
	inc	r15d
	sub	r10, r8
	sbb	rbx, r11
L2:
	lea	r14, [rcx+r13-3]
	test	r14, r14
	js	Lend
	push	r8
	push	r10
	push	r11
	mov	rcx, r11
	call	qword __gmpn_invert_limb
	pop	r11
	pop	r10
	pop	r8
	mov	rdx, r11
	mov	rdi, rax
	imul	rdx, rax
	mov	r9, rdx
	mul	r8
	xor	ecx, ecx
	add	r9, r8
	adc	rcx, -1
	add	r9, rdx
	adc	rcx, 0
	js	@2f
@@1 LABEL1
	dec	rdi
	sub	r9, r11
	sbb	rcx, 0
	jns	@1b
@@2 LABEL2
	lea	rbp, [rbp+r14*8]
	mov	rsi, r11
	neg	rsi
align 16
Ltop:
	mov	rax, rdi
	mul	rbx
	mov	rcx, r10
	add	rcx, rax
	adc	rdx, rbx
	mov	r9, rdx
	imul	rdx, rsi
	mov	rax, r8
	lea	rbx, [rdx+r10]
	xor	r10d, r10d
	mul	r9
	cmp	r13, r14
	jg	L19
	mov	r10, [r12]
	sub	r12, 8
L19:
	sub	r10, r8
	sbb	rbx, r11
	sub	r10, rax
	sbb	rbx, rdx
	xor	eax, eax
	xor	edx, edx
	cmp	rbx, rcx
	cmovnc	rax, r8
	cmovnc	rdx, r11
	adc	r9, 0
	nop
	add	r10, rax
	adc	rbx, rdx
	cmp	rbx, r11
	jae	Lfix
Lbck:
	mov	[rbp], r9
	sub	rbp, 8
	dec	r14
	jns	Ltop
Lend:
	mov	[r12+8], r10
	mov	[r12+16], rbx
	pop	rbx
	pop	rbp
	pop	r12
	pop	r13
	pop	r14
	mov	rax, r15
	pop	r15
	pop	rsi
	pop	rdi
	ret
Lfix:
	seta	dl
	cmp	r10, r8
	setae	al
	or	al, dl
	je	Lbck
	inc	r9
	sub	r10, r8
	sbb	rbx, r11
	jmp	qword Lbck
