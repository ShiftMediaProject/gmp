#define p0	%r28
#define p1	%r29
#define t32	%r19
#define t0	%r20
#define t1	%r21
#define x	%r22
#define m0	%r23
#define m1	%r24
	.level  2.0n
	.code
	.export __umul_ppmm64
__umul_ppmm64
	.proc
	.callinfo frame=128,no_calls
	.entry
	ldo		128(%r30),%r30
	depd		%r25,31,32,%r26
	std		%r26,-64(%r30)
	depd		%r23,31,32,%r24
	std		%r24,-56(%r30)

	ldw		-180(%r30),%r31

        fldd		-64(%r30),%fr4
        fldd		-56(%r30),%fr5

	xmpyu		%fr5R,%fr4R,%fr6
	fstd		%fr6,-128(%r30)
	xmpyu		%fr5R,%fr4L,%fr7
	fstd		%fr7,-120(%r30)
	xmpyu		%fr5L,%fr4R,%fr8
	fstd		%fr8,-112(%r30)
	xmpyu		%fr5L,%fr4L,%fr9
	fstd		%fr9,-104(%r30)

	depdi,z		1,31,1,t32		; t32 = 2^32

	ldd		-128(%r30),p0		; lo = low 64 bit of product
	ldd		-120(%r30),m0		; m0 = mid0 64 bit of product
	ldd		-112(%r30),m1		; m1 = mid1 64 bit of product
	ldd		-104(%r30),p1		; hi = high 64 bit of product

	add,l,*nuv	m0,m1,x			; x = m1+m0
	 add,l		t32,p1,p1		; propagate carry to mid of p1
	depd,z		x,31,32,t0		; lo32(m1+m0)
	add		t0,p0,p0
	extrd,u		x,31,32,t1		; hi32(m1+m0)
	add,dc		t1,p1,p1

	std		p0,0(%r31)		; store low half of product
	extrd,u		p1,31,32,%r28		; return high half of product
	bve		(%r2)
	.exit
	ldo		-128(%r30),%r30
	.procend
