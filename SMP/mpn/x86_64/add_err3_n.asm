section .text
align 16
global	__gmpn_add_err3_n
__gmpn_add_err3_n:
	mov	rax, [rsp+24]
	mov	r10, [rsp+16]
	push	rbx
	push	rbp
	push	r12
	push	r13
	push	r14
	push	r15
	push	rcx
	mov	rcx, [rsp+64]
	xor	ebp, ebp
	xor	r11d, r11d
	xor	r12d, r12d
	xor	r13d, r13d
	xor	r14d, r14d
	xor	r15d, r15d
	sub	r9, r8
	sub	rcx, r8
	lea	r8, [r8+r10*8-8]
	lea	rdi, [rdi+r10*8]
	lea	rsi, [rsi+r10*8]
	lea	rdx, [rdx+r10*8]
	neg	r10
align 16
Ltop:
	shr	rax, 1
	mov	rax, [rsi+r10*8]
	adc	rax, [rdx+r10*8]
	mov	[rdi+r10*8], rax
	sbb	rax, rax
	mov	rbx, [r8]
	and	rbx, rax
	add	rbp, rbx
	adc	r11, 0
	mov	rbx, [r8+r9]
	and	rbx, rax
	add	r12, rbx
	adc	r13, 0
	mov	rbx, [r8+rcx]
	and	rbx, rax
	add	r14, rbx
	adc	r15, 0
	lea	r8, [r8-8]
	inc	r10
	jnz	Ltop
Lend:
	and	eax, 1
	pop	rcx
	mov	[rcx], rbp
	mov	[rcx+8], r11
	mov	[rcx+16], r12
	mov	[rcx+24], r13
	mov	[rcx+32], r14
	mov	[rcx+40], r15
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	rbp
	pop	rbx
	ret
