	.section	".text",#alloc,#execinstr
	.register	%g3,#scratch
	.register	%g2,#scratch
	.align	8
.Ltwo52:
	.word	1127219200,0
	.global	__mpn_mul_1
__mpn_mul_1:

define(X,511)

! 6 fmul
! 6 fadd + 6 fdtox + 2 fxtod
! 7 ldX and 7 stX	14 
! 13 shift ops		13
! 30 IEU insn		15
! 1 movCOND		+1

! By adding cylimb to p0, the likelyhood for carry-out would be very low,
! making a conditional branch well-predicted and faster than a movCOND.
! That could save at least one cycle.

	save	%sp,-1024,%sp

	sethi	%hh(.Ltwo52),%o5
	sethi	%lm(.Ltwo52),%g1
	or	%o5,%hm(.Ltwo52),%o5
	or	%g1,%lo(.Ltwo52),%g1
	sllx	%o5,32,%o5
	or	%g1,%o5,%g1

	ldd	[%g1],%f4	! f4,f5 = 2^52
	fmovd	%f4,%f0		! f0,f1 = 2^52
	fmovd	%f4,%f2		! f2,f3 = 2^52

	sllx	%i2,3,%i2	! scale loop counter
	add	%i0,%i2,%i0	! make rp point to end of dst operand
	add	%i1,%i2,%i1	! make sp point ro end of src operand
	add	%i1,4,%i4	! Make sp' point just above sp (at low word)
	sub	%g0,%i2,%i2	! negate loop counter

	mov	0,%i3		! cylimb = 0

! Now set up v0, v21, and v42
! v0 in %f10
! v21 in %f12
! v42 in %f14

	b,a	.Loop
	.align	32
.Loop:
!-
!-
	ld	[%sp-1],%g0	! NOP
	nop
	nop
	fsubd	%f2,%f4,%f6	! convert u32 to IEEE double
!-
	nop
	nop
	ld	[%i1+%i2],%f3	! u32
	fsubd	%f0,%f4,%f8	! convert u0 to IEEE double
!-
	nop
	nop
	ld	[%i4+%i2],%f1	! u0
	faddd	%f4,%f4,%f26	! NOP
!-
	nop
	nop
	ldx	[%sp+X+0],%l0
	fmuld	%f6,%f14,%f16	! p74 = u32 * v42
!-
	nop
	nop
	ldx	[%sp+X+8],%l1
	fmuld	%f6,%f12,%f18	! p53 = u32 * v21
!-
	nop
	nop
	ldx	[%sp+X+16],%l2
	fmuld	%f6,%f10,%f20	! p32 = u32 * v0
!-
	nop
	nop
	ldx	[%sp+X+24],%l3
	fmuld	%f8,%f14,%f22	! p42 = u0 * v42
!-
	nop
	nop
	ldx	[%sp+X+32],%l4
	fmuld	%f8,%f12,%f24	! p21 = u0 * v21
!-
	nop
	nop
	ldx	[%sp+X+40],%l5
	fmuld	%f8,%f10,%f26	! p0  = u0 * v0
!-
	nop
	nop
	std	%f16,[%sp+X+0]
	fdtox	%f16,%f16
!-
	nop
	nop
	std	%f18,[%sp+X+8]
	fdtox	%f18,%f18
!-
	nop
	nop
	std	%f20,[%sp+X+16]
	fdtox	%f20,%f20
!-
	nop
	nop
	std	%f22,[%sp+X+24]
	fdtox	%f22,%f22
!-
	nop
	nop
	std	%f24,[%sp+X+32]
	fdtox	%f24,%f24
!-
	nop
	nop
	std	%f26,[%sp+X+40]
	fdtox	%f26,%f26
	! movcs should go here, long after any loads
!-
#	srax	%o0,11,%o3	! IEU 0
#	sllx	%o1,10,%o2	! IEU 0
#	add	%o2,%o3,%o2
#	srlx	%g5,32,%o3	! IEU 0
#	add	%o2,%o3,%o2
#	srlx	%g4,22,%o3	! IEU 0
#	add	%o2,%o3,%o2
#	srax	%g3,43,%o3	! IEU 0
#	add	%o2,%o3,%o2
#	srlx	%g2,21,%o3	! IEU 0
#	and	%g2,%g1,%g2
#	sllx	%g4,21,%g4	! IEU 0
#	add	%o3,%g3,%g3
#	add	%g3,%g4,%g3
#	sllx	%g5,11,%g4	! IEU 0
#	add	%g3,%g4,%g3
#	sllx	%o0,32,%g4	! IEU 0
#	add	%g3,%g4,%g3
#	sllx	%o1,53,%g4	! IEU 0
#	add	%g3,%g4,%g3
#	sllx	%o2,43,%g4	! IEU 0
#	sub	%g3,%g4,%g3
#	srlx	%g3,43,%g4	! IEU 0
#	sllx	%g3,21,%g3	! IEU 0
#	add	%g3,%g2,%g2
#	addcc	%g2,%i3,%g2	! IEU 1
#	add	%o2,%g4,%i3

#	add	%i3,1,%o4
#	movcs	%xcc,%o4,%i3

!	ldx	[%i0+%i2],X
!	addcc	X,%g2,%g2	! IEU 1
!	add     %i3,1,%o4
!	movcs   %xcc,%o4,%i3

#	stx	%g2,[%i0+%i2]

	nop
	add	%i2,8,%i2
	brnz,pt	%i2,.Loop
	faddd	%f4,%f4,%f28

	ret
	restore	%g0,%i3,%o0
	.type	__mpn_mul_1,2
	.size	__mpn_mul_1,(.-__mpn_mul_1)















