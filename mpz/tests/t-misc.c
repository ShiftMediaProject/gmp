/* Exercise various mpz functions. */

/*
Copyright (C) 2000 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA.
*/

#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"


#define numberof(x)  (sizeof (x) / sizeof ((x)[0]))

#ifndef BITS_PER_LONGINT
#define BITS_PER_LONGINT  (sizeof(long) * 8)
#endif

#define ULONG_HIGHBIT  ((unsigned long) 1 << (BITS_PER_LONGINT-1))
#define LONG_HIGHBIT   ((long) ULONG_HIGHBIT)

#ifndef LONG_MAX
#define LONG_MAX       ((long) (ULONG_HIGHBIT-1))
#endif


void
bin (void)
{
  static const struct {
    const char     *n;
    unsigned long  k;
    const char     *want;
  } data[] = {
    {   "0",  0, "1"   },
    {   "0",  1, "0"   },
    {   "0",  2, "0"   },
    {   "0",  3, "0"   },
    {   "0",  4, "0"   },
    {   "0", 123456, "0" },

    {   "1",  0, "1"   },
    {   "1",  1, "1"   },
    {   "1",  2, "0"   },
    {   "1",  3, "0"   },
    {   "1",  4, "0"   },
    {   "1", 123456, "0" },

    {   "2",  0, "1"   },
    {   "2",  1, "2"   },
    {   "2",  2, "1"   },
    {   "2",  3, "0"   },
    {   "2",  4, "0"   },
    {   "2", 123456, "0" },

    {   "3",  0, "1"   },
    {   "3",  1, "3"   },
    {   "3",  2, "3"   },
    {   "3",  3, "1"   },
    {   "3",  4, "0"   },
    {   "3",  5, "0"   },
    {   "3", 123456, "0" },

    {   "4",  0, "1"   },
    {   "4",  1, "4"   },
    {   "4",  2, "6"   },
    {   "4",  3, "4"   },
    {   "4",  4, "1"   },
    {   "4",  5, "0"   },
    {   "4",  6, "0"   },
    {   "4", 123456, "0" },

    {   "10",  0, "1"   },
    {   "10",  1, "10"  },
    {   "10",  2, "45"  },
    {   "10",  3, "120" },
    {   "10",  4, "210" },
    {   "10",  5, "252" },
    {   "10",  6, "210" },
    {   "10",  7, "120" },
    {   "10",  8, "45"  },
    {   "10",  9, "10"  },
    {   "10", 10, "1"   },
    {   "10", 11,     "0" },
    {   "10", 12,     "0" },
    {   "10", 123456, "0" },

    /* negatives, using bin(-n,k)=bin(n+k-1,k) */
    {   "-1",  0,  "1"  },
    {   "-1",  1, "-1"  },
    {   "-1",  2,  "1"  },
    {   "-1",  3, "-1"  },
    {   "-1",  4,  "1"  },

    {   "-2",  0,  "1"  },
    {   "-2",  1, "-2"  },
    {   "-2",  2,  "3"  },
    {   "-2",  3, "-4"  },
    {   "-2",  4,  "5"  },
    {   "-2",  5, "-6"  },
    {   "-2",  6,  "7"  },

    {   "-3",  0,   "1"  },
    {   "-3",  1,  "-3"  },
    {   "-3",  2,   "6"  },
    {   "-3",  3, "-10"  },
    {   "-3",  4,  "15"  },
    {   "-3",  5, "-21"  },
    {   "-3",  6,  "28"  },
  };

  mpz_t  n, want, got;
  int    i;

  mpz_init (n);
  mpz_init (want);
  mpz_init (got);

  for (i = 0; i < numberof (data); i++)
    {
      mpz_set_str (n, data[i].n, 0);
      mpz_set_str (want, data[i].want, 0);

      mpz_bin_ui (got, n, data[i].k);
      if (mpz_cmp (got, want) != 0)
        {
          printf ("mpz_bin_ui wrong\n");
          printf ("  n="); mpz_out_str (stdout, 10, n); printf ("\n");
          printf ("  k=%lu\n", data[i].k);
          printf ("  got="); mpz_out_str (stdout, 10, got); printf ("\n");
          printf ("  want="); mpz_out_str (stdout, 10, want); printf ("\n");
          abort();                                    
        }

      if (mpz_fits_ulong_p (n))
        {
          mpz_bin_uiui (got, mpz_get_ui (n), data[i].k);
          if (mpz_cmp (got, want) != 0)
            {
              printf ("mpz_bin_uiui wrong\n");
              printf ("  n=%lu\n", mpz_get_ui (n));
              printf ("  k=%lu\n", data[i].k);
              printf ("  got="); mpz_out_str (stdout, 10, got); printf ("\n");
              printf ("  want="); mpz_out_str (stdout, 10, want); printf ("\n");
              abort();               
            }                     
        }
    }

  mpz_clear (n);
}


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
    { 0L,  0 },
    { 1L,  1, 1 },
    { -1L, -1, 1 },
    { LONG_MAX, 1, LONG_MAX },
    { -LONG_MAX, -1, LONG_MAX },
    { LONG_HIGHBIT, -1, ULONG_HIGHBIT },
  };

  mpz_t  n;
  int    i;

  mpz_init (n);
  for (i = 0; i < numberof (data); i++)
    {
      mpz_set_si (n, data[i].n);

      if (n->_mp_size != data[i].want_size
          || (n->_mp_size != 0 && n->_mp_d[0] != data[i].want_limb))
        {
          printf ("mpz_set_si wrong on data[%d]\n", i); 
          abort();                                    
        }
    }

  mpz_clear (n);
}


int
main (void)
{
  bin ();
  oddeven ();
  check_mpz_set_si ();

  exit (0);
}
