/* __gmp_doprnt_integer_cxx -- integer style formatted output, for C++.

   THE FUNCTIONS IN THIS FILE ARE FOR INTERNAL USE ONLY.  THEY'RE ALMOST
   CERTAIN TO BE SUBJECT TO INCOMPATIBLE CHANGES OR DISAPPEAR COMPLETELY IN
   FUTURE GNU MP RELEASES.

Copyright 2001 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */


/* __gmp_doprnt_integer, compiled by C++ under a new name.

   The same object code can't be shared by C and C++ since, in g++ 3.0 at
   least, the exception support in the C++ version will require libstdc++
   and we don't want to demand that for plain C.

   __gmp_doprnt_integer could inspect ios::flags() directly, rather than via
   doprnt_params_t.  This wouldn't be hard, and in fact some code along
   those lines exists, but for now it's considered better to share the
   sources for all formatted output, so that changes or fixes can be applied
   more easily.

   Perhaps a scheme of macros could eliminate doprnt_params_t from the C++
   code, but it's really not too bad as it stands.  */


#define __gmp_doprnt_integer  __gmp_doprnt_integer_cxx
#include "doprnti.c"
