.text
	.even
.globl ___udiv_qrnnd
___udiv_qrnnd:
	movel sp@(4),a0
	movel sp@(8),d1
	movel sp@(12),d0
	divul sp@(16),d1:d0
	movel d1,a0@
	rts
