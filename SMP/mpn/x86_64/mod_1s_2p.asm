extern __gmpn_invert_limb
section .text
align 16
global	__gmpn_mod_1s_2p
__gmpn_mod_1s_2p:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	push	r14
	test	sil, 1
	mov	r14, rdx
	push	r13
	mov	r13, rcx
	push	r12
	push	rbp
	push	rbx
	mov	r10, [rcx+16]
	mov	rbx, [rcx+24]
	mov	rbp, [rcx+32]
	je	Lb0
	dec	rsi
	je	Lone
	mov	rax, [rdi+rsi*8-8]
	mul	r10
	mov	r9, rax
	mov	r8, rdx
	mov	rax, [rdi+rsi*8]
	add	r9, [rdi+rsi*8-16]
	adc	r8, 0
	mul	rbx
	add	r9, rax
	adc	r8, rdx
	jmp	qword L11
Lb0:
	mov	r8, [rdi+rsi*8-8]
	mov	r9, [rdi+rsi*8-16]
L11:
	sub	rsi, 4
	jb	Led2
	lea	rdi, [rdi+rsi*8+40]
	mov	r11, [rdi-40]
	mov	rax, [rdi-32]
	jmp	qword Lm0
align 16
Ltop:
	mov	r9, [rdi-24]
	add	r11, rax
	mov	rax, [rdi-16]
	adc	r12, rdx
	mul	r10
	add	r9, rax
	mov	rax, r11
	mov	r8, rdx
	adc	r8, 0
	mul	rbx
	add	r9, rax
	mov	rax, r12
	adc	r8, rdx
	mul	rbp
	sub	rsi, 2
	jb	Led1
	mov	r11, [rdi-40]
	add	r9, rax
	mov	rax, [rdi-32]
	adc	r8, rdx
Lm0:
	mul	r10
	add	r11, rax
	mov	rax, r9
	mov	r12, rdx
	adc	r12, 0
	mul	rbx
	add	r11, rax
	lea	rdi, [rdi-32]
	mov	rax, r8
	adc	r12, rdx
	mul	rbp
	sub	rsi, 2
	jae	Ltop
Led0:
	mov	r9, r11
	mov	r8, r12
Led1:
	add	r9, rax
	adc	r8, rdx
Led2:
	mov	edi, [r13+8]
	mov	rax, r8
	mov	r8, r9
	mul	r10
	add	r8, rax
	adc	rdx, 0
L1:
	xor	ecx, ecx
	mov	r9, r8
	sub	ecx, edi
	shr	r9, cl
	mov	ecx, edi
	sal	rdx, cl
	or	r9, rdx
	sal	r8, cl
	mov	rax, r9
	mul	qword [r13]
	mov	rsi, rax
	inc	r9
	add	rsi, r8
	adc	rdx, r9
	imul	rdx, r14
	sub	r8, rdx
	lea	rax, [r8+r14]
	cmp	rsi, r8
	cmovc	r8, rax
	mov	rax, r8
	sub	rax, r14
	cmovc	rax, r8
	mov	ecx, edi
	shr	rax, cl
	pop	rbx
	pop	rbp
	pop	r12
	pop	r13
	pop	r14
	pop	rsi
	pop	rdi
	ret
Lone:
	mov	r8, [rdi]
	mov	edi, [rcx+8]
	xor	rdx, rdx
	jmp	qword L1
align 16
global	__gmpn_mod_1s_2p_cps
__gmpn_mod_1s_2p_cps:
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
	mov	r8, r12
	mov	r11, rax
	mov	[rbx], rax
	mov	[rbx+8], rbp
	neg	r8
	mov	ecx, ebp
	mov	esi, 1
	shld	rsi, rax, cl
	imul	rsi, r8
	mul	rsi
	add	rdx, rsi
	shr	rsi, cl
	mov	[rbx+16], rsi
	not	rdx
	imul	rdx, r12
	lea	rsi, [rdx+r12]
	cmp	rax, rdx
	cmovnc	rsi, rdx
	mov	rax, r11
	mul	rsi
	add	rdx, rsi
	shr	rsi, cl
	mov	[rbx+24], rsi
	not	rdx
	imul	rdx, r12
	add	r12, rdx
	cmp	rax, rdx
	cmovnc	r12, rdx
	shr	r12, cl
	mov	[rbx+32], r12
	pop	r12
	pop	rbx
	pop	rbp
	pop	rsi
	pop	rdi
	ret
