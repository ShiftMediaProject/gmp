section .text
align 32
global	__gmpn_addmul_2
__gmpn_addmul_2:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	push	rbx
	push	rbp
	push	r12
	push	r13
	mov	rbx, [rcx]
	mov	rbp, [rcx+8]
	mov	rax, [rsi]
	mov	rcx, rdx
	neg	rcx
	lea	rsi, [rsi+rdx*8]
	lea	rdi, [rdi+rdx*8+8]
	mul	rbx
	test	cl, 1
	jnz	Lbx1
Lbx0:
	mov	r12, [rdi+rcx*8-8]
	mov	r9, rdx
	add	r12, rax
	adc	r9, 0
	mov	rax, [rsi+rcx*8]
	xor	r8, r8
	xor	r11, r11
	test	cl, 2
	jnz	Lb10
Lb00:
	nop
	mul	rbp
	mov	r13, [rdi+rcx*8]
	jmp	qword Llo0
Lb10:
	lea	rcx, [rcx-2]
	jmp	qword Llo2
Lbx1:
	mov	r13, [rdi+rcx*8-8]
	mov	r11, rdx
	add	r13, rax
	adc	r11, 0
	mov	rax, [rsi+rcx*8]
	xor	r9, r9
	xor	r10, r10
	test	cl, 2
	jz	Lb11
Lb01:
	mov	r12, [rdi+rcx*8]
	inc	rcx
	jmp	qword Llo1
Lb11:
	dec	rcx
	jmp	qword Llo3
align 32
Ltop:
Llo1:
	mul	rbp
	mov	r8, rdx
	add	r12, rax
	adc	r8, 0
	add	r13, r9
	adc	r11, 0
	add	r12, r10
	adc	r8, 0
	mov	rax, [rsi+rcx*8]
	mul	rbx
	add	r12, rax
	mov	r9, rdx
	adc	r9, 0
	mov	rax, [rsi+rcx*8]
	mul	rbp
	mov	[rdi+rcx*8-16], r13
	mov	r13, [rdi+rcx*8]
	add	r12, r11
	adc	r9, 0
Llo0:
	mov	r10, rdx
	mov	[rdi+rcx*8-8], r12
	add	r13, rax
	adc	r10, 0
	mov	rax, [rsi+rcx*8+8]
	add	r13, r8
	adc	r10, 0
	mul	rbx
	add	r13, rax
	mov	r11, rdx
	adc	r11, 0
	mov	rax, [rsi+rcx*8+8]
Llo3:
	mul	rbp
	add	r13, r9
	mov	r12, [rdi+rcx*8+8]
	adc	r11, 0
	mov	r8, rdx
	add	r12, rax
	adc	r8, 0
	mov	rax, [rsi+rcx*8+16]
	mul	rbx
	add	r12, r10
	mov	[rdi+rcx*8], r13
	mov	r9, rdx
	adc	r8, 0
	add	r12, rax
	adc	r9, 0
	mov	rax, [rsi+rcx*8+16]
	add	r12, r11
	adc	r9, 0
Llo2:
	mul	rbp
	mov	r13, [rdi+rcx*8+16]
	add	r13, rax
	mov	r10, rdx
	adc	r10, 0
	mov	rax, [rsi+rcx*8+24]
	mov	[rdi+rcx*8+8], r12
	mul	rbx
	add	r13, r8
	mov	r11, rdx
	adc	r10, 0
	add	r13, rax
	mov	rax, [rsi+rcx*8+24]
	mov	r12, [rdi+rcx*8+24]
	adc	r11, 0
	add	rcx, 4
	jnc	Ltop
Lend:
	mul	rbp
	add	r13, r9
	adc	r11, 0
	add	rax, r10
	adc	rdx, 0
	mov	[rdi-16], r13
	add	rax, r11
	adc	rdx, 0
	mov	[rdi-8], rax
	mov	rax, rdx
	pop	r13
	pop	r12
	pop	rbp
	pop	rbx
	pop	rsi
	pop	rdi
	ret
