divert(-1)

dnl  m4 macros for SPARC assembler (32 and 64 bit).


dnl  Copyright 2002, 2011, 2013 Free Software Foundation, Inc.
dnl
dnl  This file is part of the GNU MP Library.
dnl
dnl  The GNU MP Library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Lesser General Public License as
dnl  published by the Free Software Foundation; either version 3 of the
dnl  License, or (at your option) any later version.
dnl
dnl  The GNU MP Library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Lesser General Public License for more details.
dnl
dnl  You should have received a copy of the GNU Lesser General Public License
dnl  along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.


changecom(;)	dnl cannot use default # since that's used in REGISTER decls


dnl  Usage: REGISTER(reg,attr)
dnl
dnl  Give a ".register reg,attr" directive, if the assembler supports it.
dnl  HAVE_REGISTER comes from the GMP_ASM_SPARC_REGISTER configure test.

define(REGISTER,
m4_assert_numargs(2)
m4_assert_defined(`HAVE_REGISTER')
`ifelse(HAVE_REGISTER,yes,
`.register `$1',`$2'')')

dnl  Usage: LEA(symbol,reg,pic_reg)
dnl
dnl  Use whatever code sequence is appropriate to load "symbol" into register
dnl  "reg", potentially using register "pic_reg" to perform the calculations.
dnl  This takes into consideration things like PIC, whether we are generating
dnl  64-bit code, etc.

define(LEA,
m4_assert_numargs(3)
m4_assert_defined(`HAVE_GOTDATA')
`ifdef(`PIC',`
ifelse(HAVE_GOTDATA,yes,`
	sethi	%hi(_GLOBAL_OFFSET_TABLE_-4), %`$3'
	call	__sparc_get_pc_thunk.`$3'
	 or	%`$3', %lo(_GLOBAL_OFFSET_TABLE_+4), %`$3'
	sethi	%gdop_hix22(`$1'), %`$2'
	xor	%`$2', %gdop_lox10(`$1'), %`$2'
ifdef(`HAVE_ABI_64',`
	ldx	[%`$3' + %`$2'], %`$2', %gdop(`$1')',`
	ld	[%`$3' + %`$2'], %`$2', %gdop(`$1')')',`
	sethi	%hi(_GLOBAL_OFFSET_TABLE_-4), %`$3'
	call	__sparc_get_pc_thunk.`$3'
	 or	%`$3', %lo(_GLOBAL_OFFSET_TABLE_+4), %`$3'
	sethi	%hi(`$1'), %`$2'
	or	%`$2', %lo(`$1'), %`$2'
ifdef(`HAVE_ABI_64',`
	ldx	[%`$3' + %`$2'], %`$2'',`
	ld	[%`$3' + %`$2'], %`$2'')')',`
ifdef(`HAVE_ABI_64',`
	setx	`$1', %`$3', %`$2'',`
	set	`$1', %`$2'')')')

dnl  Usage: LEA_LEAF(symbol,reg,pic_reg)
dnl
dnl  Exactly the same as LEA except that it works in a leaf function.
dnl  Specifically, when generating PIC code, it makes sure to preserve the %o7
dnl  register.

define(LEA_LEAF,
m4_assert_numargs(3)
m4_assert_defined(`HAVE_GOTDATA')
`ifdef(`PIC',`
ifelse(HAVE_GOTDATA,yes,`
	sethi	%hi(_GLOBAL_OFFSET_TABLE_-4), %`$3'
	mov	%o7, %`$2'
	call	__sparc_get_pc_thunk.`$3'
	 or	%`$3', %lo(_GLOBAL_OFFSET_TABLE_+4), %`$3'
	mov	%`$2', %o7
	sethi	%gdop_hix22(`$1'), %`$2'
	xor	%`$2', %gdop_lox10(`$1'), %`$2'
ifdef(`HAVE_ABI_64',`
	ldx	[%`$3' + %`$2'], %`$2', %gdop(`$1')',`
	ld	[%`$3' + %`$2'], %`$2', %gdop(`$1')')',`
	sethi	%hi(_GLOBAL_OFFSET_TABLE_-4), %`$3'
	mov	%o7, %`$2'
	call	__sparc_get_pc_thunk.`$3'
	 or	%`$3', %lo(_GLOBAL_OFFSET_TABLE_+4), %`$3'
	mov	%`$2', %o7
	sethi	%hi(`$1'), %`$2'
	or	%`$2', %lo(`$1'), %`$2'
ifdef(`HAVE_ABI_64',`
	ldx	[%`$3' + %`$2'], %`$2'',`
	ld	[%`$3' + %`$2'], %`$2'')')',`
ifdef(`HAVE_ABI_64',`
	setx	`$1', %`$3', %`$2'',`
	set	`$1', %`$2'')')')

dnl  Usage: LEA_THUNK(pic_reg)
dnl
dnl  Files that make use of LEA and LEA_LEAF must emit a PIC thunk using
dnl  LEA_THUNK.  The "pic_reg" argument given must exactly the same as the
dnl  one given to the LEA LEA_LEAF invocations.
dnl
dnl  If multiple PIC registers are used in invocations of LEA and LEA_LEAF then
dnl  multiple thunks needs to be emitted, one for each PIC register used.

define(LEA_THUNK,
m4_assert_numargs(1)
m4_assert_defined(`HAVE_SHARED_THUNKS')
`ifdef(`PIC',`
ifelse(HAVE_SHARED_THUNKS,yes,`
	.section	.text.__sparc_get_pc_thunk.`$1',"axG",@progbits,__sparc_get_pc_thunk.`$1',comdat
	.weak	__sparc_get_pc_thunk.`$1'
	.hidden	__sparc_get_pc_thunk.`$1'
	.type	__sparc_get_pc_thunk.`$1', #function
__sparc_get_pc_thunk.`$1':
	jmp	%o7+8
	 add	%o7, %`$1', %`$1'
	TEXT
',`
	TEXT
__sparc_get_pc_thunk.`$1':
	retl
	 add	%o7, %`$1', %`$1'')')')

C Testing mechanism for running newer code on older processors
ifdef(`FAKE_T3',`
  include_mpn(`sparc64/ultrasparct3/missing.m4')
',`
  define(`addxccc',	``addxccc'	$1, $2, $3')
  define(`addxc',	``addxc'	$1, $2, $3')
  define(`umulxhi',	``umulxhi'	$1, $2, $3')
  define(`lzcnt',	``lzd'	$1, $2')
')

divert
