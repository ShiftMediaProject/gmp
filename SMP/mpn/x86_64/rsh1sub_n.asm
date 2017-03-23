section .text
align 16
global	__gmpn_rsh1sub_nc
__gmpn_rsh1sub_nc:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	mov	r8, [rsp+56]
	push	rbx
	push	rbp
	neg	r8
	mov	rbp, [rsi]
	sbb	rbp, [rdx]
	jmp	qword Lent
align 16
global	__gmpn_rsh1sub_n
__gmpn_rsh1sub_n:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	push	rbx
	push	rbp
	mov	rbp, [rsi]
	sub	rbp, [rdx]
Lent:
	sbb	ebx, ebx
	mov	eax, ebp
	and	eax, 1
	mov	r11d, ecx
	and	r11d, 3
	cmp	r11d, 1
	je	Ldo
Ln1:
	cmp	r11d, 2
	jne	Ln2
	add	ebx, ebx
	mov	r10, [rsi+8]
	sbb	r10, [rdx+8]
	lea	rsi, [rsi+8]
	lea	rdx, [rdx+8]
	lea	rdi, [rdi+8]
	sbb	ebx, ebx
	shrd	rbp, r10, 1
	mov	[rdi-8], rbp
	jmp	qword Lcj1
Ln2:
	cmp	r11d, 3
	jne	Ln3
	add	ebx, ebx
	mov	r9, [rsi+8]
	mov	r10, [rsi+16]
	sbb	r9, [rdx+8]
	sbb	r10, [rdx+16]
	lea	rsi, [rsi+16]
	lea	rdx, [rdx+16]
	lea	rdi, [rdi+16]
	sbb	ebx, ebx
	shrd	rbp, r9, 1
	mov	[rdi-16], rbp
	jmp	qword Lcj2
Ln3:
	dec	rcx
	add	ebx, ebx
	mov	r8, [rsi+8]
	mov	r9, [rsi+16]
	sbb	r8, [rdx+8]
	sbb	r9, [rdx+16]
	mov	r10, [rsi+24]
	sbb	r10, [rdx+24]
	lea	rsi, [rsi+24]
	lea	rdx, [rdx+24]
	lea	rdi, [rdi+24]
	sbb	ebx, ebx
	shrd	rbp, r8, 1
	mov	[rdi-24], rbp
	shrd	r8, r9, 1
	mov	[rdi-16], r8
Lcj2:
	shrd	r9, r10, 1
	mov	[rdi-8], r9
Lcj1:
	mov	rbp, r10
Ldo:
	shr	rcx, 2
	je	Lend
align 16
Ltop:
	add	ebx, ebx
	mov	r8, [rsi+8]
	mov	r9, [rsi+16]
	sbb	r8, [rdx+8]
	sbb	r9, [rdx+16]
	mov	r10, [rsi+24]
	mov	r11, [rsi+32]
	sbb	r10, [rdx+24]
	sbb	r11, [rdx+32]
	lea	rsi, [rsi+32]
	lea	rdx, [rdx+32]
	sbb	ebx, ebx
	shrd	rbp, r8, 1
	mov	[rdi], rbp
	shrd	r8, r9, 1
	mov	[rdi+8], r8
	shrd	r9, r10, 1
	mov	[rdi+16], r9
	shrd	r10, r11, 1
	mov	[rdi+24], r10
	dec	rcx
	mov	rbp, r11
	lea	rdi, [rdi+32]
	jne	Ltop
Lend:
	shrd	rbp, rbx, 1
	mov	[rdi], rbp
	pop	rbp
	pop	rbx
	pop	rsi
	pop	rdi
	ret
