section .text
align 32
global	__gmpn_mullo_basecase
__gmpn_mullo_basecase:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	mov	rax, [rsi]
	mov	r8, rdx
	cmp	rcx, 4
	jb	Lsmall
	mov	r9, [rdx]
	push	rbx
	lea	rdi, [rdi+rcx*8]
	push	rbp
	lea	rsi, [rsi+rcx*8]
	push	r12
	neg	rcx
	push	r13
	mul	r9
	mov	rbx, [r8+8]
	test	cl, 1
	jnz	Lm2b1
Lm2b0:
	lea	rbp, [rcx]
	xor	r10, r10
	mov	r12, rax
	mov	r11, rdx
	jmp	qword Lm2l0
Lm2b1:
	lea	rbp, [rcx+1]
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
	mul	rbx
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
	mul	rbx
	mov	[rdi+rbp*8], r12
	add	r10, rax
	mov	r12, rdx
	mov	rax, [rsi+rbp*8+8]
	adc	r12, 0
	add	rbp, 2
	jnc	Lm2tp
Lm2ed:
	imul	rax, r9
	add	rax, r10
	add	rax, r11
	mov	[rdi-8], rax
	add	rcx, 2
	lea	r8, [r8+16]
	lea	rsi, [rsi-16]
	cmp	rcx, -2
	jge	Lcor1
	push	r14
	push	r15
Louter:
	mov	r9, [r8]
	mov	rbx, [r8+8]
	mov	rax, [rsi+rcx*8]
	mul	r9
	test	cl, 1
	jnz	La1x1
La1x0:
	mov	r15, [rdi+rcx*8]
	xor	r12, r12
	xor	r11, r11
	test	cl, 2
	jnz	La110
La100:
	lea	rbp, [rcx+1]
	jmp	qword Llo0
La110:
	lea	rbp, [rcx+3]
	mov	r13, rdx
	add	r15, rax
	mov	rax, [rsi+rcx*8]
	mov	r14, [rdi+rcx*8+8]
	adc	r13, 0
	jmp	qword Llo2
La1x1:
	mov	r14, [rdi+rcx*8]
	xor	r10, r10
	mov	r11, rdx
	test	cl, 2
	jz	La111
La101:
	lea	rbp, [rcx+2]
	add	r14, rax
	adc	r11, 0
	mov	rax, [rsi+rcx*8]
	mul	rbx
	mov	r15, [rdi+rcx*8+8]
	jmp	qword Llo1
La111:
	lea	rbp, [rcx]
	xor	r13, r13
	jmp	qword Llo3
align 32
Ltop:
Llo2:
	mul	rbx
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
	mul	rbx
	mov	[rdi+rbp*8-24], r15
	mov	r15, [rdi+rbp*8-8]
	add	r14, r13
	adc	r11, 0
Llo1:
	mov	r12, rdx
	mov	[rdi+rbp*8-16], r14
	add	r15, rax
	adc	r12, 0
	mov	rax, [rsi+rbp*8-8]
	add	r15, r10
	adc	r12, 0
	mul	r9
Llo0:
	add	r15, rax
	mov	r13, rdx
	adc	r13, 0
	mov	rax, [rsi+rbp*8-8]
	mul	rbx
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
Llo3:
	add	r14, rax
	adc	r11, 0
	mov	rax, [rsi+rbp*8]
	add	r14, r13
	adc	r11, 0
	mul	rbx
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
	imul	rax, rbx
	add	r14, rax
	add	r15, r11
	adc	r13, 0
	add	r14, r12
	mov	rax, [rsi-8]
	imul	rax, r9
	add	rax, r14
	mov	[rdi-16], r15
	add	rax, r13
	mov	[rdi-8], rax
	add	rcx, 2
	lea	r8, [r8+16]
	lea	rsi, [rsi-16]
	cmp	rcx, -2
	jl	Louter
	pop	r15
	pop	r14
	jnz	Lcor0
Lcor1:
	mov	r9, [r8]
	mov	rbx, [r8+8]
	mov	rax, [rsi-16]
	mul	r9
	add	rax, [rdi-16]
	adc	rdx, [rdi-8]
	mov	r10, [rsi-8]
	imul	r10, r9
	mov	r11, [rsi-16]
	imul	r11, rbx
	mov	[rdi-16], rax
	add	r11, r10
	add	r11, rdx
	mov	[rdi-8], r11
	pop	r13
	pop	r12
	pop	rbp
	pop	rbx
	pop	rsi
	pop	rdi
	ret
Lcor0:
	mov	r11, [r8]
	imul	r11, [rsi-8]
	add	r11, rax
	mov	[rdi-8], r11
	pop	r13
	pop	r12
	pop	rbp
	pop	rbx
	pop	rsi
	pop	rdi
	ret
align 16
Lsmall:
	cmp	rcx, 2
	jae	Lgt1
Ln1:
	imul	rax, [rdx]
	mov	[rdi], rax
	pop	rsi
	pop	rdi
	ret
Lgt1:
	ja	Lgt2
Ln2:
	mov	r9, [rdx]
	mul	r9
	mov	[rdi], rax
	mov	rax, [rsi+8]
	imul	rax, r9
	add	rdx, rax
	mov	r9, [r8+8]
	mov	rcx, [rsi]
	imul	rcx, r9
	add	rdx, rcx
	mov	[rdi+8], rdx
	pop	rsi
	pop	rdi
	ret
Lgt2:
Ln3:
	mov	r9, [rdx]
	mul	r9
	mov	[rdi], rax
	mov	r10, rdx
	mov	rax, [rsi+8]
	mul	r9
	imul	r9, [rsi+16]
	add	r10, rax
	adc	r9, rdx
	mov	r11, [r8+8]
	mov	rax, [rsi]
	mul	r11
	add	r10, rax
	adc	r9, rdx
	imul	r11, [rsi+8]
	add	r9, r11
	mov	[rdi+8], r10
	mov	r10, [r8+16]
	mov	rax, [rsi]
	imul	r10, rax
	add	r9, r10
	mov	[rdi+16], r9
	pop	rsi
	pop	rdi
	ret
