divert(-1)
dnl  m4 macros for powerpc64 GNU/Linux assembly.

dnl  Copyright 2003, 2005, 2006 Free Software Foundation, Inc.

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
`ifdef(`ELFv2_ABI',
`
	.abiversion 2
')')

dnl  Called: PROLOGUE_cpu(GSYM_PREFIX`'foo[,toc])
dnl          EPILOGUE_cpu(GSYM_PREFIX`'foo)
dnl

define(`PROLOGUE_cpu',
m4_assert_numargs_range(1,2)
`ifelse(`$2',toc,,
`ifelse(`$2',,,`m4_error(`Unrecognised PROLOGUE parameter')')')dnl
ifdef(`ELFv2_ABI',
`
	.globl	$1
	.type	$1, @function
	.section	".text"
	.align	5
$1:
ifelse(`$2',toc,`
0:	addis	2, 12, (.TOC.-0b)@ha
	addi	2, 2, (.TOC.-0b)@l
	.localentry $1, .-$1
',)
',`
	.globl	$1
	.globl	.$1
	.section	".opd","aw"
	.align	3
$1:
	.llong	.$1, .TOC.@tocbase, 0
	.size	$1, 24
	.type	.$1, @function
	.section	".text"
	.align	5
.$1:
')')

define(`EPILOGUE_cpu',
m4_assert_numargs(1)
`ifdef(`ELFv2_ABI',`
	.size	$1, .-$1
',`
	.size	.$1, .-.$1
')')

define(`TOC_ENTRY', `')

define(`LEA',
m4_assert_numargs(2)
`define(`TOC_ENTRY',
`	.section	".toc", "aw"
..$2:	.tc	$2[TC], $2')'
	`ld	$1, ..$2@toc(2)')

define(`LEAL',
m4_assert_numargs(2)
`LEA($1,$2)')


define(`EXTERN',
m4_assert_numargs(1)
`dnl')

define(`EXTERN_FUNC',
m4_assert_numargs(1)
`dnl')

define(`DEF_OBJECT',
m4_assert_numargs_range(1,2)
`
	.section	.rodata
	ALIGN(ifelse($#,1,2,$2))
	.type	$1, @object
$1:
')

define(`END_OBJECT',
m4_assert_numargs(1)
`	.size	$1, .-$1')

define(`CALL',
	`bl	GSYM_PREFIX`'$1
	nop')

define(`ASM_END', `TOC_ENTRY')

undefine(`EXTRA_REGISTER')

divert
