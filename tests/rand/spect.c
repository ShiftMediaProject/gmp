/* spect.c -- the spectral test */

/* T is upper dimension.  Z_A is the LC multiplier, which is
   relatively prime to Z_M, the LC modulus.  The result is put in
   rop[] with v[t] in rop[t-2]. */

/* BUGS: Due to lazy allocation scheme, maximum T is hard coded to MAXT. */

#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"

void zdiv_round (mpz_t rop, mpz_t n, mpz_t d);

static int g_debug = 0;

/* f_floor (rop, op) -- Set rop = floor (op). */
void
f_floor (mpf_t rop, mpf_t op)
{
  mpz_t z;

  mpz_init (z);

  /* No mpf_floor().  Convert to mpz and back. */
  mpz_set_f (z, op);
  mpf_set_z (rop, z);

  mpz_clear (z);
}


/* vz_dot (rop, v1, v2, nelem) -- compute dot product of z-vectors V1,
   V2.  N is number of elements in vectors V1 and V2. */

void
vz_dot (mpz_t rop, mpz_t V1[], mpz_t V2[], unsigned int n)
{
  mpz_t t;

  mpz_init (t);
  mpz_set_ui (rop, 0);
  while (n--)
    {
      mpz_mul (t, V1[n], V2[n]);
      mpz_add (rop, rop, t);
    }

  mpz_clear (t);
}

#define MAXT 10

void
spectral_test (mpf_t rop[], unsigned int T, mpz_t a, mpz_t m)
{
  /* Knuth "Seminumerical Algorithms, Third Edition", section 3.3.4
     (pp. 101-103). */

  /* v[t] = min { sqrt (x[1]^2 + ... + x[t]^2) |
     x[1] + a*x[2] + ... + pow (a, t-1) * x[t] is congruent to 0 (mod m) } */


  /* Variables. */
  unsigned int ui_t;
  unsigned int ui_i, ui_j, ui_k, ui_l;
  mpf_t f_tmp1, f_tmp2;
  mpz_t tmp1, tmp2, tmp3;
  mpz_t U[MAXT][MAXT], V[MAXT][MAXT], X[MAXT], Y[MAXT], Z[MAXT];
  mpz_t h, hp, r, s, p, pp, q, u, v;

  /* GMP inits. */
  mpf_init (f_tmp1);
  mpf_init (f_tmp2);
  for (ui_i = 0; ui_i < MAXT; ui_i++)
    {
      for (ui_j = 0; ui_j < MAXT; ui_j++)
	{
	  mpz_init_set_ui (U[ui_i][ui_j], 0);
	  mpz_init_set_ui (V[ui_i][ui_j], 0);
	}
      mpz_init_set_ui (X[ui_i], 0);
      mpz_init_set_ui (Y[ui_i], 0);
      mpz_init (Z[ui_i]);
    }
  mpz_init (tmp1);
  mpz_init (tmp2);
  mpz_init (tmp3);
  mpz_init (h);
  mpz_init (hp);
  mpz_init (r);
  mpz_init (s);
  mpz_init (p);
  mpz_init (pp);
  mpz_init (q);
  mpz_init (u);
  mpz_init (v);

  /* Implementation inits. */
  if (T > MAXT)
    T = MAXT;			/* FIXME: Lazy. */

  /* S1 [Initialize.] */
  ui_t = 2 - 1;			/* NOTE: `t' in description == ui_t + 1
                                   for easy indexing */
  mpz_set (h, a);
  mpz_set (hp, m);
  mpz_set_ui (p, 1);
  mpz_set_ui (pp, 0);
  mpz_set (r, a);
  mpz_pow_ui (s, a, 2);		/* s = 1 + a^2 */
  mpz_add_ui (s, s, 1);

  /* S2 [Euclidean step.] */
  while (1)
    {
      mpz_tdiv_q (q, hp, h);

      mpz_mul (tmp1, q, h);
      mpz_sub (u, hp, tmp1);

      mpz_mul (tmp1, q, p);
      mpz_sub (v, pp, tmp1);
  
      mpz_pow_ui (tmp1, u, 2);
      mpz_pow_ui (tmp2, v, 2);
      mpz_add (tmp1, tmp1, tmp2);
      if (mpz_cmp (tmp1, s) < 0)
	{
	  mpz_set (s, tmp1);	/* s = u^2 + v^2 */
	  mpz_set (hp, h);
	  mpz_set (h, u);
	  mpz_set (pp, p);
	  mpz_set (p, v);
	}
      else
	break;
    }

  /* S3 [Compute v2.] */
  mpz_sub (u, u, h);
  mpz_sub (v, v, p);

  mpz_pow_ui (tmp1, u, 2);
  mpz_pow_ui (tmp2, v, 2);
  mpz_add (tmp1, tmp1, tmp2);
  if (mpz_cmp (tmp1, s) < 0)
    {
      mpz_set (s, tmp1);	/* s = u^2 + v^2 */
      mpz_set (hp, u);
      mpz_set (pp, v);
    }
  mpf_set_z (f_tmp1, s);
  mpf_sqrt (rop[ui_t - 1], f_tmp1);
      
  /* S4 [Advance t.] */
  mpz_neg (U[0][0], h);
  mpz_set (U[0][1], p);
  mpz_neg (U[1][0], hp);
  mpz_set (U[1][1], pp);
  
  mpz_set (V[0][0], pp);
  mpz_set (V[0][1], hp);
  mpz_neg (V[1][0], p);
  mpz_neg (V[1][1], h);
  if (mpz_cmp_ui (pp, 0) > 0)
    {
      mpz_neg (V[0][0], V[0][0]);
      mpz_neg (V[0][1], V[0][1]);
      mpz_neg (V[1][0], V[1][0]);
      mpz_neg (V[1][1], V[1][1]);
    }

  while (ui_t + 1 != T)		/* S4 loop */
    {
      ui_t++;
      mpz_mul (r, a, r);
      mpz_mod (r, r, m);

      /* Add new row and column to U and V.  They are initialized with
	 all elements set to zero, so clearing is not necessary. */

      mpz_neg (U[ui_t][0], r); /* U: First col in new row. */
      mpz_set_ui (U[ui_t][ui_t], 1); /* U: Last col in new row. */

      mpz_set (V[ui_t][ui_t], m); /* V: Last col in new row. */
      
      /* "Finally, for 1 <= i < t,
	   set q = round (vi1 * r / m),
	   vit = vi1*r - q*m,
	   and Ut=Ut+q*Ui */

      for (ui_i = 0; ui_i < ui_t; ui_i++)
	{
	  mpz_mul (tmp1, V[ui_i][0], r); /* tmp1=vi1*r */
	  zdiv_round (q, tmp1, m); /* q=round(vi1*r/m) */
	  mpz_mul (tmp2, q, m);	/* tmp2=q*m */
	  mpz_sub (V[ui_i][ui_t], tmp1, tmp2);

	  for (ui_j = 0; ui_j <= ui_t; ui_j++) /* U[t] = U[t] + q*U[i] */
	    {
	      mpz_mul (tmp1, q, U[ui_i][ui_j]);	/* tmp=q*uij */
	      mpz_add (U[ui_t][ui_j], U[ui_t][ui_j], tmp1); /* utj = utj + q*uij */
	    }
	}

      /* s = min (s, zdot (U[t], U[t]) */
      vz_dot (tmp1, U[ui_t], U[ui_t], ui_t + 1);
      if (mpz_cmp (tmp1, s) < 0)
	mpz_set (s, tmp1);

      ui_k = ui_t;
      ui_j = 0;			/* WARNING: ui_j no longer a temp. */

      /* S5 [Transform.] */
      if (g_debug)
	printf ("(t, k, j, q1, q2, ...)\n");
      do 
	{
	  if (g_debug)
	    printf ("(%u, %u, %u", ui_t + 1, ui_k + 1, ui_j + 1);

	  for (ui_i = 0; ui_i <= ui_t; ui_i++)
	    {
	      if (ui_i != ui_j)
		{
		  vz_dot (tmp1, V[ui_i], V[ui_j], ui_t + 1); /* tmp1=dot(Vi,Vj). */
		  mpz_abs (tmp2, tmp1);
		  mpz_mul_ui (tmp2, tmp2, 2); /* tmp2 = 2*abs(dot(Vi,Vj) */
		  vz_dot (tmp3, V[ui_j], V[ui_j], ui_t + 1); /* tmp3=dot(Vj,Vj). */

		  if (mpz_cmp (tmp2, tmp3) > 0)
		    {
		      zdiv_round (q, tmp1, tmp3); /* q=round(Vi.Vj/Vj.Vj) */
		      if (g_debug)
			{
			  printf (", ");
			  mpz_out_str (stdout, 10, q);
			}

		      for (ui_l = 0; ui_l <= ui_t; ui_l++)
			{
			  mpz_mul (tmp1, q, V[ui_j][ui_l]);
			  mpz_sub (V[ui_i][ui_l], V[ui_i][ui_l], tmp1); /* Vi=Vi-q*Vj */
			  mpz_mul (tmp1, q, U[ui_i][ui_l]);
			  mpz_add (U[ui_j][ui_l], U[ui_j][ui_l], tmp1); /* Uj=Uj+q*Ui */
			}
		      
		      vz_dot (tmp1, U[ui_j], U[ui_j], ui_t + 1); /* tmp1=dot(Uj,Uj) */
		      if (mpz_cmp (tmp1, s) < 0) /* s = min(s,dot(Uj,Uj)) */
			mpz_set (s, tmp1);
		      ui_k = ui_j;
		    }
		  else if (g_debug)
		    printf (", #"); /* 2|Vi.Vj| <= Vj.Vj */
		}
	      else if (g_debug)
		printf (", *");	/* i == j */
	    }

	  if (g_debug)
	    printf (")\n");

	  /* S6 [Advance j.] */
	  if (ui_j == ui_t)
	    ui_j = 0;
	  else
	    ui_j++;
	}
      while (ui_j != ui_k);	/* S5 */

      /* S7 [Prepare for search.] */
      ui_k = ui_t;
      if (g_debug)
	printf ("searching...");

      /* Z[i] = floor (sqrt (floor (dot(V[i],V[i]) * s / m^2))); */
      mpz_pow_ui (tmp1, m, 2);
#ifdef DEBUG
      fprintf (stdout, "m: ");
      mpz_out_str (stdout, 10, m);
      fputs ("\n", stdout);
      fflush (stdout);
#endif      
      mpf_set_z (f_tmp1, tmp1);
      mpf_set_z (f_tmp2, s);
      mpf_div (f_tmp1, f_tmp2, f_tmp1);	/* f_tmp1 = s/m^2 */
      for (ui_i = 0; ui_i <= ui_t; ui_i++)
	{
	  vz_dot (tmp1, V[ui_i], V[ui_i], ui_t + 1);
	  mpf_set_z (f_tmp2, tmp1);
	  mpf_mul (f_tmp2, f_tmp2, f_tmp1);
	  f_floor (f_tmp2, f_tmp2);
	  mpf_sqrt (f_tmp2, f_tmp2);
	  mpz_set_f (Z[ui_i], f_tmp2);
	}

      /* S8 [Advance X[k].] */
      do 
	{
	  if (mpz_cmp (X[ui_k], Z[ui_k]))
	    {
	      mpz_add_ui (X[ui_k], X[ui_k], 1);
	      for (ui_i = 0; ui_i <= ui_t; ui_i++)
		mpz_add (Y[ui_i], Y[ui_i], U[ui_k][ui_i]);

	      /* S9 [Advance k.] */
	      while (++ui_k <= ui_t)
		{
		  mpz_neg (X[ui_k], Z[ui_k]);
		  mpz_mul_ui (tmp1, Z[ui_k], 2);
		  for (ui_i = 0; ui_i <= ui_t; ui_i++)
		    {
		      mpz_mul (tmp2, tmp1, U[ui_k][ui_i]);
		      mpz_sub (Y[ui_i], Y[ui_i], tmp2);
		    }
		}
	      vz_dot (tmp1, Y, Y, ui_t + 1);
	      if (mpz_cmp (tmp1, s) < 0)
		mpz_set (s, tmp1);
	    }
	}
      while (--ui_k);

      mpf_set_z (f_tmp1, s);
      mpf_sqrt (rop[ui_t - 1], f_tmp1);
      if (g_debug)
	printf ("done.\n");
    } /* S4 loop */

  /* Clear GMP variables. */

  mpf_clear (f_tmp1);
  mpf_clear (f_tmp2);
  for (ui_i = 0; ui_i < MAXT; ui_i++)
    {
      for (ui_j = 0; ui_j < MAXT; ui_j++)
	{
	  mpz_clear (U[ui_i][ui_j]);
	  mpz_clear (V[ui_i][ui_j]);
	}
      mpz_clear (X[ui_i]);
      mpz_clear (Y[ui_i]);
      mpz_clear (Z[ui_i]);
    }
  mpz_clear (tmp1);
  mpz_clear (tmp2);
  mpz_clear (tmp3);
  mpz_clear (h);
  mpz_clear (hp);
  mpz_clear (r);
  mpz_clear (s);
  mpz_clear (p);
  mpz_clear (pp);
  mpz_clear (q);
  mpz_clear (u);
  mpz_clear (v);

  return;
}

int
main (int argc, char *argv[])
{
  const char usage[] = "usage: spect [-d] a m n\n";
  int c;
  unsigned int n;
  mpz_t a, m;
  mpf_t res[MAXT], f_tmp;
  register int f;


  mpz_init (a);
  mpz_init (m);
  for (f = 0; f < MAXT; f++)
    mpf_init (res[f]);
  mpf_init (f_tmp);

  while ((c = getopt (argc, argv, "dh")) != -1)
    switch (c)
      {
      case 'd':			/* debug */
	g_debug++;
	break;
      case 'h':
      default:
	fputs (usage, stderr);
	exit (1);
      }
  argc -= optind;
  argv += optind;

  if (argc < 3)
    {
      fputs (usage, stderr);
      exit (1);
    }

  mpz_set_str (a, argv[0], 0);
  mpz_set_str (m, argv[1], 0);
  n = (unsigned int) atoi (argv[2]);
  if (n + 1 > MAXT)
    n = MAXT + 1;

  spectral_test (res, n, a, m);

  for (f = 0; f < n - 1; f++)
    {
      printf ("v[%d] = ", f);
      mpf_out_str (stdout, 10, 0, res[f]);
      printf (" (^2 = ");
      mpf_mul (f_tmp, res[f], res[f]);
      mpf_out_str (stdout, 10, 0, f_tmp);
      printf (")");
      puts ("");
    }

  mpz_clear (a);
  mpz_clear (m);
  for (f = 0; f < MAXT; f++)
    mpf_clear (res[f]);
  mpf_clear (f_tmp);

  return 0;
}


void 
debug_foo()
{
  if (0)
    {
      mpz_dump (0);
      mpf_dump (0);
    }
}
