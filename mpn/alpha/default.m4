divert(-1)

dnl  m4 macros for alpha assembler (everywhere except unicos).


dnl  Copyright 2000 Free Software Foundation, Inc.
dnl 
dnl  This file is part of the GNU MP Library.
dnl
dnl  The GNU MP Library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Lesser General Public License as
dnl  published by the Free Software Foundation; either version 2.1 of the
dnl  License, or (at your option) any later version.
dnl
dnl  The GNU MP Library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Lesser General Public License for more details.
dnl
dnl  You should have received a copy of the GNU Lesser General Public
dnl  License along with the GNU MP Library; see the file COPYING.LIB.  If
dnl  not, write to the Free Software Foundation, Inc., 59 Temple Place -
dnl  Suite 330, Boston, MA 02111-1307, USA.


dnl  Usage: ASM_START()
define(`ASM_START',
m4_assert_numargs(0)
`	.set noreorder
	.set noat')

dnl  Usage: X(value)
define(`X',
m4_assert_numargs(1)
`0x$1')

dnl  Usage: FLOAT64(label,value)
define(`FLOAT64',
m4_assert_numargs(2)
`	.align	3
$1:	.t_floating $2')

dnl  Usage: PROLOGUE(name)
define(`PROLOGUE',
m4_assert_numargs(1)
`	.text
	.align	3
	.globl	GSYM_PREFIX$1
	.ent	GSYM_PREFIX$1
GSYM_PREFIX$1:
	.frame r30,0,r26
	.prologue 0')

dnl  Usage: PROLOGUE_GP(name)
define(`PROLOGUE_GP',
m4_assert_numargs(1)
`	.text
	.align	3
	.globl	GSYM_PREFIX$1
	.ent	GSYM_PREFIX$1
GSYM_PREFIX$1:
	ldgp	r29,0(r27)
	.frame	r30,0,r26
	.prologue 1')

dnl  Usage: EPILOGUE(name)
define(`EPILOGUE',
m4_assert_numargs(1)
`	.end	GSYM_PREFIX$1')


dnl  Usage: r0 ... r31
dnl         f0 ... f31
dnl
dnl  Map register names r0 to $0, and f0 to $f0, etc.
dnl  This is needed on all systems but Unicos
dnl
dnl  defreg() is used to protect the $ in $0 (otherwise it would represent a
dnl  macro argument).  Double quoting is used to protect the f0 in $f0
dnl  (otherwise it would be an infinite recursion).

forloop(i,0,31,`defreg(`r'i,$i)')
forloop(i,0,31,`deflit(`f'i,``$f''i)')


dnl  Usage: DATASTART(name)
dnl         DATAEND()

define(`DATASTART',
m4_assert_numargs(1)
`	DATA
$1:')
define(`DATAEND',
m4_assert_numargs(0)
)


dnl  Usage: ASM_END()
define(`ASM_END',
m4_assert_numargs(0)
)

divert
