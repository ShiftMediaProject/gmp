section .text
align 16
global	__gmpn_div_qr_2u_pi1
__gmpn_div_qr_2u_pi1:
	mov	r10, [rsp+0+16]
	mov	r11, rdx
	push	r15
	push	r14
	push	r13
	push	r12
	push	rbx
	push	rbp
	push	rsi
	lea	rbp, [rcx-2]
	mov	r15, r8
	neg	r15
	mov	ecx, dword [rsp+56+8]
	xor	ebx, ebx
	mov	r12, [r11+rbp*8+8]
	shld	rbx, r12, cl
	mov	rax, r10
	mul	rbx
	mov	rsi, [r11+rbp*8]
	shld	r12, rsi, cl
	mov	r14, r12
	add	r14, rax
	adc	rdx, rbx
	mov	r13, rdx
	imul	rdx, r15
	mov	rax, r9
	lea	rbx, [rdx+r12]
	mul	r13
	mov	r12, rsi
	shl	r12, cl
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
	jae	Lfix_qh
Lbck_qh:
	push	r13
	jmp	qword Lnext
align 16
Lloop:
	mov	rax, r10
	mul	rbx
	mov	rsi, [r11+rbp*8]
	xor	r13d, r13d
	shld	r13, rsi, cl
	or	r12, r13
	mov	r14, r12
	add	r14, rax
	adc	rdx, rbx
	mov	r13, rdx
	imul	rdx, r15
	mov	rax, r9
	lea	rbx, [rdx+r12]
	mul	r13
	mov	r12, rsi
	shl	r12, cl
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
	mov	[rdi+rbp*8], r13
Lnext:
	sub	rbp, 1
	jnc	Lloop
Lend:
	pop	rax
	pop	rsi
	shrd	r12, rbx, cl
	shr	rbx, cl
	mov	[rsi+8], rbx
	mov	[rsi], r12
	pop	rbp
	pop	rbx
	pop	r12
	pop	r13
	pop	r14
	pop	r15
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
Lfix_qh:
	seta	dl
	cmp	r12, r9
	setae	al
	or	al, dl
	je	Lbck_qh
	inc	r13
	sub	r12, r9
	sbb	rbx, r8
	jmp	qword Lbck_qh
