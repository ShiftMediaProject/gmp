extern __gmpn_invert_limb
section .text
align 16
global	__gmpn_preinv_divrem_1
__gmpn_preinv_divrem_1:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	mov	r8, [rsp+56]
	mov	r9, [rsp+64]
	xor	eax, eax
	push	r13
	push	r12
	push	rbp
	push	rbx
	mov	r12, rsi
	mov	rbx, rcx
	add	rcx, rsi
	mov	rsi, rdx
	lea	rdi, [rdi+rcx*8-8]
	test	r8, r8
	js	Lnent
	mov	cl, [rsp+104]
	shl	r8, cl
	jmp	qword Luent
align 16
global	__gmpn_divrem_1
__gmpn_divrem_1:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	mov	r8, [rsp+56]
	xor	eax, eax
	push	r13
	push	r12
	push	rbp
	push	rbx
	mov	r12, rsi
	mov	rbx, rcx
	add	rcx, rsi
	mov	rsi, rdx
	je	Lret
	lea	rdi, [rdi+rcx*8-8]
	xor	ebp, ebp
	test	r8, r8
	jns	Lunnormalized
Lnormalized:
	test	rbx, rbx
	je	L8
	mov	rbp, [rsi+rbx*8-8]
	dec	rbx
	mov	rax, rbp
	sub	rbp, r8
	cmovc	rbp, rax
	sbb	eax, eax
	inc	eax
	mov	[rdi], rax
	lea	rdi, [rdi-8]
L8:
	push	r8
	sub	rsp, 32
	mov	rcx, r8
	call	qword __gmpn_invert_limb
	add	rsp, 32
	pop	r8
	mov	r9, rax
	mov	rax, rbp
	jmp	qword Lnent
align 16
Lntop:
	mov	r10, [rsi+rbx*8]
	mul	r9
	add	rax, r10
	adc	rdx, rbp
	mov	rbp, rax
	mov	r13, rdx
	imul	rdx, r8
	sub	r10, rdx
	mov	rax, r8
	add	rax, r10
	cmp	r10, rbp
	cmovc	rax, r10
	adc	r13, -1
	cmp	rax, r8
	jae	Lnfx
Lnok:
	mov	[rdi], r13
	sub	rdi, 8
Lnent:
	lea	rbp, [rax+1]
	dec	rbx
	jns	Lntop
	xor	ecx, ecx
	jmp	qword Lfrac
Lnfx:
	sub	rax, r8
	inc	r13
	jmp	qword Lnok
Lunnormalized:
	test	rbx, rbx
	je	L44
	mov	rax, [rsi+rbx*8-8]
	cmp	rax, r8
	jae	L44
	mov	[rdi], rbp
	mov	rbp, rax
	lea	rdi, [rdi-8]
	je	Lret
	dec	rbx
L44:
	bsr	rcx, r8
	not	ecx
	shl	r8, cl
	shl	rbp, cl
	push	rcx
	push	r8
	sub	rsp, 40
	mov	rcx, r8
	call	qword __gmpn_invert_limb
	add	rsp, 40
	pop	r8
	pop	rcx
	mov	r9, rax
	mov	rax, rbp
	test	rbx, rbx
	je	Lfrac
Luent:
	dec	rbx
	mov	rbp, [rsi+rbx*8]
	neg	ecx
	shr	rbp, cl
	neg	ecx
	or	rax, rbp
	jmp	qword Lent
align 16
Lutop:
	mov	r10, [rsi+rbx*8]
	shl	rbp, cl
	neg	ecx
	shr	r10, cl
	neg	ecx
	or	rbp, r10
	mul	r9
	add	rax, rbp
	adc	rdx, r11
	mov	r11, rax
	mov	r13, rdx
	imul	rdx, r8
	sub	rbp, rdx
	mov	rax, r8
	add	rax, rbp
	cmp	rbp, r11
	cmovc	rax, rbp
	adc	r13, -1
	cmp	rax, r8
	jae	Lufx
Luok:
	mov	[rdi], r13
	sub	rdi, 8
Lent:
	mov	rbp, [rsi+rbx*8]
	dec	rbx
	lea	r11, [rax+1]
	jns	Lutop
Luend:
	shl	rbp, cl
	mul	r9
	add	rax, rbp
	adc	rdx, r11
	mov	r11, rax
	mov	r13, rdx
	imul	rdx, r8
	sub	rbp, rdx
	mov	rax, r8
	add	rax, rbp
	cmp	rbp, r11
	cmovc	rax, rbp
	adc	r13, -1
	cmp	rax, r8
	jae	Lefx
Leok:
	mov	[rdi], r13
	sub	rdi, 8
	jmp	qword Lfrac
Lufx:
	sub	rax, r8
	inc	r13
	jmp	qword Luok
Lefx:
	sub	rax, r8
	inc	r13
	jmp	qword Leok
Lfrac:
	mov	rbp, r8
	neg	rbp
	jmp	qword Lfent
align 16
Lftop:
	mul	r9
	add	rdx, r11
	mov	r11, rax
	mov	r13, rdx
	imul	rdx, rbp
	mov	rax, r8
	add	rax, rdx
	cmp	rdx, r11
	cmovc	rax, rdx
	adc	r13, -1
	mov	[rdi], r13
	sub	rdi, 8
Lfent:
	lea	r11, [rax+1]
	dec	r12
	jns	Lftop
	shr	rax, cl
Lret:
	pop	rbx
	pop	rbp
	pop	r12
	pop	r13
	pop	rsi
	pop	rdi
	ret
