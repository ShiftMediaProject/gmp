section .text
align 16
global	mpn_lshsub_n
mpn_lshsub_n:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rcx, r9
	mov	r8d, [rsp+56]
	push	r12
	push	r13
	push	r14
	push	r15
	push	rbx
	mov	rax, rcx
	xor	ebx, ebx
	mov	ecx, r8d
	xor	r15d, r15d
	mov	r11d, eax
	and	r11d, 3
	je	L4
	sub	r11d, 1
Loopette:
	add	ebx, ebx
	mov	r8, [rsi+0]
	lea	rsi, [rsi+8]
	sbb	r8, [rdx+0]
	mov	r12, r8
	sbb	ebx, ebx
	shl	r8, cl
	or	r8, r15
	mov	r15, r12
	lea	rdx, [rdx+8]
	neg	cl
	shr	r15, cl
	neg	cl
	mov	[rdi+0], r8
	lea	rdi, [rdi+8]
	sub	r11d, 1
	jnc	Loopette
L4:
	sub	rax, 4
	jc	Lend
align 16
Loop:
	add	ebx, ebx
	mov	r8, [rsi+0]
	mov	r9, [rsi+8]
	mov	r10, [rsi+16]
	mov	r11, [rsi+24]
	lea	rsi, [rsi+32]
	sbb	r8, [rdx+0]
	mov	r12, r8
	sbb	r9, [rdx+8]
	mov	r13, r9
	sbb	r10, [rdx+16]
	mov	r14, r10
	sbb	r11, [rdx+24]
	sbb	ebx, ebx
	shl	r8, cl
	shl	r9, cl
	shl	r10, cl
	or	r8, r15
	mov	r15, r11
	shl	r11, cl
	lea	rdx, [rdx+32]
	neg	cl
	shr	r12, cl
	shr	r13, cl
	shr	r14, cl
	shr	r15, cl
	or	r9, r12
	or	r10, r13
	or	r11, r14
	neg	cl
	mov	[rdi+0], r8
	mov	[rdi+8], r9
	mov	[rdi+16], r10
	mov	[rdi+24], r11
	lea	rdi, [rdi+32]
	sub	rax, 4
	jnc	Loop
Lend:
	neg	ebx
	shl	rbx, cl
	adc	rbx, r15
	mov	rax, rbx
	pop	rbx
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	rsi
	pop	rdi
	ret
