include(`../config.m4')

ASM_START()
PROLOGUE(__umul_ppmm)
	umul %o1,%o2,%g2
	st %g2,[%o0]
	retl
	rd %y,%o0
EPILOGUE(__umul_ppmm)
