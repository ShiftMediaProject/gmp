divert(-1)
dnl  m4 macros for AIX 32-bit assembly.

dnl  Copyright 2000, 2001, 2002 Free Software Foundation, Inc.
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

define(`ASM_START',
	`.toc')

dnl  Called: PROLOGUE_cpu(GSYM_PREFIX`'foo)
dnl          EPILOGUE_cpu(GSYM_PREFIX`'foo)
dnl
dnl  Don't want ELF style .size in the epilogue.

define(`PROLOGUE_cpu',
m4_assert_numargs(1)
	`
	.globl	$1
	.globl	.$1
	.csect	[DS],2
$1:
	.long	.$1, TOC[tc0], 0
	.csect	[PR]
	.align	2
.$1:')

define(`EPILOGUE_cpu',
m4_assert_numargs(1)
`')

divert
