.text
	.align	16
.globl	___mpn_mul_1
___mpn_mul_1:
	subq	$8,sp
	storw	r6,(sp)
	loadw	12(sp),r2
	loadw	16(sp),r3
	loadq	$0,r6		; clear carry limb

.Loop:	loadw	(r1),r4
	mulwux	r3,r4
	addw	r6,r4		; add old carry limb into low product limb
	loadq	$0,r6
	addwc	r5,r6		; propagate cy into high product limb
	storw	r4,(r0)
	addq	$4,r0
	addq	$4,r1
	subq	$1,r2
	brne	.Loop

	movw	r6,r0
	loadw	0(sp),r6
	addq	$8,sp
	ret	sp
