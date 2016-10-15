













































































	.text
	.align	16, 0x90
	.globl	___gmpn_sec_tabselect
	
	.def	___gmpn_sec_tabselect
	.scl	2
	.type	32
	.endef
___gmpn_sec_tabselect:




	push	%edi
	push	%esi
	push	%ebx
	push	%ebp
	mov	20(%esp), %edi
	mov	24(%esp), %esi
	mov	28(%esp), %ebx
	mov	32(%esp), %ecx

	lea	(%edi,%ebx,4), %edi
	lea	(%esi,%ebx,4), %esi
	sub	%ecx, 36(%esp)
Louter:
	mov	36(%esp), %eax
	add	%ecx, %eax
	neg	%eax			
	sbb	%eax, %eax
	mov	%eax, 32(%esp)
	not	%eax
	mov	%eax, 20(%esp)

	mov	%ebx, %ebp
	neg	%ebp

	.align	16, 0x90
Ltop:	mov	(%esi,%ebp,4), %eax
	and	20(%esp), %eax
	mov	(%edi,%ebp,4), %edx
	and	32(%esp), %edx
	or	%edx, %eax
	mov	%eax, (%edi,%ebp,4)
	inc	%ebp
	js	Ltop

Lend:	mov	%ebx, %eax
	lea	(%esi,%eax,4), %esi
	dec	%ecx
	jne	Louter

Louter_end:
	pop	%ebp
	pop	%ebx
	pop	%esi
	pop	%edi
	ret


	
