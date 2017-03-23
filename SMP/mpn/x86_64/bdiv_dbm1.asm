section .text
align 16
global	__gmpn_bdiv_dbm1c
__gmpn_bdiv_dbm1c:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	mov	r8, [rsp+56]
	mov	rax, [rsi]
	mov	r9, rdx
	mov	r11d, edx
	mul	rcx
	lea	rsi, [rsi+r9*8]
	lea	rdi, [rdi+r9*8]
	neg	r9
	and	r11d, 3
	jz	Llo0
	lea	r9, [r9+r11-4]
	cmp	r11d, 2
	jc	Llo1
	jz	Llo2
	jmp	qword Llo3
align 16
Ltop:
	mov	rax, [rsi+r9*8]
	mul	rcx
Llo0:
	sub	r8, rax
	mov	[rdi+r9*8], r8
	sbb	r8, rdx
	mov	rax, [rsi+r9*8+8]
	mul	rcx
Llo3:
	sub	r8, rax
	mov	[rdi+r9*8+8], r8
	sbb	r8, rdx
	mov	rax, [rsi+r9*8+16]
	mul	rcx
Llo2:
	sub	r8, rax
	mov	[rdi+r9*8+16], r8
	sbb	r8, rdx
	mov	rax, [rsi+r9*8+24]
	mul	rcx
Llo1:
	sub	r8, rax
	mov	[rdi+r9*8+24], r8
	sbb	r8, rdx
	add	r9, 4
	jnz	Ltop
	mov	rax, r8
	pop	rsi
	pop	rdi
	ret
