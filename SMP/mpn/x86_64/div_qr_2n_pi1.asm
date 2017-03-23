section .text
align 16
global	__gmpn_div_qr_2n_pi1
__gmpn_div_qr_2n_pi1:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	mov	r8, [rsp+56]
	mov	r9, [rsp+64]
	mov	r10, [rsp+72]
	mov	r11, rdx
	push	r15
	push	r14
	push	r13
	push	r12
	push	rbx
	mov	r12, [r11+rcx*8-16]
	mov	rbx, [r11+rcx*8-8]
	mov	r14, r12
	mov	r13, rbx
	sub	r14, r9
	sbb	r13, r8
	cmovnc	r12, r14
	cmovnc	rbx, r13
	sbb	rax, rax
	inc	rax
	push	rax
	lea	rcx, [rcx-2]
	mov	r15, r8
	neg	r15
	jmp	qword Lnext
align 16
Lloop:
	mov	rax, r10
	mul	rbx
	mov	r14, r12
	add	r14, rax
	adc	rdx, rbx
	mov	r13, rdx
	imul	rdx, r15
	mov	rax, r9
	lea	rbx, [rdx+r12]
	mul	r13
	mov	r12, [r11+rcx*8]
	sub	r12, r9
	sbb	rbx, r8
	sub	r12, rax
	sbb	rbx, rdx
	xor	eax, eax
	xor	edx, edx
	cmp	rbx, r14
	cmovnc	rax, r9
	cmovnc	rdx, r8
	adc	r13, 0
	nop
	add	r12, rax
	adc	rbx, rdx
	cmp	rbx, r8
	jae	Lfix
Lbck:
	mov	[rdi+rcx*8], r13
Lnext:
	sub	rcx, 1
	jnc	Lloop
Lend:
	mov	[rsi+8], rbx
	mov	[rsi], r12
	pop	rax
	pop	rbx
	pop	r12
	pop	r13
	pop	r14
	pop	r15
	pop	rsi
	pop	rdi
	ret
Lfix:
	seta	dl
	cmp	r12, r9
	setae	al
	or	al, dl
	je	Lbck
	inc	r13
	sub	r12, r9
	sbb	rbx, r8
	jmp	qword Lbck
