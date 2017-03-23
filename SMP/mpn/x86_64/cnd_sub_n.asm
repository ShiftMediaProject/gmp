section .text
align 16
global	__gmpn_cnd_sub_n
__gmpn_cnd_sub_n:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	mov	r8d, [rsp+56]
	push	rbx
	push	rbp
	push	r12
	push	r13
	push	r14
	neg	rdi
	sbb	rdi, rdi
	lea	rcx, [rcx+r8*8]
	lea	rdx, [rdx+r8*8]
	lea	rsi, [rsi+r8*8]
	mov	eax, r8d
	neg	r8
	and	eax, 3
	jz	Ltop
	cmp	eax, 2
	jc	Lb1
	jz	Lb2
Lb3:
	mov	r12, [rcx+r8*8]
	mov	r13, [rcx+r8*8+8]
	mov	r14, [rcx+r8*8+16]
	and	r12, rdi
	mov	r10, [rdx+r8*8]
	and	r13, rdi
	mov	rbx, [rdx+r8*8+8]
	and	r14, rdi
	mov	rbp, [rdx+r8*8+16]
	sub	r10, r12
	mov	[rsi+r8*8], r10
	sbb	rbx, r13
	mov	[rsi+r8*8+8], rbx
	sbb	rbp, r14
	mov	[rsi+r8*8+16], rbp
	sbb	eax, eax
	add	r8, 3
	js	Ltop
	jmp	qword Lend
Lb2:
	mov	r12, [rcx+r8*8]
	mov	r13, [rcx+r8*8+8]
	mov	r10, [rdx+r8*8]
	and	r12, rdi
	mov	rbx, [rdx+r8*8+8]
	and	r13, rdi
	sub	r10, r12
	mov	[rsi+r8*8], r10
	sbb	rbx, r13
	mov	[rsi+r8*8+8], rbx
	sbb	eax, eax
	add	r8, 2
	js	Ltop
	jmp	qword Lend
Lb1:
	mov	r12, [rcx+r8*8]
	mov	r10, [rdx+r8*8]
	and	r12, rdi
	sub	r10, r12
	mov	[rsi+r8*8], r10
	sbb	eax, eax
	add	r8, 1
	jns	Lend
align 16
Ltop:
	mov	r12, [rcx+r8*8]
	mov	r13, [rcx+r8*8+8]
	mov	r14, [rcx+r8*8+16]
	mov	r11, [rcx+r8*8+24]
	and	r12, rdi
	mov	r10, [rdx+r8*8]
	and	r13, rdi
	mov	rbx, [rdx+r8*8+8]
	and	r14, rdi
	mov	rbp, [rdx+r8*8+16]
	and	r11, rdi
	mov	r9, [rdx+r8*8+24]
	add	eax, eax
	sbb	r10, r12
	mov	[rsi+r8*8], r10
	sbb	rbx, r13
	mov	[rsi+r8*8+8], rbx
	sbb	rbp, r14
	mov	[rsi+r8*8+16], rbp
	sbb	r9, r11
	mov	[rsi+r8*8+24], r9
	sbb	eax, eax
	add	r8, 4
	js	Ltop
Lend:
	neg	eax
	pop	r14
	pop	r13
	pop	r12
	pop	rbp
	pop	rbx
	pop	rsi
	pop	rdi
	ret
