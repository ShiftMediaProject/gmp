/* Test file for mpfr_sub1sp.

Copyright 2003 Free Software Foundation.

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

#include "mpfr-test.h"

void check_special(void);
void check_random(mpfr_prec_t p);

int main(void)
{
  mpfr_prec_t p;

  tests_start_mpfr ();

  check_special ();
  for(p = 2 ; p < 200 ; p++)
    check_random (p);

  tests_end_mpfr ();
  return 0;
}

#define STD_ERROR \
            {\
              printf("ERROR: for %s and p=%lu and i=%d:\nY=",\
		      mpfr_print_rnd_mode(r), p, i);\
	       mpfr_print_binary(y);\
	       printf("\nZ="); mpfr_print_binary(z);\
	       printf("\nReal: "); mpfr_print_binary(x2);\
	       printf("\nGot : "); mpfr_print_binary(x);\
	       putchar('\n');\
               abort();\
            }

#define STD_ERROR2 \
            {\
              printf("ERROR: for %s and p=%lu and i=%d:\nY=",\
                      mpfr_print_rnd_mode(r), p, i);\
               mpfr_print_binary(y);\
               printf("\nZ="); mpfr_print_binary(z);\
               printf("\nR="); mpfr_print_binary(x);\
               printf("\nWrong inexact flag. Real: %d. Got: %d", \
		      inexact1, inexact2); \
               abort();\
            }

void check_random(mpfr_prec_t p)
{
  mpfr_t x,y,z,x2;
  mp_rnd_t r;
  int i, inexact1, inexact2;
  gmp_randstate_t state;

  mpfr_inits2(p, x,y,z,x2,NULL);
  gmp_randinit_lc_2exp_size (state, 128);
  gmp_randseed_ui (state, 17422471);

  for(i = 0 ; i < 1000 ; i++)
    {
      mpfr_urandomb(y, state);
      mpfr_urandomb(z, state);
      if (MPFR_IS_PURE_FP(y) && MPFR_IS_PURE_FP(z))
	for(r = 0 ; r < 4 ; r++)
	  {
	    inexact1 = mpfr_sub1(x2, y, z, r);
	    inexact2 = mpfr_sub1sp(x, y, z, r);
	    if (mpfr_cmp(x, x2))
	      STD_ERROR;
	    if (inexact1 != inexact2)
	      STD_ERROR2;
	  }
    }

  mpfr_clears(x,y,z,x2,NULL);
  gmp_randclear(state);
}

void check_special(void)
{
  mpfr_t x,y,z,x2;
  mp_rnd_t r;
  mpfr_prec_t p;
  int i = -1, inexact1, inexact2;

  mpfr_inits(x,y,z,x2,NULL);

  for(r = 0 ; r < 4 ; r++)
    {
      p = 53;
      mpfr_set_prec(x, 53);
      mpfr_set_prec(x2, 53);
      mpfr_set_prec(y, 53);
      mpfr_set_prec(z, 53);

      mpfr_set_str_binary (y, 
       "0.10110111101101110010010010011011000001101101011011001E31");
  
      mpfr_sub1sp(x, y, y, r);
      if (mpfr_cmp_ui(x, 0))
	{
	  printf("Error for x-x with p=%lu. Expected 0. Got:", p);
	  mpfr_print_binary(x);
	  abort();
	}
      
      mpfr_set(z, y, r);
      mpfr_sub1sp(x, y, z, r);
      if (mpfr_cmp_ui(x, 0))
	{
	  printf("Error for x-y with y=x and p=%lu. Expected 0. Got:", p);
	  mpfr_print_binary(x);
	  abort();
	}
      /* diff = 0 */
      mpfr_set_str_binary (y,
       "0.10110111101101110010010010011011001001101101011011001E31");
      inexact1 = mpfr_sub1(x2, y, z, r);
      inexact2 = mpfr_sub1sp(x, y, z, r);
      if (mpfr_cmp(x, x2))
	STD_ERROR;
      if (inexact1 != inexact2)
	STD_ERROR2;
      
      /* Diff = 1 */
      mpfr_set_str_binary (y,
       "0.10110111101101110010010010011011000001101101011011001E30");
      inexact1 = mpfr_sub1(x2, y, z, r);
      inexact2 = mpfr_sub1sp(x, y, z, r);
      if (mpfr_cmp(x, x2))
	STD_ERROR;
      if (inexact1 != inexact2)
	STD_ERROR2;
      
      /* Diff = 2 */
      mpfr_set_str_binary (y,
       "0.10110111101101110010010010011011000101101101011011001E32");
      inexact1 = mpfr_sub1(x2, y, z, r);
      inexact2 = mpfr_sub1sp(x, y, z, r);
      if (mpfr_cmp(x, x2))
	STD_ERROR;
      if (inexact1 != inexact2)
	STD_ERROR2;
      
      /* Diff = 32 */
      mpfr_set_str_binary (y,
       "0.10110111101101110010010010011011000001101101011011001E63");
      inexact1 = mpfr_sub1(x2, y, z, r);
      inexact2 = mpfr_sub1sp(x, y, z, r);
      if (mpfr_cmp(x, x2))
	STD_ERROR;
      if (inexact1 != inexact2)
	STD_ERROR2;
      
      /* Diff = 52 */
      mpfr_set_str_binary (y,
       "0.10110111101101110010010010011011010001101101011011001E83");
      inexact1 = mpfr_sub1(x2, y, z, r);
      inexact2 = mpfr_sub1sp(x, y, z, r);
      if (mpfr_cmp(x, x2))
	STD_ERROR;
      if (inexact1 != inexact2)
	STD_ERROR2;
      
      /* Diff = 53 */
      mpfr_set_str_binary (y,
       "0.10110111101101110010010010011111000001101101011011001E31");
      inexact1 = mpfr_sub1(x2, y, z, r);
      inexact2 = mpfr_sub1sp(x, y, z, r);
      if (mpfr_cmp(x, x2))
        STD_ERROR;
      if (inexact1 != inexact2)
	STD_ERROR2;

      /* Diff > 200 */
      mpfr_set_str_binary (y,
       "0.10110111101101110010010010011011000001101101011011001E331");
      inexact1 = mpfr_sub1(x2, y, z, r);
      inexact2 = mpfr_sub1sp(x, y, z, r);
      if (mpfr_cmp(x, x2))
	STD_ERROR;
      if (inexact1 != inexact2)
	STD_ERROR2;

     mpfr_set_str_binary (y,
       "0.10000000000000000000000000000000000000000000000000000E31");
     mpfr_set_str_binary (z,
       "0.11111111111111111111111111111111111111111111111111111E30");
     inexact1 = mpfr_sub1(x2, y, z, r);
     inexact2 = mpfr_sub1sp(x, y, z, r);
     if (mpfr_cmp(x, x2))
       STD_ERROR;
     if (inexact1 != inexact2)
       STD_ERROR2;

     mpfr_set_str_binary (y,
       "0.10000000000000000000000000000000000000000000000000000E31");
     mpfr_set_str_binary (z,
       "0.11111111111111111111111111111111111111111111111111111E29");
     inexact1 = mpfr_sub1(x2, y, z, r);
     inexact2 = mpfr_sub1sp(x, y, z, r);
     if (mpfr_cmp(x, x2))
       STD_ERROR;
     if (inexact1 != inexact2)
       STD_ERROR2;

     mpfr_set_str_binary (y,
       "0.10000000000000000000000000000000000000000000000000000E52");
     mpfr_set_str_binary (z,
       "0.10000000000010000000000000000000000000000000000000000E00");
     inexact1 = mpfr_sub1(x2, y, z, r);
     inexact2 = mpfr_sub1sp(x, y, z, r);
     if (mpfr_cmp(x, x2))
       STD_ERROR;
     if (inexact1 != inexact2)
       STD_ERROR2;

     mpfr_set_str_binary (y,
	"0.11100000000000000000000000000000000000000000000000000E53");
     mpfr_set_str_binary (z,
	"0.10000000000000000000000000000000000000000000000000000E00");
     inexact1 = mpfr_sub1(x2, y, z, r);
     inexact2 = mpfr_sub1sp(z, y, z, r);
     mpfr_set(x, z, r);
     if (mpfr_cmp(x, x2))
       STD_ERROR;
     if (inexact1 != inexact2)
       STD_ERROR2;
     
     mpfr_set_str_binary (y,
       "0.10000000000000000000000000000000000000000000000000000E53");
     mpfr_set_str_binary (z,
       "0.10100000000000000000000000000000000000000000000000000E00");
     inexact1 = mpfr_sub1(x2, y, z, r);
     inexact2 = mpfr_sub1sp(x, y, z, r);
     if (mpfr_cmp(x, x2))
       STD_ERROR; 
     if (inexact1 != inexact2)
       STD_ERROR2;

     mpfr_set_str_binary (y,
	"0.10000000000000000000000000000000000000000000000000000E54");
     mpfr_set_str_binary (z,
	"0.10100000000000000000000000000000000000000000000000000E00");
     inexact1 = mpfr_sub1(x2, y, z, r);
     inexact2 = mpfr_sub1sp(x, y, z, r);
     if (mpfr_cmp(x, x2))
       STD_ERROR;
     if (inexact1 != inexact2)
       STD_ERROR2;
     
     p = 63;
     mpfr_set_prec(x, p);
     mpfr_set_prec(x2, p);
     mpfr_set_prec(y, p);
     mpfr_set_prec(z, p);
     mpfr_set_str_binary (y,
      "0.100000000000000000000000000000000000000000000000000000000000000E62");
     mpfr_set_str_binary (z,
      "0.110000000000000000000000000000000000000000000000000000000000000E00");
     inexact1 = mpfr_sub1(x2, y, z, r);
     inexact2 = mpfr_sub1sp(x, y, z, r);
     if (mpfr_cmp(x, x2))
       STD_ERROR;
     if (inexact1 != inexact2)
       STD_ERROR2;

     p = 64;
     mpfr_set_prec(x, 64);
     mpfr_set_prec(x2, 64);
     mpfr_set_prec(y, 64);
     mpfr_set_prec(z, 64);

     mpfr_set_str_binary (y,
      "0.1100000000000000000000000000000000000000000000000000000000000000E31");
     mpfr_set_str_binary (z,
      "0.1111111111111111111111111110000000000000000000000000011111111111E29");
     inexact1 = mpfr_sub1(x2, y, z, r);
     inexact2 = mpfr_sub1sp(x, y, z, r);
     if (mpfr_cmp(x, x2))
       STD_ERROR;
     if (inexact1 != inexact2)
       STD_ERROR2;

     mpfr_set_str_binary (y,
      "0.1000000000000000000000000000000000000000000000000000000000000000E63");
     mpfr_set_str_binary (z,
      "0.1011000000000000000000000000000000000000000000000000000000000000E00");
     inexact1 = mpfr_sub1(x2, y, z, r);
     inexact2 = mpfr_sub1sp(x, y, z, r);
     if (mpfr_cmp(x, x2))
       STD_ERROR;
     if (inexact1 != inexact2)
       STD_ERROR2;
     
     mpfr_set_str_binary (y,
      "0.1000000000000000000000000000000000000000000000000000000000000000E63");
     mpfr_set_str_binary (z,
      "0.1110000000000000000000000000000000000000000000000000000000000000E00");
     inexact1 = mpfr_sub1(x2, y, z, r);
     inexact2 = mpfr_sub1sp(x, y, z, r);
     if (mpfr_cmp(x, x2))
       STD_ERROR;
     if (inexact1 != inexact2)
       STD_ERROR2;

     mpfr_set_str_binary (y,
        "0.10000000000000000000000000000000000000000000000000000000000000E63");
     mpfr_set_str_binary (z,
        "0.10000000000000000000000000000000000000000000000000000000000000E00");
     inexact1 = mpfr_sub1(x2, y, z, r);
     inexact2 = mpfr_sub1sp(x, y, z, r);
     if (mpfr_cmp(x, x2))
       STD_ERROR;
     if (inexact1 != inexact2)
       STD_ERROR2;

     mpfr_set_str_binary (y,
      "0.1000000000000000000000000000000000000000000000000000000000000000E64");
     mpfr_set_str_binary (z,
      "0.1010000000000000000000000000000000000000000000000000000000000000E00");
     inexact1 = mpfr_sub1(x2, y, z, r);
     inexact2 = mpfr_sub1sp(x, y, z, r);
     if (mpfr_cmp(x, x2))
       STD_ERROR;
     if (inexact1 != inexact2)
       STD_ERROR2;

     MPFR_SET_NAN(x);
     MPFR_SET_NAN(x2);
     mpfr_set_str_binary (y,
      "0.1000000000000000000000000000000000000000000000000000000000000000"
			  "E-1073741823");
     mpfr_set_str_binary (z,
      "0.1100000000000000000000000000000000000000000000000000000000000000"
			  "E-1073741823");
     inexact1 = mpfr_sub1(x2, y, z, r);
     inexact2 = mpfr_sub1sp(x, y, z, r);
     if (mpfr_cmp(x, x2))
       STD_ERROR;
     if (inexact1 != inexact2)
       STD_ERROR2;

    }

  mpfr_clears(x,y,z,x2,NULL);  
}
