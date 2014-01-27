; Clipper __gmpn_add_n -- Add two limb vectors of the same length > 0 and store
; sum in a third limb vector.

; Copyright 1995, 2000 Free Software Foundation, Inc.

;  This file is part of the GNU MP Library.
;
;  The GNU MP Library is free software; you can redistribute it and/or modify
;  it under the terms of either:
;
;    * the GNU Lesser General Public License as published by the Free
;      Software Foundation; either version 3 of the License, or (at your
;      option) any later version.
;
;  or
;
;    * the GNU General Public License as published by the Free Software
;      Foundation; either version 2 of the License, or (at your option) any
;      later version.
;
;  or both in parallel, as here.
;
;  The GNU MP Library is distributed in the hope that it will be useful, but
;  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
;  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
;  for more details.
;
;  You should have received copies of the GNU General Public License and the
;  GNU Lesser General Public License along with the GNU MP Library.  If not,
;  see https://www.gnu.org/licenses/.

.text
	.align 16
.globl ___gmpn_add_n
___gmpn_add_n:
	subq	$8,sp
	storw	r6,(sp)
	loadw	12(sp),r2
	loadw	16(sp),r3
	loadq	$0,r6		; clear carry-save register

.Loop:	loadw	(r1),r4
	loadw	(r2),r5
	addwc	r6,r6		; restore carry from r6
	addwc	r5,r4
	storw	r4,(r0)
	subwc	r6,r6		; save carry in r6
	addq	$4,r0
	addq	$4,r1
	addq	$4,r2
	subq	$1,r3
	brne	.Loop

	negw	r6,r0
	loadw	(sp),r6
	addq	$8,sp
	ret	sp
