section .text
align 8
global	___gmpn_umul_ppmm
___gmpn_umul_ppmm:
	mov	ecx, dword [esp+4]
	mov	eax, dword [esp+8]
	mul	dword [esp+12]
	mov	dword [ecx], eax
	mov	eax, edx
	ret
