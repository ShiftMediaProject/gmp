.text
	.align 16
.globl ___mpn_add_n
___mpn_add_n:
	subq	$8,sp
	storw	r6,(sp)
	loadw	12(sp),r2
	loadw	16(sp),r3
	loadq	$0,r6		; clear carry-save register

.Loop:	loadw	(r1),r4
	loadw	(r2),r5
	addwc	r6,r6		; restore carry from r6
	addwc	r5,r4
	storw	r4,(r0)
	subwc	r6,r6		; save carry in r6
	addq	$4,r0
	addq	$4,r1
	addq	$4,r2
	subq	$1,r3
	brne	.Loop

	negw	r6,r0
	loadw	(sp),r6
	addq	$8,sp
	ret	sp
