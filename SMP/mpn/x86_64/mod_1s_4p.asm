extern __gmpn_invert_limb
section .text
align 16
global	__gmpn_mod_1s_4p
__gmpn_mod_1s_4p:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	push	r15
	push	r14
	push	r13
	push	r12
	push	rbp
	push	rbx
	mov	r15, rdx
	mov	r14, rcx
	mov	r11, [rcx+16]
	mov	rbx, [rcx+24]
	mov	rbp, [rcx+32]
	mov	r13, [rcx+40]
	mov	r12, [rcx+48]
	xor	r8d, r8d
	mov	edx, esi
	and	edx, 3
	je	Lb0
	cmp	edx, 2
	jc	Lb1
	je	Lb2
Lb3:
	lea	rdi, [rdi+rsi*8-24]
	mov	rax, [rdi+8]
	mul	r11
	mov	r9, [rdi]
	add	r9, rax
	adc	r8, rdx
	mov	rax, [rdi+16]
	mul	rbx
	jmp	qword Lm0
align 8
Lb0:
	lea	rdi, [rdi+rsi*8-32]
	mov	rax, [rdi+8]
	mul	r11
	mov	r9, [rdi]
	add	r9, rax
	adc	r8, rdx
	mov	rax, [rdi+16]
	mul	rbx
	add	r9, rax
	adc	r8, rdx
	mov	rax, [rdi+24]
	mul	rbp
	jmp	qword Lm0
align 8
Lb1:
	lea	rdi, [rdi+rsi*8-8]
	mov	r9, [rdi]
	jmp	qword Lm1
align 8
Lb2:
	lea	rdi, [rdi+rsi*8-16]
	mov	r8, [rdi+8]
	mov	r9, [rdi]
	jmp	qword Lm1
align 16
Ltop:
	mov	rax, [rdi-24]
	mov	r10, [rdi-32]
	mul	r11
	add	r10, rax
	mov	rax, [rdi-16]
	mov	ecx, 0
	adc	rcx, rdx
	mul	rbx
	add	r10, rax
	mov	rax, [rdi-8]
	adc	rcx, rdx
	sub	rdi, 32
	mul	rbp
	add	r10, rax
	mov	rax, r13
	adc	rcx, rdx
	mul	r9
	add	r10, rax
	mov	rax, r12
	adc	rcx, rdx
	mul	r8
	mov	r9, r10
	mov	r8, rcx
Lm0:
	add	r9, rax
	adc	r8, rdx
Lm1:
	sub	rsi, 4
	ja	Ltop
Lend:
	mov	esi, [r14+8]
	mov	rax, r8
	mul	r11
	mov	r8, rax
	add	r8, r9
	adc	rdx, 0
	xor	ecx, ecx
	sub	ecx, esi
	mov	rdi, r8
	shr	rdi, cl
	mov	ecx, esi
	sal	rdx, cl
	or	rdi, rdx
	mov	rax, rdi
	mul	qword [r14]
	mov	rbx, r15
	mov	r9, rax
	sal	r8, cl
	inc	rdi
	add	r9, r8
	adc	rdx, rdi
	imul	rdx, rbx
	sub	r8, rdx
	lea	rax, [r8+rbx]
	cmp	r9, r8
	cmovc	r8, rax
	mov	rax, r8
	sub	rax, rbx
	cmovc	rax, r8
	shr	rax, cl
	pop	rbx
	pop	rbp
	pop	r12
	pop	r13
	pop	r14
	pop	r15
	pop	rsi
	pop	rdi
	ret
align 16
global	__gmpn_mod_1s_4p_cps
__gmpn_mod_1s_4p_cps:
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
	lea	rsi, [rdx+r12]
	cmp	rax, rdx
	cmovnc	rsi, rdx
	mov	rax, r11
	mul	rsi
	add	rdx, rsi
	shr	rsi, cl
	mov	[rbx+32], rsi
	not	rdx
	imul	rdx, r12
	lea	rsi, [rdx+r12]
	cmp	rax, rdx
	cmovnc	rsi, rdx
	mov	rax, r11
	mul	rsi
	add	rdx, rsi
	shr	rsi, cl
	mov	[rbx+40], rsi
	not	rdx
	imul	rdx, r12
	add	r12, rdx
	cmp	rax, rdx
	cmovnc	r12, rdx
	shr	r12, cl
	mov	[rbx+48], r12
	pop	r12
	pop	rbx
	pop	rbp
	pop	rsi
	pop	rdi
	ret
