section .text
align 32
global	___gmpn_copyd
___gmpn_copyd:
	mov	ecx, dword [esp+12]
	mov	eax, esi
	mov	esi, dword [esp+8]
	mov	edx, edi
	mov	edi, dword [esp+4]
	lea	esi, [esi+ecx*4-4]
	lea	edi, [edi+ecx*4-4]
	std
	rep	movsd
	cld
	mov	esi, eax
	mov	edi, edx
	ret
