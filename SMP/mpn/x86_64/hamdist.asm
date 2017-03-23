section .text
align 32
global	__gmpn_hamdist
__gmpn_hamdist:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	r8, [rdi]
	xor	r8, [rsi]
	lea	rdi, [rdi+rdx*8]
	lea	rsi, [rsi+rdx*8]
	neg	rdx
	bt	edx, 0
	jnc	L2
L1:
 db 0xf3,0x49,0x0f,0xb8,0xc0
	xor	r10d, r10d
	add	rdx, 1
	js	Ltop
	pop	rsi
	pop	rdi
	ret
align 16
L2:
	mov	r9, [rdi+rdx*8+8]
 db 0xf3,0x49,0x0f,0xb8,0xc0
	xor	r9, [rsi+rdx*8+8]
 db 0xf3,0x4d,0x0f,0xb8,0xd1
	add	rdx, 2
	js	Ltop
	lea	rax, [r10+rax]
	pop	rsi
	pop	rdi
	ret
align 16
Ltop:
	mov	r8, [rdi+rdx*8]
	lea	rax, [r10+rax]
	mov	r9, [rdi+rdx*8+8]
	xor	r8, [rsi+rdx*8]
	xor	r9, [rsi+rdx*8+8]
 db 0xf3,0x49,0x0f,0xb8,0xc8
	lea	rax, [rcx+rax]
 db 0xf3,0x4d,0x0f,0xb8,0xd1
	add	rdx, 2
	js	Ltop
	lea	rax, [r10+rax]
	pop	rsi
	pop	rdi
	ret
