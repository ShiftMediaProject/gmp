	.machine	"ppc64"
.toc
.csect .text[PR]
	.align 2
	.globl __mpn_addmul_1
	.globl .__mpn_addmul_1
.csect __mpn_addmul_1[DS],3
__mpn_addmul_1:
	.llong .__mpn_addmul_1, TOC[tc0], 0
.csect .text[PR]
.__mpn_addmul_1:
	mtctr	5
	li	9,0		# cy_limb = 0
	addic	0,0,0
	cal	3,-8(3)
	cal	4,-8(4)
Loop:
	ldu	0,8(4)
	ld	10,8(3)
	mulld	7,0,6
	adde	7,7,9
	mulhdu	9,0,6
	addze	9,9
	addc	7,7,10
	stdu	7,8(3)
	bdnz	Loop

	addze	3,9
	blr
LT..__mpn_addmul_1:
	.long 0
	.byte 0,0,32,64,0,0,4,0
	.long 0
	.long LT..__mpn_addmul_1-.__mpn_addmul_1
	.short 14
	.byte "__mpn_addmul_1"
_section_.text:
.csect .data[RW]
	.long _section_.text
