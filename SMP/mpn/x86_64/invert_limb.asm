extern __gmpn_invert_limb_table
section .text
align 16
global	__gmpn_invert_limb
__gmpn_invert_limb:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rax, rdi
	shr	rax, 55
	lea	r8, [rel -512+__gmpn_invert_limb_table]
	movzx	dword ecx, word [r8+rax*2]

	mov	rsi, rdi
	mov	eax, ecx
	imul	ecx, ecx
	shr	rsi, 24
	inc	rsi
	imul	rcx, rsi
	shr	rcx, 40
	sal	eax, 11
	dec	eax
	sub	eax, ecx
	mov	rcx, 0x1000000000000000
	imul	rsi, rax
	sub	rcx, rsi
	imul	rcx, rax
	sal	rax, 13
	shr	rcx, 47
	add	rcx, rax
	mov	rsi, rdi
	shr rsi, 1
	sbb	rax, rax
	sub	rsi, rax
	imul	rsi, rcx
	and	rax, rcx
	shr rax, 1
	sub	rax, rsi
	mul	rcx
	sal	rcx, 31
	shr rdx, 1
	add	rcx, rdx
	mov	rax, rdi
	mul	rcx
	add	rax, rdi
	mov	rax, rcx
	adc	rdx, rdi
	sub	rax, rdx
	pop	rsi
	pop	rdi
	ret
