section .text
align 8
global	___gmpn_udiv_qrnnd
___gmpn_udiv_qrnnd:
	mov	eax, dword [esp+12]
	mov	edx, dword [esp+8]
	div	dword [esp+16]
	mov	ecx, dword [esp+4]
	mov	dword [ecx], edx
	ret
