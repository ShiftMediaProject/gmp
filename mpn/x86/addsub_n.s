	r1[] = s1[] + s2[]
	r2[] = s1[] - s2[]

			r1  r2
			s1  s2
	eax,ebx,ecx,edx,esi,edi,ebp

	movl	(%esi),%eax
	movl	(%edi),%edx
	adcl	%edx,%eax
	movl	%eax,(%edi)

	movl	(%esi),%eax
	movl	(%edi),%ebp
	adcl	%edx,%eax
	movl	%eax,(%edi)

	sbbl	(%edi),%edx


------------------------

/* Cute i386/i486/p6 addsub loop for the "full overlap" case r1=s2,r2=s1.
   We let subtraction and addition alternate in being two limbs
   ahead of the other, thereby avoiding some SAVE_RESTORE_CY. */
Loop0:				// L=load E=execute S=store
	movl	(%edi),%ebx	// sub 0 L
	sbbl	(%esi),%ebx	// sub 0 LE
	movl	4(%edi),%ecx	// sub 1 L
	sbbl	4(%esi),%ecx	// sub 1 LE
	SAVE_RESTORE_CY(%edx)
	movl	(%esi),%eax	// add 0 L
	adcl	%eax,(%edi)	// add 0 LES
	movl	4(%esi),%eax	// add 1 L
	adcl	%eax,4(%edi)	// add 1 LES
	movl	%ebx,(%esi)	// sub 0 S
	movl	%ecx,4(%esi)	// sub 1 S
	movl	8(%esi),%ebx	// add 2 L
	adcl	8(%edi),%ebx	// add 2 LE
	movl	12(%esi),%ecx	// add 3 L
	adcl	12(%edi),%ecx	// add 3 LE
	SAVE_RESTORE_CY(%edx)
	movl	8(%esi),%eax	// sub 2 L
	sbbl	%eax,8(%edi)	// sub 2 LES
	movl	12(%esi),%eax	// sub 3 L
	sbbl	%eax,12(%edi)	// sub 3 LES
	movl	%ebx,8(%esi)	// add 2 S
	movl	%ecx,12(%esi)	// add 3 S
	decl	%ebp
	jnz	Loop0

/* Cute i386/i486/p6 addsub loop for the "full overlap" case r1=s1,r2=s2.
   We let subtraction and addition alternate in being two limbs
   ahead of the other, thereby avoiding some SAVE_RESTORE_CY. */
