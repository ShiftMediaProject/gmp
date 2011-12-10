/* mpz_fac_ui(result, n) -- Set RESULT to N!.

Contributed to the GNU project by Marco Bodrato.

Copyright 1991, 1993, 1994, 1995, 2000, 2001, 2002, 2003, 2011 Free
Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.  */

#include "gmp.h"
#include "gmp-impl.h"

#include "fac_ui.h"

/* TODO:
   - write the code for tuning thresholds;
   - split this file in smaller parts with functions that can be recycled for different computations.
 */

/*************************************************************/
/* Section macros: common macros, for swing/fac/bin (&sieve) */
/*************************************************************/

/* This is intended for constant THRESHOLDs only, where the compiler
   can completely fold the result.  */
#define LOG2C(n) \
 (((n) >=    0x1) + ((n) >=    0x2) + ((n) >=    0x4) + ((n) >=    0x8) + \
  ((n) >=   0x10) + ((n) >=   0x20) + ((n) >=   0x40) + ((n) >=   0x80) + \
  ((n) >=  0x100) + ((n) >=  0x200) + ((n) >=  0x400) + ((n) >=  0x800) + \
  ((n) >= 0x1000) + ((n) >= 0x2000) + ((n) >= 0x4000) + ((n) >= 0x8000))

#define FACTOR_LIST_APPEND(PR, MAX_PR, VEC, I)			\
  if ((PR) > (MAX_PR)) {					\
    (VEC)[(I)++] = (PR);					\
    (PR) = 1;							\
  }

#define FACTOR_LIST_STORE(P, PR, MAX_PR, VEC, I)		\
  do {								\
    if ((PR) > (MAX_PR)) {					\
      (VEC)[(I)++] = (PR);					\
      (PR) = (P);						\
    } else							\
      (PR) *= (P);						\
  } while (0)

#define LOOP_ON_SIEVE_BEGIN(prime,start,end,off,sieve)		\
  do {								\
    mp_limb_t __mask, __index, __max_i, __i;			\
								\
    __i = (start)-(off);					\
    __index = __i / GMP_LIMB_BITS;				\
    __mask = CNST_LIMB(1) << (__i % GMP_LIMB_BITS);		\
    __i += (off);						\
								\
    __max_i = (end);						\
								\
    do {							\
      ++__i;							\
      if (((sieve)[__index] & __mask) == 0)			\
	{							\
	  (prime) = id_to_n(__i)

#define LOOP_ON_SIEVE_END					\
	}							\
      __mask = __mask << 1 | __mask >> (GMP_LIMB_BITS-1);	\
      __index += __mask & 1;					\
    }  while (__i <= __max_i);					\
  } while (0)

/*********************************************************/
/* Section sieve: sieving functions and tools for primes */
/*********************************************************/

static mp_limb_t
bit_to_n (mp_limb_t bit) { return (bit*3+4)|1; }

/* id_to_n (x) = bit_to_n (x-1) = id*3+1+(id&1)*/
static mp_limb_t
id_to_n  (mp_limb_t id)  { return (id*3+1)|1; }

static mp_limb_t
n_to_bit (mp_limb_t n) { return ((n-5)|1)/3U; }

static mp_size_t
primesieve_size (mp_limb_t n) { return n_to_bit(n) / GMP_LIMB_BITS + 1; }

/* FIXME: also for 8... */
#if GMP_LIMB_BITS > 30
#if GMP_LIMB_BITS < 62
#define SIEVE_SEED CNST_LIMB(0x69128480)
#define SEED_LIMIT 114
#else
#define SIEVE_SEED CNST_LIMB(0x3294C9E069128480)
#define SEED_LIMIT 202
#endif
#else
#if GMP_LIMB_BITS > 15
#define SIEVE_SEED CNST_LIMB(0x8480)
#define SEED_LIMIT 54
#else
#error Not implemented
#endif
#endif

static void
first_block_primesieve (mp_limb_t *bit_array, mp_limb_t n)
{
  mp_size_t bits, limbs;

  ASSERT (n > 4);

  bits  = n_to_bit(n);
  limbs = bits / GMP_LIMB_BITS + 1;

  /* FIXME: We can skip 5 too, filling with a 5-part pattern. */
  MPN_ZERO (bit_array, limbs);
  bit_array[0] = SIEVE_SEED;

  if ((bits + 1) % GMP_LIMB_BITS != 0)
    bit_array[limbs-1] |= MP_LIMB_T_MAX << ((bits + 1) % GMP_LIMB_BITS);

  if (n > SEED_LIMIT) {
    mp_limb_t mask, index, i;

    ASSERT (n > 49);

    mask = 1;
    index = 0;
    i = 1;
    do {
      if ((bit_array[index] & mask) == 0)
	{
	  mp_size_t step, lindex;
	  mp_limb_t lmask;
	  int maskrot;

	  step = id_to_n(i);
/*	  lindex = n_to_bit(id_to_n(i)*id_to_n(i)); */
	  lindex = i*(step+1+(i&1))-1+(i&1);
/*	  lindex = i*(i*3+2+(i&1)*2)-1+(i&1); */
	  if (lindex > bits)
	    break;

	  step <<= 1;
	  maskrot = step % GMP_LIMB_BITS;

	  lmask = CNST_LIMB(1) << (lindex % GMP_LIMB_BITS);
	  do {
	    bit_array[lindex / GMP_LIMB_BITS] |= lmask;
	    lmask = lmask << maskrot | lmask >> (GMP_LIMB_BITS - maskrot);
	    lindex += step;
	  } while (lindex <= bits);

/*	  lindex = n_to_bit(id_to_n(i)*bit_to_n(i)); */
	  lindex = i*(i*3+6)+(i&1);

	  lmask = CNST_LIMB(1) << (lindex % GMP_LIMB_BITS);
	  for ( ; lindex <= bits; lindex += step) {
	    bit_array[lindex / GMP_LIMB_BITS] |= lmask;
	    lmask = lmask << maskrot | lmask >> (GMP_LIMB_BITS - maskrot);
	  };
	}
      mask = mask << 1 | mask >> (GMP_LIMB_BITS-1);
      index += mask & 1;
      i++;
    } while (1);
  }
}

static void
block_resieve (mp_ptr bit_array, mp_size_t limbs, mp_limb_t offset,
		      mp_srcptr sieve, mp_limb_t sieve_bits)
{
  mp_size_t bits, step;

  ASSERT (limbs > 0);

  bits = limbs * GMP_LIMB_BITS - 1;

  /* FIXME: We can skip 5 too, filling with a 5-part pattern. */
  MPN_ZERO (bit_array, limbs);

  LOOP_ON_SIEVE_BEGIN(step,0,sieve_bits,0,sieve);
  {
    mp_size_t lindex;
    mp_limb_t lmask;
    int maskrot;

/*  lindex = n_to_bit(id_to_n(i)*id_to_n(i)); */
    lindex = __i*(step+1+(__i&1))-1+(__i&1);
/*  lindex = i*(i*3+2+(i&1)*2)-1+(i&1); */
    if (lindex > bits + offset)
      break;

    step <<= 1;
    maskrot = step % GMP_LIMB_BITS;

    if (lindex < offset)
      lindex += step * ((offset - lindex - 1) / step + 1);

    lindex -= offset;

    lmask = CNST_LIMB(1) << (lindex % GMP_LIMB_BITS);
    for ( ; lindex <= bits; lindex += step) {
      bit_array[lindex / GMP_LIMB_BITS] |= lmask;
      lmask = lmask << maskrot | lmask >> (GMP_LIMB_BITS - maskrot);
    };

/*  lindex = n_to_bit(id_to_n(i)*bit_to_n(i)); */
    lindex = __i*(__i*3+6)+(__i&1);
    if (lindex > bits + offset)
      continue;

    if (lindex < offset)
      lindex += step * ((offset - lindex - 1) / step + 1);

    lindex -= offset;

    lmask = CNST_LIMB(1) << (lindex % GMP_LIMB_BITS);
    for ( ; lindex <= bits; lindex += step) {
      bit_array[lindex / GMP_LIMB_BITS] |= lmask;
      lmask = lmask << maskrot | lmask >> (GMP_LIMB_BITS - maskrot);
    };
  }
  LOOP_ON_SIEVE_END;
}

#define BLOCK_SIZE 2048

/* Fills bit_array with the characteristic function of composite
   numbers up to the parameter n. I.e. a bit set to "1" represent a
   composite, a "0" represent a prime.

   The primesieve_size(n) limbs pointed to by bit_array are
   overwritten. The returned value counts prime integers in the
   interval [4, n]. Note that n > 4.

   Even numbers and multiples of 3 are excluded "a priori", only
   numbers equivalent to +/- 1 mod 6 have their bit in the array.

   Once sieved, if the bit b is ZERO it represent a prime, the
   represented prime is bit_to_n(b), if the LSbit is bit 0, or
   id_to_n(b), if you call "1" the first bit.
 */

static mp_limb_t
bitwise_primesieve (mp_ptr bit_array, mp_limb_t n)
{
  mp_size_t size;
  mp_limb_t bits;

  ASSERT (n > 4);

  bits = n_to_bit(n);
  size = bits / GMP_LIMB_BITS + 1;

  if (size > BLOCK_SIZE * 2) {
    mp_size_t off;
    off = BLOCK_SIZE + (size % BLOCK_SIZE);
    first_block_primesieve (bit_array, id_to_n (off * GMP_LIMB_BITS));
    for ( ; off < size; off += BLOCK_SIZE)
      block_resieve (bit_array + off, BLOCK_SIZE, off * GMP_LIMB_BITS, bit_array, off * GMP_LIMB_BITS - 1);
  } else {
    first_block_primesieve (bit_array, n);
  }

  if ((bits + 1) % GMP_LIMB_BITS != 0)
    bit_array[size-1] |= MP_LIMB_T_MAX << ((bits + 1) % GMP_LIMB_BITS);


  return size * GMP_LIMB_BITS - mpn_popcount (bit_array, size);
}

#undef BLOCK_SIZE

/*********************************************************/
/* Section list-prod: product of a list -> mpz_t         */
/*********************************************************/

/* FIXME: should be tuned */
#ifndef RECURSIVE_PROD_THRESHOLD
#define RECURSIVE_PROD_THRESHOLD MUL_TOOM22_THRESHOLD
#endif

/* Computes the product of the j>1 limbs pointed by factors, puts the
   result in x.
*/

static void
mpz_prodlimbs (mpz_ptr x, mp_limb_t *factors, mp_limb_t j)
{
  mp_limb_t i;
  mp_size_t size;
  mp_ptr    prod;

  ASSERT (j > 1);
  ASSERT (RECURSIVE_PROD_THRESHOLD > 3);

  if (BELOW_THRESHOLD (j, RECURSIVE_PROD_THRESHOLD)) {
    mp_limb_t cy;

    PTR (x)[0] = factors[0];
    SIZ (x) = 1;
    prod = MPZ_REALLOC (x, j);
    size = 1;

    i = 1;
    do
      {
	cy = mpn_mul_1 (prod, prod, size, factors[i]);
	prod[size] = cy;
	size += cy != 0;
 	i++;
      } while (i < j);

    SIZ (x) = size;
  } else {
    mpz_t x1, x2;
    TMP_DECL;

    i = j >> 1;
    TMP_MARK;

    MPZ_TMP_INIT (x2, j - i);

    PTR (x1) = factors + i;
    ALLOC (x1) = j - i;
    mpz_prodlimbs (x2, factors + i, j - i);
    mpz_prodlimbs (x1, factors, i);
    size = SIZ (x1) + SIZ (x2);
    prod = MPZ_REALLOC (x, size);
    if (SIZ (x1) >= SIZ (x2))
      i = mpn_mul (prod, PTR(x1), SIZ(x1), PTR(x2), SIZ(x2));
    else
      i = mpn_mul (prod, PTR(x2), SIZ(x2), PTR(x1), SIZ(x1));
    SIZ (x) = size - (i == 0);

    TMP_FREE;
  }
}

/*********************************************************/
/* Section swing: swing factorial                        */
/*********************************************************/

#define SWING_A_PRIME(P, PR, MAX_PR, VEC, I)	\
  do {						\
    mp_limb_t __q, __prime;			\
    __prime = (P);				\
    FACTOR_LIST_APPEND(PR, MAX_PR, VEC, I);		\
    __q = n;					\
    do {					\
      __q /= __prime;				\
      if ((__q & 1) != 0) (PR) *= __prime;	\
    } while (__q >= __prime);			\
  } while (0)

/* mpz_oddswing_1 computes the odd part of the swing factorial of the parameter n.
   The result x is an odd positive integer so that n$ = x 2^a.

   Uses the algorithm described by Peter Luschny in "Divide, Swing and
   Conquer the Factorial!".

   The pointer sieve points to primesieve_size(n) limbs containing a
   bit-array where primes are marked as 0.
   Enough (FIXME: explain :-) limbs must be pointed by factors.
 */

static void
mpz_oddswing_1 (mpz_ptr x, mp_limb_t n, mp_limb_t *sieve, mp_limb_t *factors)
{
  mp_limb_t prod, max_prod;
  mp_limb_t j;

  ASSERT (n > 15);

  j = 0;

  prod = 1;
  max_prod = GMP_NUMB_MAX / n;

  /* Handle prime = 3 separately. */
  SWING_A_PRIME (3, prod, max_prod, factors, j);

  /* Swing primes from 5 to n/3 */
  if (1 || n > 5*3)
    {
      mp_limb_t prime;
      LOOP_ON_SIEVE_BEGIN (prime, 0, n_to_bit (n/3), 0,sieve);
      SWING_A_PRIME (prime, prod, max_prod, factors, j);
      LOOP_ON_SIEVE_END;
    }

  /* Store primes from (n+1)/2 to n */
  {
    mp_limb_t prime;
    LOOP_ON_SIEVE_BEGIN (prime, n_to_bit (n >> 1) + 1, n_to_bit (n), 0,sieve);
    FACTOR_LIST_STORE (prime, prod, max_prod, factors, j);
    LOOP_ON_SIEVE_END;
  }

  if (j != 0)
    {
      factors[j++] = prod;
      mpz_prodlimbs (x, factors, j);
    }
  else
    {
      PTR (x)[0] = prod;
      SIZ (x) = 1;
    }

}

#undef SWING_A_PRIME

/*********************************************************/
/* Section oddfac: odd factorial, needed also by binomial*/
/*********************************************************/

/* FIXME: should be tuned */
#ifndef FAC_DSC_THRESHOLD
#define FAC_DSC_THRESHOLD 210
#endif

#if TUNE_PROGRAM_BUILD
#define FAC_DSC_THRESHOLD_LIMIT 1024
#define FACTORS_PER_LIMB (GMP_NUMB_BITS / (LOG2C(FAC_DSC_THRESHOLD_LIMIT-1)+1))
#else
#define FACTORS_PER_LIMB (GMP_NUMB_BITS / (LOG2C(FAC_DSC_THRESHOLD-1)+1))
#endif

/* n^log <= GMP_NUMB_MAX, a limb can store log factors less than n */
static unsigned
log_n_max (mp_limb_t n)
{
  unsigned log;

  if (n < CNST_LIMB(1) << (GMP_NUMB_BITS/8))
    log = 8;
  else
    {
      mp_limb_t max, cur;
      log = 1;
      max = GMP_NUMB_MAX / n;
      for (cur = n; cur <= max; cur *= n) log++;
    }

  return log;
}

/* mpz_bc_oddfac_1 computes the odd part of the factorial of the parameter n.
   I.e. n! = x 2^a, the result x is an odd positive integer.
 */

static void
mpz_bc_oddfac_1 (mpz_ptr x, mp_limb_t n)
{
  static const mp_limb_t table[] = { ONE_LIMB_ODD_FACTORIAL_TABLE };

  mp_limb_t *factors, prod, max_prod, i, j;
  TMP_SDECL;

  ASSERT (numberof (table) > 3);
#if TUNE_PROGRAM_BUILD
  ASSERT (FAC_DSC_THRESHOLD_LIMIT >= FAC_DSC_THRESHOLD);
#endif

  j = 0;
  prod = 1;

  TMP_SMARK;
  factors = TMP_SALLOC_LIMBS (1 + n / FACTORS_PER_LIMB);

  do {
    i = 3;
    max_prod = GMP_NUMB_MAX / n;
    do {
      FACTOR_LIST_STORE (i, prod, max_prod, factors, j);
      i += 2;
    } while (i <= n);
    n >>= 1;
  } while (n >= numberof (table));

  factors[j++] = prod;
  factors[j++] = table[n];
  mpz_prodlimbs (x, factors, j);

  TMP_SFREE;
}

/*********************************************************/
/* Section factorial: fast factorial implementations     */
/*********************************************************/

/* FIXME: should be tuned */
#ifndef FAC_ODD_THRESHOLD
#define FAC_ODD_THRESHOLD 93
#endif

/* mpz_dsc_oddfac_1 computes the odd part of the factorial of the parameter n.
   I.e. n! = x 2^a, the result x is an odd positive integer.

   Uses the algorithm described by Peter Luschny in "Divide, Swing and
   Conquer the Factorial!".
 */

static void
mpz_dsc_oddfac_1 (mpz_ptr x, mp_limb_t n)
{
  mpz_t swing;
  mp_limb_t *sieve, *factors;
  mp_size_t size;
  unsigned s;
  TMP_DECL;

  ASSERT (n <= GMP_NUMB_MAX);

  s = 1;
  size = n;
  for ( ; (size >>= 1) > FAC_DSC_THRESHOLD; s++)
    ;
  mpz_bc_oddfac_1 (x, size);

  TMP_MARK;
  sieve = TMP_ALLOC_LIMBS (primesieve_size (n));

  size = (bitwise_primesieve (sieve, n) + 1) / log_n_max (n) + 1;

  MPZ_TMP_INIT (swing, size << 1);
  factors = PTR(swing) + size;
  do {
    mpz_t square;
    TMP_DECL;

    s--;
    mpz_oddswing_1 (swing, n>>s, sieve, factors);

    TMP_MARK;
    size = SIZ (x);
    MPZ_TMP_INIT (square, size << 1);
    mpn_sqr (PTR (square), PTR (x), size);
    SIZ (square) = (size << 1) - (PTR (square)[(size << 1) - 1] == 0);
    mpz_mul (x, square, swing); /* n!= n$ * floor(n/2)!^2 */

    TMP_FREE;
  } while (s != 0);
  TMP_FREE;
}

/* Computes n!, the factorial of n.
   WARNING: it assumes that n fits in a limb!
 */
void
mpz_fac_ui (mpz_ptr x, unsigned long n)
{
  static const mp_limb_t table[] = { ONE_LIMB_FACTORIAL_TABLE };

  ASSERT (n <= GMP_NUMB_MAX);

  if (n < numberof (table))
    {
      PTR (x)[0] = table[n];
      SIZ (x) = 1;
    }
  else if (BELOW_THRESHOLD (n, FAC_ODD_THRESHOLD))
    {
      mp_limb_t *factors, prod, max_prod, i, j;
      TMP_SDECL;

      TMP_SMARK;
      i = numberof (table);
      factors = TMP_SALLOC_LIMBS (2 + (n - numberof (table)) / FACTORS_PER_LIMB);

      factors[0] = table[numberof (table)-1];
      j = 1;
      prod = 1;
      max_prod = GMP_NUMB_MAX / n; /* GMP_NUMB_MAX/FAC_DSC_THRESHOLD */
      do {
	FACTOR_LIST_STORE (i, prod, max_prod, factors, j);
      } while (++i <= n);

      factors[j++] = prod;
      mpz_prodlimbs (x, factors, j);

      TMP_SFREE;
    }
  else
    {
      mp_limb_t count;
      if (BELOW_THRESHOLD (n, FAC_DSC_THRESHOLD))
	mpz_bc_oddfac_1 (x, n);
      else
	mpz_dsc_oddfac_1 (x, n);
      popc_limb (count, n);
      mpz_mul_2exp (x, x, n - count);
    }
}
