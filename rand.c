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

  switch (alg) {
  case GMP_RAND_ALG_LC:
    if (! gmp_randinit_lc_2exp_size (rstate, va_arg (ap, unsigned long)))
      gmp_errno |= GMP_ERROR_INVALID_ARGUMENT;
    break;
  default:
    gmp_errno |= GMP_ERROR_UNSUPPORTED_ARGUMENT;
    break;
  }
  va_end (ap);
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
