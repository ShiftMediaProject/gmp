/* statlib.c -- Statistical functions.  Good for testing the
   randomness of number sequences. */

/* The theories for these functions are taken from D. Knuth's "The Art
of Computer Programming: Volume 2, Seminumerical Algorithms", Third
Edition, Addison Wesley, 1998. */

/* Implementation notes.

The Kolmogorov-Smirnov test.

Eq. (13) in Knuth, p. 50, says that if X1, X2, ..., Xn are independent
observations arranged into ascending order

	Kp = sqr(n) * max(j/n - F(Xj))		for all 1<=j<=n
	Km = sqr(n) * max(F(Xj) - (j-1)/n))	for all 1<=j<=n

where F(x) = Pr(X <= x) = probability that (X <= x), which for a
uniformly distributed random real number between zero and one is
exactly the number itself (x).


The answer to exercise 23 gives the following implementation, which
doesn't need the observations to be sorted in ascending order:

for (k = 0; k < m; k++)
	a[k] = 1.0
	b[k] = 0.0
	c[k] = 0

for (each observation Xj)
	Y = F(Xj)
	k = floor (m * Y)
	a[k] = min (a[k], Y)
	b[k] = max (b[k], Y)
	c[k] += 1

	j = 0
	rp = rm = 0
	for (k = 0; k < m; k++)
		if (c[k] > 0)
			rm = max (rm, a[k] - j/n)
			j += c[k]
			rp = max (rp, j/n - b[k])
			
Kp = sqr (n) * rp
Km = sqr (n) * rm 

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gmp.h>

extern unsigned int stat_debug;

/* ks (Kp, Km, X, P, n) -- Perform a Kolmogorov-Smirnov test on the N
   real numbers between zero and one in vector X.  P is the
   distribution function, called for each entry in X, which should
   calculate the probability of X being greater than or equal to any
   number in the sequence.  (For a uniformly distributed sequence of
   real numbers between zero and one, this is simply equal to X.)  The
   result is put in Kp and Km.  */

void
ks (mpf_t Kp,
    mpf_t Km,
    mpf_t X[],
    void (P) (mpf_t, mpf_t),
    unsigned long int n)
{
  mpf_t Kt;			/* temp */
  mpf_t f_x;
  mpf_t f_j;			/* j */
  mpf_t f_jnq;			/* j/n or (j-1)/n */
  unsigned long int j;

  /* Sort the vector in ascending order. */  
  qsort (X, n, sizeof (__mpf_struct), mpf_cmp);

  /* K-S test. */
  /* 	Kp = sqr(n) * max(j/n - F(Xj))		for all 1<=j<=n
	Km = sqr(n) * max(F(Xj) - (j-1)/n))	for all 1<=j<=n
  */

  mpf_init (Kt); mpf_init (f_x); mpf_init (f_j); mpf_init (f_jnq); 
  mpf_set_ui (Kp, 0);  mpf_set_ui (Km, 0);
  for (j = 1; j <= n; j++)
    {
      P (f_x, X[j-1]);
      mpf_set_ui (f_j, j);

      mpf_div_ui (f_jnq, f_j, n);
      mpf_sub (Kt, f_jnq, f_x);
      if (mpf_cmp (Kt, Kp) > 0)
	mpf_set (Kp, Kt);
      if (stat_debug > 2)
	{
	  printf ("j=%lu ", j);
	  printf ("P()="); mpf_out_str (stdout, 10, 2, f_x); printf ("\t");

	  printf ("jnq="); mpf_out_str (stdout, 10, 2, f_jnq); printf (" ");
	  printf ("diff="); mpf_out_str (stdout, 10, 2, Kt); printf (" ");
	  printf ("Kp="); mpf_out_str (stdout, 10, 2, Kp); printf ("\t");
	}
      mpf_sub_ui (f_j, f_j, 1);
      mpf_div_ui (f_jnq, f_j, n);
      mpf_sub (Kt, f_x, f_jnq);
      if (mpf_cmp (Kt, Km) > 0)
	mpf_set (Km, Kt);

      if (stat_debug > 2)
	{
	  printf ("jnq="); mpf_out_str (stdout, 10, 2, f_jnq); printf (" ");
	  printf ("diff="); mpf_out_str (stdout, 10, 2, Kt); printf (" ");
	  printf ("Km="); mpf_out_str (stdout, 10, 2, Km); printf (" ");
	  printf ("\n");
	}
    }
  mpf_sqrt_ui (Kt, n);
  mpf_mul (Kp, Kp, Kt);
  mpf_mul (Km, Km, Kt);

  mpf_clear (Kt); mpf_clear (f_x); mpf_clear (f_j); mpf_clear (f_jnq); 
}

/* ks_table(val, n) -- calculate probability for Kp/Km less than or
   equal to VAL with N observations.  See [Knuth section 3.3.1] */

void
ks_table (mpf_t p, mpf_t val, const unsigned int n)
{
  /* We use Eq. (27), Knuth p.58, skipping O(1/n) for simplicity.
     This shortcut will result in too high probabilities, especially
     when n is small.

     Pr(Kp(n) <= s) = 1 - pow(e, -2*s^2) * (1 - 2/3*s/sqrt(n) + O(1/n)) */

  mpf_t t1, t2;

  mpf_init (t1); mpf_init (t2);

  /* t1 = 1 - 2/3 * s/sqrt(n) */
  mpf_sqrt_ui (t1, n);
  mpf_div (t1, val, t1);
  mpf_mul_ui (t1, t1, 2);
  mpf_div_ui (t1, t1, 3);
  mpf_ui_sub (t1, 1, t1);

  /* t2 = pow(e, -2*s^2) */
  /* hmmm, gmp doesn't have pow() for floats.  use doubles. */
  mpf_set_d (t2, pow (M_E, -(2 * pow (mpf_get_d (val), 2))));

  /* p = 1 - t1 * t2 */
  mpf_mul (t1, t1, t2);
  mpf_ui_sub (p, 1, t1);

  mpf_clear (t1); mpf_clear (t2);
}

static double x2_table_X[][7] = {
  { -2.33, -1.64, -.674, 0.0, 0.674, 1.64, 2.33 }, /* x */
  { 5.4289, 2.6896, .454276, 0.0, .454276, 2.6896, 5.4289} /* x^2 */
};

#define _2D3 ((double) .6666666666)

/* x2_table (t, v, n) -- return chi-square table row for V in T[]. */
void
x2_table (double t[],
	  unsigned int v)
{
  int f;


  /* FIXME: Do a table lookup for v <= 30 since the following formula
     [Knuth, vol 2, 3.3.1] is only good for v > 30. */

  /* value = v + sqrt(2*v) * X[p] + (2/3) * X[p]^2 - 2/3 + O(1/sqrt(t) */
  /* NOTE: The O() term is ignored for simplicity. */
  
  for (f = 0; f < 7; f++)
      t[f] =
	v +
	sqrt (2 * v) * x2_table_X[0][f] +
	_2D3 * x2_table_X[1][f] - _2D3;
}


/* P(p, x) -- Distribution function.  Calculate the probability of X
being greater than or equal to any number in the sequence.  For a
random real number between zero and one given by a uniformly
distributed random number generator, this is simply equal to X. */

static void 
P (mpf_t p, mpf_t x)
{
  mpf_set (p, x);
}

/* mpf_freqt() -- Frequency test using KS on N real numbers between zero
   and one.  See [Knuth vol 2, p.61]. */
void
mpf_freqt (mpf_t Kp,
	   mpf_t Km,
	   mpf_t X[],
	   const unsigned long int n)
{
  ks (Kp, Km, X, P, n);
}


/* The Chi-square test.  Eq. (8) in Knuth vol. 2 says that if Y[]
   holds the observations and p[] is the probability for.. (to be
   continued!)

   V = 1/n * sum((s=1 to k) Y[s]^2 / p[s]) - n */

void
x2 (mpf_t V,			/* result */
    unsigned long int X[],	/* data */
    unsigned int k,		/* #of categories */
    void (P) (mpf_t, unsigned long int, void *), /* probability func */
    void *x,			/* extra user data passed to P() */
    unsigned long int n)	/* #of samples */
{
  unsigned int f;
  mpf_t f_t, f_t2;		/* temp floats */

  mpf_init (f_t); mpf_init (f_t2);


  mpf_set_ui (V, 0);
  for (f = 0; f < k; f++)
    {
      if (stat_debug > 1)
	fprintf (stderr, "%u: P()=", f);
      mpf_set_ui (f_t, X[f]);
      mpf_mul (f_t, f_t, f_t);	/* f_t = X[f]^2 */
      P (f_t2, f, x);		/* f_t2 = Pr(f) */
      if (stat_debug > 1)
	mpf_out_str (stderr, 10, 2, f_t2);
      mpf_div (f_t, f_t, f_t2);
      mpf_add (V, V, f_t);
      if (stat_debug > 1)
	{
	  fprintf (stderr, "\tV=");
	  mpf_out_str (stderr, 10, 2, V);
	  fprintf (stderr, "\t");
	}
    }
  if (stat_debug > 1)
    fprintf (stderr, "\n");
  mpf_div_ui (V, V, n);
  mpf_sub_ui (V, V, n);
  
  mpf_clear (f_t); mpf_clear (f_t2);
}

/* Pzf(p, s, x) -- Probability for category S in mpz_freqt().  It's
   1/d for all S.  X is a pointer to an unsigned int holding 'd'. */
static void
Pzf (mpf_t p, unsigned long int s, void *x)
{
  mpf_set_ui (p, 1);
  mpf_div_ui (p, p, *((unsigned int *) x));
}

/* mpz_freqt(V, X, imax, n) -- Frequency test on integers.  [Knuth,
   vol 2, 3.3.2].  Keep IMAX low on this one, since we loop from 0 to
   IMAX.  128 or 256 could be nice.

   X[] must not contain numbers outside the range 0 <= X <= IMAX.

   Return value is number of observations actally used, after
   discarding entries out of range.

   Since X[] contains integers between zero and IMAX, inclusive, we
   have IMAX+1 categories.

   Note that N should be at least 5*IMAX.  Result is put in V and can
   be compared to output from x2_table (v=IMAX). */

unsigned long int
mpz_freqt (mpf_t V,
	   mpz_t X[],
	   unsigned int imax,
	   const unsigned long int n)
{
  unsigned long int *v;		/* result */
  unsigned int f;
  unsigned int d;		/* number of categories = imax+1 */
  unsigned int uitemp;
  unsigned long int usedn;


  d = imax + 1;

  v = (unsigned long int *) calloc (imax + 1, sizeof (unsigned long int));
  if (NULL == v)
    {
      fprintf (stderr, "mpz_freqt(): out of memory\n");
      exit (1);
    }

  /* count */
  usedn = n;			/* actual number of observations */
  for (f = 0; f < n; f++)
    {
      uitemp = mpz_get_ui(X[f]);
      if (uitemp > imax)	/* sanity check */
	{
	  if (stat_debug)
	    fprintf (stderr, "mpz_freqt(): warning: input insanity: %u, "\
		     "ignored.\n", uitemp);
	  usedn--;
	  continue;
	}
      v[uitemp]++;
    }

  if (stat_debug > 1)
    {
      fprintf (stderr, "counts:\n");
      for (f = 0; f <= imax; f++)
	fprintf (stderr, "%u:\t%lu\n", f, v[f]);
    }

  /* chi-square with k=imax+1 and P(x)=1/(imax+1) for all x.*/
  x2 (V, v, d, Pzf, (void *) &d, usedn);

  free (v);
  return (usedn);
}



