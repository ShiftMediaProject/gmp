section .text
align 32
global	__gmpn_rsblsh_n
__gmpn_rsblsh_n:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	mov	r8d, [rsp+56]
	push	rbx
	xor	ebx, ebx
Lent:
	push	rbp
	mov	ebp, ecx
	mov	rax, rcx
	mov	ecx, r8d
	neg	ecx
	and	ebp, 3
	jz	Lb0
	lea	rdx, [rdx+rbp*8-32]
	lea	rsi, [rsi+rbp*8-32]
	lea	rdi, [rdi+rbp*8-32]
	cmp	ebp, 2
	jc	Lb1
	jz	Lb2
Lb3:
	xor	r8, r8
	mov	r9, [rdx+8]
	mov	r10, [rdx+16]
	shrd	r8, r9, cl
	shrd	r9, r10, cl
	mov	r11, [rdx+24]
	shrd	r10, r11, cl
	sub	rax, 3
	jz	L3
	add	ebx, ebx
	lea	rdx, [rdx+32]
	sbb	r8, [rsi+8]
	sbb	r9, [rsi+16]
	sbb	r10, [rsi+24]
	lea	rsi, [rsi+32]
	jmp	qword Llo3
L3:
	add	ebx, ebx
	lea	rdx, [rdx+32]
	sbb	r8, [rsi+8]
	sbb	r9, [rsi+16]
	sbb	r10, [rsi+24]
	jmp	qword Lwd3
Lb0:
	mov	r8, [rdx]
	mov	r9, [rdx+8]
	xor	ebp, ebp
	jmp	qword Llo0
Lb1:
	xor	r10, r10
	mov	r11, [rdx+24]
	shrd	r10, r11, cl
	sub	rax, 1
	jz	L1
	add	ebx, ebx
	lea	rdx, [rdx+32]
	sbb	r10, [rsi+24]
	lea	rsi, [rsi+32]
	mov	r8, [rdx]
	jmp	qword Llo1
L1:
	add	ebx, ebx
	sbb	r10, [rsi+24]
	jmp	qword Lwd1
Lb2:
	xor	r9, r9
	mov	r10, [rdx+16]
	shrd	r9, r10, cl
	mov	r11, [rdx+24]
	shrd	r10, r11, cl
	sub	rax, 2
	jz	L2
	add	ebx, ebx
	lea	rdx, [rdx+32]
	sbb	r9, [rsi+16]
	sbb	r10, [rsi+24]
	lea	rsi, [rsi+32]
	jmp	qword Llo2
L2:
	add	ebx, ebx
	sbb	r9, [rsi+16]
	sbb	r10, [rsi+24]
	jmp	qword Lwd2
align 32
Ltop:
	shrd	r10, r11, cl
	add	ebx, ebx
	lea	rdx, [rdx+32]
	sbb	rbp, [rsi]
	sbb	r8, [rsi+8]
	sbb	r9, [rsi+16]
	sbb	r10, [rsi+24]
	mov	[rdi], rbp
	lea	rsi, [rsi+32]
Llo3:
	mov	[rdi+8], r8
Llo2:
	mov	[rdi+16], r9
	mov	r8, [rdx]
Llo1:
	mov	[rdi+24], r10
	mov	r9, [rdx+8]
	mov	rbp, r11
	lea	rdi, [rdi+32]
	sbb	ebx, ebx
Llo0:
	shrd	rbp, r8, cl
	mov	r10, [rdx+16]
	shrd	r8, r9, cl
	shrd	r9, r10, cl
	mov	r11, [rdx+24]
	sub	rax, 4
	jg	Ltop
	shrd	r10, r11, cl
	add	ebx, ebx
	sbb	rbp, [rsi]
	sbb	r8, [rsi+8]
	sbb	r9, [rsi+16]
	sbb	r10, [rsi+24]
	mov	[rdi], rbp
Lwd3:
	mov	[rdi+8], r8
Lwd2:
	mov	[rdi+16], r9
Lwd1:
	mov	[rdi+24], r10
	adc	eax, eax
	shr	r11, cl
	sub	rax, r11
	neg	rax
	pop	rbp
	pop	rbx
	pop	rsi
	pop	rdi
	ret
global	__gmpn_rsblsh_nc
__gmpn_rsblsh_nc:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	mov	r8d, [rsp+56]
	mov	r9, [rsp+64]
	push	rbx
	neg	r9
	sbb	ebx, ebx
	jmp	qword Lent
