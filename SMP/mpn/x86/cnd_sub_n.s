


































































	
	
	




	.text
	.align	16, 0x90
	.globl	___gmpn_cnd_sub_n
	
	.def	___gmpn_cnd_sub_n
	.scl	2
	.type	32
	.endef
___gmpn_cnd_sub_n:




	add	$-16, %esp
	mov	%ebp, (%esp)
	mov	%ebx, 4(%esp)
	mov	%esi, 8(%esp)
	mov	%edi, 12(%esp)

	
	mov	20(%esp), %eax
	neg	%eax
	sbb	%eax, %eax
	mov	%eax, 20(%esp)

	
	mov	24(%esp), %edi
	mov	28(%esp), %esi
	mov	32(%esp), %ebp
	mov	36(%esp), %ecx

	mov	(%ebp), %eax
	mov	(%esi), %ebx

	
	lea	(%ebp,%ecx,4), %ebp
	lea	(%esi,%ecx,4), %esi
	lea	-4(%edi,%ecx,4), %edi
	neg	%ecx

	and	20(%esp), %eax
	sub	%eax, %ebx
	sbb	%edx, %edx
	inc	%ecx
	je	Lend

	.align	16, 0x90
Ltop:	mov	(%ebp,%ecx,4), %eax
	and	20(%esp), %eax
	mov	%ebx, (%edi,%ecx,4)
	mov	(%esi,%ecx,4), %ebx
	add	%edx, %edx
	sbb	%eax, %ebx
	sbb	%edx, %edx
	inc	%ecx
	jne	Ltop

Lend:	mov	%ebx, (%edi)
	xor	%eax, %eax
	sub	%edx, %eax

	mov	(%esp), %ebp
	mov	4(%esp), %ebx
	mov	8(%esp), %esi
	mov	12(%esp), %edi
	add	$16, %esp
	ret


	

