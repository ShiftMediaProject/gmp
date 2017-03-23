section .text
align 8
global	___gmpn_sub_nc
___gmpn_sub_nc:
	push edi
	push esi
	mov	edi, dword [esp+12]
	mov	esi, dword [esp+16]
	mov	edx, dword [esp+20]
	mov	ecx, dword [esp+24]
	mov	eax, ecx
	shr	ecx, 3
	neg eax
	and	eax, 7
	jz	Loopgo
	inc ecx
	shl	eax, 2
	sub	edi, eax
	sub	esi, eax
	sub	edx, eax
	shr	eax, 2
	lea	eax, [eax+eax*8+Loop-3]
	push ebp
	mov	ebp, dword [esp+32]
	shr ebp, 1
	pop ebp
	jmp	dword eax
align 16
global	___gmpn_sub_n
___gmpn_sub_n:
	push edi
	push esi
	mov	edi, dword [esp+12]
	mov	esi, dword [esp+16]
	mov	edx, dword [esp+20]
	mov	ecx, dword [esp+24]
	mov	eax, ecx
	shr	ecx, 3
	neg eax
	and	eax, 7
	jz	Loop
	inc ecx
	shl	eax, 2
	sub	edi, eax
	sub	esi, eax
	sub	edx, eax
	shr	eax, 2
	lea	eax, [eax+eax*8+Loop-3]
	jmp	dword eax
Loopgo:
	push ebp
	mov	ebp, dword [esp+32]
	shr ebp, 1
	pop ebp
align 16
Loop:
	mov	eax, dword [esi]
	sbb	eax, dword [edx]
	mov	dword [edi], eax
	mov	eax, dword [esi+4]
	sbb	eax, dword [edx+4]
	mov	dword [edi+4], eax
	mov	eax, dword [esi+8]
	sbb	eax, dword [edx+8]
	mov	dword [edi+8], eax
	mov	eax, dword [esi+12]
	sbb	eax, dword [edx+12]
	mov	dword [edi+12], eax
	mov	eax, dword [esi+16]
	sbb	eax, dword [edx+16]
	mov	dword [edi+16], eax
	mov	eax, dword [esi+20]
	sbb	eax, dword [edx+20]
	mov	dword [edi+20], eax
	mov	eax, dword [esi+24]
	sbb	eax, dword [edx+24]
	mov	dword [edi+24], eax
	mov	eax, dword [esi+28]
	sbb	eax, dword [edx+28]
	mov	dword [edi+28], eax
	lea	edi, [edi+32]
	lea	esi, [esi+32]
	lea	edx, [edx+32]
	dec ecx
	jnz	Loop
	sbb	eax, eax
	neg eax
	pop esi
	pop edi
	ret
