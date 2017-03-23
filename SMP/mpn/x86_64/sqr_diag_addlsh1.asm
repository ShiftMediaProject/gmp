section .text
align 32
global	__gmpn_sqr_diag_addlsh1
__gmpn_sqr_diag_addlsh1:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	push	rbx
	dec	rcx
	shr rcx, 1
	mov	rax, [rdx]
	lea	rdi, [rdi+rcx*8]
	lea	rsi, [rsi+rcx*8]
	lea	r11, [rdx+rcx*4]
	neg	rcx
	mul	rax
	mov	[rdi+rcx*8], rax
	xor	ebx, ebx
	jmp	qword Lmid
align 16
Ltop:
	add	r8, r10
	adc	r9, rax
	mov	[rdi+rcx*8-8], r8
	mov	[rdi+rcx*8], r9
Lmid:
	mov	rax, [r11+rcx*4+8]
	mov	r8, [rsi+rcx*8]
	mov	r9, [rsi+rcx*8+8]
	adc	r8, r8
	adc	r9, r9
	lea	r10, [rdx+rbx]
	setc	bl
	mul	rax
	add	rcx, 2
	js	Ltop
Lend:
	add	r8, r10
	adc	r9, rax
	mov	[rdi-8], r8
	mov	[rdi], r9
	adc	rdx, rbx
	mov	[rdi+8], rdx
	pop	rbx
	pop	rsi
	pop	rdi
	ret
