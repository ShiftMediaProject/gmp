
































































	.text
	.align	16, 0x90
	.globl	___gmpn_bdiv_dbm1c
	
	.def	___gmpn_bdiv_dbm1c
	.scl	2
	.type	32
	.endef
___gmpn_bdiv_dbm1c:




	mov	16(%esp), %ecx		
	push	%esi
	mov	12(%esp), %esi		
	push	%edi
	mov	12(%esp), %edi		
	push	%ebp
	mov	24(%esp), %ebp		
	push	%ebx

	mov	(%esi), %eax
	mul	%ecx
	mov	36(%esp), %ebx
	sub	%eax, %ebx
	mov	%ebx, (%edi)
	sbb	%edx, %ebx

	mov	%ebp, %eax
	and	$3, %eax
	jz	Lb0
	cmp	$2, %eax
	jc	Lb1
	jz	Lb2

Lb3:	lea	-8(%esi), %esi
	lea	8(%edi), %edi
	add	$-3, %ebp
	jmp	L3

Lb0:	mov	4(%esi), %eax
	lea	-4(%esi), %esi
	lea	12(%edi), %edi
	add	$-4, %ebp
	jmp	L0

Lb2:	mov	4(%esi), %eax
	lea	4(%esi), %esi
	lea	4(%edi), %edi
	add	$-2, %ebp
	jmp	L2

	.align	8, 0x90
Ltop:	mov	4(%esi), %eax
	mul	%ecx
	lea	16(%edi), %edi
	sub	%eax, %ebx
	mov	8(%esi), %eax
	mov	%ebx, -12(%edi)
	sbb	%edx, %ebx
L0:	mul	%ecx
	sub	%eax, %ebx
	mov	%ebx, -8(%edi)
	sbb	%edx, %ebx
L3:	mov	12(%esi), %eax
	mul	%ecx
	sub	%eax, %ebx
	mov	%ebx, -4(%edi)
	mov	16(%esi), %eax
	lea	16(%esi), %esi
	sbb	%edx, %ebx
L2:	mul	%ecx
	sub	%eax, %ebx
	mov	%ebx, 0(%edi)
	sbb	%edx, %ebx
Lb1:	add	$-4, %ebp
	jns	Ltop

	mov	%ebx, %eax
	pop	%ebx
	pop	%ebp
	pop	%edi
	pop	%esi
	ret


	
