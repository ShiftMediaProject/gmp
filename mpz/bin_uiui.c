/* mpz_bin_uiui - compute n over k.

Copyright 2011, 2012 Free Software Foundation, Inc.

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
#include "longlong.h"

#include "fac_ui.h"

/* Algorithm:

   Accumulate chunks of factors first limb-by-limb (using one of mul0-mul8)
   which are then accumulated into mpn numbers.  The first inner loop
   accumulates divisor factors, the 2nd inner loop accumulates exactly the same
   number of dividend factors.  We avoid accumulating more for the divisor,
   even with its smaller factors, since we else cannot guarantee divisibility.

   Since we know each division will yield an integer, we compute the quotient
   using Hensel norm: If the quotient is limited by 2^t, we compute A / B mod
   2^t.

   Improvements:

   (1) An obvious improvement to this code would be to compute mod 2^t
   everywhere.  Unfortunately, we cannot determine t beforehand, unless we
   invoke some approximation, such as Stirling's formula.  Of course, we don't
   need t to be tight.  However, it is not clear that this would help much,
   our numbers are kept reasonably small already.

   (2) Compute nmax/kmax semi-accurately, without scalar division or a loop.
   Extracting the 3 msb, then doing a table lookup using cnt*8+msb as index,
   would make it both reasonably accurate and fast.  (We could use a table
   stored into a limb, perhaps.)  The table should take the removed factors of
   2 into account (those done on-the-fly in mulN).
*/

/* This threshold determines how large divisor to accumulate before we call
   bdiv.  Perhaps we should never call bdiv, and accumulate all we are told,
   since we are just basecase code anyway?  Presumably, this depends on the
   relative speed of the asymptotically fast code ad this code.  */
#define SOME_THRESHOLD 20

/* Multiply-into-limb functions.  These remove factors of 2 on-the-fly.  FIXME:
   All versions of MAXFACS don't take this 2 removal into account now, meaning
   that then, shifting just adds some overhead.  (We remove factors from the
   completed limb anyway.)  */

static mp_limb_t
mul1 (mp_limb_t m)
{
  return m;
}

static mp_limb_t
mul2 (mp_limb_t m)
{
  mp_limb_t m01 = (m + 0) * (m + 1) >> 1;
  return m01;
}

static mp_limb_t
mul3 (mp_limb_t m)
{
  mp_limb_t m01 = (m + 0) * (m + 1) >> 1;
  mp_limb_t m2 = (m + 2);
  return m01 * m2;
}

static mp_limb_t
mul4 (mp_limb_t m)
{
  mp_limb_t m01 = (m + 0) * (m + 1) >> 1;
  mp_limb_t m23 = (m + 2) * (m + 3) >> 1;
  return m01 * m23 >> 1;
}

static mp_limb_t
mul5 (mp_limb_t m)
{
  mp_limb_t m012 = (m + 0) * (m + 1) * (m + 2) >> 1;
  mp_limb_t m34 = (m + 3) * (m + 4) >> 1;
  return m012 * m34 >> 1;
}

static mp_limb_t
mul6 (mp_limb_t m)
{
  mp_limb_t m01 = (m + 0) * (m + 1);
  mp_limb_t m23 = (m + 2) * (m + 3);
  mp_limb_t m45 = (m + 4) * (m + 5) >> 1;
  mp_limb_t m0123 = m01 * m23 >> 3;
  return m0123 * m45;
}

static mp_limb_t
mul7 (mp_limb_t m)
{
  mp_limb_t m01 = (m + 0) * (m + 1);
  mp_limb_t m23 = (m + 2) * (m + 3);
  mp_limb_t m456 = (m + 4) * (m + 5) * (m + 6) >> 1;
  mp_limb_t m0123 = m01 * m23 >> 3;
  return m0123 * m456;
}

static mp_limb_t
mul8 (mp_limb_t m)
{
  mp_limb_t m01 = (m + 0) * (m + 1);
  mp_limb_t m23 = (m + 2) * (m + 3);
  mp_limb_t m45 = (m + 4) * (m + 5);
  mp_limb_t m67 = (m + 6) * (m + 7);
  mp_limb_t m0123 = m01 * m23 >> 3;
  mp_limb_t m4567 = m45 * m67 >> 3;
  return m0123 * m4567 >> 1;
}

typedef mp_limb_t (* mulfunc_t) (mp_limb_t);

static const mulfunc_t mulfunc[] = {0,mul1,mul2,mul3,mul4,mul5,mul6,mul7,mul8};
#define M (numberof(mulfunc)-1)

/* Number of factors-of-2 removed by the corresponding mulN functon.  */
static const unsigned char tcnttab[] = {0,0,1,1,3,3,4,4,7};

/* Rough computation of how many factors we can multiply together without
   spilling a limb.  */
#if 0
/* This variant is inaccurate and relies on an expensive division.  */
#define MAXFACS(max,l)							\
  do {									\
    int __cnt, __logb;							\
    count_leading_zeros (__cnt, (mp_limb_t) (l));			\
    __logb = GMP_LIMB_BITS - __cnt;					\
    (max) = (GMP_NUMB_BITS + 1) / __logb; /* FIXME: remove division */	\
  } while (0)
#else

/* This variant is exact but uses a loop.  It takes the 2 removal of mulN into
 account.  */
static const unsigned long ftab[] =
#if GMP_NUMB_BITS == 64
  /* 1 to 8 factors per iteration */
  {0xfffffffffffffffful,0x100000000ul,0x32cbfe,0x16a0b,0x24c4,0xa16,0x34b,0x1b2 /*,0xdf,0x8d */};
#endif
#if GMP_NUMB_BITS == 32
  /* 1 to 7 factors per iteration */
  {0xffffffff,0x10000,0x801,0x16b,0x71,0x42,0x26 /* ,0x1e */};
#endif

#define MAXFACS(max,l)							\
  do {									\
    int __i;								\
    for (__i = numberof (ftab) - 1; l > ftab[__i]; __i--)		\
      ;									\
    (max) = __i + 1;							\
  } while (0)
#endif

static void
mpz_bdiv_bin_uiui (mpz_ptr r, unsigned long int n, unsigned long int k)
{
  int nmax, kmax, nmaxnow, numfac;
  mp_ptr np, kp;
  mp_size_t nn, kn, alloc;
  mp_limb_t i, j, t, iii, jjj, cy, dinv;
  mp_bitcnt_t i2cnt, j2cnt;
  int cnt;
  mp_size_t maxn;
  TMP_DECL;

  TMP_MARK;

  maxn = 1 + n / GMP_NUMB_BITS;    /* absolutely largest result size (limbs) */

  /* FIXME: This allocation might be insufficient, but is usually way too
     large.  */
  alloc = SOME_THRESHOLD + MAX (3 * maxn / 2, SOME_THRESHOLD);
  np = TMP_ALLOC_LIMBS (alloc);
  kp = TMP_ALLOC_LIMBS (alloc);

  MAXFACS (nmax, n);
  nmax = MIN (nmax, M);
  MAXFACS (kmax, k);
  kmax = MIN (kmax, M);

  j = 1;
  i = n - k + 1;

  nmax = MIN (nmax, k);
  kmax = MIN (kmax, k);

  np[0] = 1; nn = 1;

  i2cnt = 0;				/* total low zeros in dividend */
  j2cnt = 0;				/* total low zeros in divisor */

  while (kmax != 0)
    {
      numfac = j;

      jjj = mulfunc[kmax] (j);
      j += kmax;				/* number of factors used */
      count_trailing_zeros (cnt, jjj);		/* count low zeros */
      jjj >>= cnt;				/* remove remaining low zeros */
      j2cnt += tcnttab[kmax] + cnt;		/* update low zeros count */
      kp[0] = jjj;				/* store new factors */
      kn = 1;
      t = k - j + 1;
      kmax = MIN (kmax, t);

      while (kmax != 0 && kn <  SOME_THRESHOLD)
	{
	  jjj = mulfunc[kmax] (j);
	  j += kmax;				/* number of factors used */
	  count_trailing_zeros (cnt, jjj);	/* count low zeros */
	  jjj >>= cnt;				/* remove remaining low zeros */
	  j2cnt += tcnttab[kmax] + cnt;		/* update low zeros count */
	  cy = mpn_mul_1 (kp, kp, kn, jjj);	/* accumulate new factors */
	  kp[kn] = cy;
	  kn += cy != 0;
	  t = k - j + 1;
	  kmax = MIN (kmax, t);
	}
      numfac = j - numfac;

      while (numfac != 0)
	{
	  nmaxnow = MIN (nmax, numfac);
	  iii = mulfunc[nmaxnow] (i);
	  i += nmaxnow;				/* number of factors used */
	  count_trailing_zeros (cnt, iii);	/* count low zeros */
	  iii >>= cnt;				/* remove remaining low zeros */
	  i2cnt += tcnttab[nmaxnow] + cnt;	/* update low zeros count */
	  cy = mpn_mul_1 (np, np, nn, iii);	/* accumulate new factors */
	  np[nn] = cy;
	  nn += cy != 0;
	  numfac -= nmaxnow;
	}

      ASSERT (nn < alloc);

      binvert_limb (dinv, kp[0]);
      nn += (np[nn - 1] >= kp[kn - 1]);
      nn -= kn;
      mpn_sbpi1_bdiv_q (np, np, nn, kp, MIN(kn,nn), -dinv);
    }

  /* Put back the right number of factors of 2.  */
  cnt = i2cnt - j2cnt;
  if (cnt != 0)
    {
      ASSERT (cnt < GMP_NUMB_BITS); /* can happen, but not for intended use */
      cy = mpn_lshift (np, np, nn, cnt);
      np[nn] = cy;
      nn += cy != 0;
    }

  nn -= np[nn - 1] == 0;	/* normalisation */

  MPZ_REALLOC (r, nn);
  SIZ(r) = nn;
  MPN_COPY (PTR(r), np, nn);
  TMP_FREE;
}

/* Entry i contains (i!/2^t) where t is chosen such that the parenthesis
   is an odd integer. */
static const mp_limb_t fac[] = { ONE_LIMB_ODD_FACTORIAL_TABLE };

#define MAX_K (numberof(fac)-1)

/* Entry i contains (i!/2^t)^(-1) where t is chosen such that the parenthesis
   is an odd integer. */
static const mp_limb_t facinv[] = {
#if GMP_NUMB_BITS == 64
  0x0000000000000001,
  0x0000000000000001,  /*  1 */  0x0000000000000001,  /*  2 */
  0xaaaaaaaaaaaaaaab,  /*  3 */  0xaaaaaaaaaaaaaaab,  /*  4 */
  0xeeeeeeeeeeeeeeef,  /*  5 */  0x4fa4fa4fa4fa4fa5,  /*  6 */
  0x2ff2ff2ff2ff2ff3,  /*  7 */  0x2ff2ff2ff2ff2ff3,  /*  8 */
  0x938cc70553e3771b,  /*  9 */  0xb71c27cddd93e49f,  /* 10 */
  0xb38e3229fcdee63d,  /* 11 */  0xe684bb63544a4cbf,  /* 12 */
  0xc2f684917ca340fb,  /* 13 */  0xf747c9cba417526d,  /* 14 */
  0xbb26eb51d7bd49c3,  /* 15 */  0xbb26eb51d7bd49c3,  /* 16 */
  0xb0a7efb985294093,  /* 17 */  0xbe4b8c69f259eabb,  /* 18 */
  0x6854d17ed6dc4fb9,  /* 19 */  0xe1aa904c915f4325,  /* 20 */
  0x3b8206df131cead1,  /* 21 */  0x79c6009fea76fe13,  /* 22 */
  0xd8c5d381633cd365,  /* 23 */  0x4841f12b21144677,  /* 24 */
  0x4a91ff68200b0d0f,  /* 25 */  0x8f9513a58c4f9e8b,  /* 26 */
  0x2b3e690621a42251,  /* 27 */  0x4f520f00e03c04e7,  /* 28 */
  0x2edf84ee600211d3,  /* 29 */  0xadcaa2764aaacdfd,  /* 30 */
  0x161f4f9033f4fe63,  /* 31 */  0x161f4f9033f4fe63   /* 32 */
#endif
#if GMP_NUMB_BITS == 32
  0x00000001,
  0x00000001,  /*  1 */  0x00000001,  /*  2 */
  0xaaaaaaab,  /*  3 */  0xaaaaaaab,  /*  4 */
  0xeeeeeeef,  /*  5 */  0xa4fa4fa5,  /*  6 */
  0xf2ff2ff3,  /*  7 */  0xf2ff2ff3,  /*  8 */
  0x53e3771b,  /*  9 */  0xdd93e49f,  /* 10 */
  0xfcdee63d,  /* 11 */  0x544a4cbf,  /* 12 */
  0x7ca340fb,  /* 13 */  0xa417526d,  /* 14 */
  0xd7bd49c3,  /* 15 */  0xd7bd49c3   /* 16 */
#endif
};

/* Entry i contains i-popc(i). */
static const unsigned char tabbe[] = {0,0,1,1,3,3,4,4,7,7,8,8,10,10,11,11,15,15,16,16,18,18,19,19,22,22,23};

static void
mpz_smallk_bin_uiui (mpz_ptr r, unsigned long int n, unsigned long int k)
{
  int nmax, numfac;
  mp_ptr rp;
  mp_size_t rn, alloc;
  mp_limb_t i, iii, cy;
  mp_bitcnt_t i2cnt, cnt;

  if (UNLIKELY (k == 0))
    {
      PTR(r)[0] = 1;
      SIZ(r) = 1;
      return;
    }

  count_leading_zeros (cnt, (mp_limb_t) n);
  cnt = GMP_LIMB_BITS - cnt;
  alloc = cnt * k / GMP_NUMB_BITS + 3;	/* FIXME: ensure rounding is enough. */
  MPZ_REALLOC (r, alloc);
  rp = PTR(r);

  MAXFACS (nmax, n);
  nmax = MIN (nmax, M);

  i = n - k + 1;

  nmax = MIN (nmax, k);
  rp[0] = mulfunc[nmax] (i);
  rn = 1;
  i += nmax;				/* number of factors used */
  i2cnt = tcnttab[nmax];		/* low zeros count */
  numfac = k - nmax;
  while (numfac != 0)
    {
      nmax = MIN (nmax, numfac);
      iii = mulfunc[nmax] (i);
      i += nmax;			/* number of factors used */
      i2cnt += tcnttab[nmax];		/* update low zeros count */
      cy = mpn_mul_1 (rp, rp, rn, iii);	/* accumulate new factors */
      rp[rn] = cy;
      rn += cy != 0;
      numfac -= nmax;
    }

  ASSERT (rn < alloc);

  mpn_pi1_bdiv_q_1 (rp, rp, rn, fac[k], facinv[k], 0);
  rn -= rp[rn - 1] == 0;		/* normalisation */

  /* We will now have some accumulated excess factors of 2, since the mulN
     functions only suppress factors conservatively.  Since both n and in
     particular k are limited, we have < GMP_NUMB_BITS factors. */
  cnt = tabbe[k] - i2cnt;
  if (cnt != 0)
    {
      cy = mpn_rshift (rp, rp, rn, cnt);
      rn -= rp[rn - 1] == 0;		/* normalisation */
    }

  SIZ(r) = rn;
}

/* Algorithm:

   Plain and simply multiply things together.

   We tabulate factorials (k!/2^t)^(-1) mod B for 1= < k <= 32 (where t is
   chosen such that k!/2^t is odd).

   Improvement:

   Shift i, not c, in the loop.  Requires that we compute a starting pos in ctz
   using the low bits of n-k+1.
*/

static unsigned char ctz[] = {0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5};

static void
mpz_bc_bin_uiui (mpz_ptr r, unsigned long int n, unsigned long int k)
{
  mp_limb_t i, j, c;

  c = facinv[k];
  i = n - k + 1;
  for (j = 0; j < k; j++)
    {
      c = c * i;
      c = c >> ctz[j];
      i++;
    }

  c = c & ((CNST_LIMB(1) << GMP_NUMB_BITS-4) - 1);

  PTR(r)[0] = c;
  SIZ(r) = 1;
}

void
mpz_bin_uiui (mpz_ptr r, unsigned long int n, unsigned long int k)
{
  if (UNLIKELY (n < k)) {
    SIZ (r) = 0;
  } else {
    /* Rewrite bin(n,k) as bin(n,n-k) if that is smaller. */
    k = MIN (k, n-k);
    if (n < GMP_NUMB_BITS)
      mpz_bc_bin_uiui (r, n, k);
    else if (k < MAX_K)
      mpz_smallk_bin_uiui (r, n, k);
    else
      mpz_bdiv_bin_uiui (r, n, k);
  }
}
