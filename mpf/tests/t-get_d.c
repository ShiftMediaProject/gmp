/* Test mpf_set_d.

   Copyright (C) 1996, 1999, 2000 Free Software Foundation, Inc.

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
   MA 02111-1307, USA. */

#include "gmp.h"

#ifdef __vax__
#define LOW_BOUND 1e-38
#define HIGH_BOUND 8e37
#else
#define LOW_BOUND 1e-300
#define HIGH_BOUND 1e300
#endif

int
#if __STDC__
main (int argc, char **argv)
#else
main (argc, argv)
     int  argc;
     char **argv;
#endif
{
  double d, e, r;
  mpf_t u, v;

  mpf_init (u);
  mpf_init (v);

  mpf_set_d (u, LOW_BOUND);
  for (d = 2.0 * LOW_BOUND; d < HIGH_BOUND; d *= 1.01)
    {
      mpf_set_d (v, d);
      if (mpf_cmp (u, v) >= 0)
	abort ();
      e = mpf_get_d (v);
      r = e/d;
      if (r < 0.999999999999999 | r > 1.000000000000001)
	abort ();
      mpf_set (u, v);
    }

  exit (0);
}
