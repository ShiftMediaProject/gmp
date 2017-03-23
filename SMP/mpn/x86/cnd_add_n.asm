section .text
align 16
global	___gmpn_cnd_add_n
___gmpn_cnd_add_n:
	add	esp, -16
	mov	[esp], ebp
	mov	[esp+4], ebx
	mov	[esp+8], esi
	mov	[esp+12], edi
	mov	eax, [esp+20]
	neg	eax
	sbb	eax, eax
	mov	[esp+20], eax
	mov	edi, [esp+24]
	mov	esi, [esp+28]
	mov	ebp, [esp+32]
	mov	ecx, [esp+36]
	mov	eax, [ebp]
	mov	ebx, [esi]
	lea	ebp, [ebp+ecx*4]
	lea	esi, [esi+ecx*4]
	lea	edi, [edi+ecx*4-4]
	neg	ecx
	and	eax, [esp+20]
	add	ebx, eax
	sbb	edx, edx
	inc	ecx
	je	Lend
align 16
Ltop:
	mov	eax, [ebp+ecx*4]
	and	eax, [esp+20]
	mov	[edi+ecx*4], ebx
	mov	ebx, [esi+ecx*4]
	add	edx, edx
	adc	ebx, eax
	sbb	edx, edx
	inc	ecx
	jne	Ltop
Lend:
	mov	[edi], ebx
	xor	eax, eax
	sub	eax, edx
	mov	ebp, [esp]
	mov	ebx, [esp+4]
	mov	esi, [esp+8]
	mov	edi, [esp+12]
	add	esp, 16
	ret
