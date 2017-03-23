section .text
align 32
global	___gmpn_copyi
___gmpn_copyi:
	mov	ecx, dword [esp+12]
	mov	eax, esi
	mov	esi, dword [esp+8]
	mov	edx, edi
	mov	edi, dword [esp+4]
	cld
	rep	movsd
	mov	esi, eax
	mov	edi, edx
	ret
