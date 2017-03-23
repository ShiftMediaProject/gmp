section .text
align 16
global	___gmpn_divrem_1c
___gmpn_divrem_1c:
	mov	ecx, dword [esp+16]
	push edi
	mov	edi, dword [esp+16]
	push esi
	mov	esi, dword [esp+28]
	push ebx
	mov	ebx, dword [esp+16]
	push ebp
	mov	ebp, dword [esp+24]
	or	ecx, ecx
	mov	edx, dword [esp+40]
	jz	Lfraction
	lea	ebx, [ebx+ebp*4-4]
	jmp	dword Linteger_top
global	___gmpn_divrem_1
___gmpn_divrem_1:
	mov	ecx, dword [esp+16]
	push edi
	mov	edi, dword [esp+16]
	push esi
	mov	esi, dword [esp+28]
	or	ecx, ecx
	jz	Lsize_zero
	push ebx
	mov	eax, dword [edi+ecx*4-4]
	xor	edx, edx
	mov	ebx, dword [esp+16]
	push ebp
	mov	ebp, dword [esp+24]
	cmp	eax, esi
	lea	ebx, [ebx+ebp*4-4]
	jae	Linteger_entry
	mov	dword [ebx+ecx*4], edx
	dec ecx
	mov	edx, eax
	jz	Lfraction
Linteger_top:
	mov	eax, dword [edi+ecx*4-4]
Linteger_entry:
	div esi
	mov	dword [ebx+ecx*4], eax
	dec ecx
	jnz	Linteger_top
Lfraction:
	or	ecx, ebp
	jz	Ldone
	mov	ebx, dword [esp+20]
Lfraction_top:
	xor	eax, eax
	div esi
	mov	dword [ebx+ecx*4-4], eax
	dec ecx
	jnz	Lfraction_top
Ldone:
	pop ebp
	mov	eax, edx
	pop ebx
	pop esi
	pop edi
	ret
Lsize_zero:
	mov	ecx, dword [esp+16]
	xor	eax, eax
	mov	edi, dword [esp+12]
	cld
	rep	stosd
	pop esi
	pop edi
	ret
