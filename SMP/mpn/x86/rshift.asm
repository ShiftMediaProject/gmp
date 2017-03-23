section .text
align 8
global	___gmpn_rshift
___gmpn_rshift:
	push edi
	push esi
	push ebx
	mov	edi, dword [esp+16]
	mov	esi, dword [esp+20]
	mov	edx, dword [esp+24]
	mov	ecx, dword [esp+28]
	lea	edi, [edi+edx*4-4]
	lea	esi, [esi+edx*4]
	neg edx
	mov	ebx, dword [esi+edx*4]
	xor	eax, eax
	shrd	eax, ebx, cl
	inc edx
	jz	Lend
	push eax
	test	dl, 1
	jnz	L1
	mov	eax, ebx
align 8
Loop:
	mov	ebx, dword [esi+edx*4]
	shrd	eax, ebx, cl
	mov	dword [edi+edx*4], eax
	inc edx
L1:
	mov	eax, dword [esi+edx*4]
	shrd	ebx, eax, cl
	mov	dword [edi+edx*4], ebx
	inc edx
	jnz	Loop
	shr	eax, cl
	mov	dword [edi], eax
	pop eax
	pop ebx
	pop esi
	pop edi
	ret
Lend:
	shr	ebx, cl
	mov	dword [edi], ebx
	pop ebx
	pop esi
	pop edi
	ret
