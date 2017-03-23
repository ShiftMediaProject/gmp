extern __gmp_binvert_limb_table
section .text
align 32
global	__gmpn_modexact_1_odd
__gmpn_modexact_1_odd:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	ecx, 0
	jmp	qword Lent
global	__gmpn_modexact_1c_odd
__gmpn_modexact_1c_odd:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
Lent:
	mov	r8, rdx
	shr edx, 1
	lea	r9, [rel __gmp_binvert_limb_table]
	and	edx, 127
	mov	r10, rcx
	movzx	dword edx, byte [r9+rdx]

	mov	rax, [rdi]
	lea	r11, [rdi+rsi*8]
	mov	rdi, r8
	lea	ecx, [rdx+rdx]
	imul	edx, edx
	neg	rsi
	imul	edx, edi
	sub	ecx, edx
	lea	edx, [rcx+rcx]
	imul	ecx, ecx
	imul	ecx, edi
	sub	edx, ecx
	xor	ecx, ecx
	lea	r9, [rdx+rdx]
	imul	rdx, rdx
	imul	rdx, r8
	sub	r9, rdx
	mov	rdx, r10
	inc	rsi
	jz	Lone
align 16
Ltop:
	sub	rax, rdx
	adc	rcx, 0
	imul	rax, r9
	mul	r8
	mov	rax, [r11+rsi*8]
	sub	rax, rcx
	setc	cl
	inc	rsi
	jnz	Ltop
Lone:
	sub	rax, rdx
	adc	rcx, 0
	imul	rax, r9
	mul	r8
	lea	rax, [rcx+rdx]
	pop	rsi
	pop	rdi
	ret
