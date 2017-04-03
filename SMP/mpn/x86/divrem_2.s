















































































	.text
	.align	16, 0x90
	.globl	___gmpn_divrem_2
	
	.def	___gmpn_divrem_2
	.scl	2
	.type	32
	.endef
___gmpn_divrem_2:




	push	%ebp
	push	%edi
	push	%esi
	push	%ebx
	sub	$36, %esp
	mov	68(%esp), %ecx		
	mov	72(%esp), %esi		
	movl	$0, 32(%esp)
	lea	0(,%ecx,4), %edi
	add	64(%esp), %edi		
	mov	(%esi), %ebx
	mov	4(%esi), %eax
	mov	%ebx, 20(%esp)
	sub	$12, %edi
	mov	%eax, 24(%esp)
	mov	%edi, 12(%esp)
	mov	8(%edi), %ebx
	mov	4(%edi), %ebp
	cmp	%eax, %ebx
	jb	L8
	seta	%dl
	cmp	20(%esp), %ebp
	setae	%al
	orb	%dl, %al		
	jne	L35
L8:
	mov	60(%esp), %esi		
	lea	-3(%esi,%ecx), %edi
	test	%edi, %edi
	js	L9
	mov	24(%esp), %edx
	mov	$-1, %esi
	mov	%esi, %eax
	mov	%esi, %ecx
	not	%edx
	divl	24(%esp)
	mov	%eax, %esi
	imul	24(%esp), %eax
	mov	%eax, (%esp)
	mov	%esi, %eax
	mull	20(%esp)
	mov	(%esp), %eax
	add	20(%esp), %eax
	adc	$0, %ecx
	add	%eax, %edx
	adc	$0, %ecx
	mov	%ecx, %eax
	js	L32
L36:	dec	%esi
	sub	24(%esp), %edx
	sbb	$0, %eax
	jns	L36
L32:
	mov	%esi, 16(%esp)		
	mov	%edi, %ecx		
	mov	12(%esp), %esi		
	mov	24(%esp), %eax
	neg	%eax
	mov	%eax, 4(%esp)		
	.align	16, 0x90
	nop




Lloop:
	mov	16(%esp), %eax		
	mul	%ebx
	add	%ebp, %eax
	mov	%eax, (%esp)		
	adc	%ebx, %edx
	mov	%edx, %edi		
	imul	4(%esp), %edx
	mov	20(%esp), %eax
	lea	(%edx, %ebp), %ebx	
	mul	%edi
	xor	%ebp, %ebp
	cmp	60(%esp), %ecx
	jl	L19
	mov	(%esi), %ebp
	sub	$4, %esi
L19:	sub	20(%esp), %ebp
	sbb	24(%esp), %ebx
	sub	%eax, %ebp
	sbb	%edx, %ebx
	mov	20(%esp), %eax		
	inc	%edi
	xor	%edx, %edx
	cmp	(%esp), %ebx
	adc	$-1, %edx		
	add	%edx, %edi		
	and	%edx, %eax		
	and	24(%esp), %edx		
	add	%eax, %ebp
	adc	%edx, %ebx
	cmp	24(%esp), %ebx
	jae	Lfix
Lbck:	mov	56(%esp), %edx
	mov	%edi, (%edx, %ecx, 4)
	dec	%ecx
	jns	Lloop

L9:	mov	64(%esp), %esi		
	mov	%ebp, (%esi)
	mov	%ebx, 4(%esi)
	mov	32(%esp), %eax
	add	$36, %esp
	pop	%ebx
	pop	%esi
	pop	%edi
	pop	%ebp
	ret



Lfix:	seta	%dl
	cmp	20(%esp), %ebp
	setae	%al
	orb	%dl, %al		
	je	Lbck
	inc	%edi
	sub	20(%esp), %ebp
	sbb	24(%esp), %ebx
	jmp	Lbck

L35:	sub	20(%esp), %ebp
	sbb	24(%esp), %ebx
	movl	$1, 32(%esp)
	jmp	L8
	
