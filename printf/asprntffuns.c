/* __gmp_asprintf_memory etc -- formatted output to allocated space.

Copyright 2001 Free Software Foundation, Inc.

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


/* These routines are provided separately to let mpz_out_ostream,
   mpq_out_ostream and mpf_out_ostream avoid dragging vsnprintf into the
   link via __gmp_asprintf_format.

   These routines are only needed to support gmp_asprintf if vsnprintf
   exists, or to support mpz_out_ostream if C++ has been enabled.  (It'd be
   a pretty strange C++ environment which didn't have vsnprintf, so testing
   WANT_CXX probably doesn't change anything.)  */

#include "config.h"

#if HAVE_VSNPRINTF || WANT_CXX

#if HAVE_STDARG
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gmp.h"
#include "gmp-impl.h"


int
__gmp_asprintf_memory (struct gmp_asprintf_t *d, const char *str, size_t len)
{
  GMP_ASPRINTF_T_NEED (d, len);
  memcpy (d->buf + d->size, str, len);
  d->size += len;
  return len;  
}

int
__gmp_asprintf_reps (struct gmp_asprintf_t *d, int c, int reps)
{
  GMP_ASPRINTF_T_NEED (d, reps);
  memset (d->buf + d->size, c, reps);
  d->size += reps;
  return reps;
}

int
__gmp_asprintf_final (struct gmp_asprintf_t *d)
{
  char  *buf = d->buf;
  ASSERT (d->alloc >= d->size + 1);
  buf[d->size] = '\0';
  __GMP_REALLOCATE_FUNC_MAYBE (buf, d->alloc, d->size+1);
  *d->result = buf;
  return 0;
}

#endif /* HAVE_VSNPRINTF || WANT_CXX */
