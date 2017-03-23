section .text
align 8
global	___gmpn_lshift
___gmpn_lshift:
	push edi
	push esi
	push ebx
	mov	edi, dword [esp+16]
	mov	esi, dword [esp+20]
	mov	edx, dword [esp+24]
	mov	ecx, dword [esp+28]
	sub	esi, 4
	mov	ebx, dword [esi+edx*4]
	xor	eax, eax
	shld	eax, ebx, cl
	dec edx
	jz	Lend
	push eax
	test	dl, 1
	jnz	L1
	mov	eax, ebx
align 8
Loop:
	mov	ebx, dword [esi+edx*4]
	shld	eax, ebx, cl
	mov	dword [edi+edx*4], eax
	dec edx
L1:
	mov	eax, dword [esi+edx*4]
	shld	ebx, eax, cl
	mov	dword [edi+edx*4], ebx
	dec edx
	jnz	Loop
	shl	eax, cl
	mov	dword [edi], eax
	pop eax
	pop ebx
	pop esi
	pop edi
	ret
Lend:
	shl	ebx, cl
	mov	dword [edi], ebx
	pop ebx
	pop esi
	pop edi
	ret
