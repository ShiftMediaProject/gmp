/* gmp_rand_init (state, alg, size, seed) -- Initialize a random state.

Copyright (C) 1999 Free Software Foundation, Inc.

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

#include <stdlib.h>		/* FIXME: For malloc(). */
#include "gmp.h"
#include "gmp-impl.h"

/* Array of CL-schemes, ordered in increasing order for the first
   member (the 'bits' value).  The 'm' entry is converted by
   mpz_set_str() with BASE=0.  End of array is indicated with an entry
   containing all zeros. */
static __gmp_rand_lc_scheme_struct __gmp_rand_scheme[] =
{
  /* FIXME: Remove. */
  {8, "7", 0, "256"},		/* Test. */
  {31,				/* fbsd rand(3) */
   "1103515245",		/* a (multiplier) */
   12345,			/* c (adder) */
   "0x80000000"},		/* m (modulo) = 2^31 */

  /*  {31, "22298549", 	     1, "0x80000000"},*/
				/* merit >= 1; no merit > 3 up to 32845469 */

  /* The following multipliers are all between 0.01m and 0.99m, and
     are congruent to 5 (mod 8).  They all pass the spectral test with
     Vt >= 2^(30/t) and merit >= 0.1.

     Up to and including 40 bits, merit >= 3.
     Up to and including 56 bits, merit >= 2.
     Up to and including 64 bits, merit >= 1.  */

  {32, "43840821", 	     1, "0x100000000"},
  {33, "85943917", 	     1, "0x200000000"},
  {34, "171799469", 	     1, "0x400000000"},
  {35, "343825285", 	     1, "0x800000000"},
  {36, "687285701", 	     1, "0x1000000000"},
  {37, "1374564613", 	     1, "0x2000000000"},
  {38, "2749193437", 	     1, "0x4000000000"},
  {39, "5497652029", 	     1, "0x8000000000"},
  {40, "10995212661", 	     1, "0x10000000000"},
  {56, "720575942521269",    1, "0x100000000000000"},
				/* no merit >3 up to 720575942889605 */
  {64, "184467440744758277", 1, "0x10000000000000000"},
				/* no merit >2 up to 184467440750242261 */

  {100, "12676506002282294015055173877", 1, "0x10000000000000000000000000"},	
				/* low merit2; (12676506002282294015146266925) */
  {128, "3402823669209384634633746074363516093",
   1, "0x100000000000000000000000000000000"},
				/* low merit2+3; (3402823669209384634633746074396970501) */
  {156, "913438523331814323877303020447676887284958173",
   1, "0x1000000000000000000000000000000000000000"},
				/* low merit* */
  {196, "1004336277661868922213726307713226626576376871114287459829",
   1, "0x10000000000000000000000000000000000000000000000000"},
				/* low merit*; (1004336277661868922213726307713226626576376871114345439621) */
  {200, "16069380442589902755419620923411626025222029937827928353349",
   1, "0x100000000000000000000000000000000000000000000000000"},
				/* low merit*; */
  {256, "1157920892373161954235709850086879078532699846656405640394575840079131296733",
   1, "0x10000000000000000000000000000000000000000000000000000000000000000"},
				/* low merit*; */

  {0, NULL,  0, NULL}		/* End of array. */
};

/* gmp_rand_init() -- Initialize a gmp_rand_state.  Return 0 on
   success and 1 on failure. */

int
#if __STDC__
gmp_rand_init (gmp_rand_state s,
	       gmp_rand_algorithm alg,
	       unsigned long int size,
	       mpz_t seed)
#else
gmp_rand_init (s, alg, size, seed)
     gmp_rand_state s;
     gmp_rand_algorithm alg;
     unsigned long int size;
     mpz_t seed;
#endif
{
  switch (alg)
    {
    case GMP_RAND_ALG_LC:	/* Linear congruental. */
      {
	__gmp_rand_lc_scheme_struct *sp;
	mpz_t a, m;


      /* Pick a scheme. */
	for (sp = __gmp_rand_scheme; sp->bits != 0; sp++)
	  if (sp->bits >= size)
	    break;
	if (!sp->bits)		/* Nothing big enough found. */
	  sp--;			/* Use largest available. */

      /* Install scheme. */
	mpz_init_set_str (a, sp->astr, 0);
	mpz_init_set_str (m, sp->mstr, 0);
	gmp_rand_init_lc (s, sp->bits, seed, a, sp->c, m);
	
	mpz_clear (a);
	mpz_clear (m);

	break;
      }

    case GMP_RAND_ALG_BBS:	/* Blum, Blum, and Shub */
      {				
	mpz_t p, q;		/* FIXME: Ok for all compilers? */
	mpz_t ztmp;

	/* FIXME: Generate p and q.  They must be ``large'' primes,
           congruent to 3 mod 4.  Should we ensure that they meet some
           of the criterias for being ``hard primes''?*/

	/* These are around 128 bits. */
	mpz_init_set_str (p, "148028650191182616877187862194899201391", 10); 
	mpz_init_set_str (q, "315270837425234199477225845240496832591", 10);
	
	/* Allocate algorithm specific data. */
	/* FIXME: Use user supplied allocation func instead of malloc? */
	s->data.bbs =
	  (__gmp_rand_data_bbs *) malloc (sizeof (__gmp_rand_data_bbs));

	mpz_init (s->data.bbs->bi); /* The Blum integer. */
	mpz_mul (s->data.bbs->bi, p, q);

	/* Find a seed, x, with gcd (x, bi) == 1. */
	mpz_init (ztmp);
	while (1)
	  {
	    mpz_gcd (ztmp, seed, s->data.bbs->bi);
	    if (!mpz_cmp_ui (ztmp, 1))
	      break;
	    mpz_add_ui (seed, seed, 1);
	  }

	s->alg = alg;
	s->size = size;
	mpz_init_set (s->seed, seed);

	mpz_clear (p);
	mpz_clear (q);
	mpz_clear (ztmp);
	break;
      }

    default:			/* Bad choice. */
      return 1;
    }

  return 0;
}
