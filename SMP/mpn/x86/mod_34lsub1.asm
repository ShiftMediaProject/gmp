section .text
align 16
global	___gmpn_mod_34lsub1
___gmpn_mod_34lsub1:
	mov	ecx, dword [esp+8]
	mov	edx, dword [esp+4]
	sub	ecx, 2
	ja	Lthree_or_more
	mov	eax, dword [edx]
	jb	Lone
	mov	ecx, dword [edx+4]
	mov	edx, eax
	shr	eax, 24
	and	edx, 0xFFFFFF
	add	eax, edx
	mov	edx, ecx
	and	ecx, 0xFFFF
	shr	edx, 16
	add	eax, edx
	shl	ecx, 8
	add	eax, ecx
Lone:
	ret
Lthree_or_more:
	mov	dword [esp+4], ebx
	xor	ebx, ebx
	push esi
	xor	esi, esi
	push edi
	xor	eax, eax
Ltop:
	lea	edx, [edx+12]
	lea	ecx, [ecx-2]
	adc	eax, dword [edx-12]
	adc	ebx, dword [edx-8]
	adc	esi, dword [edx-4]
	dec ecx
	jg	Ltop
	mov	edi, 0xFFFFFFFF
	inc ecx
	js	Lcombine
	adc	eax, dword [edx]
	mov	edi, 0xFFFFFF00
	dec ecx
	js	Lcombine
	adc	ebx, dword [edx+4]
	mov	edi, 0xFFFF0000
Lcombine:
	sbb	ecx, ecx
	mov	edx, eax
	shr	eax, 24
	and	ecx, edi
	sub	eax, ecx
	mov	edi, ebx
	shr	ebx, 16
	and	edx, 0x00FFFFFF
	add	eax, edx
	and	edi, 0xFFFF
	shl	edi, 8
	add	eax, ebx
	add	eax, edi
	mov	edx, esi
	shr	esi, 8
	and	edx, 0xFF
	shl	edx, 16
	add	eax, esi
	add	eax, edx
	pop edi
	mov	ebx, dword [esp+8]
	pop esi
	ret
