/* Exercise mpz_get_si. */

/*
Copyright 2000 Free Software Foundation, Inc.

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

int
main (void)
{
  static const struct {
    const char  *n;
    long        want;

  } data[] = {

    { "0",      0L },
    { "1",      1L },
    { "-1",     -1L },
    { "2",      2L },
    { "-2",     -2L },
    { "12345",  12345L },
    { "-12345", -12345L },

    /* The -0x100000000 case doesn't fit in a long and the result from
       mpz_get_si() is undefined, but -0x80000000 is what comes out
       currently, and it should be that value irrespective of the size of
       mp_limb_t size (long or long long).  */
#if BITS_PER_LONGINT==32
    {  " 0x7FFFFFFF",  0x7FFFFFFFL },
    {  "-0x80000000", -0x80000000L },
    { "-0x100000000", -0x80000000L },

#else
#if BITS_PER_LONGINT==64
    {  " 0x7FFFFFFFFFFFFFFF",  0x7FFFFFFFFFFFFFFFL },
    {  "-0x8000000000000000", -0x8000000000000000L },
    { "-0x10000000000000000", -0x8000000000000000L },

#else
    Unrecognised BITS_PER_LONGINT
#endif
#endif
  };

  int    i;
  mpz_t  n;
  long   got;

  mpz_init (n);
  for (i = 0; i < numberof (data); i++)
    {
      MPZ_SET_STR_OR_ABORT (n, data[i].n, 0);

      got = mpz_get_si (n);
      if (got != data[i].want)
        {
          printf ("mpz_get_si wrong at data[%d]\n", i); 
          printf ("   n     \"%s\" (", data[i].n);
          mpz_out_str (stdout, 10, n); printf (", hex ");
          mpz_out_str (stdout, 16, n); printf (")\n");
          printf ("   got   %ld (0x%lX)\n", got, got);
          printf ("   want  %ld (0x%lX)\n", data[i].want, data[i].want);
          abort();                                    
        }
    }
  mpz_clear (n);

  exit (0);
}
