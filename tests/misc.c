/* Miscellaneous test program support routines. */

/*
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
MA 02111-1307, USA.
*/

#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "tests.h"


/* The various tests setups and final checks, collected up together. */
void
tests_start (void)
{
  tests_memory_start ();
}
void
tests_end (void)
{
  tests_memory_end ();
}


/* Only used if CPU calling conventions checking is available. */
mp_limb_t (*calling_conventions_function) _PROTO ((ANYARGS));


/* Return p advanced to the next multiple of "align" bytes.  "align" must be
   a power of 2.  Care is taken not to assume sizeof(int)==sizeof(pointer).
   Using "unsigned long" avoids a warning on hpux.  */
void *
align_pointer (void *p, size_t align)
{
  unsigned long  d;
  d = ((unsigned long) p) & (align-1);
  d = (d != 0 ? align-d : 0);
  return (void *) (((char *) p) + d);
}


/* Note that memory allocated with this function can never be freed, because
   the start address of the block allocated is lost. */
void *
__gmp_allocate_func_aligned (size_t bytes, size_t align)
{
  return align_pointer ((*__gmp_allocate_func) (bytes + align-1), align);
}


void *
__gmp_allocate_or_reallocate (void *ptr, size_t oldsize, size_t newsize)
{
  if (ptr == NULL)
    return (*__gmp_allocate_func) (newsize);
  else
    return (*__gmp_reallocate_func) (ptr, oldsize, newsize);
}


void
mpz_set_n (mpz_ptr z, mp_srcptr p, mp_size_t size)
{
  ASSERT (size >= 0);
  MPN_NORMALIZE (p, size);
  MPZ_REALLOC (z, size);
  MPN_COPY (PTR(z), p, size);
  SIZ(z) = size;
}

void
mpz_init_set_n (mpz_ptr z, mp_srcptr p, mp_size_t size)
{
  ASSERT (size >= 0);

  MPN_NORMALIZE (p, size);
  ALLOC(z) = MAX (size, 1);
  PTR(z) = __GMP_ALLOCATE_FUNC_LIMBS (ALLOC(z));
  SIZ(z) = size;
  MPN_COPY (PTR(z), p, size);
}


/* Find least significant limb position where p1,size and p2,size differ.  */
mp_size_t
mpn_diff_lowest (mp_srcptr p1, mp_srcptr p2, mp_size_t size)
{
  mp_size_t  i;

  for (i = 0; i < size; i++)
    if (p1[i] != p2[i])
      return i;

  /* no differences */
  return -1;
}


/* Find most significant limb position where p1,size and p2,size differ.  */
mp_size_t
mpn_diff_highest (mp_srcptr p1, mp_srcptr p2, mp_size_t size)
{
  mp_size_t  i;

  for (i = size-1; i >= 0; i--)
    if (p1[i] != p2[i])
      return i;

  /* no differences */
  return -1;
}


void
mpz_set_str_or_abort (mpz_ptr z, const char *str, int base)
{
  if (mpz_set_str (z, str, base) != 0)
    {
      fprintf (stderr, "ERROR: mpz_set_str failed\n");
      fprintf (stderr, "   str  = \"%s\"\n", str);
      fprintf (stderr, "   base = %d\n", base);
      abort();
    }
}

void
mpf_set_str_or_abort (mpf_ptr f, const char *str, int base)
{
  if (mpf_set_str (f, str, base) != 0)
    {
      fprintf (stderr, "ERROR mpf_set_str failed\n");
      fprintf (stderr, "   str  = \"%s\"\n", str);
      fprintf (stderr, "   base = %d\n", base);
      abort();
    }
}
