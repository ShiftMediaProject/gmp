/* Exercise various mpz functions. */

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

#define SGN(x)       ((x) < 0 ? -1 : (x) == 0 ? 0 : 1)


void
oddeven (void)
{
  static const struct {
    const char  *n;
    int          odd, even;
  } data[] = {
    {   "0", 0, 1 },
    {   "1", 1, 0 },
    {   "2", 0, 1 },
    {   "3", 1, 0 },
    {   "4", 0, 1 },

    {  "-4", 0, 1 },
    {  "-3", 1, 0 },
    {  "-2", 0, 1 },
    {  "-1", 1, 0 },

    {  "0x1000000000000000000000000000000000000000000000000000", 0, 1 },
    {  "0x1000000000000000000000000000000000000000000000000001", 1, 0 },
    {  "0x1000000000000000000000000000000000000000000000000002", 0, 1 },
    {  "0x1000000000000000000000000000000000000000000000000003", 1, 0 },

    { "-0x1000000000000000000000000000000000000000000000000004", 0, 1 },
    { "-0x1000000000000000000000000000000000000000000000000003", 1, 0 },
    { "-0x1000000000000000000000000000000000000000000000000002", 0, 1 },
    { "-0x1000000000000000000000000000000000000000000000000001", 1, 0 },
  };

  mpz_t  n;
  int    i;

  mpz_init (n);
  for (i = 0; i < numberof (data); i++)
    {
      mpz_set_str (n, data[i].n, 0);

      if ((mpz_odd_p (n) != 0) != data[i].odd)
        {
          printf ("mpz_odd_p wrong on data[%d]\n", i); 
          abort();                                    
        }

      if ((mpz_even_p (n) != 0) != data[i].even)
        {
          printf ("mpz_even_p wrong on data[%d]\n", i); 
          abort();                                    
        }
    }

  mpz_clear (n);
}


void
check_mpz_set_si (void)
{
  static const struct {
    long       n;
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

  mpz_t  n;
  int    i;

  mpz_init (n);
  for (i = 0; i < numberof (data); i++)
    {
      mpz_init (n);
      mpz_set_si (n, data[i].n);
      if (n->_mp_size != data[i].want_size
          || (n->_mp_size != 0 && n->_mp_d[0] != data[i].want_limb))
        {
          printf ("mpz_set_si wrong on data[%d]\n", i); 
          abort();                                    
        }
      mpz_clear (n);

      mpz_init_set_si (n, data[i].n);
      if (n->_mp_size != data[i].want_size
          || (n->_mp_size != 0 && n->_mp_d[0] != data[i].want_limb))
        {
          printf ("mpz_init_set_si wrong on data[%d]\n", i); 
          abort();                                    
        }
      mpz_clear (n);
    }
}


void
check_mpz_cmp_si (void)
{
  static const struct {
    const char  *a, *b;
    int         want;
  } data[] = {
    { "0",  "1", -1 },
    { "0",  "0",  0 },
    { "0", "-1",  1 },

    { "1",  "1", 0 },
    { "1",  "0", 1 },
    { "1", "-1", 1 },

    { "-1",  "1", -1 },
    { "-1",  "0", -1 },
    { "-1", "-1", 0 },

    {           "0", "-0x80000000",  1 },
    {  "0x80000000", "-0x80000000",  1 },
    {  "0x80000001", "-0x80000000",  1 },
    { "-0x80000000", "-0x80000000",  0 },
    { "-0x80000001", "-0x80000000", -1 },

    {                   "0", "-0x8000000000000000",  1 },
    {  "0x8000000000000000", "-0x8000000000000000",  1 },
    {  "0x8000000000000001", "-0x8000000000000000",  1 },
    { "-0x8000000000000000", "-0x8000000000000000",  0 },
    { "-0x8000000000000001", "-0x8000000000000000", -1 },
  };

  mpz_t  a, bz;
  long   b;
  int    got;
  int    i;

  mpz_init (a);
  mpz_init (bz);
  for (i = 0; i < numberof (data); i++)
    {
      mpz_set_str (a, data[i].a, 0);
      mpz_set_str (bz, data[i].b, 0);

      if (mpz_fits_slong_p (bz))
        {
          b = mpz_get_si (bz);
          got = mpz_cmp_si (a, b);
          if (SGN (got) != data[i].want)
            {
              printf ("mpz_cmp_si wrong on data[%d]\n", i);
              printf ("  a="); mpz_out_str (stdout, 10, a); printf ("\n");
              printf ("  b=%ld\n", b);
              printf ("  got=%d\n", got);
              printf ("  want=%d\n", data[i].want);
              abort();                                    
            }
        }
    }

  mpz_clear (a);
  mpz_clear (bz);
}


int
main (void)
{
  oddeven ();
  check_mpz_set_si ();
  check_mpz_cmp_si ();

  exit (0);
}
