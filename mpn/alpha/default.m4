divert(-1)

define(`ASM_START',
	`
	.set noreorder
	.set noat')

define(`X',`0x$1')
define(`INT64',
	`
	.align	3
$1:	.quad	$2')

define(`PROLOGUE',
	`
	.text
	.align	3
	.globl	$1
	.ent	$1
$1:
	.frame r30,0,r26
	.prologue 0')

define(`PROLOGUE_GP',
	`
	.text
	.align	3
	.globl	$1
	.ent	$1
$1:
	ldgp	r29,0(r27)
	.frame	r30,0,r26
	.prologue 1')

define(`EPILOGUE',
	`
	.end	$1')

dnl Map register names r0, r1, etc, to `$0', `$1', etc.
dnl This is needed on all systems but Unicos
forloop(i,0,31,
`define(`r'i,``$''i)'
)
forloop(i,0,31,
`define(`f'i,``$f''i)'
)

define(`DATASTART',
	`dnl
	DATA
$1:')
define(`DATAEND',`dnl')

define(`ASM_END',`dnl')

divert
