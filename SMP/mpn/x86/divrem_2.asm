section .text
align 16
global	___gmpn_divrem_2
___gmpn_divrem_2:
	push	ebp
	push	edi
	push	esi
	push	ebx
	sub	esp, 36
	mov	ecx, [esp+68]
	mov	esi, [esp+72]
	mov	dword [esp+32], 0
	lea	edi, [ecx*4+0]
	add	edi, [esp+64]
	mov	ebx, [esi]
	mov	eax, [esi+4]
	mov	[esp+20], ebx
	sub	edi, 12
	mov	[esp+24], eax
	mov	[esp+12], edi
	mov	ebx, [edi+8]
	mov	ebp, [edi+4]
	cmp	ebx, eax
	jb	L8
	seta	dl
	cmp	ebp, [esp+20]
	setae	al
	or	al, dl
	jne	L35
L8:
	mov	esi, [esp+60]
	lea	edi, [esi+ecx-3]
	test	edi, edi
	js	L9
	mov	edx, [esp+24]
	mov	esi, -1
	mov	eax, esi
	mov	ecx, esi
	not	edx
	div	dword [esp+24]
	mov	esi, eax
	imul	eax, [esp+24]
	mov	[esp], eax
	mov	eax, esi
	mul	dword [esp+20]
	mov	eax, [esp]
	add	eax, [esp+20]
	adc	ecx, 0
	add	edx, eax
	adc	ecx, 0
	mov	eax, ecx
	js	L32
L36:
	dec	esi
	sub	edx, [esp+24]
	sbb	eax, 0
	jns	L36
L32:
	mov	[esp+16], esi
	mov	ecx, edi
	mov	esi, [esp+12]
	mov	eax, [esp+24]
	neg	eax
	mov	[esp+4], eax
align 16
	nop
Lloop:
	mov	eax, [esp+16]
	mul	ebx
	add	eax, ebp
	mov	[esp], eax
	adc	edx, ebx
	mov	edi, edx
	imul	edx, [esp+4]
	mov	eax, [esp+20]
	lea	ebx, [edx+ebp]
	mul	edi
	xor	ebp, ebp
	cmp	ecx, [esp+60]
	jl	L19
	mov	ebp, [esi]
	sub	esi, 4
L19:
	sub	ebp, [esp+20]
	sbb	ebx, [esp+24]
	sub	ebp, eax
	sbb	ebx, edx
	mov	eax, [esp+20]
	inc	edi
	xor	edx, edx
	cmp	ebx, [esp]
	adc	edx, -1
	add	edi, edx
	and	eax, edx
	and	edx, [esp+24]
	add	ebp, eax
	adc	ebx, edx
	cmp	ebx, [esp+24]
	jae	Lfix
Lbck:
	mov	edx, [esp+56]
	mov	[edx+ecx*4], edi
	dec	ecx
	jns	Lloop
L9:
	mov	esi, [esp+64]
	mov	[esi], ebp
	mov	[esi+4], ebx
	mov	eax, [esp+32]
	add	esp, 36
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
Lfix:
	seta	dl
	cmp	ebp, [esp+20]
	setae	al
	or	al, dl
	je	Lbck
	inc	edi
	sub	ebp, [esp+20]
	sbb	ebx, [esp+24]
	jmp	dword Lbck
L35:
	sub	ebp, [esp+20]
	sbb	ebx, [esp+24]
	mov	dword [esp+32], 1
	jmp	dword L8
