	.toc
	.globl	__sdiv_qrnnd
	.globl	.__sdiv_qrnnd
	.csect	__sdiv_qrnnd[DS]
__sdiv_qrnnd:
	.long	.__sdiv_qrnnd, TOC[tc0], 0
	.csect	.text[PR]
	.align	2
.__sdiv_qrnnd:
	mtmq	5
	div	0,4,6
	mfmq	9
	st	9,0(3)
	mr	3,0
	br
