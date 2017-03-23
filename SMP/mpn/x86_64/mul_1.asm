section .text
align 16
global	__gmpn_mul_1
__gmpn_mul_1:
	push	rsi
	push	rdi
	mov	rsi, rdx
	mov	rax, [rsi]
	mov	r10d, r8d
	lea	rsi, [rsi+r8*8]
	lea	rcx, [rcx+r8*8-8]
	neg	r8
	mul	r9
	and	r10d, 3
	jz	Lb0
	cmp	r10d, 2
	jb	Lb1
	jz	Lb2
Lb3:
	add	r8, -1
	mov	rdi, rax
	mov	r11, rdx
	mov	rax, [rsi+r8*8+16]
	jmp	qword LL3
Lb1:
	mov	rdi, rax
	mov	r11, rdx
	add	r8, 1
	jnc	LL1
	mov	[rcx], rax
	mov	rax, rdx
	pop	rdi
	pop	rsi
	ret
Lb2:
	add	r8, -2
	mov	r11, rax
	mov	rdi, rdx
	mov	rax, [rsi+r8*8+24]
	jmp	qword LL2
Lb0:
	mov	r11, rax
	mov	rdi, rdx
	mov	rax, [rsi+r8*8+8]
	jmp	qword LL0
align 8
Ltop:
	mov	r11, rdx
	add	rdi, rax
LL1:
	mov	rax, [rsi+r8*8+0]
	adc	r11, 0
	mul	r9
	add	r11, rax
	mov	[rcx+r8*8+0], rdi
	mov	rax, [rsi+r8*8+8]
	mov	rdi, rdx
	adc	rdi, 0
LL0:
	mul	r9
	mov	[rcx+r8*8+8], r11
	add	rdi, rax
	mov	r11, rdx
	mov	rax, [rsi+r8*8+16]
	adc	r11, 0
LL3:
	mul	r9
	mov	[rcx+r8*8+16], rdi
	mov	rdi, rdx
	add	r11, rax
	mov	rax, [rsi+r8*8+24]
	adc	rdi, 0
LL2:
	mul	r9
	mov	[rcx+r8*8+24], r11
	add	r8, 4
	jnc	Ltop
Lend:
	add	rdi, rax
	mov	rax, rdx
	adc	rax, 0
	mov	[rcx], rdi
	pop	rdi
	pop	rsi
	ret
