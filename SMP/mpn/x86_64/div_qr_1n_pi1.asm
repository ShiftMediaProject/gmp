section .text
align 16
global	__gmpn_div_qr_1n_pi1
__gmpn_div_qr_1n_pi1:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	mov	r8, [rsp+56]
	mov	r9, [rsp+64]
	dec	rdx
	jnz	Lfirst
	lea	r10, [rcx+1]
	mov	rax, rcx
	mul	r9
	mov	r11, [rsi]
	add	rax, r11
	adc	rdx, r10
	mov	r10, rdx
	imul	rdx, r8
	sub	r11, rdx
	cmp	rax, r11
	lea	rax, [r11+r8]
	cmovnc	rax, r11
	sbb	r10, 0
	cmp	rax, r8
	jc	Lsingle_div_done
	sub	rax, r8
	add	r10, 1
Lsingle_div_done:
	mov	[rdi], r10
	pop	rsi
	pop	rdi
	ret
Lfirst:
	push	r15
	push	r14
	push	r13
	push	r12
	push	rbx
	push	rbp
	mov	rbp, r8
	imul	rbp, r9
	neg	rbp
	mov	rbx, rbp
	sub	rbx, r8
	push	r8
	mov	r8, rdx
	mov	rax, r9
	mul	rcx
	mov	r13, rax
	add	rdx, rcx
	mov	r10, rdx
	mov	rax, rbp
	mul	rcx
	mov	r11, [rsi+r8*8-8]
	mov	rcx, [rsi+r8*8]
	mov	[rdi+r8*8], r10
	add	r11, rax
	adc	rcx, rdx
	sbb	r12, r12
	dec	r8
	mov	rax, rcx
	jz	Lfinal
align 16
Lloop:
	mov	r14, r9
	mov	r15, r12
	and	r14, r12
	neg	r15
	mul	r9
	add	r14, rdx
	adc	r15, 0
	add	r14, r13
	mov	r13, rax
	mov	rax, rbp
	lea	r10, [rbx+r11]
	adc	r15, 0
	mul	rcx
	and	r12, rbp
	add	r11, r12
	cmovnc	r10, r11
	adc	r14, rcx
	mov	r11, [rsi+r8*8-8]
	adc	[rdi+r8*8+8], r15
	jc	Lq_incr
Lq_incr_done:
	add	r11, rax
	mov	rax, r10
	adc	rax, rdx
	mov	[rdi+r8*8], r14
	sbb	r12, r12
	dec	r8
	mov	rcx, rax
	jnz	Lloop
Lfinal:
	pop	r8
	mov	r14, r12
	and	r12, r8
	sub	rax, r12
	neg	r14
	mov	rcx, rax
	sub	rax, r8
	cmovc	rax, rcx
	sbb	r14, -1
	lea	r10, [rax+1]
	mul	r9
	add	rax, r11
	adc	rdx, r10
	mov	r10, rdx
	imul	rdx, r8
	sub	r11, rdx
	cmp	rax, r11
	lea	rax, [r11+r8]
	cmovnc	rax, r11
	sbb	r10, 0
	cmp	rax, r8
	jc	Ldiv_done
	sub	rax, r8
	add	r10, 1
Ldiv_done:
	add	r13, r10
	mov	[rdi], r13
	adc	[rdi+8], r14
	jnc	Ldone
Lfinal_q_incr:
	add	qword [rdi+16], 1
	lea	rdi, [rdi+8]
	jc	Lfinal_q_incr
Ldone:
	pop	rbp
	pop	rbx
	pop	r12
	pop	r13
	pop	r14
	pop	r15
	pop	rsi
	pop	rdi
	ret
Lq_incr:
	lea	rcx, [rdi+r8*8+16]
Lq_incr_loop:
	add	qword [rcx], 1
	jnc	Lq_incr_done
	lea	rcx, [rcx+8]
	jmp	qword Lq_incr_loop
