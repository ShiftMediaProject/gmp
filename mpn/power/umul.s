.toc
.csect .text[PR]
	.align 2
	.globl __umul_ppmm
	.globl .__umul_ppmm
.csect __umul_ppmm[DS]
__umul_ppmm:
	.long .__umul_ppmm, TOC[tc0], 0
.csect .text[PR]
.__umul_ppmm:
	mul 9,4,5
	srai 0,4,31
	and 0,0,5
	srai 5,5,31
	and 5,5,4
	cax 0,0,5
	mfmq 11
	st 11,0(3)
	cax 3,9,0
	br
