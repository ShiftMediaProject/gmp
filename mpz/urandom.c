/* urandomb(rop, state) -- Generate a uniform pseudorandom integer in
the range 0 to 2^n - 1, inclusive, using `state' as the random state
previously initialized by a call to gmp_rand_init().  The `n' in the
above range is passed to gmp_rand_init().

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

/* We use (1)

	X = (aX + c) mod m

[D. Knuth, "The Art of Computer Programming: Volume 2, Seminumerical Algorithms",
Third Edition, Addison Wesley, 1998, pp. 184-185.]

	X is the seed and the result
	a is chosen so that
	    a mod 8 = 5 [3.2.1.2] and [3.2.1.3] 
	    .01m < a < .99m
	    its binary or decimal digits is not a simple, regular pattern
	    it has no large quotients when Euclid's algorithm is used to find
	      gcd(a, m) [3.3.3]
	    it passes the spectral test [3.3.4]
	    it passes several tests of [3.3.2]
	c has no factor in common with m (c=1 or c=a can be good)
	m is large (2^30)
	  is a power of 2 [3.2.1.1]
	
The least significant digits of the generated number are not very
random.  It should be regarded as a random fraction X/m.  To get a
random integer between 0 and n-1, multiply X/m by n and truncate.
(Don't use X/n [ex 3.4.1-3])

The ``accuracy'' in t dimensions is one part in ``the t'th root of m'' [3.3.4].

Don't generate more than about m/1000 numbers without changing a, c, or m.

The sequence length depends on chosen a,c,m.


Or, we use (2)
	X = a * (X mod q) - r * (long) (X/q)
	if X<0 then X+=m

[Knuth, pp. 185-186.]

	X is the seed and the result
	  as a seed is nonzero and less than m
	a is a primitive root of m (which means that a^2 <= m)
	q is (long) m / a
	r is m mod a
	m is a prime number near the largest easily computed integer

which gives

	X = a * (X % ((long) m / a)) -
	    (M % a) * ((long) (X / ((long) m / a)))

Since m is prime, the least-significant bits of X are just as random as 
the most-significant bits.

*/

#include "gmp.h"

/* Array of CL-schemes, ordered in increasing order for the first
   member (the 'bits' value).  The 'm' entry is converted by
   mpz_set_str() with BASE=0.  See its documentation for syntax of
   'm'.  End of array is indicated with an entry containing all
   zeros. */
static __gmp_rand_scheme_struct __gmp_rand_scheme[] =
{
  {31,				/* Knuth, p. 185 */
   "48271", {0},		/* a */
   0,				/* c = 0 */
   "0x7fffffff", {0}},		/* m = 2^31 - 1 */
  
  {31,				/* fbsd random(3), lazy */
   "16807", {0},		/* a = 7^5 */
   0,				/* c = 0 */
   "0x7fffffff", {0}},		/* m = 2^31 - 1 */

  {31,				/* fbsd rand(3) */
   "1103515245", {0},		/* a (multiplier) */
   12345,			/* c (adder) */
   "0x80000000", {0}},		/* m (modulo) = 2^31 */

  {32, "42949685", 	{0}, 1, "0x100000000", {0}},
  {33, "85899357", 	{0}, 1, "0x200000000", {0}},
  {34, "171798701", 	{0}, 1, "0x400000000", {0}},
  {35, "343597397", 	{0}, 1, "0x800000000", {0}},
  {36, "687194781", 	{0}, 1, "0x1000000000", {0}},
  {37, "1374389541", 	{0}, 1, "0x2000000000", {0}},
  {38, "2748779077", 	{0}, 1, "0x4000000000", {0}},
  {39, "5497558149", 	{0}, 1, "0x8000000000", {0}},
  {40, "10995116285", 	{0}, 1, "0x10000000000", {0}},

  {56, "720575940379293",
   {0}, 1, "0x100000000000000", {0}},
  {64, "184467440737095525",
   {0}, 1, "0x10000000000000000", {0}},
  {100, "12676506002282294014967032061",
   {0}, 1, "0x10000000000000000000000000", {0}},
  {128, "3402823669209384634633746074317682125",
   {0}, 1, "0x100000000000000000000000000000000", {0}},
  {156, "913438523331814323877303020447676887284957853",
   {0}, 1, "0x1000000000000000000000000000000000000000", {0}},
  {196, "1004336277661868922213726307713226626576376871114245522077",
   {0}, 1, "0x10000000000000000000000000000000000000000000000000", {0}},
  {200, "16069380442589902755419620923411626025222029937827928353021",
   {0}, 1, "0x100000000000000000000000000000000000000000000000000", {0}},
  {256, "1157920892373161954235709850086879078532699846656405640394575840079131296413",
   {0}, 1, "0x10000000000000000000000000000000000000000000000000000000000000000", {0}},

  /*  {, "", 	{0}, 1, "0x", {0}}, */
  {0, NULL, {0}, 0, NULL, {0}}	/* End of array. */
};

/* gmp_rand_init() -- Initialize a gmp_rand_state struct.  Return 0 on
   success and 1 on failure. */

int
#if __STDC__
gmp_rand_init (gmp_rand_state *s,
	       gmp_rand_algorithm alg,
	       unsigned long int size,
	       mpz_t seed)
#else
gmp_rand_init (s, alg, size, seed)
     gmp_rand_state *s;
     gmp_rand_algorithm alg;
     unsigned long int size;
     mpz_t seed;
#endif
{
  __gmp_rand_scheme_struct *sp;

  switch (alg)
    {
    case GMP_RAND_ALG_LC:	/* Linear congruental. */
      /* Convert the 'a' and 'm' strings to mpz_t's. */
      for (sp = __gmp_rand_scheme; sp->bits; sp++)
	{
	  mpz_init_set_str (sp->a, sp->astr, 0);
	  mpz_init_set_str (sp->m, sp->mstr, 0);
	}
      /* Pick a scheme. */
      s->scheme = NULL;
      for (sp = __gmp_rand_scheme; sp->bits; sp++)
	if (sp->bits >= size)
	  {
	    s->scheme = sp;
	    break;
	  }
      if (NULL ==  s->scheme)	/* Nothing big enough found. */
	s->scheme = --sp;	/* Use biggest available. */

      mpz_init (s->n);
      mpz_tdiv_q_ui (s->n, s->scheme->m, 1000UL);
      break;
    case GMP_RAND_ALG_BBS:
      return 1;			/* Not implemented yet. */
      break;
    default:			/* Bad choice. */
      return 1;
    }

  /* Common. */
  s->alg = alg;
  s->size = size;
  mpz_init_set_ui (s->maxval, 1);
  mpz_mul_2exp (s->maxval, s->maxval, s->size);
  mpz_init_set (s->seed, seed);

  return 0;
}

void
#if __STDC__
gmp_rand_clear (gmp_rand_state *s)
#else
gmp_rand_clear (s)
     gmp_rand_state *s;
#endif
{
  __gmp_rand_scheme_struct *sp;


  mpz_clear (s->seed);
  mpz_clear (s->maxval);

  switch (s->alg)
    {
    case GMP_RAND_ALG_LC:	/* Linear congruental. */
      mpz_clear (s->n);
      for (sp = __gmp_rand_scheme; sp->bits; sp++)
	{
	  mpz_clear (sp->a);
	  mpz_clear (sp->m);
	}
      break;
    }
}

void
#if __STDC__
mpz_urandomb (mpz_t rop, gmp_rand_state *s)
#else
mpz_urandomb (rop, s)
     mpz_t rop;
     gmp_rand_state *s;
#endif
{
  switch (s->alg)
    {
    case GMP_RAND_ALG_LC:
      if (mpz_cmp_ui (s->n, 0) == 0)
	{
	  if ((s->scheme + 1)->bits)
	    s->scheme++;
	  mpz_tdiv_q_ui (s->n, s->scheme->m, 1000UL);
	}
      else
	mpz_sub_ui (s->n, s->n, 1);

      /* rop = (seed * a + c) % m */
      mpz_mul (rop, s->seed, s->scheme->a);
      mpz_add_ui (rop, rop, s->scheme->c);
      mpz_mod (rop, rop, s->scheme->m);
  
      if (mpz_cmp (s->scheme->m, s->maxval))
	{
	  /* rop = (rop / m) * maxval, truncate result */
	  mpf_t ft1, ft2;

	  mpf_init (ft1);
	  mpf_init (ft2);
	  
	  mpf_set_z (ft1, rop);
	  mpf_set_z (ft2, s->scheme->m);
	  mpf_div (ft1, ft1, ft2);
	  mpf_set_z (ft2, s->maxval);
	  mpf_mul (ft1, ft1, ft2);
	  mpz_set_f (rop, ft1);	/* Truncating. */
	  
	  mpf_clear (ft1);
	  mpf_clear (ft2);
	}
      break;			/* GMP_RAND_ALG_LC */
    }

  /* Save result for next seed. */
  mpz_set (s->seed, rop);
}
