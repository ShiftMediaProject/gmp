divert(-1)

dnl  m4 macros for ARM assembler.

dnl  Copyright 2001 Free Software Foundation, Inc.
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


dnl  Standard commenting is with @, the default m4 # is for constants and we
dnl  don't want to disable macro expansions in or after them.

changecom(@)


dnl  APCS register names.

define(a1,r0)
define(a2,r1)
define(a3,r2)
define(a4,r3)
define(v1,r4)
define(v2,r5)
define(v3,r6)
define(v4,r7)
define(v5,r8)
define(v6,r9)
define(sb,r9)
define(v7,r10)
define(sl,r10)
define(fp,r11)
define(ip,r12)
define(sp,r13)
define(lr,r14)
define(pc,r15)

divert
