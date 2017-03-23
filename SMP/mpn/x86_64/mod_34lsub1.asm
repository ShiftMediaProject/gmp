section .text
align 32
global	__gmpn_mod_34lsub1
__gmpn_mod_34lsub1:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	r11, 0x0000FFFFFFFFFFFF
	sub	rsi, 2
	ja	Lgt2
	mov	rax, [rdi]
	nop
	jb	L1
	mov	rsi, [rdi+8]
	mov	rdx, rax
	shr	rax, 48
	and	rdx, r11
	add	rax, rdx
	mov	edx, esi
	shr	rsi, 32
	add	rax, rsi
	shl	rdx, 16
	add	rax, rdx
L1:
	pop	rsi
	pop	rdi
	ret
align 16
Lgt2:
	xor	eax, eax
	xor	ecx, ecx
	xor	edx, edx
	xor	r8, r8
	xor	r9, r9
	xor	r10, r10
Ltop:
	add	rax, [rdi]
	adc	r10, 0
	add	rcx, [rdi+8]
	adc	r8, 0
	add	rdx, [rdi+16]
	adc	r9, 0
	sub	rsi, 3
	jng	Lend
	add	rax, [rdi+24]
	adc	r10, 0
	add	rcx, [rdi+32]
	adc	r8, 0
	add	rdx, [rdi+40]
	lea	rdi, [rdi+48]
	adc	r9, 0
	sub	rsi, 3
	jg	Ltop
	add	rdi, -24
Lend:
	add	rax, r9
	adc	rcx, r10
	adc	rdx, r8
	inc	rsi
	mov	r10d, 0x1
	js	Lcombine
	mov	r10d, 0x10000
	adc	rax, [rdi+24]
	dec	rsi
	js	Lcombine
	adc	rcx, [rdi+32]
	mov	r10, 0x100000000
Lcombine:
	sbb	rsi, rsi
	mov	rdi, rax
	shr	rax, 48
	and	rsi, r10
	and	rdi, r11
	mov	r10d, ecx
	add	rax, rsi
	shr	rcx, 32
	add	rax, rdi
	movzx	dword edi, word dx

	shl	r10, 16
	add	rax, rcx
	shr	rdx, 16
	add	rax, r10
	shl	rdi, 32
	add	rax, rdx
	add	rax, rdi
	pop	rsi
	pop	rdi
	ret
