/* gmp_vasprintf -- formatted output to an allocated space.

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

#include "config.h"

#if HAVE_VSNPRINTF

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


/* "buf" is a __gmp_allocate_func block of "alloc" many bytes.  The first
   "size" of these have been written.  "alloc > size" is maintained, so
   there's room to store a '\0' at the end.  "result" is where the
   application wants the final block pointer.  */

struct gmp_asprintf_t {
  char    **result;
  char    *buf;
  size_t  size;
  size_t  alloc;
};


/* If a realloc is necessary, use twice the size actually required, so as to
   avoid repeated small reallocs.  */

#define NEED(n)                                                         \
  do {                                                                  \
    size_t  alloc, newsize, newalloc;                                   \
    ASSERT (d->alloc >= d->size + 1);                                   \
                                                                        \
    alloc = d->alloc;                                                   \
    newsize = d->size + (n);                                            \
    if (alloc <= newsize)                                               \
      {                                                                 \
        newalloc = 2*newsize;                                           \
        d->alloc = newalloc;                                            \
        d->buf = (__gmp_reallocate_func) (d->buf, alloc, newalloc);     \
      }                                                                 \
  } while (0)


/* vasprintf isn't used since we prefer all GMP allocs to go through
   __gmp_allocate_func, and in particular we don't want the -1 return from
   vasprintf for out-of-memory, instead __gmp_allocate_func should handle
   that.  Using vsnprintf unfortunately means we might have to re-run it if
   our current space is insufficient.

   The initial guess for the needed space is an arbitrary 256 bytes.  If
   that (and any extra NEED might give) isn't enough then an ISO C99
   standard vsnprintf will tell us what we really need.

   GLIBC 2.0.x vsnprintf returns either -1 or space-1 to indicate overflow,
   without giving any indication how much is really needed.  In this case
   keep trying with double the space each time.

   A return of space-1 is success on a C99 vsnprintf, but we're not
   bothering to identify which we've got, so just take the pessimistic
   option and assume it's glibc 2.0.x.

   Notice the use of ret+2 for the new space in the C99 case.  This ensures
   the next vsnprintf return value will be space-2, which is unambiguously
   successful.  But actually NEED() will realloc to even bigger than that
   ret+2.  */

static int
gmp_asprintf_format (struct gmp_asprintf_t *d, const char *fmt, va_list ap)
{
  int     ret;
  size_t  space = 256;

  for (;;)
    {
      NEED (space);
      space = d->alloc - d->size;
      ret = vsnprintf (d->buf + d->size, space, fmt, ap);
      if (ret == -1)
        {
          ASSERT (strlen (d->buf + d->size) == space-1);
          ret = space-1;
        }

      /* done if output fits in our space */
      if (ret < space-1)
        break;

      if (ret == space-1)
        space *= 2;     /* possible glibc 2.0.x, so double */
      else
        space = ret+2;  /* C99, so now know space required */
    }

  d->size += ret;
  return ret;
}

static int
gmp_asprintf_memory (struct gmp_asprintf_t *d, const char *str, size_t len)
{
  NEED (len);
  memcpy (d->buf + d->size, str, len);
  d->size += len;
  return len;  
}

static int
gmp_asprintf_reps (struct gmp_asprintf_t *d, int c, int reps)
{
  NEED (reps);
  memset (d->buf + d->size, c, reps);
  d->size += reps;
  return reps;
}

static int
gmp_asprintf_final (struct gmp_asprintf_t *d, int c, int reps)
{
  char  *buf = d->buf;
  ASSERT (d->alloc >= d->size + 1);
  buf[d->size] = '\0';
  __GMP_REALLOCATE_FUNC_MAYBE (buf, d->alloc, d->size+1);
  *d->result = buf;
  return 0;
}

static const struct doprnt_funs_t  gmp_asprintf_funs = {
  (doprnt_format_t) gmp_asprintf_format,
  (doprnt_memory_t) gmp_asprintf_memory,
  (doprnt_reps_t)   gmp_asprintf_reps,
  (doprnt_final_t)  gmp_asprintf_final
};


int
gmp_vasprintf (char **result, const char *fmt, va_list ap)
{
  struct gmp_asprintf_t  d;
  d.result = result;
  d.alloc = 256;
  d.buf = (__gmp_allocate_func) (d.alloc);
  d.size = 0;
  return __gmp_doprnt (&gmp_asprintf_funs, &d, fmt, ap);
}

#endif /* HAVE_VSNPRINTF */
