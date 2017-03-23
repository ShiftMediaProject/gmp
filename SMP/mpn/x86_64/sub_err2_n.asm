section .text
align 16
global	__gmpn_sub_err2_n
__gmpn_sub_err2_n:
	mov	rax, [rsp+16]
	mov	r10, [rsp+8]
	push	rbx
	push	rbp
	push	r12
	push	r13
	push	r14
	xor	ebp, ebp
	xor	r11d, r11d
	xor	r12d, r12d
	xor	r13d, r13d
	sub	r9, r8
	lea	rdi, [rdi+r10*8]
	lea	rsi, [rsi+r10*8]
	lea	rdx, [rdx+r10*8]
	test	r10, 1
	jnz	Lodd
	lea	r8, [r8+r10*8-8]
	neg	r10
	jmp	qword Ltop
align 16
Lodd:
	lea	r8, [r8+r10*8-16]
	neg	r10
	shr	rax, 1
	mov	rbx, [rsi+r10*8]
	sbb	rbx, [rdx+r10*8]
	cmovc	rbp, [r8+8]
	cmovc	r12, [r8+r9+8]
	mov	[rdi+r10*8], rbx
	sbb	rax, rax
	inc	r10
	jz	Lend
align 16
Ltop:
	mov	rbx, [rsi+r10*8]
	shr	rax, 1
	sbb	rbx, [rdx+r10*8]
	mov	[rdi+r10*8], rbx
	sbb	r14, r14
	mov	rbx, [rsi+r10*8+8]
	sbb	rbx, [rdx+r10*8+8]
	mov	[rdi+r10*8+8], rbx
	sbb	rax, rax
	mov	rbx, [r8]
	and	rbx, r14
	add	rbp, rbx
	adc	r11, 0
	and	r14, [r8+r9]
	add	r12, r14
	adc	r13, 0
	mov	rbx, [r8-8]
	and	rbx, rax
	add	rbp, rbx
	adc	r11, 0
	mov	rbx, [r8+r9-8]
	and	rbx, rax
	add	r12, rbx
	adc	r13, 0
	add	r10, 2
	lea	r8, [r8-16]
	jnz	Ltop
Lend:
	mov	[rcx], rbp
	mov	[rcx+8], r11
	mov	[rcx+16], r12
	mov	[rcx+24], r13
	and	eax, 1
	pop	r14
	pop	r13
	pop	r12
	pop	rbp
	pop	rbx
	ret
