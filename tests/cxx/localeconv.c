/* Manipulable localeconv.

Copyright 2001, 2002 Free Software Foundation, Inc.

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

#include "config.h"

#if HAVE_LOCALE_H
#include <locale.h>    /* for lconv */
#endif


/* Replace the libc localeconv with one we can manipulate.

   This is done in a C file since if it was in a C++ file then we'd have to
   match the "throw" or lack thereof declared for localeconv in <locale.h>.
   g++ 3.2 gives an error about mismatched throws under "-pedantic", other
   C++ compilers very possibly do so too.  */

#if HAVE_LOCALECONV

char *decimal_point;

struct lconv *
localeconv (void)
{
  static struct lconv  l;
  l.decimal_point = decimal_point;
  return &l;
}

#endif
