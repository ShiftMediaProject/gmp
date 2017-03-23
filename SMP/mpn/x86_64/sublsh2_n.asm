section .text
align 8
global	__gmpn_sublsh2_n
__gmpn_sublsh2_n:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	push	rbx
	push	r12
	mov	eax, ecx
	lea	rsi, [rsi+rcx*8+24]
	lea	rdx, [rdx+rcx*8+24]
	lea	rdi, [rdi+rcx*8+24]
	neg	rcx
	xor	r11d, r11d
	mov	r8, [rdx+rcx*8-24]
	shrd	r11, r8, 62
	and	eax, 3
	je	Lb0
	cmp	eax, 2
	jc	Lb1
	je	Lb2
Lb3:
	mov	r9, [rdx+rcx*8-16]
	shrd	r8, r9, 62
	mov	r10, [rdx+rcx*8-8]
	shrd	r9, r10, 62
	mov	r12, [rsi+rcx*8-24]
	sub	r12, r11
	mov	[rdi+rcx*8-24], r12
	mov	r12, [rsi+rcx*8-16]
	sbb	r12, r8
	mov	[rdi+rcx*8-16], r12
	mov	r12, [rsi+rcx*8-8]
	sbb	r12, r9
	mov	[rdi+rcx*8-8], r12
	mov	r11, r10
	sbb	eax, eax
	add	rcx, 3
	js	Ltop
	jmp	qword Lend
Lb1:
	mov	r12, [rsi+rcx*8-24]
	sub	r12, r11
	mov	[rdi+rcx*8-24], r12
	mov	r11, r8
	sbb	eax, eax
	inc	rcx
	js	Ltop
	jmp	qword Lend
Lb2:
	mov	r9, [rdx+rcx*8-16]
	shrd	r8, r9, 62
	mov	r12, [rsi+rcx*8-24]
	sub	r12, r11
	mov	[rdi+rcx*8-24], r12
	mov	r12, [rsi+rcx*8-16]
	sbb	r12, r8
	mov	[rdi+rcx*8-16], r12
	mov	r11, r9
	sbb	eax, eax
	add	rcx, 2
	js	Ltop
	jmp	qword Lend
align 16
Ltop:
	mov	r8, [rdx+rcx*8-24]
	shrd	r11, r8, 62
Lb0:
	mov	r9, [rdx+rcx*8-16]
	shrd	r8, r9, 62
	mov	r10, [rdx+rcx*8-8]
	shrd	r9, r10, 62
	mov	rbx, [rdx+rcx*8]
	shrd	r10, rbx, 62
	add	eax, eax
	mov	r12, [rsi+rcx*8-24]
	sbb	r12, r11
	mov	[rdi+rcx*8-24], r12
	mov	r12, [rsi+rcx*8-16]
	sbb	r12, r8
	mov	[rdi+rcx*8-16], r12
	mov	r12, [rsi+rcx*8-8]
	sbb	r12, r9
	mov	[rdi+rcx*8-8], r12
	mov	r12, [rsi+rcx*8]
	sbb	r12, r10
	mov	[rdi+rcx*8], r12
	mov	r11, rbx
	sbb	eax, eax
	add	rcx, 4
	js	Ltop
Lend:
	shr	r11, 62
	pop	r12
	pop	rbx
	sub	eax, r11d
	neg	eax
	pop	rsi
	pop	rdi
	ret
