divert(-1)

define(`ASM_START',
	`.ident	dummy')

define(`X',`^X$1')
define(`FLOAT64',
	`dnl
	.psect	$1@crud,data
$1:	.t_floating $2
	.endp')

define(`PROLOGUE',
	`dnl
	.stack	192		; What does this mean?  Only Cray knows.
	.psect	$1@code,code,cache
$1::')
define(`PROLOGUE_GP', `PROLOGUE($1)')

define(`EPILOGUE',
	`dnl
	.endp')

define(`DATASTART',
	`dnl
	.psect	$1@crud,data
$1:')
define(`DATAEND',
	`dnl
	.endp')

define(`ASM_END',
	`dnl
	.end')

define(`unop',`bis r31,r31,r31') ; Unicos assembler lacks unop
define(`cvttqc',`cvttq/c')

define(`ALIGN',`')		; Unicos assembler seems to align using garbage

divert

