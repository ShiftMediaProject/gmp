/* Test mpf_set_si and mpf_init_set_si.

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

void
check_data (void)
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

int
main (void)
{
#if GMP_NAIL_BITS == 0		/* bogus test criteria cause nails to fail */
  tests_start ();

  check_data ();

  tests_end ();
#endif
  exit (0);
}
