section .text
align 32
global	__gmpn_addmul_1
__gmpn_addmul_1:
	push	rsi
	push	rdi
	mov	rsi, rdx
	mov	rax, [rsi]
	push	rbx
	lea	rsi, [rsi+r8*8]
	lea	rcx, [rcx+r8*8]
	test	r8b, 1
	jnz	Lb13
Lb02:
	xor	r11d, r11d
	test	r8b, 2
	jnz	Lb2
Lb0:
	mov	ebx, 1
	sub	rbx, r8
	mul	r9
	mov	rdi, rdx
	mov	r8, [rcx+rbx*8-8]
	jmp	qword Le0
align 16
Lb2:
	mov	rbx, -1
	sub	rbx, r8
	mul	r9
	mov	r8, [rcx+rbx*8+8]
	mov	rdi, rdx
	jmp	qword Le2
align 16
Lb13:
	xor	edi, edi
	test	r8b, 2
	jnz	Lb3
Lb1:
	mov	ebx, 2
	sub	rbx, r8
	jns	L1
	mul	r9
	mov	r10, [rcx+rbx*8-16]
	mov	r11, rdx
	jmp	qword Le1
align 16
Lb3:
	xor	ebx, ebx
	sub	rbx, r8
	mul	r9
	mov	r10, [rcx+rbx*8]
	jmp	qword Le3
align 32
Ltop:
	mul	r9
	mov	r10, [rcx+rbx*8-16]
	add	r8, r11
	mov	r11, rdx
	adc	rdi, 0
	mov	[rcx+rbx*8-24], r8
Le1:
	add	r10, rax
	mov	rax, [rsi+rbx*8-8]
	adc	r11, 0
	mul	r9
	add	r10, rdi
	mov	rdi, rdx
	mov	r8, [rcx+rbx*8-8]
	adc	r11, 0
	mov	[rcx+rbx*8-16], r10
Le0:
	add	r8, rax
	adc	rdi, 0
	mov	rax, [rsi+rbx*8]
	mul	r9
	mov	r10, [rcx+rbx*8]
	add	r8, r11
	mov	[rcx+rbx*8-8], r8
	adc	rdi, 0
Le3:
	mov	r11, rdx
	add	r10, rax
	mov	rax, [rsi+rbx*8+8]
	adc	r11, 0
	mul	r9
	mov	r8, [rcx+rbx*8+8]
	add	r10, rdi
	mov	rdi, rdx
	mov	[rcx+rbx*8], r10
	adc	r11, 0
Le2:
	add	r8, rax
	adc	rdi, 0
	mov	rax, [rsi+rbx*8+16]
	add	rbx, 4
	jnc	Ltop
Lend:
	mul	r9
	mov	r10, [rcx-8]
	add	r8, r11
	mov	r11, rdx
	adc	rdi, 0
	mov	[rcx-16], r8
	add	r10, rax
	adc	r11, 0
	add	r10, rdi
	adc	r11, 0
	mov	[rcx-8], r10
	mov	rax, r11
	pop	rbx
	pop	rdi
	pop	rsi
	ret
align 16
L1:
	mul	r9
	add	[rcx-8], rax
	mov	rax, rdx
	adc	rax, 0
	pop	rbx
	pop	rdi
	pop	rsi
	ret
