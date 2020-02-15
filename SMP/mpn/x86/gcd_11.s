













































































	.section .rdata,"dr"
	.align	64, 0x90
ctz_table:

	.byte	6
	.byte	0
	.byte	1
	.byte	0
	.byte	2
	.byte	0
	.byte	1
	.byte	0
	.byte	3
	.byte	0
	.byte	1
	.byte	0
	.byte	2
	.byte	0
	.byte	1
	.byte	0
	.byte	4
	.byte	0
	.byte	1
	.byte	0
	.byte	2
	.byte	0
	.byte	1
	.byte	0
	.byte	3
	.byte	0
	.byte	1
	.byte	0
	.byte	2
	.byte	0
	.byte	1
	.byte	0
	.byte	5
	.byte	0
	.byte	1
	.byte	0
	.byte	2
	.byte	0
	.byte	1
	.byte	0
	.byte	3
	.byte	0
	.byte	1
	.byte	0
	.byte	2
	.byte	0
	.byte	1
	.byte	0
	.byte	4
	.byte	0
	.byte	1
	.byte	0
	.byte	2
	.byte	0
	.byte	1
	.byte	0
	.byte	3
	.byte	0
	.byte	1
	.byte	0
	.byte	2
	.byte	0
	.byte	1
	.byte	0

	






	.text
	.align	16, 0x90
	.globl	___gmpn_gcd_11
	
	.def	___gmpn_gcd_11
	.scl	2
	.type	32
	.endef
___gmpn_gcd_11:




	push	%edi
	push	%esi
	push	%ebx

	mov	16(%esp), %eax
	mov	20(%esp), %edx
	
	movl	$ctz_table, %esi

	sub	%edx, %eax			
	jz	Lend

	.align	16, 0x90
Ltop:	sbb	%ebx, %ebx		
	mov	%eax, %edi		
	mov	%eax, %ecx		
	and	%ebx, %edi		
	xor	%ebx, %eax		
	add	%edi, %edx		
	sub	%ebx, %eax		
Lmid:	and	$63, %ecx		
	movzbl	(%esi,%ecx), %ecx	
	jz	Lshift_alot
	shr	%cl, %eax			
	sub	%edx, %eax			
	jnz	Ltop

Lend:	mov	%edx, %eax
	pop	%ebx
	pop	%esi
	pop	%edi
	ret



Lshift_alot:
	shr	$6, %eax
	mov	%eax, %ecx
	jmp	Lmid
	

