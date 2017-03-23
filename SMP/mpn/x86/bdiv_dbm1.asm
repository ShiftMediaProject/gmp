section .text
align 16
global	___gmpn_bdiv_dbm1c
___gmpn_bdiv_dbm1c:
	mov	ecx, [esp+16]
	push	esi
	mov	esi, [esp+12]
	push	edi
	mov	edi, [esp+12]
	push	ebp
	mov	ebp, [esp+24]
	push	ebx
	mov	eax, [esi]
	mul	ecx
	mov	ebx, [esp+36]
	sub	ebx, eax
	mov	[edi], ebx
	sbb	ebx, edx
	mov	eax, ebp
	and	eax, 3
	jz	Lb0
	cmp	eax, 2
	jc	Lb1
	jz	Lb2
Lb3:
	lea	esi, [esi-8]
	lea	edi, [edi+8]
	add	ebp, -3
	jmp	dword L3
Lb0:
	mov	eax, [esi+4]
	lea	esi, [esi-4]
	lea	edi, [edi+12]
	add	ebp, -4
	jmp	dword L0
Lb2:
	mov	eax, [esi+4]
	lea	esi, [esi+4]
	lea	edi, [edi+4]
	add	ebp, -2
	jmp	dword L2
align 8
Ltop:
	mov	eax, [esi+4]
	mul	ecx
	lea	edi, [edi+16]
	sub	ebx, eax
	mov	eax, [esi+8]
	mov	[edi-12], ebx
	sbb	ebx, edx
L0:
	mul	ecx
	sub	ebx, eax
	mov	[edi-8], ebx
	sbb	ebx, edx
L3:
	mov	eax, [esi+12]
	mul	ecx
	sub	ebx, eax
	mov	[edi-4], ebx
	mov	eax, [esi+16]
	lea	esi, [esi+16]
	sbb	ebx, edx
L2:
	mul	ecx
	sub	ebx, eax
	mov	[edi+0], ebx
	sbb	ebx, edx
Lb1:
	add	ebp, -4
	jns	Ltop
	mov	eax, ebx
	pop	ebx
	pop	ebp
	pop	edi
	pop	esi
	ret
