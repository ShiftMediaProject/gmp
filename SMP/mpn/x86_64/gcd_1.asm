extern __gmpn_modexact_1_odd
extern __gmpn_mod_1
section .text
align 16
global	__gmpn_gcd_1
__gmpn_gcd_1:
	push	rdi
	push	rsi
	mov	rdi, rcx
	mov	rsi, rdx
	mov	rdx, r8
	mov	rax, [rdi]
	or	rax, rdx
	bsf	rax, rax
	bsf	rcx, rdx
	shr	rdx, cl
	push	rax
	push	rdx
	sub	rsp, 40
	cmp	rsi, 1
	jnz	Lreduce_nby1
	mov	r8, [rdi]
	mov	rax, r8
	shr	r8, 6
	cmp	rdx, r8
	ja	Lreduced
	jmp	qword Lbmod
Lreduce_nby1:
	cmp	rsi, 16
	jl	Lbmod
	mov	r8, rdx
	mov	rdx, rsi
	mov	rcx, rdi
	call	qword __gmpn_mod_1
	jmp	qword Lreduced
Lbmod:
	mov	r8, rdx
	mov	rdx, rsi
	mov	rcx, rdi
	call	qword __gmpn_modexact_1_odd
Lreduced:
	add	rsp, 40
	pop	rdx
	bsf	rcx, rax
	jnz	Lmid
	jmp	qword Lend
align 16
Ltop:
	cmovc	rax, r10
	cmovc	rdx, r9
Lmid:
	shr	rax, cl
	mov	r10, rdx
	sub	r10, rax
	bsf	rcx, r10
	mov	r9, rax
	sub	rax, rdx
	jnz	Ltop
Lend:
	pop	rcx
	mov	rax, rdx
	shl	rax, cl
	pop	rsi
	pop	rdi
	ret
