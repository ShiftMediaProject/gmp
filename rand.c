/* gmp_randinit (state, algorithm, ...) -- Initialize a random state.

Copyright 1999, 2000, 2001 Free Software Foundation, Inc.

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

#include <stdio.h> /* for NULL */
#if HAVE_STDARG
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#include "gmp.h"
#include "gmp-impl.h"

/* Array of CL-schemes, ordered in increasing order of the first
   member (the 'm2exp' value).  The end of the array is indicated with
   an entry containing all zeros.  */

/* All multipliers are in the range 0.01*m and 0.99*m, and are
congruent to 5 (mod 8).
They all pass the spectral test with Vt >= 2^(30/t) and merit >= 1.
(Up to and including 196 bits, merit is >= 3.)  */

struct __gmp_rand_lc_scheme_struct
{
  unsigned long int m2exp;	/* Modulus is 2 ^ m2exp. */
  const char *astr;		/* Multiplier in string form. */
  unsigned long int c;		/* Addend. */
};

const struct __gmp_rand_lc_scheme_struct __gmp_rand_lc_scheme[] =
{
  {32, "29CF535", 	     1},
  {33, "51F666D", 	     1},
  {34, "A3D73AD", 	     1},
  {35, "147E5B85", 	     1},
  {36, "28F725C5", 	     1},
  {37, "51EE3105", 	     1},
  {38, "A3DD5CDD", 	     1},
  {39, "147AF833D", 	     1},
  {40, "28F5DA175", 	     1},
  {56, "AA7D735234C0DD",  1},
  {64, "BAECD515DAF0B49D", 1},
  {100, "292787EBD3329AD7E7575E2FD", 1},	
  {128, "48A74F367FA7B5C8ACBB36901308FA85", 1},
  {156, "78A7FDDDC43611B527C3F1D760F36E5D7FC7C45", 1},
  {196, "41BA2E104EE34C66B3520CE706A56498DE6D44721E5E24F5", 1},
  {200, "4E5A24C38B981EAFE84CD9D0BEC48E83911362C114F30072C5", 1},
  {256, "AF66BA932AAF58A071FD8F0742A99A0C76982D648509973DB802303128A14CB5", 1},
  {0, NULL, 0}			/* End of array. */
};

void
#if HAVE_STDARG
gmp_randinit (gmp_randstate_t rstate,
	      gmp_randalg_t alg,
	      ...)
#else
gmp_randinit (va_alist)
     va_dcl
#endif
{
  va_list ap;

#if HAVE_STDARG
  va_start (ap, alg);

#else
  __gmp_randstate_struct *rstate;
  gmp_randalg_t alg;

  va_start (ap);
  rstate = va_arg (ap, __gmp_randstate_struct *);
  alg = va_arg (ap, gmp_randalg_t);
#endif

  switch (alg)
    {
    case GMP_RAND_ALG_LC:	/* Linear congruential.  */
      {
	unsigned long int size;
	const struct __gmp_rand_lc_scheme_struct *sp;
	mpz_t a;

	size = va_arg (ap, unsigned long int);

	/* Pick a scheme.  */
	for (sp = __gmp_rand_lc_scheme; sp->m2exp != 0; sp++)
	  if (sp->m2exp / 2 >= size)
	    break;

	if (sp->m2exp == 0)	/* Nothing big enough found.  */
	  {
	    gmp_errno |= GMP_ERROR_INVALID_ARGUMENT;
	    return;
	  }

	/* Install scheme.  */
	mpz_init_set_str (a, sp->astr, 16);
	gmp_randinit_lc_2exp (rstate, a, sp->c, sp->m2exp);
	mpz_clear (a);
	break;
      }

#if 0
    case GMP_RAND_ALG_BBS:	/* Blum, Blum, and Shub. */
      {				
	mpz_t p, q;
	mpz_t ztmp;

	/* FIXME: Generate p and q.  They must be ``large'' primes,
           congruent to 3 mod 4.  Should we ensure that they meet some
           of the criterias for being ``hard primes''?*/

	/* These are around 128 bits. */
	mpz_init_set_str (p, "148028650191182616877187862194899201391", 10); 
	mpz_init_set_str (q, "315270837425234199477225845240496832591", 10);
	
	/* Allocate algorithm specific data. */
	rstate->data.bbs = (__gmp_rand_data_bbs *)
	  (*__gmp_allocate_func) (sizeof (__gmp_rand_data_bbs));

	mpz_init (rstate->data.bbs->bi); /* The Blum integer. */
	mpz_mul (rstate->data.bbs->bi, p, q);

	/* Find a seed, x, with gcd (x, bi) == 1. */
	mpz_init (ztmp);
	while (1)
	  {
	    mpz_gcd (ztmp, seed, rstate->data.bbs->bi);
	    if (!mpz_cmp_ui (ztmp, 1))
	      break;
	    mpz_add_ui (seed, seed, 1);
	  }

	rstate->alg = alg;
	rstate->size = size;		/* FIXME: Remove. */
	mpz_set (rstate->seed, seed);

	mpz_clear (p);
	mpz_clear (q);
	mpz_clear (ztmp);
	break;
      }
#endif /* 0 */

    default:			/* Bad choice. */
      gmp_errno |= GMP_ERROR_UNSUPPORTED_ARGUMENT;
    }

  va_end (ap);
}
