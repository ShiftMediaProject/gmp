	.text
	.align	2
	.globl	__umul_ppmm
	.ent	__umul_ppmm
__umul_ppmm:
	multu $5,$6
	mflo	$3
	mfhi	$2
	sw	$3,0($4)
	j	$31
	.end	__umul_ppmm
