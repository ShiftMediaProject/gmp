divert(-1)


dnl  Copyright 2000, 2002 Free Software Foundation, Inc.
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


define(`ASM_START',`dnl')

dnl  Called: PROLOGUE_cpu(GSYM_PREFIX`'foo)
dnl          EPILOGUE_cpu(GSYM_PREFIX`'foo)

define(`PROLOGUE_cpu',
m4_assert_numargs(1)
	`
	.text
	.align	16
	.global	$1#
	.proc	$1#
$1:')

define(`EPILOGUE_cpu',
m4_assert_numargs(1)
	`
	.endp	$1#')

define(`DATASTART',
	`dnl
	DATA
$1:')
define(`DATAEND',`dnl')

define(`ASM_END',`dnl')

divert
