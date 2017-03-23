section .text
align 32
global	__gmpn_mul_2
__gmpn_mul_2:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	push	rbx
	push	rbp
	mov	rbx, [rcx]
	mov	rbp, [rcx+8]
	mov	rax, [rsi]
	lea	rsi, [rsi+rdx*8]
	lea	rdi, [rdi+rdx*8]
	test	dl, 1
	jnz	Lb1
Lb0:
	mov	ecx, 0
	sub	rcx, rdx
	xor	r8, r8
	mul	rbx
	mov	r10, rax
	mov	r9, rdx
	mov	rax, [rsi+rcx*8]
	jmp	qword Llo0
Lb1:
	mov	ecx, 1
	sub	rcx, rdx
	xor	r10, r10
	mul	rbx
	mov	r8, rax
	mov	r11, rdx
	mov	rax, [rsi+rcx*8-8]
	mul	rbp
	jmp	qword Llo1
align 32
Ltop:
	mul	rbx
	add	r8, rax
	mov	r11, rdx
	adc	r11, 0
	mov	rax, [rsi+rcx*8-8]
	mul	rbp
	add	r8, r9
	adc	r11, 0
Llo1:
	add	r10, rax
	mov	[rdi+rcx*8-8], r8
	mov	r8, rdx
	adc	r8, 0
	mov	rax, [rsi+rcx*8]
	mul	rbx
	add	r10, rax
	mov	r9, rdx
	adc	r9, 0
	add	r10, r11
	mov	rax, [rsi+rcx*8]
	adc	r9, 0
Llo0:
	mul	rbp
	mov	[rdi+rcx*8], r10
	add	r8, rax
	mov	r10, rdx
	mov	rax, [rsi+rcx*8+8]
	adc	r10, 0
	add	rcx, 2
	jnc	Ltop
Lend:
	mul	rbx
	add	r8, rax
	mov	r11, rdx
	adc	r11, 0
	mov	rax, [rsi-8]
	mul	rbp
	add	r8, r9
	adc	r11, 0
	add	r10, rax
	mov	[rdi-8], r8
	adc	rdx, 0
	add	r10, r11
	mov	[rdi], r10
	adc	rdx, 0
	mov	rax, rdx
	pop	rbp
	pop	rbx
	pop	rsi
	pop	rdi
	ret
