/* Test file for the various mpfr_random fonctions.

Copyright 1999, 2000, 2001, 2002 Free Software Foundation.

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
#include <math.h>
#include <time.h>
#include "gmp.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "mpfr-test.h"

void test_random _PROTO ((unsigned long, unsigned long, int));
void test_random2 _PROTO ((unsigned long, unsigned long, int));
void test_urandomb _PROTO ((unsigned long, unsigned long, int));

void
test_random (unsigned long nbtests, unsigned long prec, int verbose)
{
  mpfr_t x; 
  int *tab, size_tab, k; 
  double d, av = 0, var = 0, chi2 = 0, th; 

  mpfr_init2(x, prec); 

  size_tab = (nbtests >= 1000 ? nbtests / 50 : 20); 
  tab = (int *) malloc (size_tab * sizeof(int)); 
  for (k = 0; k < size_tab; ++k) tab[k] = 0; 

  for (k = 0; k < nbtests; k++) {
    mpfr_random(x); 
    d = mpfr_get_d1 (x); av += d; var += d*d; 
    tab[(int)(size_tab * d)]++;     
  }

  mpfr_clear(x);
  if (!verbose) { free(tab); return; }

  av /= nbtests; 
  var = (var /nbtests) - av*av; 

  th = (double) nbtests / size_tab;
  
  printf("Average = %.5f\nVariance = %.5f\n", av, var); 
  printf("Repartition for random. Each integer should be close to %d.\n", 
	 (int)th); 

  for (k = 0; k < size_tab; k++) {
    chi2 += (tab[k] - th) * (tab[k] - th) / th; 
    printf("%d ", tab[k]); 
    if (((k+1) & 7) == 0) printf("\n"); 
  }

  printf("\nChi2 statistics value (with %d degrees of freedom) : %.5f\n\n", 
	 size_tab - 1, chi2); 

  printf("\n");

  free(tab);
  return;
}

void
test_random2 (unsigned long nbtests, unsigned long prec, int verbose)
{
  mpfr_t x;
  int *tab, size_tab, k, sh, xn;
  double d, av = 0, var = 0, chi2 = 0, th; 

  mpfr_init2 (x, prec);
  xn = 1 + (prec - 1) / mp_bits_per_limb;
  sh = xn * mp_bits_per_limb - prec;

  size_tab = (nbtests >= 1000 ? nbtests / 50 : 20); 
  tab = (int *) malloc (size_tab * sizeof(int)); 
  for (k = 0; k < size_tab; ++k) tab[k] = 0; 

  for (k = 0; k < nbtests; k++) {
    mpfr_random2 (x, xn, 0);
    /* check that lower bits are zero */
    if (MPFR_MANT(x)[0] & ((MP_LIMB_T_ONE << sh) - MP_LIMB_T_ONE))
      {
        fprintf (stderr, "Error: mpfr_random2() returns invalid numbers:\n");
        mpfr_print_binary (x); putchar ('\n');
        exit (1);
      }
    /* check that exponent is in correct range */
    if (mpfr_get_exp (x) != 0)
      {
        fprintf (stderr, "Error: mpfr_random2 (.., .., 0) does not return a 0 exponent:\n");
        mpfr_print_binary (x); putchar ('\n');
        exit (1);
      }
    d = mpfr_get_d1 (x); av += d; var += d*d; 
    if (d < 1)
      tab[(int)(size_tab * d)]++;     
  }

  mpfr_clear (x);
  if (!verbose) { free(tab); return; }
  
  av /= nbtests; 
  var = (var /nbtests) - av*av; 

  th = (double)nbtests / size_tab; 
  printf("Average = %.5f\nVariance = %.5f\n", av, var); 
  printf("Repartition for random2 (taking only values < 1 into account.\n"); 

  for (k = 0; k < size_tab; k++) {
    chi2 += (tab[k] - th) * (tab[k] - th) / th; 
    printf("%d ", tab[k]); 
    if (((k+1) & 7) == 0) printf("\n"); 
  }

  printf("\nChi2 statistics value (with %d degrees of freedom) : %.5f\n\n", 
	 size_tab - 1, chi2); 

  free(tab); 
  return;
}

void
test_urandomb (unsigned long nbtests, unsigned long prec, int verbose)
{
  mpfr_t x; 
  int *tab, size_tab, k, sh, xn;
  gmp_randstate_t state; 
  double d, av = 0, var = 0, chi2 = 0, th; 

  mpfr_init2 (x, prec);
  xn = 1 + (prec - 1) / mp_bits_per_limb;
  sh = xn * mp_bits_per_limb - prec;

  size_tab = (nbtests >= 1000 ? nbtests / 50 : 20); 
  tab = (int *) malloc (size_tab * sizeof(int)); 
  for (k = 0; k < size_tab; ++k) tab[k] = 0; 

  gmp_randinit (state, GMP_RAND_ALG_LC, 128); 
  gmp_randseed_ui (state, time(NULL));

  for (k = 0; k < nbtests; k++) {
    mpfr_urandomb (x, state);
    /* check that lower bits are zero */
    if (MPFR_MANT(x)[0] & ((MP_LIMB_T_ONE << sh) - MP_LIMB_T_ONE))
      {
        fprintf (stderr, "Error: mpfr_urandomb() returns invalid numbers:\n");
        mpfr_print_binary (x); putchar ('\n');
        exit (1);
      }
    d = mpfr_get_d1 (x); av += d; var += d*d; 
    tab[(int)(size_tab * d)]++;     
  }

  mpfr_clear(x);
  gmp_randclear(state);
  if (!verbose) { free(tab); return; }

  av /= nbtests; 
  var = (var /nbtests) - av*av; 

  th = (double)nbtests / size_tab; 
  printf("Average = %.5f\nVariance = %.5f\n", av, var); 
  printf("Repartition for urandomb. Each integer should be close to %d.\n", 
	 (int)th); 

  for (k = 0; k < size_tab; k++) {
    chi2 += (tab[k] - th) * (tab[k] - th) / th; 
    printf("%d ", tab[k]); 
    if (((k+1) & 7) == 0) printf("\n"); 
  }

  printf("\nChi2 statistics value (with %d degrees of freedom) : %.5f\n\n", 
	 size_tab - 1, chi2); 

  free(tab); 
  return;
}

int
main (int argc, char *argv[])
{
  unsigned long nbtests, prec; int verbose = 0; 
  
  tests_start_mpfr ();

  if (argc > 1) verbose = 1;
 
  if (argc == 1) { nbtests = 10000; } else nbtests = atoi(argv[1]);
  if (argc <= 2) { prec = 1000; } else prec = atoi(argv[2]); 

  test_random (nbtests, prec, verbose);
  test_random2 (nbtests, prec, verbose); 
  test_urandomb (nbtests, prec, verbose);

  if (argc == 1)  /* check also small precision */
    {
      test_random (nbtests, 2, 0);
      test_random2 (nbtests, 2, 0); 
      test_urandomb (nbtests, 2, 0);
    }

  tests_end_mpfr ();
  return 0;
}
