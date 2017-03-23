section .text
align 8
global	___gmpn_sqr_basecase
___gmpn_sqr_basecase:
	mov	edx, dword [esp+12]
	mov	eax, dword [esp+8]
	cmp	edx, 2
	mov	ecx, dword [esp+4]
	je	Ltwo_limbs
	ja	Lthree_or_more
	mov	eax, dword [eax]
	mul eax
	mov	dword [ecx], eax
	mov	dword [ecx+4], edx
	ret
align 8
Ltwo_limbs:
	push ebx
	push ebp
	mov	ebx, eax
	mov	eax, dword [eax]
	mul eax
	push esi
	push edi
	mov	esi, edx
	mov	dword [ecx], eax
	mov	eax, dword [ebx+4]
	mul eax
	mov	edi, eax
	mov	ebp, edx
	mov	eax, dword [ebx]
	mul	dword [ebx+4]
	add	esi, eax
	adc	edi, edx
	adc	ebp, 0
	add	eax, esi
	adc	edx, edi
	mov	dword [ecx+4], eax
	adc	ebp, 0
	mov	dword [ecx+8], edx
	mov	dword [ecx+12], ebp
	pop edi
	pop esi
	pop ebp
	pop ebx
	ret
align 8
Lthree_or_more:
	push ebx
	push edi
	push esi
	push ebp
	lea	edi, [ecx+edx*4]
	lea	esi, [eax+edx*4]
	mov	ebp, dword [eax]
	mov	ecx, edx
	neg ecx
	xor	ebx, ebx
	inc ecx
Lmul1:
	mov	eax, dword [esi+ecx*4]
	mul ebp
	add	ebx, eax
	adc	edx, 0
	mov	dword [edi+ecx*4], ebx
	mov	ebx, edx
	inc ecx
	jnz	Lmul1
	mov	dword [edi], ebx
	mov	ecx, dword [esp+28]
	sub	ecx, 3
	jz	Lcorner
	neg ecx
Louter:
	mov	dword [esp+20], ecx
	add	edi, 4
	mov	ebp, dword [esi+ecx*4-8]
	sub	ecx, 1
	xor	ebx, ebx
Linner:
	mov	eax, dword [esi+ecx*4]
	mul ebp
	add	eax, ebx
	adc	edx, 0
	add	dword [edi+ecx*4], eax
	adc	edx, 0
	mov	ebx, edx
	add	ecx, 1
	jl	Linner
	mov	dword [edi], ebx
	mov	ecx, dword [esp+20]
	inc ecx
	jnz	Louter
Lcorner:
	mov	eax, dword [esi-4]
	mul	dword [esi-8]
	add	dword [edi+0], eax
	adc	edx, 0
	mov	dword [edi+4], edx
	mov	eax, dword [esp+28]
	neg eax
	add	eax, 1
Llshift:
	rcl dword [edi+eax*8+8], 1
	rcl dword [edi+eax*8+12], 1
	inc eax
	jnz	Llshift
	adc	eax, eax
	mov	dword [edi+8], eax
	mov	esi, dword [esp+24]
	mov	eax, dword [esi]
	mul eax
	mov	ecx, dword [esp+28]
	lea	esi, [esi+ecx*4]
	neg ecx
	mov	ebx, edx
	mov	dword [edi+ecx*8+12], eax
	inc ecx
Ldiag:
	mov	eax, dword [esi+ecx*4]
	mul eax
	add	dword [edi+ecx*8+8], ebx
	mov	ebx, edx
	adc	dword [edi+ecx*8+12], eax
	adc	ebx, 0
	inc ecx
	jnz	Ldiag
	add	dword [edi+8], ebx
	pop ebp
	pop esi
	pop edi
	pop ebx
	ret
