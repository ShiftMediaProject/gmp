extern __gmp_binvert_limb_table
section .text
align 16
global	__gmpn_divexact_1
__gmpn_divexact_1:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	push	rbx
	mov	rax, rcx
	xor	ecx, ecx
	mov	r8, rdx
	bt	eax, 0
	jnc	Levn
Lodd:
	mov	rbx, rax
	shr eax, 1
	and	eax, 127
	lea	rdx, [rel __gmp_binvert_limb_table]
	movzx	dword eax, byte [rdx+rax]

	mov	r11, rbx
	lea	edx, [rax+rax]
	imul	eax, eax
	imul	eax, ebx
	sub	edx, eax
	lea	eax, [rdx+rdx]
	imul	edx, edx
	imul	edx, ebx
	sub	eax, edx
	lea	r10, [rax+rax]
	imul	rax, rax
	imul	rax, rbx
	sub	r10, rax
	lea	rsi, [rsi+r8*8]
	lea	rdi, [rdi+r8*8-8]
	neg	r8
	mov	rax, [rsi+r8*8]
	inc	r8
	jz	Lone
	mov	rdx, [rsi+r8*8]
	shrd	rax, rdx, cl
	xor	ebx, ebx
	jmp	qword Lent
Levn:
	bsf	rcx, rax
	shr	rax, cl
	jmp	qword Lodd
align 8
Ltop:
	mul	r11
	mov	rax, [rsi+r8*8-8]
	mov	r9, [rsi+r8*8]
	shrd	rax, r9, cl
	nop
	sub	rax, rbx
	setc	bl
	sub	rax, rdx
	adc	rbx, 0
Lent:
	imul	rax, r10
	mov	[rdi+r8*8], rax
	inc	r8
	jnz	Ltop
	mul	r11
	mov	rax, [rsi-8]
	shr	rax, cl
	sub	rax, rbx
	sub	rax, rdx
	imul	rax, r10
	mov	[rdi], rax
	pop	rbx
	pop	rsi
	pop	rdi
	ret
Lone:
	shr	rax, cl
	imul	rax, r10
	mov	[rdi], rax
	pop	rbx
	pop	rsi
	pop	rdi
	ret
