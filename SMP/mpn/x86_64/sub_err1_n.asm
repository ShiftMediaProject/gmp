section .text
align 16
global	__gmpn_sub_err1_n
__gmpn_sub_err1_n:
	mov	rax, [rsp+8]
	push	rbx
	push	rbp
	push	r12
	push	r13
	push	r14
	push	r15
	lea	rsi, [rsi+r9*8]
	lea	rdx, [rdx+r9*8]
	lea	rdi, [rdi+r9*8]
	mov	r10d, r9d
	and	r10d, 3
	jz	L0mod4
	cmp	r10d, 2
	jc	L1mod4
	jz	L2mod4
L3mod4:
	xor	ebx, ebx
	xor	ebp, ebp
	xor	r10d, r10d
	xor	r11d, r11d
	lea	r8, [r8+r9*8-24]
	neg	r9
	shr	al, 1
	mov	r14, [rsi+r9*8]
	mov	r15, [rsi+r9*8+8]
	sbb	r14, [rdx+r9*8]
	mov	[rdi+r9*8], r14
	cmovc	rbx, [r8+16]
	sbb	r15, [rdx+r9*8+8]
	mov	[rdi+r9*8+8], r15
	cmovc	r10, [r8+8]
	mov	r14, [rsi+r9*8+16]
	sbb	r14, [rdx+r9*8+16]
	mov	[rdi+r9*8+16], r14
	cmovc	r11, [r8]
	setc	al
	add	rbx, r10
	adc	rbp, 0
	add	rbx, r11
	adc	rbp, 0
	add	r9, 3
	jnz	Lloop
	jmp	qword Lend
align 16
L0mod4:
	xor	ebx, ebx
	xor	ebp, ebp
	lea	r8, [r8+r9*8]
	neg	r9
	jmp	qword Lloop
align 16
L1mod4:
	xor	ebx, ebx
	xor	ebp, ebp
	lea	r8, [r8+r9*8-8]
	neg	r9
	shr	al, 1
	mov	r14, [rsi+r9*8]
	sbb	r14, [rdx+r9*8]
	mov	[rdi+r9*8], r14
	cmovc	rbx, [r8]
	setc	al
	add	r9, 1
	jnz	Lloop
	jmp	qword Lend
align 16
L2mod4:
	xor	ebx, ebx
	xor	ebp, ebp
	xor	r10d, r10d
	lea	r8, [r8+r9*8-16]
	neg	r9
	shr	al, 1
	mov	r14, [rsi+r9*8]
	mov	r15, [rsi+r9*8+8]
	sbb	r14, [rdx+r9*8]
	mov	[rdi+r9*8], r14
	cmovc	rbx, [r8+8]
	sbb	r15, [rdx+r9*8+8]
	mov	[rdi+r9*8+8], r15
	cmovc	r10, [r8]
	setc	al
	add	rbx, r10
	adc	rbp, 0
	add	r9, 2
	jnz	Lloop
	jmp	qword Lend
align 32
Lloop:
	mov	r14, [rsi+r9*8]
	shr	al, 1
	mov	r10, [r8-8]
	mov	r13d, 0
	sbb	r14, [rdx+r9*8]
	cmovnc	r10, r13
	mov	[rdi+r9*8], r14
	mov	r15, [rsi+r9*8+8]
	mov	r14, [rsi+r9*8+16]
	sbb	r15, [rdx+r9*8+8]
	mov	r11, [r8-16]
	cmovnc	r11, r13
	mov	r12, [r8-24]
	mov	[rdi+r9*8+8], r15
	sbb	r14, [rdx+r9*8+16]
	cmovnc	r12, r13
	mov	r15, [rsi+r9*8+24]
	sbb	r15, [rdx+r9*8+24]
	cmovc	r13, [r8-32]
	setc	al
	add	rbx, r10
	adc	rbp, 0
	add	rbx, r11
	adc	rbp, 0
	add	rbx, r12
	adc	rbp, 0
	lea	r8, [r8-32]
	mov	[rdi+r9*8+16], r14
	add	rbx, r13
	adc	rbp, 0
	add	r9, 4
	mov	[rdi+r9*8-8], r15
	jnz	Lloop
Lend:
	mov	[rcx], rbx
	mov	[rcx+8], rbp
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	rbp
	pop	rbx
	ret
