
	.set noreorder
	.set volatile
	.set noat

.text
	.align 3
	.globl __umul_ppmm
	.ent __umul_ppmm
__umul_ppmm:
__umul_ppmm..ng:
	.frame $30,0,$26,0
	.prologue 0
	mulq $17,$18,$1
	umulh $17,$18,$0
	stq $1,0($16)
	ret $31,($26),1
	.end __umul_ppmm
