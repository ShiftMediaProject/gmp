#ifdef AIX_SYNTAX
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
#else
	.text
	.align 4
	.globl __umul_ppmm
__umul_ppmm:
#endif
	mullw 0,4,5
	mulhwu 9,4,5
	stw 0,0(3)
	mr 3,9
	blr
