section .text
align 16
global	__gmpn_addlsh1_nc
__gmpn_addlsh1_nc:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	mov	r8, [rsp+56]
	push	rbp
	mov	rax, r8
	neg	rax
	xor	ebp, ebp
	mov	r8, [rdx]
	shrd	rbp, r8, 63
	mov	r9d, ecx
	and	r9d, 3
	je	Lb00
	cmp	r9d, 2
	jc	Lb01
	je	Lb10
	jmp	qword Lb11
align 16
global	__gmpn_addlsh1_n
__gmpn_addlsh1_n:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	push	rbp
	xor	ebp, ebp
	mov	r8, [rdx]
	shrd	rbp, r8, 63
	mov	eax, ecx
	and	eax, 3
	je	Lb00
	cmp	eax, 2
	jc	Lb01
	je	Lb10
Lb11:
	mov	r9, [rdx+8]
	shrd	r8, r9, 63
	mov	r10, [rdx+16]
	shrd	r9, r10, 63
	add	eax, eax
	adc	rbp, [rsi]
	adc	r8, [rsi+8]
	adc	r9, [rsi+16]
	mov	[rdi], rbp
	mov	[rdi+8], r8
	mov	[rdi+16], r9
	mov	rbp, r10
	lea	rsi, [rsi+24]
	lea	rdx, [rdx+24]
	lea	rdi, [rdi+24]
	sbb	eax, eax
	sub	rcx, 3
	ja	Ltop
	jmp	qword Lend
Lb01:
	add	eax, eax
	adc	rbp, [rsi]
	mov	[rdi], rbp
	mov	rbp, r8
	lea	rsi, [rsi+8]
	lea	rdx, [rdx+8]
	lea	rdi, [rdi+8]
	sbb	eax, eax
	sub	rcx, 1
	ja	Ltop
	jmp	qword Lend
Lb10:
	mov	r9, [rdx+8]
	shrd	r8, r9, 63
	add	eax, eax
	adc	rbp, [rsi]
	adc	r8, [rsi+8]
	mov	[rdi], rbp
	mov	[rdi+8], r8
	mov	rbp, r9
	lea	rsi, [rsi+16]
	lea	rdx, [rdx+16]
	lea	rdi, [rdi+16]
	sbb	eax, eax
	sub	rcx, 2
	ja	Ltop
	jmp	qword Lend
align 16
Ltop:
	mov	r8, [rdx]
	shrd	rbp, r8, 63
Lb00:
	mov	r9, [rdx+8]
	shrd	r8, r9, 63
	mov	r10, [rdx+16]
	shrd	r9, r10, 63
	mov	r11, [rdx+24]
	shrd	r10, r11, 63
	lea	rdx, [rdx+32]
	add	eax, eax
	adc	rbp, [rsi]
	adc	r8, [rsi+8]
	adc	r9, [rsi+16]
	adc	r10, [rsi+24]
	lea	rsi, [rsi+32]
	mov	[rdi], rbp
	mov	[rdi+8], r8
	mov	[rdi+16], r9
	mov	[rdi+24], r10
	mov	rbp, r11
	lea	rdi, [rdi+32]
	sbb	eax, eax
	sub	rcx, 4
	jnz	Ltop
Lend:
	shr	rbp, 63
	add	eax, eax
	adc	rbp, 0
	mov	rax, rbp
	pop	rbp
	pop	rsi
	pop	rdi
	ret
