section .text
align 16
global	___gmpn_sec_tabselect
___gmpn_sec_tabselect:
	push	edi
	push	esi
	push	ebx
	push	ebp
	mov	edi, [esp+20]
	mov	esi, [esp+24]
	mov	ebx, [esp+28]
	mov	ecx, [esp+32]
	lea	edi, [edi+ebx*4]
	lea	esi, [esi+ebx*4]
	sub	[esp+36], ecx
Louter:
	mov	eax, [esp+36]
	add	eax, ecx
	neg	eax
	sbb	eax, eax
	mov	[esp+32], eax
	not	eax
	mov	[esp+20], eax
	mov	ebp, ebx
	neg	ebp
align 16
Ltop:
	mov	eax, [esi+ebp*4]
	and	eax, [esp+20]
	mov	edx, [edi+ebp*4]
	and	edx, [esp+32]
	or	eax, edx
	mov	[edi+ebp*4], eax
	inc	ebp
	js	Ltop
Lend:
	mov	eax, ebx
	lea	esi, [esi+eax*4]
	dec	ecx
	jne	Louter
Louter_end:
	pop	ebp
	pop	ebx
	pop	esi
	pop	edi
	ret
