/* Test file for mpfr_set_str.

Copyright 1999, 2001, 2002, 2003 Free Software Foundation, Inc.

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
#include <limits.h>
#include <time.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "mpfr-test.h"

int
main (int argc, char *argv[])
{
  mpfr_t x, y;
  unsigned long k, bd, nc, i;
  char *str, *str2;
  mp_exp_t e;
  int base, logbase, prec, baseprec;

  tests_start_mpfr ();

  if (argc>=2) /* tset_str <string> <prec> */
    {
      prec = (argc>=3) ? atoi(argv[2]) : 53;
      mpfr_init2 (x, prec);
      mpfr_set_str_binary (x, argv[1]);
      mpfr_out_str (stdout, 10, 0, x, GMP_RNDN);
      puts ("");
      mpfr_clear (x);
      return 0;
    }

  mpfr_init2 (x, 2);

  nc = (argc > 1) ? atoi(argv[1]) : 53;
  if (nc < 100)
    nc = 100;

  bd = randlimb () & 8;

  str2 = str = (*__gmp_allocate_func) (nc * sizeof(char));

  if (bd)

    {
      for(k = 1; k <= bd; k++)
	*(str2++) = (randlimb () & 1) + '0';
    }
  else
    *(str2++) = '0';

  *(str2++) = '.';

  for (k = 1; k < nc - 17 - bd; k++)
    *(str2++) = '0' + (randlimb () & 1);

  *(str2++) = 'e';
  sprintf (str2, "%d", (int) (randlimb () & INT_MAX) + INT_MIN/2);

  mpfr_set_prec (x, nc + 10);
  mpfr_set_str_binary (x, str);

  mpfr_set_prec (x, 54);
  mpfr_set_str_binary (x, "0.100100100110110101001010010101111000001011100100101010E-529");
  mpfr_init2 (y, 54);
  mpfr_set_str (y, "4.936a52bc17254@-133", 16, GMP_RNDN);
  if (mpfr_cmp (x, y))
    {
      printf ("Error in mpfr_set_str (1a):\n");
      mpfr_print_binary (x);
      puts ("");
      mpfr_print_binary (y);
      puts ("");
      mpfr_clear (x);
      mpfr_clear (y);
      exit (1);
    }

  mpfr_set_str_binary (x, "0.111111101101110010111010100110000111011001010100001101E-529");
  mpfr_set_str (y, "0.fedcba98765434P-529", 16, GMP_RNDN);
  if (mpfr_cmp (x, y))
    {
      printf ("Error in mpfr_set_str (1b):\n");
      mpfr_print_binary (x);
      puts ("");
      mpfr_print_binary (y);
      puts ("");
      mpfr_clear (x);
      mpfr_clear (y);
      exit (1);
    }

  (*__gmp_free_func) (str, nc * sizeof(char));

  mpfr_set_prec (x, 53);
  mpfr_set_str_binary (x, "+110101100.01010000101101000000100111001000101011101110E00");

  mpfr_set_str_binary (x, "1.0");
  if (mpfr_get_d1 (x) != 1.0)
    {
      printf ("Error in mpfr_set_str_binary for s=1.0\n");
      mpfr_clear(x);
      mpfr_clear(y);
      exit(1);
    }

  mpfr_set_str_binary (x, "+0000");
  mpfr_set_str_binary (x, "+0000E0");
  mpfr_set_str_binary (x, "0000E0");
  if (mpfr_get_d1 (x) != 0.0)
    {
      printf ("Error in mpfr_set_str_binary for s=0.0\n");
      mpfr_clear (x);
      mpfr_clear (y);
      exit (1);
    }

  mpfr_set_str (x, "+243495834958.53452345E1", 10, GMP_RNDN);
  mpfr_set_str (x, "9007199254740993", 10, GMP_RNDN);
  mpfr_set_str (x, "9007199254740992", 10, GMP_RNDU);
  mpfr_set_str (x, "9007199254740992", 10, GMP_RNDD);
  mpfr_set_str (x, "9007199254740992", 10, GMP_RNDZ);

  /* check a random number printed and read is not modified */
  prec = 53;
  mpfr_set_prec (x, prec);
  mpfr_set_prec (y, prec);
  for (i=0;i<100000;i++)
    {
      mpfr_random (x);
      k = randlimb () % 4;
      logbase = (randlimb () % 5) + 1;
      base = 1 << logbase;
      /* Warning: the number of bits needed to print exactly a number of
	 'prec' bits in base 2^logbase may be greater than ceil(prec/logbase),
	 for example 0.11E-1 in base 2 cannot be written exactly with only
	 one digit in base 4 */
      if (base == 2)
	baseprec = prec;
      else
	baseprec = 1 + (prec - 2 + logbase) / logbase;
      str = mpfr_get_str (NULL, &e, base, baseprec, x, k);
      mpfr_set_str (y, str, base, k);
      MPFR_EXP(y) += logbase * (e - strlen (str));
      if (mpfr_cmp (x, y))
        {
          printf ("mpfr_set_str o mpfr_get_str <> id for rnd_mode=%s\n",
                  mpfr_print_rnd_mode (k));
          printf ("x=");
          mpfr_print_binary (x);
          puts ("");
          printf ("s=%s, exp=%d, base=%d\n", str, (int) e, base);
          printf ("y=");
          mpfr_print_binary (y);
          puts ("");
          mpfr_clear (x);
          mpfr_clear (y);
          exit (1);
        }
      (*__gmp_free_func) (str, strlen (str) + 1);
    }

  for (i = 2; i <= 36; i++)
    {
      if (mpfr_set_str (x, "@NaN@garbage", i, GMP_RNDN) != 0 ||
          !mpfr_nan_p(x))
        {
          printf ("mpfr_set_str failed on @NaN@garbage\n");
          exit (1);
        }

      if (mpfr_set_str (x, "@Inf@garbage", i, GMP_RNDN) != 0 ||
          !mpfr_inf_p(x) || MPFR_SIGN(x) < 0)
        {
          printf ("mpfr_set_str failed on @Inf@garbage\n");
          exit (1);
        }

      if (mpfr_set_str (x, "-@Inf@garbage", i, GMP_RNDN) != 0 ||
          !mpfr_inf_p(x) || MPFR_SIGN(x) > 0)
        {
          printf ("mpfr_set_str failed on -@Inf@garbage\n");
          exit (1);
        }

      if (mpfr_set_str (x, "+@Inf@garbage", i, GMP_RNDN) != 0 ||
          !mpfr_inf_p(x) || MPFR_SIGN(x) < 0)
        {
          printf ("mpfr_set_str failed on +@Inf@garbage\n");
          exit (1);
        }

      if (i > 16)
        continue;

      if (mpfr_set_str (x, "NaN", i, GMP_RNDN) != 0 ||
          !mpfr_nan_p(x))
        {
          printf ("mpfr_set_str failed on NaN\n");
          exit (1);
        }

      if (mpfr_set_str (x, "Inf", i, GMP_RNDN) != 0 ||
          !mpfr_inf_p(x) || MPFR_SIGN(x) < 0)
        {
          printf ("mpfr_set_str failed on Inf\n");
          exit (1);
        }

      if (mpfr_set_str (x, "-Inf", i, GMP_RNDN) != 0 ||
          !mpfr_inf_p(x) || MPFR_SIGN(x) > 0)
        {
          printf ("mpfr_set_str failed on -Inf\n");
          exit (1);
        }

      if (mpfr_set_str (x, "+Inf", i, GMP_RNDN) != 0 ||
          !mpfr_inf_p(x) || MPFR_SIGN(x) < 0)
        {
          printf ("mpfr_set_str failed on +Inf\n");
          exit (1);
        }
    }

  /* check that mpfr_set_str works for uppercase letters too */
  mpfr_set_prec (x, 10);
  mpfr_set_str (x, "B", 16, GMP_RNDN);
  if (mpfr_cmp_ui (x, 11) != 0)
    {
      printf ("mpfr_set_str does not work for uppercase letters\n");
      exit (1);
    }

  /* start of tests added by Alain Delplanque */

  /* in this example an overflow can occur */
  mpfr_set_prec (x, 64);
  mpfr_set_prec (y, 64);
  mpfr_set_str_binary (x, "1.0E-532");
  mpfr_set_str (y, "0.71128279983522479470@-160", 10, GMP_RNDU);
  if (mpfr_cmp (x, y))
    {
      printf ("Error in mpfr_set_str (2):\n");
      mpfr_print_binary (x);
      puts ("");
      mpfr_print_binary (y);
      puts ("");
      mpfr_clear (x);
      mpfr_clear (y);
      exit (1);
    }

  /* in this example, I think there was a pb in the old function :
     result of mpfr_set_str_old for the same number , but with more
     precision is: 1.111111111110000000000000000111111111111111111111111110000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100111000100001100000010101100111010e184
     this result is the same as mpfr_set_str */
  mpfr_set_prec (x, 64);
  mpfr_set_prec (y, 64);
  mpfr_set_str_binary (x, "1.111111111110000000000000000111111111111111111111111110000000001E184");
  mpfr_set_str (y, "0.jo08hg31hc5mmpj5mjjmgn55p2h35g@39", 27, GMP_RNDU);
  /* y = 49027884868983130654865109690613178467841148597221480052 */
  if (mpfr_cmp (x, y))
    {
      printf ("Error in mpfr_set_str (3):\n");
      mpfr_print_binary (x);
      puts ("");
      mpfr_print_binary (y);
      puts ("");
      mpfr_clear (x);
      mpfr_clear (y);
      exit (1);
    }

  /* not exact rounding in mpfr_set_str
     same number with more precision is : 1.111111111111111111111111111000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011011111101000001101110110010101101000010100110011101110010001110e195
     this result is the same as mpfr_set_str */
  /* problem was : can_round was call with GMP_RNDN round mode,
     so can_round use an error : 1/2 * 2^err * ulp(y)
     instead of 2^err * ulp(y)
     I have increase err by 1 */
  mpfr_set_prec (x, 64);  /* it was round down instead of up */
  mpfr_set_prec (y, 64);
  mpfr_set_str_binary (x, "1.111111111111111111111111111000000000000000000000000000000000001e195");
  mpfr_set_str (y, "0.6e23ekb6acgh96abk10b6c9f2ka16i@45", 21, GMP_RNDU);
  /* y = 100433627392042473064661483711179345482301462325708736552078 */
  if (mpfr_cmp (x, y))
    {
      printf ("Error in mpfr_set_str (4):\n");
      mpfr_print_binary (x);
      puts ("");
      mpfr_print_binary (y);
      puts ("");
      mpfr_clear (x);
      mpfr_clear (y);
      exit (1);
    }

  /* may be an error in mpfr_set_str_old
     with more precision : 1.111111100000001111110000000000011111011111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111110111101010001110111011000010111001011100110110e180 */
  mpfr_set_prec (x, 64);  /* it was round down instead of up */
  mpfr_set_prec (y, 64);
  mpfr_set_str_binary (x, "1.111111100000001111110000000000011111011111111111111111111111111e180");
  mpfr_set_str (y, "0.10j8j2k82ehahha56390df0a1de030@41", 23, GMP_RNDZ);
  /* y = 3053110535624388280648330929253842828159081875986159414 */
  if (mpfr_cmp (x, y))
    {
      printf ("Error in mpfr_set_str (5):\n");
      mpfr_print_binary (x);
      puts ("");
      mpfr_print_binary (y);
      puts ("");
      mpfr_clear (x);
      mpfr_clear (y);
      exit (1);
    }

  mpfr_set_prec (x, 64);
  mpfr_set_prec (y, 64);
  mpfr_set_str (y, "0.jrchfhpp9en7hidqm9bmcofid9q3jg@39", 28, GMP_RNDU);
  /* y = 196159429139499688661464718784226062699788036696626429952 */
  mpfr_set_str_binary (x, "0.1111111111111111111111111111111000000000000011100000001111100001E187");
  if (mpfr_cmp (x, y))
    {
      printf ("Error in mpfr_set_str (6):\n");
      mpfr_print_binary (x);
      puts ("");
      mpfr_print_binary (y);
      puts ("");
      mpfr_clear (x);
      mpfr_clear (y);
      exit (1);
    }

  mpfr_set_prec (x, 64);
  mpfr_set_prec (y, 64);
  mpfr_set_str (y, "0.h148m5ld5cf8gk1kd70b6ege92g6ba@47", 24, GMP_RNDZ);
  /* y = 52652933527468502324759448399183654588831274530295083078827114496 */
  mpfr_set_str_binary (x, "0.1111111111111100000000001000000000000000000011111111111111101111E215");
  if (mpfr_cmp (x, y))
    {
      printf ("Error in mpfr_set_str (7):\n");
      mpfr_print_binary (x);
      puts ("");
      mpfr_print_binary (y);
      puts ("");
      mpfr_clear (x);
      mpfr_clear (y);
      exit (1);
    }

  /* to check this problem : I convert limb (10--0 or 101--1) into base b
     with more than mp_bits_per_limb digits,
     so when convert into base 2 I should have
     the limb that I have choose */
  /* this use mpfr_get_str */
  {
    size_t nb_digit = mp_bits_per_limb;
    mp_limb_t check_limb[2] = {MPFR_LIMB_HIGHBIT, ~(MPFR_LIMB_HIGHBIT >> 1)};
    int base[3] = {10, 16, 19};
    mp_rnd_t rnd[3] = {GMP_RNDU, GMP_RNDN, GMP_RNDD};
    int cbase, climb, crnd;
    char *str;

    mpfr_set_prec (x, mp_bits_per_limb); /* x and y have only one limb */
    mpfr_set_prec (y, mp_bits_per_limb);

    str = (*__gmp_allocate_func) (100000 + 20);

    mpfr_set_ui (x, 1, GMP_RNDN); /* ensures that x is not NaN or Inf */
    for (; nb_digit < 100000; nb_digit *= 10)
      for (cbase = 0; cbase < 3; cbase++)
        for (climb = 0; climb < 2; climb++)
          for (crnd = 0; crnd < 3; crnd++)
            {
              char *str1;
              mp_exp_t exp;

              *(MPFR_MANT(x)) = check_limb[climb];
              MPFR_EXP(x) = 0;

              mpfr_get_str (str + 2, &exp, base[cbase],
                            nb_digit, x, rnd[crnd]);
              str[0] = '-';
              str[(str[2] == '-')] =  '0';
              str[(str[2] == '-') + 1] =  '.';

              for (str1 = str; *str1 != 0; str1++)
                ;
              sprintf (str1, "@%i", (int) exp);

              mpfr_set_str (y, str, base[cbase], rnd[2 - crnd]);

              if (mpfr_cmp (x, y) != 0)
                {
                  printf ("Error in mpfr_set_str for nb_digit=%u, base=%u, "
                          "rnd=%s:\n", (unsigned int) nb_digit, base[cbase],
                          mpfr_print_rnd_mode (rnd[crnd]));
                  printf ("instead of: ");
                  mpfr_print_binary (x);
                  puts ("");
                  printf ("return    : ");
                  mpfr_print_binary (y);
                  puts ("");
                }
            }

    (*__gmp_free_func) (str, 100000 + 20);
  }

  /* end of tests added by Alain Delplanque */

  /* check that flags are correctly cleared */
  mpfr_set_nan (x);
  mpfr_set_str (x, "+0.0", 10, GMP_RNDN);
  if (!mpfr_number_p(x) || mpfr_cmp_ui (x, 0) != 0 || mpfr_sgn (x) < 0)
    {
      printf ("x <- +0.0 failed after x=NaN\n");
      exit (1);
    }
  mpfr_set_str (x, "-0.0", 10, GMP_RNDN);
  if (!mpfr_number_p(x) || mpfr_cmp_ui (x, 0) != 0 || mpfr_sgn (x) > 0)
    {
      printf ("x <- -0.0 failed after x=NaN\n");
      exit (1);
    }


  mpfr_clear (x);
  mpfr_clear (y);

  tests_end_mpfr ();
  return 0;
}
