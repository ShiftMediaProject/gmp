extern __gmpn_invert_limb
section .text
align 16
global	__gmpn_mod_1_1p
__gmpn_mod_1_1p:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	push	rbp
	push	rbx
	mov	rbx, rdx
	mov	r8, rcx
	mov	rax, [rdi+rsi*8-8]
	cmp	rsi, 3
	jnc	Lfirst
	mov	rbp, [rdi+rsi*8-16]
	jmp	qword Lreduce_two
Lfirst:
	mov	r11, [r8+24]
	mul	r11
	mov	rbp, [rdi+rsi*8-24]
	add	rbp, rax
	mov	rax, [rdi+rsi*8-16]
	adc	rax, rdx
	sbb	rcx, rcx
	sub	rsi, 4
	jc	Lreduce_three
	mov	r10, r11
	sub	r10, rbx
align 16
Ltop:
	and	rcx, r11
	lea	r9, [r10+rbp]
	mul	r11
	add	rcx, rbp
	mov	rbp, [rdi+rsi*8]
	cmovc	rcx, r9
	add	rbp, rax
	mov	rax, rcx
	adc	rax, rdx
	sbb	rcx, rcx
	sub	rsi, 1
	jnc	Ltop
Lreduce_three:
	and	rcx, rbx
	sub	rax, rcx
Lreduce_two:
	mov	ecx, [r8+8]
	test	ecx, ecx
	jz	Lnormalized
	mul	qword [r8+16]
	xor	r9, r9
	add	rbp, rax
	adc	r9, rdx
	mov	rax, r9
	shld	rax, rbp, cl
	shl	rbp, cl
	jmp	qword Ludiv
Lnormalized:
	mov	r9, rax
	sub	r9, rbx
	cmovnc	rax, r9
Ludiv:
	lea	r9, [rax+1]
	mul	qword [r8]
	add	rax, rbp
	adc	rdx, r9
	imul	rdx, rbx
	sub	rbp, rdx
	cmp	rax, rbp
	lea	rax, [rbx+rbp]
	cmovnc	rax, rbp
	cmp	rax, rbx
	jnc	Lfix
Lok:
	shr	rax, cl
	pop	rbx
	pop	rbp
	pop	rsi
	pop	rdi
	ret
Lfix:
	sub	rax, rbx
	jmp	qword Lok
align 16
global	__gmpn_mod_1_1p_cps
__gmpn_mod_1_1p_cps:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	push	rbp
	bsr	rcx, rsi
	push	rbx
	mov	rbx, rdi
	push	r12
	xor	ecx, 63
	mov	r12, rsi
	mov	ebp, ecx
	sal	r12, cl
	mov	rcx, r12
	call	qword __gmpn_invert_limb
	neg	r12
	mov	r8, r12
	mov	[rbx], rax
	mov	[rbx+8], rbp
	imul	r12, rax
	mov	[rbx+24], r12
	mov	ecx, ebp
	test	ecx, ecx
	jz	Lz
	mov	edx, 1
	shld	rdx, rax, cl
	imul	r8, rdx
	shr	r8, cl
	mov	[rbx+16], r8
Lz:
	pop	r12
	pop	rbx
	pop	rbp
	pop	rsi
	pop	rdi
	ret
