/* mpq_out_str(stream,base,integer) */

/*
Copyright (C) 2000 Free Software Foundation, Inc.

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
MA 02111-1307, USA.
*/

#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"


size_t
#if __STDC__
mpq_out_str (FILE *stream, int base, mpq_srcptr q)
#else
mpq_out_str (stream, base, q)
     FILE       *stream;
     int        base;
     mpq_srcptr q;
#endif
{
  size_t  written;

  if (stream == NULL)
    stream = stdout;

  written = mpz_out_str (stream, base, mpq_numref (q));

  if (mpz_cmp_ui (mpq_denref (q), 1) != 0)
    {
      putc ('/', stream);
      written += 1 + mpz_out_str (stream, base, mpq_denref (q));
    }

  return ferror (stream) ? 0 : written;
}
