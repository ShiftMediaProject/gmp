/* An implementation in GMP of Scho"nhage's fast multiplication algorithm
   modulo 2^N+1, by Paul Zimmermann, INRIA Lorraine, February 1998.

   THE CONTENTS OF THIS FILE ARE FOR INTERNAL USE AND THE FUNCTIONS HAVE
   MUTABLE INTERFACES.  IT IS ONLY SAFE TO REACH THEM THROUGH DOCUMENTED
   INTERFACES.  IT IS ALMOST GUARANTEED THAT THEY'LL CHANGE OR DISAPPEAR IN
   A FUTURE GNU MP RELEASE.

Copyright 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.

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


/* References:

   Schnelle Multiplikation grosser Zahlen, by Arnold Scho"nhage and Volker
   Strassen, Computing 7, p. 281-292, 1971.

   Asymptotically fast algorithms for the numerical multiplication
   and division of polynomials with complex coefficients, by Arnold Scho"nhage,
   Computer Algebra, EUROCAM'82, LNCS 144, p. 3-15, 1982.

   Tapes versus Pointers, a study in implementing fast algorithms,
   by Arnold Scho"nhage, Bulletin of the EATCS, 30, p. 23-32, 1986.

   See also http://www.loria.fr/~zimmerma/bignum


   Future:

   It might be possible to avoid a small number of MPN_COPYs by using a
   rotating temporary or two.

   Multiplications of unequal sized operands can be done with this code, but
   it needs a tighter test for identifying squaring (same sizes as well as
   same pointers).  */


#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"


/* Change this to "#define TRACE(x) x" for some traces. */
#define TRACE(x)



FFT_TABLE_ATTRS mp_size_t mpn_fft_table[2][MPN_FFT_TABLE_SIZE] = {
  MUL_FFT_TABLE,
  SQR_FFT_TABLE
};


static void mpn_mul_fft_internal
_PROTO ((mp_ptr, mp_srcptr, mp_srcptr, mp_size_t, int, int, mp_ptr *, mp_ptr *,
	 mp_ptr, mp_ptr, mp_size_t, mp_size_t, mp_size_t, int **, mp_ptr,int));


/* Find the best k to use for a mod 2^(n*BITS_PER_MP_LIMB)+1 FFT.
   sqr==0 if for a multiply, sqr==1 for a square */
int
mpn_fft_best_k (mp_size_t n, int sqr)
{
  int i;

  for (i = 0; mpn_fft_table[sqr][i] != 0; i++)
    if (n < mpn_fft_table[sqr][i])
      return i + FFT_FIRST_K;

  /* treat 4*last as one further entry */
  if (i == 0 || n < 4 * mpn_fft_table[sqr][i - 1])
    return i + FFT_FIRST_K;
  else
    return i + FFT_FIRST_K + 1;
}


/* Returns smallest possible number of limbs >= pl for a fft of size 2^k.

   FIXME: Is this N rounded up to the next multiple of (2^k)*BITS_PER_MP_LIMB
   bits and therefore simply pl rounded up to a multiple of 2^k? */

mp_size_t
mpn_fft_next_size (mp_size_t pl, int k)
{
  mp_size_t N, M;
  int K;

  /*  if (k==0) k = mpn_fft_best_k (pl, sqr); */
  N = pl * BITS_PER_MP_LIMB;
  K = 1 << k;
  if (N % K)
    N = (N / K + 1) * K;
  M = N / K;
  if (M % BITS_PER_MP_LIMB)
    N = ((M / BITS_PER_MP_LIMB) + 1) * BITS_PER_MP_LIMB * K;
  return N / BITS_PER_MP_LIMB;
}


static void
mpn_fft_initl (int **l, int k)
{
  int i, j, K;

  l[0][0] = 0;
  for (i = 1,K = 2; i <= k; i++,K *= 2)
    {
      for (j = 0; j < K / 2; j++)
	{
	  l[i][j] = 2 * l[i - 1][j];
	  l[i][K / 2 + j] = 1 + l[i][j];
	}
    }
}


/* a <- a*2^e mod 2^(n*BITS_PER_MP_LIMB)+1 */
static void
mpn_fft_mul_2exp_modF (mp_ptr ap, int e, mp_size_t n, mp_ptr tp)
{
  int d, sh, i;
  mp_limb_t cc;

  d = e % (n * BITS_PER_MP_LIMB);	/* 2^e = (+/-) 2^d */
  sh = d % BITS_PER_MP_LIMB;
  if (sh != 0)
    mpn_lshift (tp, ap, n + 1, sh);	/* no carry here */
  else
    MPN_COPY (tp, ap, n + 1);
  d /= BITS_PER_MP_LIMB;		/* now shift of d limbs to the left */
  if (d)
    {
      /* ap[d..n-1] = tp[0..n-d-1], ap[0..d-1] = -tp[n-d..n-1] */
      /* mpn_xor would be more efficient here */
      for (i = d - 1; i >= 0; i--)
	ap[i] = ~tp[n - d + i];
      cc = 1 - mpn_add_1 (ap, ap, d, CNST_LIMB(1));
      if (cc != 0)
	cc = mpn_sub_1 (ap + d, tp, n - d, CNST_LIMB(1));
      else
	MPN_COPY (ap + d, tp, n - d);
      cc += mpn_sub_1 (ap + d, ap + d, n - d, tp[n]);
      if (cc != 0)
	ap[n] = mpn_add_1 (ap, ap, n, cc);
      else
	ap[n] = 0;
    }
  else if ((ap[n] = mpn_sub_1 (ap, tp, n, tp[n])))
    {
      ap[n] = mpn_add_1 (ap, ap, n, CNST_LIMB(1));
    }
  if ((e / (n * BITS_PER_MP_LIMB)) % 2)
    {
      mp_limb_t c;
      mpn_com_n (ap, ap, n);
      c = ap[n] + 2;
      ap[n] = 0;
      mpn_incr_u (ap, c);
    }
}


/* a <- a+b mod 2^(n*BITS_PER_MP_LIMB)+1 */
static void
mpn_fft_add_modF (mp_ptr ap, mp_ptr bp, int n)
{
  mp_limb_t c;

  c = ap[n] + bp[n] + mpn_add_n (ap, ap, bp, n);
  if (c > 1)
    {
      ap[n] = c - 1;
      mpn_decr_u (ap, 1);
    }
  else
    ap[n] = c;
}


/* input: A[0] ... A[inc*(K-1)] are residues mod 2^N+1 where
          N=n*BITS_PER_MP_LIMB
          2^omega is a primitive root mod 2^N+1
   output: A[inc*l[k][i]] <- \sum (2^omega)^(ij) A[inc*j] mod 2^N+1 */

static void
mpn_fft_fft_sqr (mp_ptr *Ap, mp_size_t K, int **ll,
                 mp_size_t omega, mp_size_t n, mp_size_t inc, mp_ptr tp)
{
  if (K == 2)
    {
#if HAVE_NATIVE_mpn_addsub_n
      if (mpn_addsub_n (Ap[0], Ap[inc], Ap[0], Ap[inc], n + 1) & 1)
	Ap[inc][n] = mpn_add_1 (Ap[inc], Ap[inc], n, CNST_LIMB(1));
#else
      MPN_COPY (tp, Ap[0], n + 1);
      mpn_add_n (Ap[0], Ap[0], Ap[inc],n + 1);
      if (mpn_sub_n (Ap[inc], tp, Ap[inc],n + 1))
	Ap[inc][n] = mpn_add_1 (Ap[inc], Ap[inc], n, CNST_LIMB(1));
#endif
    }
  else
    {
      int j, inc2 = 2 * inc;
      int *lk = *ll;
      mp_ptr tmp;
      TMP_DECL(marker);

      TMP_MARK(marker);
      tmp = TMP_ALLOC_LIMBS (n + 1);
      mpn_fft_fft_sqr (Ap, K/2,ll-1,2 * omega,n,inc2, tp);
      mpn_fft_fft_sqr (Ap+inc, K/2,ll-1,2 * omega,n,inc2, tp);
      /* A[2*j*inc]   <- A[2*j*inc] + omega^l[k][2*j*inc] A[(2j+1)inc]
	 A[(2j+1)inc] <- A[2*j*inc] + omega^l[k][(2j+1)inc] A[(2j+1)inc] */
      for (j = 0; j < K / 2; j++,lk += 2,Ap += 2 * inc)
	{
	  MPN_COPY (tp, Ap[inc], n + 1);
	  mpn_fft_mul_2exp_modF (Ap[inc], lk[1] * omega, n, tmp);
	  mpn_fft_add_modF (Ap[inc], Ap[0], n);
	  mpn_fft_mul_2exp_modF (tp, lk[0] * omega, n, tmp);
	  mpn_fft_add_modF (Ap[0], tp, n);
	}
      TMP_FREE(marker);
    }
}


/* input: A[0] ... A[inc*(K-1)] are residues mod 2^N+1 where
          N=n*BITS_PER_MP_LIMB
         2^omega is a primitive root mod 2^N+1
   output: A[inc*l[k][i]] <- \sum (2^omega)^(ij) A[inc*j] mod 2^N+1 */

static void
mpn_fft_fft (mp_ptr *Ap, mp_ptr *Bp, mp_size_t K, int **ll,
             mp_size_t omega, mp_size_t n, mp_size_t inc, mp_ptr tp)
{
  if (K == 2)
    {
#if HAVE_NATIVE_mpn_addsub_n
      if (mpn_addsub_n (Ap[0], Ap[inc], Ap[0], Ap[inc], n + 1) & 1)
	Ap[inc][n] = mpn_add_1 (Ap[inc], Ap[inc], n, CNST_LIMB(1));
#else
      MPN_COPY (tp, Ap[0], n + 1);
      mpn_add_n (Ap[0], Ap[0], Ap[inc],n + 1);
      if (mpn_sub_n (Ap[inc], tp, Ap[inc],n + 1))
	Ap[inc][n] = mpn_add_1 (Ap[inc], Ap[inc], n, CNST_LIMB(1));
#endif
#if HAVE_NATIVE_mpn_addsub_n
      if (mpn_addsub_n (Bp[0], Bp[inc], Bp[0], Bp[inc], n + 1) & 1)
	Bp[inc][n] = mpn_add_1 (Bp[inc], Bp[inc], n, CNST_LIMB(1));
#else
      MPN_COPY (tp, Bp[0], n + 1);
      mpn_add_n (Bp[0], Bp[0], Bp[inc],n + 1);
      if (mpn_sub_n (Bp[inc], tp, Bp[inc],n + 1))
	Bp[inc][n] = mpn_add_1 (Bp[inc], Bp[inc], n, CNST_LIMB(1));
#endif
    }
  else
    {
      int j, inc2=2 * inc;
      int *lk = *ll;
      mp_ptr tmp;
      TMP_DECL(marker);

      TMP_MARK(marker);
      tmp = TMP_ALLOC_LIMBS (n + 1);
      mpn_fft_fft (Ap, Bp, K/2,ll-1,2 * omega,n,inc2, tp);
      mpn_fft_fft (Ap+inc, Bp+inc, K/2,ll-1,2 * omega,n,inc2, tp);
      /* A[2*j*inc]   <- A[2*j*inc] + omega^l[k][2*j*inc] A[(2j+1)inc]
	 A[(2j+1)inc] <- A[2*j*inc] + omega^l[k][(2j+1)inc] A[(2j+1)inc] */
      for (j = 0; j < K / 2; j++,lk += 2,Ap += 2 * inc,Bp += 2 * inc)
	{
	  MPN_COPY (tp, Ap[inc], n + 1);
	  mpn_fft_mul_2exp_modF (Ap[inc], lk[1] * omega, n, tmp);
	  mpn_fft_add_modF (Ap[inc], Ap[0], n);
	  mpn_fft_mul_2exp_modF (tp, lk[0] * omega, n, tmp);
	  mpn_fft_add_modF (Ap[0], tp, n);
	  MPN_COPY (tp, Bp[inc], n + 1);
	  mpn_fft_mul_2exp_modF (Bp[inc], lk[1] * omega, n, tmp);
	  mpn_fft_add_modF (Bp[inc], Bp[0], n);
	  mpn_fft_mul_2exp_modF (tp, lk[0] * omega, n, tmp);
	  mpn_fft_add_modF (Bp[0], tp, n);
	}
      TMP_FREE(marker);
    }
}


/* Given ap[0..n] with ap[n]<=1, reduce it modulo 2^(n*BITS_PER_MP_LIMB)+1,
   by subtracting that modulus if necessary.

   If ap[0..n] is exactly 2^(n*BITS_PER_MP_LIMB) then the sub_1 produces a
   borrow and the limbs must be zeroed out again.  This will occur very
   infrequently.  */

static void
mpn_fft_norm (mp_ptr ap, mp_size_t n)
{
  ASSERT (ap[n] <= 1);

  if (ap[n])
    {
      if ((ap[n] = mpn_sub_1 (ap, ap, n, CNST_LIMB(1))))
	MPN_ZERO (ap, n);
    }
}


/* a[i] <- a[i]*b[i] mod 2^(n*BITS_PER_MP_LIMB)+1 for 0 <= i < K */
static void
mpn_fft_mul_modF_K (mp_ptr *ap, mp_ptr *bp, mp_size_t n, int K)
{
  int i;
  int sqr = (ap == bp);
  TMP_DECL(marker);

  TMP_MARK(marker);

  if (n >= (sqr ? SQR_FFT_MODF_THRESHOLD : MUL_FFT_MODF_THRESHOLD))
    {
      int k, K2,nprime2,Nprime2,M2,maxLK,l,Mp2;
      int **_fft_l;
      mp_ptr *Ap,*Bp,A,B,T;

      k = mpn_fft_best_k (n, sqr);
      K2 = 1<<k;
      maxLK = (K2>BITS_PER_MP_LIMB) ? K2 : BITS_PER_MP_LIMB;
      M2 = n*BITS_PER_MP_LIMB/K2;
      l = n/K2;
      Nprime2 = ((2 * M2+k+2+maxLK)/maxLK)*maxLK; /* ceil()(2*M2+k+3)/maxLK)*maxLK*/
      nprime2 = Nprime2/BITS_PER_MP_LIMB;
      Mp2 = Nprime2/K2;

      Ap = TMP_ALLOC_MP_PTRS (K2);
      Bp = TMP_ALLOC_MP_PTRS (K2);
      A = TMP_ALLOC_LIMBS (2 * K2 * (nprime2 + 1));
      T = TMP_ALLOC_LIMBS (nprime2 + 1);
      B = A + K2 * (nprime2 + 1);
      _fft_l = TMP_ALLOC_TYPE (k + 1, int*);
      for (i = 0; i <= k; i++)
	_fft_l[i] = TMP_ALLOC_TYPE (1<<i, int);
      mpn_fft_initl (_fft_l, k);

      TRACE (printf ("recurse: %dx%d limbs -> %d times %dx%d (%1.2f)\n", n,
		    n, K2, nprime2, nprime2, 2.0*(double)n/nprime2/K2));
      for (i = 0; i < K; i++,ap++,bp++)
	{
	  mpn_fft_norm (*ap, n);
	  if (!sqr) mpn_fft_norm (*bp, n);
	  mpn_mul_fft_internal (*ap, *ap, *bp, n, k, K2, Ap, Bp, A, B, nprime2,
			       l, Mp2, _fft_l, T, 1);
	}
    }
  else
    {
      mp_ptr a, b, tp, tpn;
      mp_limb_t cc;
      int n2 = 2 * n;
      tp = TMP_ALLOC_LIMBS (n2);
      tpn = tp+n;
      TRACE (printf ("  mpn_mul_n %d of %d limbs\n", K, n));
      for (i = 0; i < K; i++)
	{
	  a = *ap++;
	  b = *bp++;
	  if (sqr)
	    mpn_sqr_n (tp, a, n);
	  else
	    mpn_mul_n (tp, b, a, n);
	  if (a[n] != 0)
	    cc = mpn_add_n (tpn, tpn, b, n);
	  else
	    cc = 0;
	  if (b[n] != 0)
	    cc += mpn_add_n (tpn, tpn, a, n) + a[n];
	  if (cc != 0)
	    {
	      cc = mpn_add_1 (tp, tp, n2, cc);
	      ASSERT_NOCARRY (mpn_add_1 (tp, tp, n2, cc));
	    }
	  a[n] = mpn_sub_n (a, tp, tpn, n) && mpn_add_1 (a, a, n, CNST_LIMB(1));
	}
    }
  TMP_FREE(marker);
}


/* input: A^[l[k][0]] A^[l[k][1]] ... A^[l[k][K-1]]
   output: K*A[0] K*A[K-1] ... K*A[1] */

static void
mpn_fft_fftinv (mp_ptr *Ap, int K, mp_size_t omega, mp_size_t n, mp_ptr tp)
{
  if (K == 2)
    {
#if HAVE_NATIVE_mpn_addsub_n
      if (mpn_addsub_n (Ap[0], Ap[1], Ap[0], Ap[1], n + 1) & 1)
        Ap[1][n] = mpn_add_1 (Ap[1], Ap[1], n, CNST_LIMB(1));
#else
      MPN_COPY (tp, Ap[0], n + 1);
      mpn_add_n (Ap[0], Ap[0], Ap[1], n + 1);
      if (mpn_sub_n (Ap[1], tp, Ap[1], n + 1))
        Ap[1][n] = mpn_add_1 (Ap[1], Ap[1], n, CNST_LIMB(1));
#endif
    }
  else
    {
      int j, K2 = K / 2;
      mp_ptr *Bp = Ap + K2, tmp;
      TMP_DECL(marker);

      TMP_MARK(marker);
      tmp = TMP_ALLOC_LIMBS (n + 1);
      mpn_fft_fftinv (Ap, K2, 2 * omega, n, tp);
      mpn_fft_fftinv (Bp, K2, 2 * omega, n, tp);
      /* A[j]     <- A[j] + omega^j A[j+K/2]
	 A[j+K/2] <- A[j] + omega^(j+K/2) A[j+K/2] */
      for (j = 0; j < K2; j++,Ap++,Bp++)
	{
	  MPN_COPY (tp, Bp[0], n + 1);
	  mpn_fft_mul_2exp_modF (Bp[0], (j + K2) * omega, n, tmp);
	  mpn_fft_add_modF (Bp[0], Ap[0], n);
	  mpn_fft_mul_2exp_modF (tp, j * omega, n, tmp);
	  mpn_fft_add_modF (Ap[0], tp, n);
	}
      TMP_FREE(marker);
    }
}


/* A <- A/2^k mod 2^(n*BITS_PER_MP_LIMB)+1 */
static void
mpn_fft_div_2exp_modF (mp_ptr ap, int k, mp_size_t n, mp_ptr tp)
{
  int i;

  i = 2 * n * BITS_PER_MP_LIMB;
  i = (i - k) % i;
  mpn_fft_mul_2exp_modF (ap, i, n, tp);
  /* 1/2^k = 2^(2nL-k) mod 2^(n*BITS_PER_MP_LIMB)+1 */
  /* normalize so that A < 2^(n*BITS_PER_MP_LIMB)+1 */
  mpn_fft_norm (ap, n);
}


/* R <- A mod 2^(n*BITS_PER_MP_LIMB)+1, n<=an<=3*n */
static void
mpn_fft_norm_modF (mp_ptr rp, mp_ptr ap, mp_size_t n, mp_size_t an)
{
  mp_size_t l;

  if (an > 2 * n)
    {
      l = n;
      rp[n] = mpn_add_1 (rp + an - 2 * n, ap + an - 2 * n, 3 * n - an,
			mpn_add_n (rp, ap, ap + 2 * n, an - 2 * n));
    }
  else
    {
      l = an - n;
      MPN_COPY (rp, ap, n);
      rp[n] = 0;
    }
  if (mpn_sub_n (rp, rp, ap + n, l))
    {
      if (mpn_sub_1 (rp + l, rp + l, n + 1 - l, CNST_LIMB(1)))
	rp[n] = mpn_add_1 (rp, rp, n, CNST_LIMB(1));
    }
}


static void
mpn_mul_fft_internal (mp_ptr op, mp_srcptr n, mp_srcptr m, mp_size_t pl,
		      int k, int K,
		      mp_ptr *Ap, mp_ptr *Bp,
		      mp_ptr A, mp_ptr B,
		      mp_size_t nprime, mp_size_t l, mp_size_t Mp,
		      int **_fft_l,
		      mp_ptr T, int rec)
{
  int i, sqr, pla, lo, sh, j;
  mp_ptr p;

  sqr = n == m;

  TRACE (printf ("pl=%d k=%d K=%d np=%d l=%d Mp=%d rec=%d sqr=%d\n",
		 pl,k,K,nprime,l,Mp,rec,sqr));

  /* decomposition of inputs into arrays Ap[i] and Bp[i] */
  if (rec)
    for (i = 0; i < K; i++)
      {
	Ap[i] = A + i * (nprime + 1); Bp[i] = B + i * (nprime + 1);
	/* store the next M bits of n into A[i] */
	/* supposes that M is a multiple of BITS_PER_MP_LIMB */
	MPN_COPY (Ap[i], n, l); n += l;
	MPN_ZERO (Ap[i]+l, nprime + 1 - l);
	/* set most significant bits of n and m (important in recursive calls) */
	if (i == K - 1)
	  Ap[i][l] = n[0];
	mpn_fft_mul_2exp_modF (Ap[i], i * Mp, nprime, T);
	if (!sqr)
	  {
	    MPN_COPY (Bp[i], m, l); m += l;
	    MPN_ZERO (Bp[i] + l, nprime + 1 - l);
	    if (i == K - 1)
	      Bp[i][l] = m[0];
	    mpn_fft_mul_2exp_modF (Bp[i], i * Mp, nprime, T);
	  }
      }

  /* direct fft's */
  if (sqr)
    mpn_fft_fft_sqr (Ap, K, _fft_l + k, 2 * Mp, nprime, 1, T);
  else
    mpn_fft_fft (Ap, Bp, K, _fft_l + k, 2 * Mp, nprime, 1, T);

  /* term to term multiplications */
  mpn_fft_mul_modF_K (Ap, (sqr) ? Ap : Bp, nprime, K);

  /* inverse fft's */
  mpn_fft_fftinv (Ap, K, 2 * Mp, nprime, T);

  /* division of terms after inverse fft */
  for (i = 0; i < K; i++)
    mpn_fft_div_2exp_modF (Ap[i], k + ((K - i) % K) * Mp, nprime, T);

  /* addition of terms in result p */
  MPN_ZERO (T, nprime + 1);
  pla = l * (K - 1) + nprime + 1; /* number of required limbs for p */
  p = B; /* B has K*(n' + 1) limbs, which is >= pla, i.e. enough */
  MPN_ZERO (p, pla);
  sqr = 0; /* will accumulate the (signed) carry at p[pla] */
  for (i = K - 1,lo = l * i + nprime,sh = l * i; i >= 0; i--,lo -= l,sh -= l)
    {
      mp_ptr n = p+sh;
      j = (K-i)%K;
      if (mpn_add_n (n, n, Ap[j], nprime + 1))
	sqr += mpn_add_1 (n + nprime + 1, n + nprime + 1, pla - sh - nprime - 1, CNST_LIMB(1));
      T[2 * l]=i + 1; /* T = (i + 1)*2^(2*M) */
      if (mpn_cmp (Ap[j],T,nprime + 1)>0)
	{ /* subtract 2^N'+1 */
	  sqr -= mpn_sub_1 (n, n, pla - sh, CNST_LIMB(1));
	  sqr -= mpn_sub_1 (p + lo, p + lo, pla - lo, CNST_LIMB(1));
	}
    }
    if (sqr == -1)
      {
	if ((sqr = mpn_add_1 (p + pla - pl,p + pla - pl,pl, CNST_LIMB(1))))
	  {
	    /* p[pla-pl]...p[pla-1] are all zero */
	    mpn_sub_1 (p + pla - pl - 1, p + pla - pl - 1, pl + 1, CNST_LIMB(1));
	    mpn_sub_1 (p + pla - 1, p + pla - 1, 1, CNST_LIMB(1));
	  }
      }
    else if (sqr == 1)
      {
	if (pla >= 2 * pl)
	  {
	    while ((sqr = mpn_add_1 (p + pla - 2 * pl, p + pla - 2 * pl, 2 * pl, sqr)))
	      ;
	  }
	else
	  {
	    sqr = mpn_sub_1 (p + pla - pl, p + pla - pl, pl, sqr);
	    ASSERT (sqr == 0);
	  }
      }
    else
      ASSERT (sqr == 0);

    /* here p < 2^(2M) [K 2^(M(K-1)) + (K-1) 2^(M(K-2)) + ... ]
       < K 2^(2M) [2^(M(K-1)) + 2^(M(K-2)) + ... ]
       < K 2^(2M) 2^(M(K-1))*2 = 2^(M*K+M+k+1) */
    mpn_fft_norm_modF (op, p, pl, pla);
}


/* op <- n*m mod 2^N+1 with fft of size 2^k where N=pl*BITS_PER_MP_LIMB
   n and m have respectively nl and ml limbs
   op must have space for pl+1 limbs
   One must have pl = mpn_fft_next_size (pl, k).
*/

void
mpn_mul_fft (mp_ptr op, mp_size_t pl,
             mp_srcptr n, mp_size_t nl,
             mp_srcptr m, mp_size_t ml,
             int k)
{
  int K,maxLK,i,j;
  mp_size_t N, Nprime, nprime, M, Mp, l;
  mp_ptr *Ap,*Bp, A, T, B;
  int **_fft_l;
  int sqr = (n == m && nl == ml);
  TMP_DECL(marker);

  TRACE (printf ("\nmpn_mul_fft pl=%ld nl=%ld ml=%ld k=%d\n", pl, nl, ml, k));
  ASSERT_ALWAYS (mpn_fft_next_size (pl, k) == pl);

  TMP_MARK(marker);
  N = pl * BITS_PER_MP_LIMB;
  _fft_l = TMP_ALLOC_TYPE (k + 1, int*);
  for (i = 0; i <= k; i++)
    _fft_l[i] = TMP_ALLOC_TYPE (1<<i, int);
  mpn_fft_initl (_fft_l, k);
  K = 1<<k;
  M = N/K;	/* N = 2^k M */
  l = M/BITS_PER_MP_LIMB;
  maxLK = (K>BITS_PER_MP_LIMB) ? K : BITS_PER_MP_LIMB;

  Nprime = ((2 * M + k + 2 + maxLK) / maxLK) * maxLK; /* ceil((2*M+k+3)/maxLK)*maxLK; */
  nprime = Nprime / BITS_PER_MP_LIMB;
  TRACE (printf ("N=%d K=%d, M=%d, l=%d, maxLK=%d, Np=%d, np=%d\n",
		 N, K, M, l, maxLK, Nprime, nprime));
  if (nprime >= (sqr ? SQR_FFT_MODF_THRESHOLD : MUL_FFT_MODF_THRESHOLD))
    {
      maxLK = (1 << mpn_fft_best_k (nprime,n == m)) * BITS_PER_MP_LIMB;
      if (Nprime % maxLK)
	{
	  Nprime = ((Nprime / maxLK) + 1) * maxLK;
	  nprime = Nprime / BITS_PER_MP_LIMB;
	}
      TRACE (printf ("new maxLK=%d, Np=%d, np=%d\n", maxLK, Nprime, nprime));
    }

  T = TMP_ALLOC_LIMBS (nprime + 1);
  Mp = Nprime/K;

  TRACE (printf ("%dx%d limbs -> %d times %dx%d limbs (%1.2f)\n",
		pl,pl,K,nprime,nprime,2.0*(double)N/Nprime/K);
	 printf ("   temp space %ld\n", 2 * K * (nprime + 1)));

  A = __GMP_ALLOCATE_FUNC_LIMBS (2 * K * (nprime + 1));
  B = A + K * (nprime + 1);
  Ap = TMP_ALLOC_MP_PTRS (K);
  Bp = TMP_ALLOC_MP_PTRS (K);
  /* special decomposition for main call */
  for (i = 0; i < K; i++)
    {
      Ap[i] = A + i * (nprime + 1); Bp[i] = B + i * (nprime + 1);
      /* store the next M bits of n into A[i] */
      /* supposes that M is a multiple of BITS_PER_MP_LIMB */
      if (nl > 0)
	{
	  j = (nl>=l) ? l : nl; /* limbs to store in Ap[i] */
	  MPN_COPY (Ap[i], n, j); n += l;
	  MPN_ZERO (Ap[i] + j, nprime + 1 - j);
	  mpn_fft_mul_2exp_modF (Ap[i], i * Mp, nprime, T);
	}
      else MPN_ZERO (Ap[i], nprime + 1);
      nl -= l;
      if (n != m)
	{
	  if (ml > 0)
	    {
	      j = (ml>=l) ? l : ml; /* limbs to store in Bp[i] */
	      MPN_COPY (Bp[i], m, j); m += l;
	      MPN_ZERO (Bp[i] + j, nprime + 1 - j);
	      mpn_fft_mul_2exp_modF (Bp[i], i * Mp, nprime, T);
	    }
	  else
	    MPN_ZERO (Bp[i], nprime + 1);
	}
      ml -= l;
    }
  mpn_mul_fft_internal (op, n, m, pl, k, K, Ap, Bp, A, B, nprime, l, Mp, _fft_l, T, 0);
  TMP_FREE(marker);
  __GMP_FREE_FUNC_LIMBS (A, 2 * K * (nprime + 1));
}


/* Multiply {n,nl}*{m,ml} and write the result to {op,nl+ml}.

   FIXME: Duplicating the result like this is wasteful, do something better
   perhaps at the norm_modF stage above. */

void
mpn_mul_fft_full (mp_ptr op,
                  mp_srcptr n, mp_size_t nl,
                  mp_srcptr m, mp_size_t ml)
{
  mp_ptr pad_op;
  mp_size_t pl;
  int k;
  int sqr = (n == m && nl == ml);

  k = mpn_fft_best_k (nl + ml, sqr);
  pl = mpn_fft_next_size (nl + ml, k);

  TRACE (printf ("mpn_mul_fft_full nl=%ld ml=%ld -> pl=%ld k=%d\n", nl, ml, pl, k));

  pad_op = __GMP_ALLOCATE_FUNC_LIMBS (pl + 1);
  mpn_mul_fft (pad_op, pl, n, nl, m, ml, k);

  ASSERT_MPN_ZERO_P (pad_op + nl + ml, pl + 1 - (nl + ml));
  MPN_COPY (op, pad_op, nl + ml);

  __GMP_FREE_FUNC_LIMBS (pad_op, pl + 1);
}
