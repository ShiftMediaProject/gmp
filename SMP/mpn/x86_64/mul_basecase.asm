section .text
align 16
global	__gmpn_mul_basecase
__gmpn_mul_basecase:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	mov	r8d, [rsp+56]
	push	rbx
	push	rbp
	mov	rbx, rdx
	neg	rbx
	mov	rax, [rsi]
	lea	rsi, [rsi+rdx*8]
	lea	rdi, [rdi+rdx*8]
	mov	r9, [rcx]
	mul	r9
	test	r8b, 1
	jz	Ldo_mul_2
Ldo_mul_1:
	test	bl, 1
	jnz	Lm1x1
Lm1x0:
	mov	r10, rax
	mov	r11, rdx
	mov	rax, [rsi+rbx*8+8]
	test	bl, 2
	jnz	Lm110
Lm100:
	lea	rbp, [rbx+2]
	jmp	qword Lm1l0
Lm110:
	lea	rbp, [rbx]
	jmp	qword Lm1l2
Lm1x1:
	mov	r11, rax
	mov	r10, rdx
	test	bl, 2
	jz	Lm111
Lm101:
	lea	rbp, [rbx+3]
	test	rbp, rbp
	js	Lm1l1
	mov	[rdi-8], rax
	mov	[rdi], rdx
	pop	rbp
	pop	rbx
	pop	rsi
	pop	rdi
	ret
Lm111:
	lea	rbp, [rbx+1]
	mov	rax, [rsi+rbx*8+8]
	jmp	qword Lm1l3
align 16
Lm1tp:
	mov	r10, rdx
	add	r11, rax
Lm1l1:
	mov	rax, [rsi+rbp*8-16]
	adc	r10, 0
	mul	r9
	add	r10, rax
	mov	[rdi+rbp*8-24], r11
	mov	rax, [rsi+rbp*8-8]
	mov	r11, rdx
	adc	r11, 0
Lm1l0:
	mul	r9
	mov	[rdi+rbp*8-16], r10
	add	r11, rax
	mov	r10, rdx
	mov	rax, [rsi+rbp*8]
	adc	r10, 0
Lm1l3:
	mul	r9
	mov	[rdi+rbp*8-8], r11
	mov	r11, rdx
	add	r10, rax
	mov	rax, [rsi+rbp*8+8]
	adc	r11, 0
Lm1l2:
	mul	r9
	mov	[rdi+rbp*8], r10
	add	rbp, 4
	jnc	Lm1tp
Lm1ed:
	add	r11, rax
	adc	rdx, 0
	mov	[rdi-8], r11
	mov	[rdi], rdx
	dec	r8d
	jz	Lret2
	lea	rcx, [rcx+8]
	lea	rdi, [rdi+8]
	push	r12
	push	r13
	push	r14
	jmp	qword Ldo_addmul
Ldo_mul_2:
	push	r12
	push	r13
	push	r14
	mov	r14, [rcx+8]
	test	bl, 1
	jnz	Lm2b1
Lm2b0:
	lea	rbp, [rbx]
	xor	r10, r10
	mov	r12, rax
	mov	r11, rdx
	jmp	qword Lm2l0
Lm2b1:
	lea	rbp, [rbx+1]
	xor	r11, r11
	xor	r12, r12
	mov	r10, rax
	mov	r13, rdx
	jmp	qword Lm2l1
align 32
Lm2tp:
	mul	r9
	add	r10, rax
	mov	r13, rdx
	adc	r13, 0
Lm2l1:
	mov	rax, [rsi+rbp*8-8]
	mul	r14
	add	r10, r11
	adc	r13, 0
	add	r12, rax
	mov	[rdi+rbp*8-8], r10
	mov	r10, rdx
	adc	r10, 0
	mov	rax, [rsi+rbp*8]
	mul	r9
	add	r12, rax
	mov	r11, rdx
	adc	r11, 0
	add	r12, r13
Lm2l0:
	mov	rax, [rsi+rbp*8]
	adc	r11, 0
	mul	r14
	mov	[rdi+rbp*8], r12
	add	r10, rax
	mov	r12, rdx
	mov	rax, [rsi+rbp*8+8]
	adc	r12, 0
	add	rbp, 2
	jnc	Lm2tp
Lm2ed:
	mul	r9
	add	r10, rax
	mov	r13, rdx
	adc	r13, 0
	mov	rax, [rsi-8]
	mul	r14
	add	r10, r11
	adc	r13, 0
	add	r12, rax
	mov	[rdi-8], r10
	adc	rdx, 0
	add	r12, r13
	mov	[rdi], r12
	adc	rdx, 0
	mov	[rdi+8], rdx
	add	r8d, -2
	jz	Lret5
	lea	rcx, [rcx+16]
	lea	rdi, [rdi+16]
Ldo_addmul:
	push	r15
	push	r8
Louter:
	mov	r9, [rcx]
	mov	r8, [rcx+8]
	mov	rax, [rsi+rbx*8]
	mul	r9
	test	bl, 1
	jnz	La1x1
La1x0:
	mov	r14, [rdi+rbx*8]
	xor	r10, r10
	mov	r11, rdx
	test	bl, 2
	jnz	La110
La100:
	lea	rbp, [rbx+2]
	add	r14, rax
	adc	r11, 0
	mov	rax, [rsi+rbx*8]
	mul	r8
	mov	r15, [rdi+rbx*8+8]
	jmp	qword Llo0
La110:
	lea	rbp, [rbx]
	xor	r13, r13
	jmp	qword Llo2
La1x1:
	mov	r15, [rdi+rbx*8]
	xor	r12, r12
	xor	r11, r11
	test	bl, 2
	jz	La111
La101:
	lea	rbp, [rbx+3]
	mov	r13, rdx
	add	r15, rax
	mov	rax, [rsi+rbx*8]
	mov	r14, [rdi+rbx*8+8]
	adc	r13, 0
	jmp	qword Ltop
La111:
	lea	rbp, [rbx+1]
	jmp	qword Llo3
align 32
Ltop:
	mul	r8
	mov	r10, rdx
	add	r14, rax
	adc	r10, 0
	add	r15, r11
	adc	r13, 0
	add	r14, r12
	adc	r10, 0
	mov	rax, [rsi+rbp*8-16]
	mul	r9
	add	r14, rax
	mov	r11, rdx
	adc	r11, 0
	mov	rax, [rsi+rbp*8-16]
	mul	r8
	mov	[rdi+rbp*8-24], r15
	mov	r15, [rdi+rbp*8-8]
	add	r14, r13
	adc	r11, 0
Llo0:
	mov	r12, rdx
	mov	[rdi+rbp*8-16], r14
	add	r15, rax
	adc	r12, 0
	mov	rax, [rsi+rbp*8-8]
	add	r15, r10
	adc	r12, 0
	mul	r9
Llo3:
	add	r15, rax
	mov	r13, rdx
	adc	r13, 0
	mov	rax, [rsi+rbp*8-8]
	mul	r8
	add	r15, r11
	mov	r14, [rdi+rbp*8]
	adc	r13, 0
	mov	r10, rdx
	add	r14, rax
	adc	r10, 0
	mov	rax, [rsi+rbp*8]
	mul	r9
	add	r14, r12
	mov	[rdi+rbp*8-8], r15
	mov	r11, rdx
	adc	r10, 0
Llo2:
	add	r14, rax
	adc	r11, 0
	mov	rax, [rsi+rbp*8]
	add	r14, r13
	adc	r11, 0
	mul	r8
	mov	r15, [rdi+rbp*8+8]
	add	r15, rax
	mov	r12, rdx
	adc	r12, 0
	mov	rax, [rsi+rbp*8+8]
	mov	[rdi+rbp*8], r14
	mul	r9
	add	r15, r10
	mov	r13, rdx
	adc	r12, 0
	add	r15, rax
	mov	rax, [rsi+rbp*8+8]
	mov	r14, [rdi+rbp*8+16]
	adc	r13, 0
	add	rbp, 4
	jnc	Ltop
Lend:
	mul	r8
	add	r15, r11
	adc	r13, 0
	add	rax, r12
	adc	rdx, 0
	mov	[rdi-8], r15
	add	rax, r13
	adc	rdx, 0
	mov	[rdi], rax
	mov	[rdi+8], rdx
	add	dword [rsp], -2
	lea	rcx, [rcx+16]
	lea	rdi, [rdi+16]
	jnz	Louter
	pop	rax
	pop	r15
Lret5:
	pop	r14
	pop	r13
	pop	r12
Lret2:
	pop	rbp
	pop	rbx
	pop	rsi
	pop	rdi
	ret
