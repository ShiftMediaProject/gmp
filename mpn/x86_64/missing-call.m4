dnl  AMD64 MULX/ADX simulation support, function call version.

dnl  Contributed to the GNU project by Torbjörn Granlund.

dnl  Copyright 2013 Free Software Foundation, Inc.

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


define(`adox',`
	push	$1
	push	$2
	call	__gmp_adox
	pop	$2
')

define(`adcx',`
	push	$1
	push	$2
	call	__gmp_adcx
	pop	$2
')

define(`mulx',`
	push	$1
	call	__gmp_mulx
	pop	$2
	pop	$3
')
