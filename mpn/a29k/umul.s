	.file "umul.c"
	.sect .lit,lit
gcc2_compiled.:
___gnu_compiled_c:
	.text
	.align 4
	.global ___umul_ppmm
	.word 0x50000
___umul_ppmm:
	multiplu gr116,lr3,lr4
	multmu gr96,lr3,lr4
	jmpi lr0
	store 0,0,gr116,lr2
