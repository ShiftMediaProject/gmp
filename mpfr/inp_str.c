/* mpf_inp_str(dest_float, stream, base) -- Input a number in base
   BASE from stdio stream STREAM and store the result in DEST_FLOAT.

Copyright 1999, 2001 Free Software Foundation, Inc.
(Copied from GMP, file mpf/inp_str.c)

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include <ctype.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

size_t
mpfr_inp_str (mpfr_ptr rop, FILE *stream, int base, mp_rnd_t rnd_mode)
{
  char *str;
  size_t alloc_size, str_size;
  int c;
  size_t retval;
  size_t nread;

  MPFR_CLEAR_FLAGS(rop); 
  if (stream == 0)
    stream = stdin;

  alloc_size = 100;
  str = (char *) (*__gmp_allocate_func) (alloc_size);
  str_size = 0;
  nread = 0;

  /* Skip whitespace.  */
  do
    {
      c = getc (stream);
      nread++;
    }
  while (isspace (c));

  for (;;)
    {
      if (str_size >= alloc_size)
	{
	  size_t old_alloc_size = alloc_size;
	  alloc_size = alloc_size * 3 / 2;
	  str = (char *) (*__gmp_reallocate_func) (str, old_alloc_size, alloc_size);
	}
      if (c == EOF || isspace (c))
	break;
      str[str_size++] = c;
      c = getc (stream);
    }
  ungetc (c, stream);

  if (str_size >= alloc_size)
    {
      size_t old_alloc_size = alloc_size;
      alloc_size = alloc_size * 3 / 2;
      str = (char *) (*__gmp_reallocate_func) (str, old_alloc_size, alloc_size);
    }
  str[str_size] = 0;

  retval = mpfr_set_str (rop, str, base, rnd_mode);
  (*__gmp_free_func) (str, alloc_size);

  if (retval == -1)
    return 0;			/* error */

  return str_size + nread;
}
