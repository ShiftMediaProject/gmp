	.code
	.export		__umul_ppmm
	.align 4
__umul_ppmm
	.proc
	.callinfo frame=64,no_calls
	.entry

	ldo 64(%r30),%r30
	stw %r25,-16(0,%r30)
	fldws -16(0,%r30),%fr22R
	stw %r24,-16(0,%r30)
	fldws -16(0,%r30),%fr22L
	xmpyu %fr22R,%fr22L,%fr22
	fstds %fr22,-16(0,%r30)
	ldw -16(0,%r30),%r28
	ldw -12(0,%r30),%r29
	stw %r29,0(0,%r26)
	bv 0(%r2)
	ldo -64(%r30),%r30

	.exit
	.procend
