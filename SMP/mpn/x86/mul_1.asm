section .text
align 8
global	___gmpn_mul_1
___gmpn_mul_1:
	push edi
	push esi
	push ebx
	push ebp
	mov	edi, dword [esp+20]
	mov	esi, dword [esp+24]
	mov	ecx, dword [esp+28]
	xor	ebx, ebx
	and	ecx, 3
	jz	Lend0
Loop0:
	mov	eax, dword [esi]
	mul	dword [esp+32]
	lea	esi, [esi+4]
	add	eax, ebx
	mov	ebx, 0
	adc	edx, ebx
	mov	dword [edi], eax
	mov	ebx, edx
	lea	edi, [edi+4]
	dec ecx
	jnz	Loop0
Lend0:
	mov	ecx, dword [esp+28]
	shr	ecx, 2
	jz	Lend
align 8
Loop:
	mov	eax, dword [esi]
	mul	dword [esp+32]
	add	ebx, eax
	mov	ebp, 0
	adc	ebp, edx
	mov	eax, dword [esi+4]
	mul	dword [esp+32]
	mov	dword [edi], ebx
	add	ebp, eax
	mov	ebx, 0
	adc	ebx, edx
	mov	eax, dword [esi+8]
	mul	dword [esp+32]
	mov	dword [edi+4], ebp
	add	ebx, eax
	mov	ebp, 0
	adc	ebp, edx
	mov	eax, dword [esi+12]
	mul	dword [esp+32]
	mov	dword [edi+8], ebx
	add	ebp, eax
	mov	ebx, 0
	adc	ebx, edx
	mov	dword [edi+12], ebp
	lea	esi, [esi+16]
	lea	edi, [edi+16]
	dec ecx
	jnz	Loop
Lend:
	mov	eax, ebx
	pop ebp
	pop ebx
	pop esi
	pop edi
	ret
