section .text
align 32
global	__gmpn_iorn_n
__gmpn_iorn_n:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	mov	r8, [rdx]
	not r8
	mov	eax, ecx
	lea	rdx, [rdx+rcx*8]
	lea	rsi, [rsi+rcx*8]
	lea	rdi, [rdi+rcx*8]
	neg rcx
	and	eax, 3
	je	Lb00
	cmp	eax, 2
	jc	Lb01
	je	Lb10
Lb11:
	or	r8, qword [rsi+rcx*8]
	mov	qword [rdi+rcx*8], r8
	dec rcx
	jmp	qword Le11
Lb10:
	add	rcx, -2
	jmp	qword Le10
 db 0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90
Lb01:
	or	r8, qword [rsi+rcx*8]
	mov	qword [rdi+rcx*8], r8
	inc rcx
	jz	Lret
Loop:
	mov	r8, [rdx+rcx*8]
	not r8
Lb00:
	mov	r9, [rdx+rcx*8+8]
	not r9
	or	r8, qword [rsi+rcx*8]
	or	r9, qword [rsi+rcx*8+8]
	mov	qword [rdi+rcx*8], r8
	mov	qword [rdi+rcx*8+8], r9
Le11:
	mov	r8, [rdx+rcx*8+16]
	not r8
Le10:
	mov	r9, [rdx+rcx*8+24]
	not r9
	or	r8, qword [rsi+rcx*8+16]
	or	r9, qword [rsi+rcx*8+24]
	mov	qword [rdi+rcx*8+16], r8
	mov	qword [rdi+rcx*8+24], r9
	add	rcx, 4
	jnc	Loop
Lret:
	pop	rsi
	pop	rdi
	ret
