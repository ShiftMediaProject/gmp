section .text
align 32
global	__gmpn_sub_n
__gmpn_sub_n:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	xor	r8, r8
Lent:
	mov	eax, ecx
	shr	rcx, 2
	test	al, 1
	jnz	Lbx1
Lbx0:
	test	al, 2
	jnz	Lb10
Lb00:
	neg	r8
	mov	r8, [rsi]
	mov	r9, [rsi+8]
	sbb	r8, [rdx]
	sbb	r9, [rdx+8]
	mov	r10, [rsi+16]
	mov	r11, [rsi+24]
	lea	rsi, [rsi+32]
	sbb	r10, [rdx+16]
	sbb	r11, [rdx+24]
	lea	rdx, [rdx+32]
	lea	rdi, [rdi-16]
	jmp	qword Llo0
Lb10:
	neg	r8
	mov	r10, [rsi]
	mov	r11, [rsi+8]
	sbb	r10, [rdx+0]
	sbb	r11, [rdx+8]
	jrcxz	Le2
	mov	r8, [rsi+16]
	mov	r9, [rsi+24]
	lea	rsi, [rsi+16]
	sbb	r8, [rdx+16]
	sbb	r9, [rdx+24]
	lea	rdx, [rdx+16]
	lea	rdi, [rdi]
	jmp	qword Llo2
Le2:
	mov	[rdi], r10
	mov	[rdi+8], r11
	setc	al
	pop	rsi
	pop	rdi
	ret
Lbx1:
	test	al, 2
	jnz	Lb11
Lb01:
	neg	r8
	mov	r11, [rsi]
	sbb	r11, [rdx]
	jrcxz	Le1
	mov	r8, [rsi+8]
	mov	r9, [rsi+16]
	lea	rsi, [rsi+8]
	lea	rdi, [rdi-8]
	sbb	r8, [rdx+8]
	sbb	r9, [rdx+16]
	lea	rdx, [rdx+8]
	jmp	qword Llo1
Le1:
	mov	[rdi], r11
	setc	al
	pop	rsi
	pop	rdi
	ret
Lb11:
	neg	r8
	mov	r9, [rsi]
	sbb	r9, [rdx]
	mov	r10, [rsi+8]
	mov	r11, [rsi+16]
	lea	rsi, [rsi+24]
	sbb	r10, [rdx+8]
	sbb	r11, [rdx+16]
	lea	rdx, [rdx+24]
	mov	[rdi], r9
	lea	rdi, [rdi+8]
	jrcxz	Lend
align 32
Ltop:
	mov	r8, [rsi]
	mov	r9, [rsi+8]
	sbb	r8, [rdx]
	sbb	r9, [rdx+8]
Llo2:
	mov	[rdi], r10
Llo1:
	mov	[rdi+8], r11
	mov	r10, [rsi+16]
	mov	r11, [rsi+24]
	lea	rsi, [rsi+32]
	sbb	r10, [rdx+16]
	sbb	r11, [rdx+24]
	lea	rdx, [rdx+32]
Llo0:
	mov	[rdi+16], r8
Llo3:
	mov	[rdi+24], r9
	lea	rdi, [rdi+32]
	dec	rcx
	jnz	Ltop
Lend:
	mov	eax, ecx
	mov	[rdi], r10
	mov	[rdi+8], r11
	setc	al
	pop	rsi
	pop	rdi
	ret
align 16
global	__gmpn_sub_nc
__gmpn_sub_nc:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	mov	r8, [rsp+56]
	jmp	qword Lent
