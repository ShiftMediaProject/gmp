/* mtox -- Convert OPERAND to hexadecimal and return a malloc'ed string
   with the result of the conversion.

Copyright 1991, 1994, 2000, 2001, 2002 Free Software Foundation, Inc.

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

#include "mp.h"
#include "gmp.h"
#include "gmp-impl.h"

char *
mtox (const MINT *x)
{
  mp_size_t xsize = x->_mp_size;
  mp_size_t xsign;
  unsigned char *str, *s;
  size_t str_size, alloc_size, i;
  int zeros;

  if (xsize == 0)
    {
      str = (unsigned char *) (*__gmp_allocate_func) (2);
      str[0] = '0';
      str[1] = 0;
      return (char *) str;
    }
  xsign = xsize;
  if (xsize < 0)
    xsize = -xsize;

  MPN_GET_STR_SIZE (alloc_size, 16, xsize);
  alloc_size += 2; /* '\0' and possible '-' */
  str = (unsigned char *) (*__gmp_allocate_func) (alloc_size);
  s = str;

  if (xsign < 0)
    *s++ = '-';

  str_size = mpn_get_str (s, 16, PTR(x), xsize);

  /* mpn_get_str might make some leading zeros.  Skip them.  */
  for (zeros = 0; s[zeros] == 0; zeros++)
    str_size--;

  /* Translate to printable chars and move string down.  */
  for (i = 0; i < str_size; i++)
    s[i] = "0123456789abcdef"[s[zeros + i]];
  s[str_size] = 0;

  str_size += 1 + (s - str);
  ASSERT (str_size == strlen (str) + 1);
  __GMP_REALLOCATE_FUNC_MAYBE_TYPE (str, alloc_size, str_size, unsigned char);
  return (char *) str;
}
