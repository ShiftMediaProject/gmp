/* Exercise various mpf functions.

Copyright 2000, 2001 Free Software Foundation, Inc.

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

#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "tests.h"

#define SGN(x)       ((x) < 0 ? -1 : (x) == 0 ? 0 : 1)


void
check_mpf_set_si (void)
{
  static const struct {
    long       x;
    mp_size_t  want_size;
    mp_limb_t  want_limb;
  } data[] = {

    {  0L,  0 },
    {  1L,  1, 1 },
    { -1L, -1, 1 },

    {  LONG_MAX,  1, LONG_MAX },
    { -LONG_MAX, -1, LONG_MAX },

    { LONG_HIGHBIT, -1, ULONG_HIGHBIT },
  };

  mpf_t  x;
  int    i;

  for (i = 0; i < numberof (data); i++)
    {
      mpf_init (x);
      mpf_set_si (x, data[i].x);
      MPF_CHECK_FORMAT (x);
      if (x->_mp_size != data[i].want_size
          || (x->_mp_size != 0
              && (x->_mp_d[0] != data[i].want_limb
                  || x->_mp_exp != 1)))
        {
          printf ("mpf_set_si wrong on data[%d]\n", i); 
          abort();                                    
        }
      mpf_clear (x);

      mpf_init_set_si (x, data[i].x);
      MPF_CHECK_FORMAT (x);
      if (x->_mp_size != data[i].want_size
          || (x->_mp_size != 0
              && (x->_mp_d[0] != data[i].want_limb
                  || x->_mp_exp != 1)))
        {
          printf ("mpf_init_set_si wrong on data[%d]\n", i); 
          abort();                                    
        }
      mpf_clear (x);
    }
}


void
check_mpf_cmp_si (void)
{
  static const struct {
    int         a_base;
    const char  *a;
    const char  *b;
    int         want;
  } data[] = {
    { 10, "0",  "1", -1 },
    { 10, "0",  "0",  0 },
    { 10, "0", "-1",  1 },

    { 10, "1",  "1", 0 },
    { 10, "1",  "0", 1 },
    { 10, "1", "-1", 1 },

    { 10, "-1",  "1", -1 },
    { 10, "-1",  "0", -1 },
    { 10, "-1", "-1", 0 },

    { 16,         "0", "-0x80000000",  1 },
    { 16,  "80000000", "-0x80000000",  1 },
    { 16,  "80000001", "-0x80000000",  1 },
    { 16, "-80000000", "-0x80000000",  0 },
    { 16, "-80000001", "-0x80000000", -1 },

    { 16,                 "0", "-0x8000000000000000",  1 },
    { 16,  "8000000000000000", "-0x8000000000000000",  1 },
    { 16,  "8000000000000001", "-0x8000000000000000",  1 },
    { 16, "-8000000000000000", "-0x8000000000000000",  0 },
    { 16, "-8000000000000001", "-0x8000000000000000", -1 },
  };

  mpf_t  a;
  mpz_t  bz;
  long   b;
  int    got;
  int    i;

  mpf_init (a);
  mpz_init (bz);
  for (i = 0; i < numberof (data); i++)
    {
      mpf_set_str_or_abort (a, data[i].a, data[i].a_base);
      mpz_set_str_or_abort (bz, data[i].b, 0);

      if (mpz_fits_slong_p (bz))
        {
          b = mpz_get_si (bz);
          got = mpf_cmp_si (a, b);
          if (SGN (got) != data[i].want)
            {
              printf ("mpf_cmp_si wrong on data[%d]\n", i);
              printf ("  a="); mpf_out_str (stdout, 10, 0, a);
              printf (" (%s)\n", data[i].a);
              printf ("  b=%ld (%s)\n", b, data[i].b);
              printf ("  got=%d\n", got);
              printf ("  want=%d\n", data[i].want);
              abort();                                    
            }
        }
    }

  mpf_clear (a);
  mpz_clear (bz);
}


void
check_mpf_getset_prec (void)
{
  mpf_t  x;
  unsigned long  i, a, b;

  mpf_init (x);

  for (i = 1; i < 2000; i++)
    {
      mpf_set_prec (x, i);
      a = mpf_get_prec (x);
      mpf_set_prec (x, a);
      b = mpf_get_prec (x);
      if (a != b)
        {
          printf ("mpf_get_prec / mpf_set_prec inconsistency\n");
          printf ("   set %lu gives %lu, but then set %lu gives %lu\n",
                  i, a,
                  a, b);
          abort ();
        }
    }

  mpf_clear (x);
}

int
main (void)
{
  tests_start ();
  check_mpf_set_si ();
  check_mpf_cmp_si ();
  check_mpf_getset_prec ();
  tests_end ();
  exit (0);
}
