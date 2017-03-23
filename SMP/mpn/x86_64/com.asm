section .text
align 32
global	__gmpn_com
__gmpn_com:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	r8, [rsi]
	mov	eax, edx
	lea	rsi, [rsi+rdx*8]
	lea	rdi, [rdi+rdx*8]
	neg rdx
	and	eax, 3
	je	Lb00
	cmp	eax, 2
	jc	Lb01
	je	Lb10
Lb11:
	not r8
	mov	qword [rdi+rdx*8], r8
	dec rdx
	jmp	qword Le11
Lb10:
	add	rdx, -2
	jmp	qword Le10
 db 0x90,0x90,0x90,0x90,0x90,0x90
Lb01:
	not r8
	mov	qword [rdi+rdx*8], r8
	inc rdx
	jz	Lret
Loop:
	mov	r8, [rsi+rdx*8]
Lb00:
	mov	r9, [rsi+rdx*8+8]
	not r8
	not r9
	mov	qword [rdi+rdx*8], r8
	mov	qword [rdi+rdx*8+8], r9
Le11:
	mov	r8, [rsi+rdx*8+16]
Le10:
	mov	r9, [rsi+rdx*8+24]
	not r8
	not r9
	mov	qword [rdi+rdx*8+16], r8
	mov	qword [rdi+rdx*8+24], r9
	add	rdx, 4
	jnc	Loop
Lret:
	pop	rsi
	pop	rdi
	ret
