	.file "udiv.c"
	.sect .lit,lit
gcc2_compiled.:
___gnu_compiled_c:
	.text
	.align 4
	.global ___udiv_qrnnd
	.word 0x60000
___udiv_qrnnd:
	mtsr q,lr3
	dividu gr96,lr4,lr5
	mfsr gr116,q
	jmpi lr0
	store 0,0,gr116,lr2
