.text
	.align	2
.globl	___mpn_mul_1
___mpn_mul_1:
	mova	(pr0)[pr2*4],pr0
	mova	(pr1)[pr2*4],pr1
	mnegw	pr2,pr2
	movw	$0,tr3

Loop:	movw	(pr1)[pr2*4],tr1
	uemul	pr3,tr0
	addw	tr3,tr1
	movw	$0,tr3
	addwc	tr0,tr3
	movw	tr1,(pr0)[pr2*4]
	addw	$1,pr2
	bne	Loop

	movw	tr3,pr0
	ret
