 # Alpha EV5 __mpn_sub_n -- Subtract two limb vectors of the same length > 0
 # and store difference in a third limb vector.

 # Copyright (C) 1995, 1999 Free Software Foundation, Inc.

 # This file is part of the GNU MP Library.

 # The GNU MP Library is free software; you can redistribute it and/or modify
 # it under the terms of the GNU Library General Public License as published by
 # the Free Software Foundation; either version 2 of the License, or (at your
 # option) any later version.

 # The GNU MP Library is distributed in the hope that it will be useful, but
 # WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 # or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 # License for more details.

 # You should have received a copy of the GNU Library General Public License
 # along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
 # the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 # MA 02111-1307, USA.


 # INPUT PARAMETERS
 # res_ptr	$16
 # s1_ptr	$17
 # s2_ptr	$18
 # size		$19

	.set	noreorder
	.set	noat
.text
	.align	3
	.globl	__mpn_sub_n
	.ent	__mpn_sub_n
__mpn_sub_n:
	.frame	$30,0,$26,0

	or	$31,$31,$25		# clear cy
	subq	$19,4,$19		# decr loop cnt
	blt	$19,.Lend2		# if less than 4 limbs, goto 2nd loop
 # Start software pipeline for 1st loop
	ldq	$0,0($18)
	ldq	$4,0($17)
	ldq	$1,8($18)
	ldq	$5,8($17)
	addq	$17,32,$17		# update s1_ptr
	ldq	$2,16($18)
	subq	$4,$0,$20		# 1st main subtract
	ldq	$3,24($18)
	subq	$19,4,$19		# decr loop cnt
	ldq	$6,-16($17)
	cmpult	$4,$0,$25		# compute cy from last subtract
	ldq	$7,-8($17)
	subq	$5,$1,$28		# 2nd main subtract
	addq	$18,32,$18		# update s2_ptr
	subq	$28,$25,$21		# 2nd carry subtract
	cmpult	$5,$1,$8		# compute cy from last subtract
	blt	$19,.Lend1		# if less than 4 limbs remain, jump
 # 1st loop handles groups of 4 limbs in a software pipeline
	.align	4
.Loop:	cmpult	$28,$25,$25		# compute cy from last subtract
	ldq	$0,0($18)
	or	$8,$25,$25		# combine cy from the two subtracts
	ldq	$1,8($18)
	subq	$6,$2,$28		# 3rd main subtract
	ldq	$4,0($17)
	subq	$28,$25,$22		# 3rd carry subtract
	ldq	$5,8($17)
	cmpult	$6,$2,$8		# compute cy from last subtract
	cmpult	$28,$25,$25		# compute cy from last subtract
	stq	$20,0($16)
	or	$8,$25,$25		# combine cy from the two subtracts
	stq	$21,8($16)
	subq	$7,$3,$28		# 4th main subtract
	subq	$28,$25,$23		# 4th carry subtract
	cmpult	$7,$3,$8		# compute cy from last subtract
	cmpult	$28,$25,$25		# compute cy from last subtract
		addq	$17,32,$17		# update s1_ptr
	or	$8,$25,$25		# combine cy from the two subtracts
		addq	$16,32,$16		# update res_ptr
	subq	$4,$0,$28		# 1st main subtract
	ldq	$2,16($18)
	subq	$28,$25,$20		# 1st carry subtract
	ldq	$3,24($18)
	cmpult	$4,$0,$8		# compute cy from last subtract
	ldq	$6,-16($17)
	cmpult	$28,$25,$25		# compute cy from last subtract
	ldq	$7,-8($17)
	or	$8,$25,$25		# combine cy from the two subtracts
	subq	$19,4,$19		# decr loop cnt
	stq	$22,-16($16)
	subq	$5,$1,$28		# 2nd main subtract
	stq	$23,-8($16)
	subq	$28,$25,$21		# 2nd carry subtract
		addq	$18,32,$18		# update s2_ptr
	cmpult	$5,$1,$8		# compute cy from last subtract
	bge	$19,.Loop
 # Finish software pipeline for 1st loop
.Lend1:	cmpult	$28,$25,$25		# compute cy from last subtract
	or	$8,$25,$25		# combine cy from the two subtracts
	subq	$6,$2,$28		# cy add
	subq	$28,$25,$22		# 3rd main subtract
	cmpult	$6,$2,$8		# compute cy from last subtract
	cmpult	$28,$25,$25		# compute cy from last subtract
	stq	$20,0($16)
	or	$8,$25,$25		# combine cy from the two subtracts
	stq	$21,8($16)
	subq	$7,$3,$28		# cy add
	subq	$28,$25,$23		# 4th main subtract
	cmpult	$7,$3,$8		# compute cy from last subtract
	cmpult	$28,$25,$25		# compute cy from last subtract
	or	$8,$25,$25		# combine cy from the two subtracts
	addq	$16,32,$16		# update res_ptr
	stq	$22,-16($16)
	stq	$23,-8($16)
.Lend2:	addq	$19,4,$19		# restore loop cnt
	beq	$19,.Lret
 # Start software pipeline for 2nd loop
	ldq	$0,0($18)
	ldq	$4,0($17)
	subq	$19,1,$19
	beq	$19,.Lend0
 # 2nd loop handles remaining 1-3 limbs
	.align	4
.Loop0:	subq	$4,$0,$28		# main subtract
	cmpult	$4,$0,$8		# compute cy from last subtract
	ldq	$0,8($18)
	ldq	$4,8($17)
	subq	$28,$25,$20		# carry subtract
	addq	$18,8,$18
	addq	$17,8,$17
	stq	$20,0($16)
	cmpult	$28,$25,$25		# compute cy from last subtract
	subq	$19,1,$19		# decr loop cnt
	or	$8,$25,$25		# combine cy from the two subtracts
	addq	$16,8,$16
	bne	$19,.Loop0
.Lend0:	subq	$4,$0,$28		# main subtract
	subq	$28,$25,$20		# carry subtract
	cmpult	$4,$0,$8		# compute cy from last subtract
	cmpult	$28,$25,$25		# compute cy from last subtract
	stq	$20,0($16)
	or	$8,$25,$25		# combine cy from the two subtracts

.Lret:	or	$25,$31,$0		# return cy
	ret	$31,($26),1
	.end	__mpn_sub_n
