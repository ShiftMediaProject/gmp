extern ___gmp_binvert_limb_table
section .text
align 16
global	___gmpn_pi1_bdiv_q_1
___gmpn_pi1_bdiv_q_1:
	mov	ecx, dword [esp+24]
	push ebp
	mov	eax, dword [esp+24]
	mov	ebp, dword [esp+16]
	push ebx
Lcommon:
	push edi
	push esi
	mov	esi, dword [esp+24]
	mov	edi, dword [esp+20]
	lea	esi, [esi+ebp*4]
	lea	edi, [edi+ebp*4]
	neg ebp
	mov	dword [esp+24], eax
	mov	eax, dword [esi+ebp*4]
	xor	ebx, ebx
	xor	edx, edx
	inc ebp
	jz	Lone
	mov	edx, dword [esi+ebp*4]
	shrd	eax, edx, cl
	mov	edx, dword [esp+24]
	jmp	dword Lentry
align 8
	nop
	nop
Ltop:
	mov	eax, dword [esi+ebp*4-4]
	sub	edx, ebx
	mov	ebx, dword [esi+ebp*4]
	shrd	eax, ebx, cl
	sub	eax, edx
	mov	edx, dword [esp+24]
	sbb	ebx, ebx
Lentry:
	imul	eax, edx
	mov	dword [edi+ebp*4-4], eax
	mov	edx, dword [esp+32]
	mul edx
	inc ebp
	jnz	Ltop
	mov	eax, dword [esi-4]
Lone:
	shr	eax, cl
	pop esi
	add	eax, ebx
	sub	eax, edx
	imul	eax, dword [esp+20]
	mov	dword [edi-4], eax
	pop edi
	pop ebx
	pop ebp
	ret
align 16
global	___gmpn_bdiv_q_1
___gmpn_bdiv_q_1:
	mov	eax, dword [esp+16]
	push ebp
	mov	ecx, -1
	mov	ebp, dword [esp+16]
	push ebx
Lstrip_twos:
	inc ecx
	shr eax, 1
	jnc	Lstrip_twos
	lea	ebx, [eax+eax+1]
	and	eax, 127
	movzx	dword eax, byte [eax+___gmp_binvert_limb_table]

	lea	edx, [eax+eax]
	mov	dword [esp+24], ebx
	imul	eax, eax
	imul	eax, ebx
	sub	edx, eax
	lea	eax, [edx+edx]
	imul	edx, edx
	imul	edx, ebx
	sub	eax, edx
	jmp	dword Lcommon
