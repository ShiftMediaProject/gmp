divert(-1)
dnl  m4 macros for gmp assembler, shared by all CPUs.
dnl
dnl  These macros are designed for use with either GNU m4 or the system m4,
dnl  but if you have trouble with your system m4, try GNU m4 instead.  Also,
dnl  GNU m4 will give you filenames and line numbers in error messages.


dnl  Copyright (C) 1999-2000 Free Software Foundation, Inc.
dnl 
dnl  This file is part of the GNU MP Library.
dnl
dnl  The GNU MP Library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Library General Public License as
dnl  published by the Free Software Foundation; either version 2 of the
dnl  License, or (at your option) any later version.
dnl
dnl  The GNU MP Library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Library General Public License for more details.
dnl
dnl  You should have received a copy of the GNU Library General Public
dnl  License along with the GNU MP Library; see the file COPYING.LIB.  If
dnl  not, write to the Free Software Foundation, Inc., 675 Mass Ave,
dnl  Cambridge, MA 02139, USA.

define(`ASM_START',
	`')
define(`PROLOGUE',
	`
	TEXT
	ALIGN(4)
	GLOBL	GSYM_PREFIX($1)
	TYPE(GSYM_PREFIX($1),`function')
GSYM_PREFIX($1):')
define(`EPILOGUE',
	`
	SIZE(GSYM_PREFIX($1),.-GSYM_PREFIX($1))')
define(`L',LSYM_PREFIX`$1')
define(`INT32',
	`
	ALIGN(4)
$1:
	W32	$2
	')
define(`INT64',
	`
	ALIGN(8)
$1:
	W32	$2
	W32	$3
	')

ifdef(`MPN_PREFIX',,
  `define(`MPN_PREFIX',`__mpn_')')

define(`mpn_copyi', MPN_PREFIX`copyi')
define(`mpn_copyd', MPN_PREFIX`copyd')
define(`mpn_add_n', MPN_PREFIX`add_n')
define(`mpn_add_nc', MPN_PREFIX`add_nc')
define(`mpn_sub_n', MPN_PREFIX`sub_n')
define(`mpn_sub_nc', MPN_PREFIX`sub_nc')
define(`mpn_mul_1', MPN_PREFIX`mul_1')
define(`mpn_mul_1c', MPN_PREFIX`mul_1c')
define(`mpn_addmul_1', MPN_PREFIX`addmul_1')
define(`mpn_addmul_1c', MPN_PREFIX`addmul_1c')
define(`mpn_submul_1', MPN_PREFIX`submul_1')
define(`mpn_submul_1c', MPN_PREFIX`submul_1c')
define(`mpn_lshift', MPN_PREFIX`lshift')
define(`mpn_lshiftc', MPN_PREFIX`lshiftc')
define(`mpn_rshift', MPN_PREFIX`rshift')
define(`mpn_rshiftc', MPN_PREFIX`rshiftc')
define(`mpn_mul_basecase', MPN_PREFIX`mul_basecase')
define(`mpn_sqr_basecase', MPN_PREFIX`sqr_basecase')
define(`mpn_divrem_1', MPN_PREFIX`divrem_1')


dnl  Usage: m4_log2(x)
dnl  Calculate a logarithm to base 2.
dnl  x must be an integral power of 2, and no bigger than 2^31.

define(m4_log2,
`m4_log2_try(0,`$1')')

define(m4_log2_try,
`ifelse(eval((1<<$1) == `$2'),1,$1,
`ifelse(eval($1<31),1,
`m4_log2_try(incr($1),`$2')',
`m4_error(`m4_log2() argument too big or not a power of two: $2
')0')')')

divert`'dnl
