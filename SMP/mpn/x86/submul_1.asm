section .text
align 8
global	___gmpn_submul_1
___gmpn_submul_1:
	push edi
	push esi
	push ebx
	push ebp
	mov	edi, dword [esp+16+4]
	mov	esi, dword [esp+16+8]
	mov	ecx, dword [esp+16+12]
	xor	ebx, ebx
	and	ecx, 3
	jz	Lend0
Loop0:
	mov	eax, dword [esi]
	mul	dword [esp+16+16]
	lea	esi, [esi+4]
	add	eax, ebx
	mov	ebx, 0
	adc	edx, ebx
	sub	dword [edi], eax
	adc	ebx, edx
	lea	edi, [edi+4]
	dec ecx
	jnz	Loop0
Lend0:
	mov	ecx, dword [esp+16+12]
	shr	ecx, 2
	jz	Lend
align 8
Loop:
	mov	eax, dword [esi]
	mul	dword [esp+16+16]
	add	ebx, eax
	mov	ebp, 0
	adc	ebp, edx
	mov	eax, dword [esi+4]
	mul	dword [esp+16+16]
	sub	dword [edi], ebx
	adc	ebp, eax
	mov	ebx, 0
	adc	ebx, edx
	mov	eax, dword [esi+8]
	mul	dword [esp+16+16]
	sub	dword [edi+4], ebp
	adc	ebx, eax
	mov	ebp, 0
	adc	ebp, edx
	mov	eax, dword [esi+12]
	mul	dword [esp+16+16]
	sub	dword [edi+8], ebx
	adc	ebp, eax
	mov	ebx, 0
	adc	ebx, edx
	sub	dword [edi+12], ebp
	adc	ebx, 0
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
