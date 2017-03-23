section .text
align 32
global	__gmpn_sqr_basecase
__gmpn_sqr_basecase:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	cmp	rdx, 2
	jae	Lgt1
	mov	rax, [rsi]
	mul	rax
	mov	[rdi], rax
	mov	[rdi+8], rdx
	pop	rsi
	pop	rdi
	ret
Lgt1:
	jne	Lgt2
	mov	rax, [rsi]
	mov	r8, rax
	mul	rax
	mov	r11, [rsi+8]
	mov	[rdi], rax
	mov	rax, r11
	mov	r9, rdx
	mul	rax
	mov	r10, rax
	mov	rax, r11
	mov	r11, rdx
	mul	r8
	xor	r8, r8
	add	r9, rax
	adc	r10, rdx
	adc	r11, r8
	add	r9, rax
	mov	[rdi+8], r9
	adc	r10, rdx
	mov	[rdi+16], r10
	adc	r11, r8
	mov	[rdi+24], r11
	pop	rsi
	pop	rdi
	ret
Lgt2:
	cmp	rdx, 4
	jae	Lgt3
	mov	rax, [rsi]
	mov	r10, rax
	mul	rax
	mov	r11, [rsi+8]
	mov	[rdi], rax
	mov	rax, r11
	mov	[rdi+8], rdx
	mul	rax
	mov	rcx, [rsi+16]
	mov	[rdi+16], rax
	mov	rax, rcx
	mov	[rdi+24], rdx
	mul	rax
	mov	[rdi+32], rax
	mov	[rdi+40], rdx
	mov	rax, r11
	mul	r10
	mov	r8, rax
	mov	rax, rcx
	mov	r9, rdx
	mul	r10
	xor	r10, r10
	add	r9, rax
	mov	rax, r11
	mov	r11, r10
	adc	r10, rdx
	mul	rcx
	add	r10, rax
	adc	rdx, r11
	add	r8, r8
	adc	r9, r9
	adc	r10, r10
	adc	rdx, rdx
	adc	r11, r11
	add	[rdi+8], r8
	adc	[rdi+16], r9
	adc	[rdi+24], r10
	adc	[rdi+32], rdx
	adc	[rdi+40], r11
	pop	rsi
	pop	rdi
	ret
Lgt3:
Ldo_mul_2:
	mov	r8, [rsi]
	push	rbx
	lea	rdi, [rdi+rdx*8]
	mov	rax, [rsi+8]
	push	rbp
	lea	rsi, [rsi+rdx*8]
	mov	r9, rax
	push	r12
	mov	r12d, 1
	push	r13
	sub	r12, rdx
	push	r14
	push	r12
	mul	r8
	mov	[rdi+r12*8], rax
	mov	rax, [rsi+r12*8+8]
	test	r12b, 1
	jnz	Lm2b1
Lm2b0:
	lea	rcx, [r12+2]
	xor	r11d, r11d
	xor	ebx, ebx
	mov	r10, rdx
	jmp	qword Lm2l0
Lm2b1:
	lea	rcx, [r12+1]
	xor	ebp, ebp
	xor	r10d, r10d
	mov	rbx, rdx
	jmp	qword Lm2l1
align 32
Lm2tp:
Lm2l0:
	mul	r8
	add	r10, rax
	mov	rbp, rdx
	adc	rbp, 0
	mov	rax, [rsi+rcx*8-8]
	mul	r9
	add	r10, r11
	adc	rbp, 0
	add	rbx, rax
	mov	[rdi+rcx*8-8], r10
	mov	r10, rdx
	adc	r10, 0
	mov	rax, [rsi+rcx*8]
Lm2l1:
	mul	r8
	add	rbx, rax
	mov	r11, rdx
	adc	r11, 0
	add	rbx, rbp
	mov	rax, [rsi+rcx*8]
	adc	r11, 0
	mul	r9
	mov	[rdi+rcx*8], rbx
	add	r10, rax
	mov	rbx, rdx
	mov	rax, [rsi+rcx*8+8]
	adc	rbx, 0
	add	rcx, 2
	jnc	Lm2tp
Lm2ed:
	mul	r8
	add	r10, rax
	mov	rbp, rdx
	adc	rbp, 0
	mov	rax, [rsi-8]
	mul	r9
	add	r10, r11
	adc	rbp, 0
	add	rbx, rax
	mov	[rdi-8], r10
	adc	rdx, 0
	add	rbx, rbp
	mov	[rdi], rbx
	adc	rdx, 0
	mov	[rdi+8], rdx
	add	r12, 2
Ldo_addmul_2:
Louter:
	lea	rdi, [rdi+16]
	cmp	r12d, -2
	jge	Lcorner
	mov	r8, [rsi+r12*8-8]
	mov	rax, [rsi+r12*8]
	mov	r9, rax
	mul	r8
	test	r12b, 1
	jnz	La1x1
La1x0:
	mov	r13, [rdi+r12*8]
	xor	r10, r10
	mov	r14, [rdi+r12*8+8]
	add	r13, rax
	mov	r11, rdx
	adc	r11, 0
	xor	rbx, rbx
	mov	[rdi+r12*8], r13
	mov	rax, [rsi+r12*8+8]
	test	r12b, 2
	jnz	La110
La100:
	lea	rcx, [r12+2]
	jmp	qword Llo0
La110:
	lea	rcx, [r12]
	jmp	qword Llo2
La1x1:
	mov	r14, [rdi+r12*8]
	xor	rbx, rbx
	mov	r13, [rdi+r12*8+8]
	add	r14, rax
	mov	rbp, rdx
	adc	rbp, 0
	xor	r10, r10
	mov	rax, [rsi+r12*8+8]
	test	r12b, 2
	jz	La111
La101:
	lea	rcx, [r12+3]
	jmp	qword Llo1
La111:
	lea	rcx, [r12+1]
	jmp	qword Llo3
align 32
Ltop:
	mul	r9
	mov	r10, rdx
	add	r13, rax
	adc	r10, 0
	add	r14, r11
	adc	rbp, 0
	add	r13, rbx
	adc	r10, 0
	mov	rax, [rsi+rcx*8-16]
Llo1:
	mul	r8
	add	r13, rax
	mov	r11, rdx
	adc	r11, 0
	mov	rax, [rsi+rcx*8-16]
	mul	r9
	mov	[rdi+rcx*8-24], r14
	mov	r14, [rdi+rcx*8-8]
	add	r13, rbp
	adc	r11, 0
	mov	rbx, rdx
	mov	[rdi+rcx*8-16], r13
	add	r14, rax
	adc	rbx, 0
	mov	rax, [rsi+rcx*8-8]
	add	r14, r10
	adc	rbx, 0
Llo0:
	mul	r8
	add	r14, rax
	mov	rbp, rdx
	adc	rbp, 0
	mov	rax, [rsi+rcx*8-8]
	mul	r9
	add	r14, r11
	mov	r13, [rdi+rcx*8]
	adc	rbp, 0
	mov	r10, rdx
	add	r13, rax
	adc	r10, 0
	mov	rax, [rsi+rcx*8]
Llo3:
	mul	r8
	add	r13, rbx
	mov	[rdi+rcx*8-8], r14
	mov	r11, rdx
	adc	r10, 0
	add	r13, rax
	adc	r11, 0
	mov	rax, [rsi+rcx*8]
	add	r13, rbp
	adc	r11, 0
	mul	r9
	mov	r14, [rdi+rcx*8+8]
	add	r14, rax
	mov	rbx, rdx
	adc	rbx, 0
	mov	rax, [rsi+rcx*8+8]
	mov	[rdi+rcx*8], r13
Llo2:
	mul	r8
	add	r14, r10
	mov	rbp, rdx
	adc	rbx, 0
	add	r14, rax
	mov	rax, [rsi+rcx*8+8]
	mov	r13, [rdi+rcx*8+16]
	adc	rbp, 0
	add	rcx, 4
	jnc	Ltop
Lend:
	mul	r9
	add	r14, r11
	adc	rbp, 0
	add	rax, rbx
	adc	rdx, 0
	mov	[rdi-8], r14
	add	rax, rbp
	adc	rdx, 0
	mov	[rdi], rax
	mov	[rdi+8], rdx
	add	r12, 2
	jmp	qword Louter
Lcorner:
	pop	rcx
	jg	Lsmall_corner
	lea	rdi, [rdi+8]
	mov	r8, [rsi-24]
	mov	rax, [rsi-16]
	mov	r9, rax
	mul	r8
	mov	r13, [rdi-24]
	mov	r14, [rdi-16]
	add	r13, rax
	mov	r11, rdx
	adc	r11, 0
	xor	rbx, rbx
	mov	[rdi-24], r13
	mov	rax, [rsi-8]
	mul	r8
	add	r14, 0
	mov	rbp, rdx
	adc	rbx, 0
	add	r14, rax
	mov	rax, [rsi-8]
	adc	rbp, 0
	mul	r9
	add	r14, r11
	adc	rbp, 0
	add	rax, rbx
	adc	rdx, 0
	mov	[rdi-16], r14
	jmp	qword Lcom
Lsmall_corner:
	mov	rbp, [rdi-8]
	mov	r8, [rsi-16]
	mov	rax, [rsi-8]
	mul	r8
Lcom:
	add	rax, rbp
	adc	rdx, 0
	mov	[rdi-8], rax
	mov	[rdi], rdx
Lsqr_diag_addlsh1:
	mov	rax, [rsi+rcx*8-8]
	shr rcx, 1
	mul	rax
	mov	[rdi+rcx*8], rax
	xor	ebx, ebx
	mov	r8, [rdi+rcx*8+8]
	mov	r9, [rdi+rcx*8+16]
	jmp	qword Ldm
align 32
Ldtop:
	add	r10, r8
	adc	rax, r9
	mov	r8, [rdi+rcx*8+8]
	mov	r9, [rdi+rcx*8+16]
	mov	[rdi+rcx*8-8], r10
	mov	[rdi+rcx*8], rax
Ldm:
	adc	r8, r8
	adc	r9, r9
	mov	rax, [rsi+rcx*4]
	lea	r10, [rdx+rbx]
	setc	bl
	mul	rax
	add	rcx, 2
	js	Ldtop
Ldend:
	add	r10, r8
	adc	rax, r9
	mov	[rdi-8], r10
	mov	[rdi], rax
	adc	rdx, rbx
	mov	[rdi+8], rdx
	pop	r14
	pop	r13
	pop	r12
	pop	rbp
	pop	rbx
	pop	rsi
	pop	rdi
	ret
