extern __gmp_binvert_limb_table
section .text
align 16
global	__gmpn_bdiv_q_1
__gmpn_bdiv_q_1:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	push	rbx
	mov	rax, rcx
	xor	ecx, ecx
	mov	r10, rdx
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
	lea	r8, [rax+rax]
	imul	rax, rax
	imul	rax, rbx
	sub	r8, rax
	jmp	qword Lcom
Levn:
	bsf	rcx, rax
	shr	rax, cl
	jmp	qword Lodd
global	__gmpn_pi1_bdiv_q_1
__gmpn_pi1_bdiv_q_1:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	mov	r8, [rsp+56]
	mov	r9, [rsp+64]
	push	rbx
	mov	r11, rcx
	mov	r10, rdx
	mov	rcx, r9
Lcom:
	mov	rax, [rsi]
	dec	r10
	jz	Lone
	mov	rdx, [rsi+8]
	lea	rsi, [rsi+r10*8]
	lea	rdi, [rdi+r10*8]
	neg	r10
	shrd	rax, rdx, cl
	xor	ebx, ebx
	jmp	qword Lent
align 8
Ltop:
	mul	r11
	mov	rax, [rsi+r10*8]
	mov	r9, [rsi+r10*8+8]
	shrd	rax, r9, cl
	nop
	sub	rax, rbx
	setc	bl
	sub	rax, rdx
	adc	rbx, 0
Lent:
	imul	rax, r8
	mov	[rdi+r10*8], rax
	inc	r10
	jnz	Ltop
	mul	r11
	mov	rax, [rsi]
	shr	rax, cl
	sub	rax, rbx
	sub	rax, rdx
	imul	rax, r8
	mov	[rdi], rax
	pop	rbx
	pop	rsi
	pop	rdi
	ret
Lone:
	shr	rax, cl
	imul	rax, r8
	mov	[rdi], rax
	pop	rbx
	pop	rsi
	pop	rdi
	ret
