.text
	.even
.globl ___umul_ppmm
___umul_ppmm:
	movel sp@(4),a0
	movel sp@(8),d1
	movel sp@(12),d0
	mulul d0,d0:d1
	movel d1,a0@
	rts
