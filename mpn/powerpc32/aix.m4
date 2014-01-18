divert(-1)
dnl  m4 macros for AIX 32-bit assembly.

dnl  Copyright 2000-2002, 2005, 2006 Free Software Foundation, Inc.

dnl  This file is part of the GNU MP Library.

dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU Lesser General Public License as published
dnl  by the Free Software Foundation; either version 3 of the License, or (at
dnl  your option) any later version.

dnl  The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
dnl  License for more details.

dnl  You should have received a copy of the GNU Lesser General Public License
dnl  along with the GNU MP Library.  If not, see https://www.gnu.org/licenses/.

define(`ASM_START',
`	.toc')

dnl  Called: PROLOGUE_cpu(GSYM_PREFIX`'foo)
dnl          EPILOGUE_cpu(GSYM_PREFIX`'foo)
dnl
dnl  Don't want ELF style .size in the epilogue.

define(`PROLOGUE_cpu',
m4_assert_numargs(1)
	`
	.globl	$1
	.globl	.$1
	.csect	[DS], 2
$1:
	.long	.$1, TOC[tc0], 0
	.csect	[PR]
	.align	2
.$1:')

define(`EPILOGUE_cpu',
m4_assert_numargs(1)
`')

define(`TOC_ENTRY', `')

define(`LEA',
m4_assert_numargs(2)
`define(`TOC_ENTRY',
`	.toc
tc$2:
	.tc	$2[TC], $2')'
`	lwz	$1, tc$2(2)')

define(`EXTERN',
m4_assert_numargs(1)
`	.globl	$1')

define(`DEF_OBJECT',
m4_assert_numargs_range(1,2)
`	.csect	[RO], 3
	ALIGN(ifelse($#,1,2,$2))
$1:
')

define(`END_OBJECT',
m4_assert_numargs(1))

define(`ASM_END', `TOC_ENTRY')

divert
