/* mpfr_get_str -- output a floating-point number to a string

Copyright 1999, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.
This function was contributed by Alain Delplanque and Paul Zimmermann.

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"

static double _mpfr_ceil _PROTO ((double));
static int mpfr_get_str_aux _PROTO ((char *, mp_exp_t *, mp_limb_t *,
		       mp_size_t, mp_exp_t, long, int, size_t, mp_rnd_t));
static mp_exp_t mpfr_get_str_compute_g _PROTO ((int, mp_exp_t));

static char num_to_text[] = "0123456789abcdefghijklmnopqrstuvwxyz";

/* for 2 <= b <= 36, log_b2[b-2] + log_b2_low[b-2] is a 76-bit upper 
   approximation of log(2)/log(b), with log_b2[b-2] having 23 significative
   bits only. These approximations were computed with the following program.

#include <stdio.h>
#include "gmp.h"
#include "mpfr.h"

double log_b2[35], log_b2_low[35];

main()
{
  int beta;
  mpfr_t l, l0;

  for (beta=2;beta<=36;beta++)
    {
      mpfr_init2 (l, 77);
      mpfr_set_ui (l, beta, GMP_RNDD);
      mpfr_log2 (l, l, GMP_RNDD);
      mpfr_ui_div (l, 1, l, GMP_RNDU);
      mpfr_init2 (l0, 23);
      mpfr_set (l0, l, GMP_RNDD);
      mpfr_sub (l, l, l0, GMP_RNDU);
      mpfr_prec_round (l, 53, GMP_RNDU);
      log_b2[beta-2] = mpfr_get_d (l0, GMP_RNDU);
      log_b2_low[beta-2] = mpfr_get_d (l, GMP_RNDU);
      mpfr_clear (l0);
      mpfr_clear (l);
    }

  printf ("static const double log_b2[35] = {");
  for (beta=2;beta<=36;beta++)
    {
      printf ("\n%1.20e", log_b2[beta-2]);
      if (beta < 36) printf (",");
    }
  printf ("\n};\n");

  printf ("static const double log_b2_low[35] = {");
  for (beta=2;beta<=36;beta++)
    {
      printf ("\n%1.20e", log_b2_low[beta-2]);
      if (beta < 36) printf (",");
    }
  printf ("\n};\n");
}
*/


static const double log_b2[35] = {
  1.00000000000000000000e+00,
  6.30929708480834960938e-01,
  5.00000000000000000000e-01,
  4.30676519870758056641e-01,
  3.86852800846099853516e-01,
  3.56207132339477539062e-01,
  3.33333313465118408203e-01,
  3.15464854240417480469e-01,
  3.01029980182647705078e-01,
  2.89064824581146240234e-01,
  2.78942942619323730469e-01,
  2.70238101482391357422e-01,
  2.62649476528167724609e-01,
  2.55958020687103271484e-01,
  2.50000000000000000000e-01,
  2.44650512933731079102e-01,
  2.39812463521957397461e-01,
  2.35408902168273925781e-01,
  2.31378197669982910156e-01,
  2.27670222520828247070e-01,
  2.24243819713592529297e-01,
  2.21064716577529907227e-01,
  2.18104273080825805664e-01,
  2.15338259935379028320e-01,
  2.12746024131774902344e-01,
  2.10309892892837524414e-01,
  2.08014577627182006836e-01,
  2.05846816301345825195e-01,
  2.03795045614242553711e-01,
  2.01849073171615600586e-01,
  1.99999988079071044922e-01,
  1.98239862918853759766e-01,
  1.96561604738235473633e-01,
  1.94959014654159545898e-01,
  1.93426400423049926758e-01
};

static const double log_b2_low[35] = {
  0.00000000000000000000e+00,
  4.50906224761620348192e-08,
  0.00000000000000000000e+00,
  3.82026349940294905572e-08,
  6.38844173335462308442e-09,
  5.47685446374516835508e-08,
  1.98682149251302105391e-08,
  2.25453112380810174096e-08,
  1.54813334901356166450e-08,
  1.73674161903183898500e-09,
  3.03180611272229558617e-09,
  5.29449283838722401896e-08,
  5.85090258233695360801e-08,
  4.12271221790330610183e-09,
  0.00000000000000000000e+00,
  2.91844949512882013763e-08,
  3.04617404727474892263e-09,
  1.11983643106657188415e-08,
  1.54897762641074502508e-08,
  2.61761247509117662805e-08,
  4.50398291018069050027e-09,
  1.28799738389232369510e-08,
  1.89047057535652783545e-08,
  1.91013174970147452786e-08,
  2.94215882512624420131e-08,
  2.49643149546191168760e-08,
  2.00493274507626165734e-08,
  1.61590886321114899524e-08,
  1.47626363632181082532e-09,
  1.34104842501325808254e-08,
  1.19209289550781256617e-08,
  2.51706771840338761560e-10,
  2.74945871340834855649e-08,
  7.23962676182708790191e-09,
  3.19422086667731154221e-09
};

/* copy most important limbs of {op, n2} in {rp, n1} */
/* if n1 > n2 put 0 in low limbs of {rp, n1} */
#define MPN_COPY2(rp, n1, op, n2) \
  if ((n1) <= (n2)) \
    { \
      MPN_COPY ((rp), (op) + (n2) - (n1), (n1)); \
    } \
  else \
    { \
      MPN_COPY ((rp) + (n1) - (n2), (op), (n2)); \
      MPN_ZERO ((rp), (n1) - (n2)); \
    }

static double
_mpfr_ceil (double x)
{
  double y = (double) (long int) x;
  return ((y < x) ? y + 1.0 : y);
}

/* this function computes an approximation of b^e in {a, n}, with exponent
   stored in exp_r. The computed value is rounded towards zero (truncated).
   It returns an integer f such that the final error is bounded by 2^f ulps,
   that is:
   a*2^exp_r <= b^e <= 2^exp_r (a + 2^f),
   where a represents {a, n}, i.e. the integer 
   a[0] + a[1]*B + ... + a[n-1]*B^(n-1) where B=2^BITS_PER_MP_LIMB */
long
mpn_exp (mp_limb_t *a, mp_exp_t *exp_r, int b, mp_exp_t e, size_t n)
{
  mp_limb_t *c, B;
  mp_exp_t f, h;
  int i;
  unsigned long t; /* number of bits in e */
  unsigned long bits;
  size_t n1;
  unsigned int erreur;           /* (number - 1) of loop a^2b inexact */
                                 /* erreur == t meens no error */
  int err_s_a2 = 0;
  int err_s_ab = 0;              /* number of error when shift A^2, AB */
  TMP_DECL(marker);

  MPFR_ASSERTN(e > 0);
  MPFR_ASSERTN((2 <= b) && (b <= 36));

  TMP_MARK(marker);

  /* initialization of a, b, f, h */

  /* normalize the base */
  B = (mp_limb_t) b;
  count_leading_zeros (h, B);

  bits = BITS_PER_MP_LIMB - h;

  B = B << h;
  h = - h;

  /* allocate space for A and set it to B */
  c = (mp_limb_t*) TMP_ALLOC(2 * n * BYTES_PER_MP_LIMB);
  a [n - 1] = B;
  MPN_ZERO (a, n - 1);
  /* initial exponent for A: invariant is A = {a, n} * 2^f */
  f = h - (n - 1) * BITS_PER_MP_LIMB;

  /* determine number of bits in e */
  count_leading_zeros (t, (mp_limb_t) e);

  t = BITS_PER_MP_LIMB - t; /* number of bits of exponent e */

  erreur = t;

  MPN_ZERO (c, 2 * n);

  for (i = t - 2; i >= 0; i--)
    {

      /* determine precision needed */
      bits = n * BITS_PER_MP_LIMB - mpn_scan1 (a, 0);
      n1 = (n * BITS_PER_MP_LIMB - bits) / BITS_PER_MP_LIMB;

      /* square of A : {c+2n1, 2(n-n1)} = {a+n1, n-n1}^2 */
      mpn_sqr_n (c + 2 * n1, a + n1, n - n1);

      /* set {c+n, 2n1-n} to 0 : {c, n} = {a, n}^2*K^n */

      f = 2 * f + n * BITS_PER_MP_LIMB;
      if ((c[2*n - 1] & MPFR_LIMB_HIGHBIT) == 0)
	{
	  /* shift A by one bit to the left */
	  mpn_lshift (a, c + n, n, 1);
	  a[0] |= mpn_lshift (c + n - 1, c + n - 1, 1, 1);
	  f --;
	  if (erreur != t)
	    err_s_a2 ++;
	}
      else
	MPN_COPY (a, c + n, n);

      if ((erreur == t) && (2 * n1 <= n) &&
	  (mpn_scan1 (c + 2 * n1, 0) < (n - 2 * n1) * BITS_PER_MP_LIMB))
	erreur = i;

      if (e & (1 << i))
	{
	  /* multiply A by B */
	  c[2 * n - 1] = mpn_mul_1 (c + n - 1, a, n, B);
	  f += h + BITS_PER_MP_LIMB;
	  if ((c[2 * n - 1] & MPFR_LIMB_HIGHBIT) == 0)
	    { /* shift A by one bit to the left */
	      mpn_lshift (a, c + n, n, 1);
	      a[0] |= mpn_lshift (c + n - 1, c + n - 1, 1, 1);
	      f --;
	    }
	  else
	    {
	      MPN_COPY (a, c + n, n);
	      if (erreur != t)
		err_s_ab ++;
	    }
	  if ((erreur == t) && (c[n - 1] != 0))
            erreur = i;
	  
	}
    }

  TMP_FREE(marker);

  *exp_r = f;  

  if (erreur == t)
    return -1; /* exact */
  else
    {
      /* if there are p loops after the first inexact result, with
	 j shifts in a^2 and l shifts in a*b, then the final error is
	 at most 2^(p+ceil((j+1)/2)+l+1)*ulp(res).
         This is bounded by 2^(5/2*t-1/2) where t is the number of bits of e.
      */
      erreur = erreur + err_s_ab + err_s_a2 / 2 + 3;
      if ((erreur - 1) >= ((n * BITS_PER_MP_LIMB - 1) / 2))
	erreur = n * BITS_PER_MP_LIMB; /* completely wrong: this is very
                                        unlikely to happen since erreur is
                                       at most 5/2*log_2(e), and 
                                       n * BITS_PER_MP_LIMB is at least
                                       3*log_2(e) */
    }

  return erreur;  
}


/* Input: an approximation r*2^f of an real Y, with |r*2^f-Y| <= 2^(e+f).
   Returns if possible in the string s the mantissa corresponding to 
   the integer nearest to Y, within the direction rnd, and returns the
   the exponent in exp.
   n is the number of limbs of r.
   e represents the maximal error in the approximation of Y
      (e < 0 iff the approximation is exact, i.e. r*2^f = Y).
   b is the wanted base (2 <= b <= 36).
   m is the number of wanted digits in the mantissa.
   rnd is the rounding mode.
   It is assumed that b^(m-1) <= Y < b^(m+1), thus the returned value 
   satisfies b^(m-1) <= rnd(Y) < b^(m+1).

   Return value:
   - the direction of rounding (-1, 0, 1) if rounding is possible
   - 2 otherwise (rounding is not possible)
*/
static int
mpfr_get_str_aux (char *str, mp_exp_t *exp, mp_limb_t *r, mp_size_t n,
		  mp_exp_t f, long e, int b, size_t m, mp_rnd_t rnd)
{
  int dir;                  /* direction of the rounded result */
  mp_limb_t ret = 0;        /* possible carry in addition */
  mp_size_t i0, j0;         /* number of limbs and bits of Y */
  unsigned char *str1;      /* string of m+2 characters */
  size_t size_s1;           /* length of str1 */
  mp_rnd_t rnd1;
  size_t i;
  int exact = (e < 0);
  TMP_DECL(marker);

  /* if f > 0, then the maximal error 2^(e+f) is larger than 2 so we can't
     determine the integer Y */
  MPFR_ASSERTN(f <= 0);
  /* if f is too small, then r*2^f is smaller than 1 */
  MPFR_ASSERTN(f > (-n * BITS_PER_MP_LIMB));

  TMP_MARK(marker);

  /* R = 2^f sum r[i]K^(i)
     r[i] = (r_(i,k-1)...r_(i,0))_2
     R = sum r(i,j)2^(j+ki+f)
     the bits from R are referenced by pairs (i,j) */

  /* check if is possible to round r with rnd mode
     where |r*2^f-Y| <= 2^(e+f)
     the exponent of R is: f + n*BITS_PER_MP_LIMB
     we must have e + f == f + n*BITS_PER_MP_LIMB - err
     err = n*BITS_PER_MP_LIMB - e
     R contains exactly -f bits after the integer point:
     to determine the nearest integer, we thus need a precision of
     n * BITS_PER_MP_LIMB + f */

  if (exact || mpfr_can_round_raw (r, n, (mp_size_t) 1,
            n * BITS_PER_MP_LIMB - e, GMP_RNDN, rnd, n * BITS_PER_MP_LIMB + f))
    {
      /* compute the nearest integer to R */

      /* bit of weight 0 in R has position j0 in limb r[i0] */
      i0 = (-f) / BITS_PER_MP_LIMB;
      j0 = (-f) % BITS_PER_MP_LIMB;

      ret = mpfr_round_raw_generic (r + i0, r, n * BITS_PER_MP_LIMB, 0,
				     n * BITS_PER_MP_LIMB + f, rnd, &dir, 0);

      /* warning: mpfr_round_raw_generic returns 2 or -2 in case of even
         rounding */
      if (dir > 0)      /* when dir = MPFR_EVEN_INEX */
	dir = 1;
      else if (dir < 0) /* when dir = -MPFR_EVEN_INEX */
	dir = -1;

      if (ret) /* Y is a power of 2 */
	{
	  if (j0)
	    r[n - 1] = MPFR_LIMB_HIGHBIT >> (j0 - 1);
	  else /* j0=0, necessarily i0 >= 1 otherwise f=0 and r is exact */
	    {
	      r[n - 1] = ret;
              r[--i0] = 0; /* set to zero the new low limb */
	    }
	}
      else /* shift r to the right by (-f) bits (i0 already done) */
	{
	  if (j0)
            mpn_rshift (r + i0, r + i0, n - i0, j0);
	}

      /* now the rounded value Y is in {r+i0, n-i0} */

      /* convert r+i0 into base b */
      str1 = (unsigned char*) TMP_ALLOC (m + 3); /* need one extra character for mpn_get_str */
      size_s1 = mpn_get_str (str1, b, r + i0, n - i0);

      /* round str1 */
      MPFR_ASSERTN(size_s1 >= m);
      *exp = size_s1 - m; /* number of superfluous characters */

      /* if size_s1 = m + 2, necessarily we have b^(m+1) as result,
	 and the result will not change */

      /* so we have to double-round only when size_s1 = m + 1 and 
	 (i) the result is inexact 
	 (ii) or the last digit is non-zero */
      if ((size_s1 == m + 1) && ((dir != 0) || (str1[size_s1 - 1] != 0)))
        {
          /* rounding mode */
          rnd1 = rnd;    

          /* round to nearest case */
          if (rnd == GMP_RNDN)    
            {
              if (2 * str1[size_s1 - 1] == b)	   
                {
                  if (dir == -1)
                    rnd1 = GMP_RNDU;
                  else if (dir == 0) /* exact: even rounding */
		    {
		      if (exact)
			rnd1 = ((str1[size_s1-2] & 1) == 0)
			  ? GMP_RNDD : GMP_RNDU;
		      else
                        goto cannot_round;
		    }
                  else
                    rnd1 = GMP_RNDD;
                }
              else if (2 * str1[size_s1 - 1] < b)
                rnd1 = GMP_RNDD;
	      else
                rnd1 = GMP_RNDU;
            }

	  /* now rnd1 is either GMP_RNDD or GMP_RNDZ -> truncate
	                     or GMP_RDNU -> round towards infinity */

          /* round away from zero */
          if (rnd1 == GMP_RNDU)
	    {
	      if (str1[size_s1 - 1] != 0)
		{
                  /* the carry cannot propagate to the whole string, since
                     Y = x*b^(m-g) < 2*b^m <= b^(m+1)-b
                     where x is the input float */
                  MPFR_ASSERTN(size_s1 >= 2);
                  i = size_s1 - 2;
                  while (str1[i] == b - 1)
                    {
                      MPFR_ASSERTD(i > 0);
                      str1[i--] = 0;
                    }
                  str1[i]++;
                }
	      dir = 1;    
	    }
          /* round toward zero (truncate) */
          else
            dir = -1;
        }

      /* copy str1 into str and convert to ASCII */
      for (i = 0; i < m; i++)
	str[i] = num_to_text[(int) str1[i]];
      str[m] = 0;
    }
  /* mpfr_can_round_raw failed: rounding is not possible */
  else
    {
    cannot_round:
      dir = 2;
    }
  
  TMP_FREE(marker);

  return (dir);
}

/* returns ceil(e/log_2(beta)) */
static mp_exp_t
mpfr_get_str_compute_g (int beta, mp_exp_t e)
{
  double g0, g1;
  mp_exp_t g;
  
  g0 = (double) e * log_b2[beta - 2];
  g1 = (double) e * log_b2_low[beta - 2];
  g = (mp_exp_t) _mpfr_ceil (g0);
  g0 -= (double) g;
  return g + (mp_exp_t) _mpfr_ceil (g0 + g1);
}

/* prints the mantissa of x in the string s, and writes the corresponding
   exponent in e.
   x is rounded with direction rnd, m is the number of digits of the mantissa,
   b is the given base (2 <= b <= 36).

   Return value:
   if s=NULL, allocates a string to store the mantissa, with
   m characters, plus a final '\0', plus a possible minus sign
   (thus m+1 or m+2 characters).

   Important: when you call this function with s=NULL, don't forget to free
   the memory space allocated, with free(s, strlen(s)).
*/
char*
mpfr_get_str (char *s, mp_exp_t *e, int b, size_t m, mpfr_srcptr x, mp_rnd_t rnd)
{
  int exact;                      /* exact result */
  mp_exp_t exp, g;
  mp_exp_t prec, log_2prec; /* precision of the computation */
  long err; 
  mp_limb_t *a;
  mp_exp_t exp_a;
  mp_limb_t *result;
  mp_limb_t *xp, *x1;
  mp_limb_t *reste;
  size_t nx, nx1;
  size_t n, i;
  char *s0;
  int neg;

  /* if exact = 1 then err is undefined */
  /* otherwise err is such that |x*b^(m-g)-a*2^exp_a| < 2^(err+exp_a) */

  TMP_DECL(marker);

  /* is the base valid? */
  if (b < 2 || b > 36)
    return NULL;

  if (m == 0)
    {
      m = (size_t) _mpfr_ceil (__mp_bases[b].chars_per_bit_exactly 
                    * (double) MPFR_PREC(x));
      if (m < 2)
        m = 2;
    }

  /* Do not use MPFR_PREC_MIN as this applies to base 2 only. Perhaps we
     should allow n == 1 for directed rounding modes and odd bases. */
  MPFR_ASSERTN (m >= 2);

  if (MPFR_IS_NAN(x))
    {
      if (s == NULL)
        s = (char*) (*__gmp_allocate_func) (6);
      strcpy (s, "@NaN@");
      return s;
    }

  neg = MPFR_SIGN(x) < 0; /* 0 if positive, 1 if negative */

  if (MPFR_IS_INF(x))
    {
      if (s == NULL)
        s = (char*) (*__gmp_allocate_func) (neg + 6);
      strcpy (s, (neg) ? "-@Inf@" : "@Inf@");
      return s;
    }

  /* x is a floating-point number */

  if (MPFR_IS_ZERO(x))
    {
      if (s == NULL)
        s = (char*) (*__gmp_allocate_func) (neg + m + 1);
      s0 = s;
      if (neg)
        *s++ = '-';
      memset (s, '0', m);
      s[m] = '\0';
      *e = 0; /* a bit like frexp() in ISO C99 */
      return s0; /* strlen(s0) = neg + m */
    }

  /* si x < 0, on se ram`ene au cas x > 0 */
  if (neg)
    {
      switch (rnd)
	{
	case GMP_RNDU : rnd = GMP_RNDD; break;
	case GMP_RNDD : rnd = GMP_RNDU; break;
	}
    }

  if (s == NULL)
    s = (char*) (*__gmp_allocate_func) (neg + m + 1);
  s0 = s;
  if (neg)
    *s++ = '-';

  xp = MPFR_MANT(x);

  if (IS_POW2(b))
    {
      int pow2;
      mp_exp_t f, r;
      mp_limb_t *x1;
      mp_size_t nb;
      int inexp;

      count_leading_zeros (pow2, (mp_limb_t) b);
      pow2 = BITS_PER_MP_LIMB - pow2 - 1; /* base = 2^pow2 */
 
      /* set MPFR_EXP(x) = f*pow2 + r, 1 <= r <= pow2 */
      f = (MPFR_GET_EXP (x) - 1) / pow2;
      r = MPFR_GET_EXP (x) - f * pow2;
      if (r <= 0)
	{
	  f --;
	  r += pow2;
	}

      /* the first digit will contain only r bits */
      prec = (m - 1) * pow2 + r;
      n = (prec - 1) / BITS_PER_MP_LIMB + 1;

      TMP_MARK (marker);
      x1 = (mp_limb_t*) TMP_ALLOC((n + 1) * sizeof (mp_limb_t));
      nb = n * BITS_PER_MP_LIMB - prec;
      /* round xp to the precision prec, and put it into x1
	 put the carry into x1[n] */
      if ((x1[n] = mpfr_round_raw_generic (x1, xp, MPFR_PREC(x), MPFR_ISNEG(x),
                                           prec, rnd, &inexp, 0)))
        {
	  /* overflow when rounding x: x1 = 2^prec */
	  if (r == pow2)    /* prec = m * pow2,
			       2^prec will need (m+1) digits in base 2^pow2 */
	    {
	      /* divide x1 by 2^pow2, and increase the exponent */
	      mpn_rshift (x1, x1, n + 1, pow2);
	      f ++;
	    }
	  else /* 2^prec needs still m digits, but x1 may need n+1 limbs */
            n ++;
        }
      
      /* it remains to shift x1 by nb limbs to the right, since mpn_get_str
         expects a right-normalized number */
      if (nb != 0)
        {
          mpn_rshift (x1, x1, n, nb);
          /* the most significant word may be zero */
          if (x1[n - 1] == 0)
            n --;
        }
      
      mpn_get_str ((unsigned char*) s, b, x1, n);
      for (i=0; i<m; i++)
        s[i] = num_to_text[(int) s[i]];
      s[m] = 0;

      /* the exponent of s is f + 1 */
      *e = f + 1;

      TMP_FREE(marker);

      return (s0);
   }

  g = mpfr_get_str_compute_g (b, MPFR_GET_EXP (x) - 1);
  exact = 1;
  prec = (mp_exp_t) _mpfr_ceil ((double) m / log_b2[b-2]) + 1;
  exp = ((mp_exp_t) m < g) ? g - (mp_exp_t) m : (mp_exp_t) m - g;
  log_2prec = (mp_exp_t) __gmpfr_ceil_log2 ((double) prec);
  prec += log_2prec; /* number of guard bits */
  if (exp != 0) /* add maximal exponentiation error */
    prec += 3 * (mp_exp_t) __gmpfr_ceil_log2 ((double) exp);

  for (;;)
    {
      TMP_MARK(marker);

      exact = 1;

      /* number of limbs */
      n = 1 + (prec - 1) / BITS_PER_MP_LIMB;

      /* a will contain the approximation of the mantissa */
      a = (mp_limb_t*) TMP_ALLOC (n * sizeof (mp_limb_t));

      nx = 1 + (MPFR_PREC(x) - 1) / BITS_PER_MP_LIMB;

      if ((mp_exp_t) m == g) /* final exponent is 0, no multiplication or
				division to perform */
	{
	  if (nx > n)
            exact = mpn_scan1 (xp, 0) >= (nx - n) * BITS_PER_MP_LIMB;
	  err = !exact;
	  MPN_COPY2 (a, n, xp, nx);
	  exp_a = MPFR_GET_EXP (x) - n * BITS_PER_MP_LIMB;
	}
      else if ((mp_exp_t) m > g) /* we have to multiply x by b^exp */
        {
	  /* a2*2^exp_a =  b^e */
	  err = mpn_exp (a, &exp_a, b, exp, n);
	  /* here, the error on a is at most 2^err ulps */
	  exact = (err == -1);

	  /* x = x1*2^(n*BITS_PER_MP_LIMB) */
	  x1 = (nx >= n) ? xp + nx - n : xp;
	  nx1 = (nx >= n) ? n : nx; /* nx1 = min(n, nx) */

	  /* test si exact */
	  if (nx > n)
            exact = (exact && 
                     ((mpn_scan1 (xp, 0) >= (nx - n) * BITS_PER_MP_LIMB)));

	  /* we loose one more bit in the multiplication,
	     except when err=0 where we loose two bits */
	  err = (err <= 0) ? 2 : err + 1;
	  
          /* result = a * x */
	  result = (mp_limb_t*) TMP_ALLOC ((n + nx1) * sizeof (mp_limb_t));
	  mpn_mul (result, a, n, x1, nx1);
          exp_a += MPFR_GET_EXP (x);
	  if (mpn_scan1 (result, 0) < (nx1 * BITS_PER_MP_LIMB))
	    exact = 0;

          /* normalize a and truncate */
          if ((result[n + nx1 - 1] & MPFR_LIMB_HIGHBIT) == 0)
	    {
	      mpn_lshift (a, result + nx1, n , 1);
	      a[0] |= result[nx1 - 1] >> (BITS_PER_MP_LIMB - 1);
              exp_a --;
	    }
	  else
            MPN_COPY (a, result + nx1, n);
        }
      else
        {
	  /* a2*2^exp_a =  b^e */
	  err = mpn_exp (a, &exp_a, b, exp, n);
	  exact = (err == -1);

	  /* allocate memory for x1, result and reste */
	  x1 = (mp_limb_t*) TMP_ALLOC (2 * n * sizeof (mp_limb_t));
	  result = (mp_limb_t*) TMP_ALLOC ((n + 1) * sizeof (mp_limb_t));
          reste = (mp_limb_t*) TMP_ALLOC (n * sizeof (mp_limb_t));

	  /* initialize x1 = x */
	  MPN_COPY2 (x1, 2 * n, xp, nx);
	  if ((exact) && (nx > 2 * n) &&
	      (mpn_scan1 (xp, 0) < (nx - 2 * n) * BITS_PER_MP_LIMB))
	    exact = 0;

	  /* result = x / a */
	  mpn_tdiv_qr (result, reste, 0, x1, 2 * n, a, n);
	  exp_a = MPFR_GET_EXP (x) - exp_a - 2 * n * BITS_PER_MP_LIMB;

	  /* test if division was exact */
	  if (exact)
            exact = mpn_popcount (reste, n) == 0;

	  /* normalize the result and copy into a */
	  if (result[n] == 1)
	    {
	      mpn_rshift (a, result, n, 1);
	      a[n - 1] |= MPFR_LIMB_HIGHBIT;;
	      exp_a ++;
	    }
	  else
            MPN_COPY (a, result, n);

	  err = (err == -1) ? 2 : err + 2;
        }

      /* check if rounding is possible */
      if (exact)
        err = -1;
      if (mpfr_get_str_aux (s, e, a, n, exp_a, err, b, m, rnd) != 2)
	break;

      /* increment the working precision */
      prec += log_2prec;

      TMP_FREE(marker);
    }

  *e += g;

  TMP_FREE(marker);

  return s0;

}
