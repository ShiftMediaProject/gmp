/* Test file for mpfr_add and mpfr_sub.

Copyright 1999, 2000, 2001, 2002, 2003 Free Software Foundation.

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

#define N 100000

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>
#include "gmp.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "mpfr-test.h"

void checknan _PROTO((double, double, mp_rnd_t, unsigned int, unsigned int, unsigned int)); 
void check3 _PROTO((double, double, mp_rnd_t));
void check4 _PROTO((double, double, mp_rnd_t));
void check5 _PROTO((double, mp_rnd_t));
void check2 _PROTO((double, int, double, int, int, int)); 
void check2a _PROTO((double, int, double, int, int, int, char *)); 
void check64 _PROTO((void)); 
void check_same _PROTO((void)); 
void check_case_1b _PROTO((void)); 
void check_case_2 _PROTO((void));
void check_inexact _PROTO((void));


/* Parameter "z1" of check() used to be last in the argument list, but that
   tickled a bug in 32-bit sparc gcc 2.95.2.  A "double" in that position is
   passed on the stack at an address which is 4mod8, but the generated code
   didn't take into account that alignment, resulting in bus errors.  The
   easiest workaround is to move it to the start of the arg list (where it's
   passed in registers), this macro does that.  FIXME: Change the actual
   calls to check(), rather than using a macro.  */

#define check(x,y,rnd_mode,px,py,pz,z1)  _check(x,y,z1,rnd_mode,px,py,pz)

/* checks that x+y gives the same results in double
   and with mpfr with 53 bits of precision */
static void
_check (double x, double y, double z1, mp_rnd_t rnd_mode, unsigned int px, 
        unsigned int py, unsigned int pz)
{
  double z2; mpfr_t xx,yy,zz; int cert=0;

  mpfr_init2(xx, px);
  mpfr_init2(yy, py);
  mpfr_init2(zz, pz);
  mpfr_set_d(xx, x, rnd_mode);
  mpfr_set_d(yy, y, rnd_mode);
  mpfr_add(zz, xx, yy, rnd_mode);
  if (px==53 && py==53 && pz==53)
    cert = 1;
  z2 = mpfr_get_d1 (zz);
  mpfr_set_d (yy, z2, GMP_RNDN);
  if (!mpfr_cmp (zz, yy) && cert && z1!=z2 && !(Isnan(z1) && Isnan(z2)))
    {
      printf ("expected sum is %1.20e, got %1.20e\n", z1, z2);
      printf ("mpfr_add failed for x=%1.20e y=%1.20e with rnd_mode=%s\n",
              x, y, mpfr_print_rnd_mode (rnd_mode));
      exit (1);
    }
  mpfr_clear (xx);
  mpfr_clear (yy);
  mpfr_clear (zz);
}

void
checknan (double x, double y, mp_rnd_t rnd_mode, unsigned int px, 
          unsigned int py, unsigned int pz)
{
  double z2;
  mpfr_t xx, yy, zz;

  mpfr_init2 (xx, px);
  mpfr_init2 (yy, py);
  mpfr_init2 (zz, pz);
  mpfr_set_d (xx, x, rnd_mode);
  mpfr_set_d (yy, y, rnd_mode);
  mpfr_add (zz, xx, yy, rnd_mode);
  if (MPFR_IS_NAN(zz) == 0)
    {
      printf ("Error, not an MPFR_NAN for xx = %1.20e, y = %1.20e\n", x, y);
      exit (1);
    }
  z2 = mpfr_get_d1 (zz);
  if (!Isnan(z2))
    {
      printf ("Error, not a NaN after conversion, xx = %1.20e yy = %1.20e, got %1.20e\n", x, y, z2);
      exit (1);
    }

  mpfr_clear (xx);
  mpfr_clear (yy);
  mpfr_clear (zz);
}

void
check2a (double x, int px, double y, int py, int pz, mp_rnd_t rnd_mode,
	      char *res)
{
  mpfr_t xx, yy, zz;

  mpfr_init2(xx,px); mpfr_init2(yy,py); mpfr_init2(zz,pz);
  mpfr_set_d(xx, x, rnd_mode);
  mpfr_set_d(yy, y, rnd_mode);
  mpfr_add(zz, xx, yy, rnd_mode);
  mpfr_set_prec(xx, pz);
  mpfr_set_str(xx, res, 16, GMP_RNDN);
  if (mpfr_cmp(xx, zz)) { 
    printf("x=%1.20e,%d y=%1.20e,%d pz=%d,rnd=%s\n",
	   x,px,y,py,pz,mpfr_print_rnd_mode(rnd_mode));
    printf("got        "); mpfr_print_binary(zz); putchar('\n');
    printf("instead of "); mpfr_print_binary(xx); putchar('\n');
    exit(1); 
  }
  mpfr_clear(xx); mpfr_clear(yy); mpfr_clear(zz);
}

void
check64 (void)
{
  mpfr_t x, t, u;

  mpfr_init (x);
  mpfr_init (t);
  mpfr_init (u);

  mpfr_set_prec (x, 29);
  mpfr_set_str_raw (x, "1.1101001000101111011010010110e-3");
  mpfr_set_prec (t, 58);
  mpfr_set_str_raw (t, "0.11100010011111001001100110010111110110011000000100101E-1");
  mpfr_set_prec (u, 29);
  mpfr_add (u, x, t, GMP_RNDD);
  mpfr_set_str_raw (t, "1.0101011100001000011100111110e-1");
  if (mpfr_cmp (u, t))
    {
      fprintf (stderr, "mpfr_add(u, x, t) failed for prec(x)=29, prec(t)=58\n");
      printf ("expected "); mpfr_out_str (stdout, 2, 29, t, GMP_RNDN);
      putchar ('\n');
      printf ("got      "); mpfr_out_str (stdout, 2, 29, u, GMP_RNDN);
      putchar ('\n');
      exit(1);
    }

  mpfr_set_prec (x, 4);
  mpfr_set_str_raw (x, "-1.0E-2");
  mpfr_set_prec (t, 2);
  mpfr_set_str_raw (t, "-1.1e-2");
  mpfr_set_prec (u, 2);
  mpfr_add (u, x, t, GMP_RNDN);
  if (MPFR_MANT(u)[0] << 2)
    {
      fprintf (stderr, "result not normalized for prec=2\n");
      mpfr_print_binary (u); putchar ('\n');
      exit (1);
    }
  mpfr_set_str_raw (t, "-1.0e-1");
  if (mpfr_cmp (u, t))
    {
      fprintf (stderr, "mpfr_add(u, x, t) failed for prec(x)=4, prec(t)=2\n");
      printf ("expected -1.0e-1\n");
      printf ("got      "); mpfr_out_str (stdout, 2, 4, u, GMP_RNDN);
      putchar ('\n');
      exit (1);
    }

  mpfr_set_prec (x, 8);
  mpfr_set_str_raw (x, "-0.10011010"); /* -77/128 */
  mpfr_set_prec (t, 4);
  mpfr_set_str_raw (t, "-1.110e-5"); /* -7/128 */
  mpfr_set_prec (u, 4);
  mpfr_add (u, x, t, GMP_RNDN); /* should give -5/8 */
  mpfr_set_str_raw (t, "-1.010e-1");
  if (mpfr_cmp (u, t)) {
    fprintf (stderr, "mpfr_add(u, x, t) failed for prec(x)=8, prec(t)=4\n");
    printf ("expected -1.010e-1\n");
    printf ("got      "); mpfr_out_str (stdout, 2, 4, u, GMP_RNDN);
    putchar ('\n');
    exit (1);
  }

  mpfr_set_prec (x, 112); mpfr_set_prec (t, 98); mpfr_set_prec (u, 54);
  mpfr_set_str_raw (x, "-0.11111100100000000011000011100000101101010001000111E-401");
  mpfr_set_str_raw (t, "0.10110000100100000101101100011111111011101000111000101E-464");
  mpfr_add (u, x, t, GMP_RNDN);
  if (mpfr_cmp (u, x)) {
    fprintf (stderr, "mpfr_add(u, x, t) failed for prec(x)=112, prec(t)=98\n");
    exit (1);
  }

  mpfr_set_prec (x, 92); mpfr_set_prec (t, 86); mpfr_set_prec (u, 53);
  mpfr_set_d (x, -5.03525136761487735093e-74, GMP_RNDN);
  mpfr_set_d (t, 8.51539046314262304109e-91, GMP_RNDN);
  mpfr_add (u, x, t, GMP_RNDN);
  if (mpfr_get_d1 (u) != -5.0352513676148773509283672e-74) {
    fprintf (stderr, "mpfr_add(u, x, t) failed for prec(x)=92, prec(t)=86\n");
    exit (1);
  }

  mpfr_set_prec(x, 53); mpfr_set_prec(t, 76); mpfr_set_prec(u, 76);
  mpfr_set_str_raw(x, "-0.10010010001001011011110000000000001010011011011110001E-32");
  mpfr_set_str_raw(t, "-0.1011000101110010000101111111011111010001110011110111100110101011110010011111");
  mpfr_sub(u, x, t, GMP_RNDU);
  mpfr_set_str_raw(t, "0.1011000101110010000101111111011100111111101010011011110110101011101000000100");
  if (mpfr_cmp(u,t)) {
    printf("expect "); mpfr_print_binary(t); putchar('\n');
    fprintf (stderr, "mpfr_add failed for precisions 53-76\n"); exit(1);
  }
  mpfr_set_prec(x, 53); mpfr_set_prec(t, 108); mpfr_set_prec(u, 108);
  mpfr_set_str_raw(x, "-0.10010010001001011011110000000000001010011011011110001E-32");
  mpfr_set_str_raw(t, "-0.101100010111001000010111111101111101000111001111011110011010101111001001111000111011001110011000000000111111");
  mpfr_sub(u, x, t, GMP_RNDU);
  mpfr_set_str_raw(t, "0.101100010111001000010111111101110011111110101001101111011010101110100000001011000010101110011000000000111111");
  if (mpfr_cmp(u,t)) {
    printf("expect "); mpfr_print_binary(t); putchar('\n');
    fprintf(stderr, "mpfr_add failed for precisions 53-108\n"); exit(1);
  }
  mpfr_set_prec(x, 97); mpfr_set_prec(t, 97); mpfr_set_prec(u, 97);
  mpfr_set_str_raw(x, "0.1111101100001000000001011000110111101000001011111000100001000101010100011111110010000000000000000E-39");
  mpfr_set_ui(t, 1, GMP_RNDN);
  mpfr_add(u, x, t, GMP_RNDN);
  mpfr_set_str_raw(x, "0.1000000000000000000000000000000000000000111110110000100000000101100011011110100000101111100010001E1");
  if (mpfr_cmp(u,x)) {
    fprintf(stderr, "mpfr_add failed for precision 97\n"); exit(1);
  }
  mpfr_set_prec(x, 128); mpfr_set_prec(t, 128); mpfr_set_prec(u, 128);
  mpfr_set_str_raw(x, "0.10101011111001001010111011001000101100111101000000111111111011010100001100011101010001010111111101111010100110111111100101100010E-4");
  mpfr_set(t, x, GMP_RNDN);
  mpfr_sub(u, x, t, GMP_RNDN);
  mpfr_set_prec(x, 96); mpfr_set_prec(t, 96); mpfr_set_prec(u, 96);
  mpfr_set_str_raw(x, "0.111000000001110100111100110101101001001010010011010011100111100011010100011001010011011011000010E-4");
  mpfr_set(t, x, GMP_RNDN);
  mpfr_sub(u, x, t, GMP_RNDN);
  mpfr_set_prec(x, 85); mpfr_set_prec(t, 85); mpfr_set_prec(u, 85);
  mpfr_set_str_raw(x, "0.1111101110100110110110100010101011101001100010100011110110110010010011101100101111100E-4");
  mpfr_set_str_raw(t, "0.1111101110100110110110100010101001001000011000111000011101100101110100001110101010110E-4");
  mpfr_sub(u, x, t, GMP_RNDU);
  mpfr_sub(x, x, t, GMP_RNDU);
  if (mpfr_cmp(x, u) != 0) {
    printf("Error in mpfr_sub: u=x-t and x=x-t give different results\n");
    exit(1);
  }
  if ((MPFR_MANT(u)[(MPFR_PREC(u)-1)/mp_bits_per_limb] & 
      ((mp_limb_t)1<<(mp_bits_per_limb-1)))==0) {
    printf("Error in mpfr_sub: result is not msb-normalized (1)\n"); exit(1);
  }
  mpfr_set_prec(x, 65); mpfr_set_prec(t, 65); mpfr_set_prec(u, 65);
  mpfr_set_str_raw(x, "0.10011010101000110101010000000011001001001110001011101011111011101E623");
  mpfr_set_str_raw(t, "0.10011010101000110101010000000011001001001110001011101011111011100E623");
  mpfr_sub(u, x, t, GMP_RNDU);
  if (mpfr_get_d1 (u) != 9.4349060620538533806e167) { /* 2^558 */
    printf("Error (1) in mpfr_sub\n"); exit(1);
  }

  mpfr_set_prec(x, 64); mpfr_set_prec(t, 64); mpfr_set_prec(u, 64);
  mpfr_set_str_raw(x, "0.1000011110101111011110111111000011101011101111101101101100000100E-220");
  mpfr_set_str_raw(t, "0.1000011110101111011110111111000011101011101111101101010011111101E-220");
  mpfr_add(u, x, t, GMP_RNDU);
  if ((MPFR_MANT(u)[0] & 1) != 1) { 
    printf("error in mpfr_add with rnd_mode=GMP_RNDU\n");
    printf("b=  "); mpfr_print_binary(x); putchar('\n');
    printf("c=  "); mpfr_print_binary(t); putchar('\n');
    printf("b+c="); mpfr_print_binary(u); putchar('\n');
    exit(1);
  }

  /* bug found by Norbert Mueller, 14 Sep 2000 */
  mpfr_set_prec(x, 56); mpfr_set_prec(t, 83); mpfr_set_prec(u, 10);
  mpfr_set_str_raw(x, "0.10001001011011001111101100110100000101111010010111010111E-7");
  mpfr_set_str_raw(t, "0.10001001011011001111101100110100000101111010010111010111000000000111110110110000100E-7");
  mpfr_sub(u, x, t, GMP_RNDU);

  /* array bound write found by Norbert Mueller, 26 Sep 2000 */
  mpfr_set_prec(x, 109); mpfr_set_prec(t, 153); mpfr_set_prec(u, 95);
  mpfr_set_str_raw(x,"0.1001010000101011101100111000110001111111111111111111111111111111111111111111111111111111111111100000000000000E33");
  mpfr_set_str_raw(t,"-0.100101000010101110110011100011000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011100101101000000100100001100110111E33");
  mpfr_add(u, x, t, GMP_RNDN);

  /* array bound writes found by Norbert Mueller, 27 Sep 2000 */
  mpfr_set_prec(x, 106); mpfr_set_prec(t, 53); mpfr_set_prec(u, 23);
  mpfr_set_str_raw(x, "-0.1000011110101111111001010001000100001011000000000000000000000000000000000000000000000000000000000000000000E-59");
  mpfr_set_str_raw(t, "-0.10000111101011111110010100010001101100011100110100000E-59");
  mpfr_sub(u, x, t, GMP_RNDN);
  mpfr_set_prec(x, 177); mpfr_set_prec(t, 217); mpfr_set_prec(u, 160);
  mpfr_set_str_raw(x, "-0.111010001011010000111001001010010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000E35");
  mpfr_set_str_raw(t, "0.1110100010110100001110010010100100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111011010011100001111001E35");
  mpfr_add(u, x, t, GMP_RNDN);
  mpfr_set_prec(x, 214); mpfr_set_prec(t, 278); mpfr_set_prec(u, 207);
  mpfr_set_str_raw(x, "0.1000100110100110101101101101000000010000100111000001001110001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000E66");
  mpfr_set_str_raw(t, "-0.10001001101001101011011011010000000100001001110000010011100010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001111011111001001100011E66");
  mpfr_add(u, x, t, GMP_RNDN);
  mpfr_set_prec(x, 32); mpfr_set_prec(t, 247); mpfr_set_prec(u, 223);
  mpfr_set_str_raw(x, "0.10000000000000000000000000000000E1");
  mpfr_set_str_raw(t, "0.1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111100000110001110100000100011110000101110110011101110100110110111111011010111100100000000000000000000000000E0");
  mpfr_sub(u, x, t, GMP_RNDN);
  if ((MPFR_MANT(u)[(MPFR_PREC(u)-1)/mp_bits_per_limb] & 
      ((mp_limb_t)1<<(mp_bits_per_limb-1)))==0) {
    printf("Error in mpfr_sub: result is not msb-normalized (2)\n"); exit(1);
  }

  /* bug found by Nathalie Revol, 21 March 2001 */
  mpfr_set_prec (x, 65);
  mpfr_set_prec (t, 65);
  mpfr_set_prec (u, 65);
  mpfr_set_str_raw (x, "0.11100100101101001100111011111111110001101001000011101001001010010E-35");
  mpfr_set_str_raw (t, "0.10000000000000000000000000000000000001110010010110100110011110000E1");
  mpfr_sub (u, t, x, GMP_RNDU);
  if ((MPFR_MANT(u)[(MPFR_PREC(u)-1)/mp_bits_per_limb] & 
      ((mp_limb_t)1<<(mp_bits_per_limb-1)))==0) {
    fprintf(stderr, "Error in mpfr_sub: result is not msb-normalized (3)\n");
    exit (1);
  }

  /* bug found by Fabrice Rouillier, 27 Mar 2001 */
  mpfr_set_prec (x, 107);
  mpfr_set_prec (t, 107);
  mpfr_set_prec (u, 107);
  mpfr_set_str_raw (x, "0.10111001001111010010001000000010111111011011011101000001001000101000000000000000000000000000000000000000000E315");
  mpfr_set_str_raw (t, "0.10000000000000000000000000000000000101110100100101110110000001100101011111001000011101111100100100111011000E350");
  mpfr_sub (u, x, t, GMP_RNDU);
  if ((MPFR_MANT(u)[(MPFR_PREC(u)-1)/mp_bits_per_limb] & 
      ((mp_limb_t)1<<(mp_bits_per_limb-1)))==0) {
    fprintf(stderr, "Error in mpfr_sub: result is not msb-normalized (4)\n");
    exit (1);
  }
  
  /* checks that NaN flag is correctly reset */
  mpfr_set_d (t, 1.0, GMP_RNDN);
  mpfr_set_d (u, 1.0, GMP_RNDN);
  mpfr_set_nan (x);
  mpfr_add (x, t, u, GMP_RNDN);
  if (mpfr_cmp_ui (x, 2)) {
    fprintf (stderr, "Error in mpfr_add: 1+1 gives %e\n", mpfr_get_d1 (x));
    exit (1);
  }

  mpfr_clear(x); mpfr_clear(t); mpfr_clear(u);
}

/* check case when c does not overlap with a, but both b and c count
   for rounding */
void
check_case_1b (void)
{
  mpfr_t a, b, c;
  unsigned int prec_a, prec_b, prec_c, dif;

  mpfr_init (a);
  mpfr_init (b);
  mpfr_init (c);

  for (prec_a = 2; prec_a <= 64; prec_a++)
    {
      mpfr_set_prec (a, prec_a);
      for (prec_b = prec_a + 2; prec_b <= 64; prec_b++)
	{
	  dif = prec_b - prec_a;
	  mpfr_set_prec (b, prec_b);
	  /* b = 1 - 2^(-prec_a) + 2^(-prec_b) */
	  mpfr_set_ui (b, 1, GMP_RNDN);
	  mpfr_div_2exp (b, b, dif, GMP_RNDN);
	  mpfr_sub_ui (b, b, 1, GMP_RNDN);
	  mpfr_div_2exp (b, b, prec_a, GMP_RNDN);
	  mpfr_add_ui (b, b, 1, GMP_RNDN);
	  for (prec_c = dif; prec_c <= 64; prec_c++)
	    {
	      /* c = 2^(-prec_a) - 2^(-prec_b) */
	      mpfr_set_prec (c, prec_c);
	      mpfr_set_si (c, -1, GMP_RNDN);
	      mpfr_div_2exp (c, c, dif, GMP_RNDN);
	      mpfr_add_ui (c, c, 1, GMP_RNDN);
	      mpfr_div_2exp (c, c, prec_a, GMP_RNDN);
	      mpfr_add (a, b, c, GMP_RNDN);
	      if (mpfr_cmp_ui (a, 1) != 0)
		{
		  fprintf (stderr, "case (1b) failed for prec_a=%u, prec_b=%u, prec_c=%u\n", prec_a, prec_b, prec_c);
		  printf("b="); mpfr_print_binary(b); putchar('\n');
		  printf("c="); mpfr_print_binary(c); putchar('\n');
		  printf("a="); mpfr_print_binary(a); putchar('\n');
		  exit (1);
		}
	    }
	}
    }

  mpfr_clear (a);
  mpfr_clear (b);
  mpfr_clear (c);
}

/* check case when c overlaps with a */
void
check_case_2 (void)
{
  mpfr_t a, b, c, d;

  mpfr_init2 (a, 300);
  mpfr_init2 (b, 800);
  mpfr_init2 (c, 500);
  mpfr_init2 (d, 800);

  mpfr_set_str_raw(a, "1E110");  /* a = 2^110 */
  mpfr_set_str_raw(b, "1E900");  /* b = 2^900 */
  mpfr_set_str_raw(c, "1E500");  /* c = 2^500 */
  mpfr_add(c, c, a, GMP_RNDZ);   /* c = 2^500 + 2^110 */
  mpfr_sub(d, b, c, GMP_RNDZ);   /* d = 2^900 - 2^500 - 2^110 */
  mpfr_add(b, b, c, GMP_RNDZ);   /* b = 2^900 + 2^500 + 2^110 */
  mpfr_add(a, b, d, GMP_RNDZ);   /* a = 2^901 */
  if (mpfr_cmp_ui_2exp (a, 1, 901))
    {
      fprintf (stderr, "b + d fails for b=2^900+2^500+2^110, d=2^900-2^500-2^110\n");
      fprintf (stderr, "expected 1.0e901, got ");
      mpfr_out_str (stderr, 2, 0, a, GMP_RNDN);
      fprintf (stderr, "\n");
      exit (1);
    }

  mpfr_clear (a);
  mpfr_clear (b);
  mpfr_clear (c);
  mpfr_clear (d);
}

/* checks when source and destination are equal */
void
check_same (void)
{
  mpfr_t x;

  mpfr_init(x); mpfr_set_d(x, 1.0, GMP_RNDZ);
  mpfr_add(x, x, x, GMP_RNDZ);
  if (mpfr_get_d1 (x) != 2.0) {
    printf("Error when all 3 operands are equal\n"); exit(1);
  }
  mpfr_clear(x);
}

#define check53(x, y, r, z) check(x, y, r, 53, 53, 53, z)
#define check53nan(x, y, r) checknan(x, y, r, 53, 53, 53); 

#define MAX_PREC 100

void
check_inexact (void)
{
  mpfr_t x, y, z, u;
  mp_prec_t px, py, pu, pz;
  int inexact, cmp;
  mp_rnd_t rnd;
  
  mpfr_init (x);
  mpfr_init (y);
  mpfr_init (z);
  mpfr_init (u);

  mpfr_set_prec (x, 2);
  mpfr_set_str_raw (x, "0.1E-4");
  mpfr_set_prec (u, 33);
  mpfr_set_str_raw (u, "0.101110100101101100000000111100000E-1");
  mpfr_set_prec (y, 31);
  if ((inexact = mpfr_add (y, x, u, GMP_RNDN)))
    {
      fprintf (stderr, "Wrong inexact flag (2): expected 0, got %d\n", inexact);
      exit (1);
    }

  mpfr_set_prec (x, 2);
  mpfr_set_str_raw (x, "0.1E-4");
  mpfr_set_prec (u, 33);
  mpfr_set_str_raw (u, "0.101110100101101100000000111100000E-1");
  mpfr_set_prec (y, 28);
  if ((inexact = mpfr_add (y, x, u, GMP_RNDN)))
    {
      fprintf (stderr, "Wrong inexact flag (1): expected 0, got %d\n", inexact);
      exit (1);
    }

  for (px=2; px<MAX_PREC; px++)
    {
      mpfr_set_prec (x, px);
      mpfr_random (x);
      for (pu=2; pu<MAX_PREC; pu++)
	{
	  mpfr_set_prec (u, pu);
	  mpfr_random (u);
	  for (py=2; py<MAX_PREC; py++)
	    {
	      mpfr_set_prec (y, py);
	      pz =  (mpfr_cmpabs (x, u) >= 0) ? MPFR_EXP(x)-MPFR_EXP(u)
		: MPFR_EXP(u)-MPFR_EXP(x);
	      /* x + u is exactly representable with precision
		 abs(EXP(x)-EXP(u)) + max(prec(x), prec(u)) + 1 */
	      pz = pz + MAX(MPFR_PREC(x), MPFR_PREC(u)) + 1;
	      mpfr_set_prec (z, pz);
	      rnd = LONG_RAND () % 4;
	      if (mpfr_add (z, x, u, rnd))
		{
		  fprintf (stderr, "z <- x + u should be exact\n");
		  printf ("x="); mpfr_print_binary (x); putchar ('\n');
		  printf ("u="); mpfr_print_binary (u); putchar ('\n');
		  printf ("z="); mpfr_print_binary (z); putchar ('\n');
		  exit (1);
		}
	      for (rnd=0; rnd<4; rnd++)
		{
		  inexact = mpfr_add (y, x, u, rnd);
		  cmp = mpfr_cmp (y, z);
		  if (((inexact == 0) && (cmp != 0)) ||
		      ((inexact > 0) && (cmp <= 0)) ||
		      ((inexact < 0) && (cmp >= 0)))
		    {
		      fprintf (stderr, "Wrong inexact flag for rnd=%s\n",
			   mpfr_print_rnd_mode(rnd));
		      printf ("expected %d, got %d\n", cmp, inexact);
		      printf ("x="); mpfr_print_binary (x); putchar ('\n');
		      printf ("u="); mpfr_print_binary (u); putchar ('\n');
		      printf ("y=  "); mpfr_print_binary (y); putchar ('\n');
		      printf ("x+u="); mpfr_print_binary (z); putchar ('\n');
		      exit (1);
		    }
		}
	    }
	}
    }

  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (z);
  mpfr_clear (u);
}

int
main (int argc, char *argv[])
{
  double x;
  int i;

  tests_start_mpfr ();

  mpfr_test_init ();
  check_inexact ();
  check_case_1b ();
  check_case_2 ();
  check64();
  check(293607738.0, 1.9967571564050541e-5, GMP_RNDU, 64, 53, 53,
	2.9360773800002003e8);
  check(880524.0, -2.0769715792901673e-5, GMP_RNDN, 64, 53, 53,
	8.8052399997923023e5);
  check(1196426492.0, -1.4218093058435347e-3, GMP_RNDN, 64, 53, 53,
	1.1964264919985781e9);
  check(982013018.0, -8.941829477291838e-7, GMP_RNDN, 64, 53, 53,
	9.8201301799999905e8);
  check(1092583421.0, 1.0880649218158844e9, GMP_RNDN, 64, 53, 53,
	2.1806483428158846e9);
  check(1.8476886419022969e-6, 961494401.0, GMP_RNDN, 53, 64, 53,
	9.6149440100000179e8);
  check(-2.3222118418069868e5, 1229318102.0, GMP_RNDN, 53, 64, 53,
	1.2290858808158193e9);
  check(-3.0399171300395734e-6, 874924868.0, GMP_RNDN, 53, 64, 53,
	8.749248679999969e8);
  check(9.064246624706179e1, 663787413.0, GMP_RNDN, 53, 64, 53,
	6.6378750364246619e8);
  check(-1.0954322421551264e2, 281806592.0, GMP_RNDD, 53, 64, 53,
	2.8180648245677572e8);
  check(5.9836930386056659e-8, 1016217213.0, GMP_RNDN, 53, 64, 53,
	1.0162172130000001e9);
  check(-1.2772161928500301e-7, 1237734238.0, GMP_RNDN, 53, 64, 53,
	1.2377342379999998e9);
  check(-4.567291988483277e8, 1262857194.0, GMP_RNDN, 53, 64, 53,
	8.0612799515167236e8);
  check(4.7719471752925262e7, 196089880.0, GMP_RNDN, 53, 53, 53, 
	2.4380935175292528e8);
  check(4.7719471752925262e7, 196089880.0, GMP_RNDN, 53, 64, 53, 
	2.4380935175292528e8);
  check(-1.716113812768534e-140, 1271212614.0, GMP_RNDZ, 53, 64, 53,
	1.2712126139999998e9);
  check(-1.2927455200185474e-50, 1675676122.0, GMP_RNDD, 53, 64, 53,
	1.6756761219999998e9);
  check53(1.22191250737771397120e+20, 948002822.0, GMP_RNDN, 
	  122191250738719408128.0);
  check53(9966027674114492.0, 1780341389094537.0, GMP_RNDN,
	  11746369063209028.0);
  check53(2.99280481918991653800e+272, 5.34637717585790933424e+271, GMP_RNDN,
	  3.5274425367757071711e272);
  check_same();
  check53(6.14384195492641560499e-02, -6.14384195401037683237e-02, GMP_RNDU,
	  9.1603877261370314499e-12);
  check53(1.16809465359248765399e+196, 7.92883212101990665259e+196, GMP_RNDU,
	  9.0969267746123943065e196);
  check53(3.14553393112021279444e-67, 3.14553401015952024126e-67, GMP_RNDU,
	  6.2910679412797336946e-67);

  SEED_RAND (time(NULL));
  check53(5.43885304644369509058e+185,-1.87427265794105342763e-57,GMP_RNDN,
	  5.4388530464436950905e185);
  check53(5.43885304644369509058e+185,-1.87427265794105342763e-57, GMP_RNDZ,
	  5.4388530464436944867e185);
  check53(5.43885304644369509058e+185,-1.87427265794105342763e-57, GMP_RNDU,
	  5.4388530464436950905e185);
  check53(5.43885304644369509058e+185,-1.87427265794105342763e-57, GMP_RNDD,
	  5.4388530464436944867e185);
  check2a(6.85523243386777784171e+107,187,-2.78148588123699111146e+48,87,178,
	  GMP_RNDD, "4.ab980a5cb9407ffffffffffffffffffffffffffffffe@89");
  check2a(-1.21510626304662318398e+145,70,1.21367733647758957118e+145,65,61,
	 GMP_RNDD, "-1.2bfad031d94@118");
  check2a(2.73028857032080744543e+155,83,-1.16446121423113355603e+163,59,125,
	  GMP_RNDZ, "-3.3c42dee09703d0639a6@135");
  check2a(-4.38589520019641698848e+78,155,-1.09923643769309483415e+72,15,159,
	  GMP_RNDD, "-2.5e09955c663d@65");
  check2a(-1.49963910666191123860e+265,76,-2.30915090591874527520e-191,8,75,
	  GMP_RNDZ, "-1.dc3ec027da54e@220");
  check2a(3.25471707846623300604e-160,81,-7.93846654265839958715e-274,58,54,
	  GMP_RNDN, "4.936a52bc17254@-133");
  check2a(5.17945380930936917508e+112,119,1.11369077158813567738e+108,15,150,
	  GMP_RNDZ, "5.62661692498ec@93");
  check2a(-2.66910493504493276454e-52,117,1.61188644159592323415e-52,61,68,
	  GMP_RNDZ, "-a.204acdd25d788@-44");
  check2a(-1.87427265794105342764e-57,175,1.76570844587489516446e+190,2,115,
	  GMP_RNDZ, "b.fffffffffffffffffffffffffffe@157");
  check2a(-1.15706375390780417299e-135,94,-1.07455137477117851576e-129,66,111,
	  GMP_RNDU, "-b.eae2643497ff6286b@-108");
  check2a(-1.15706375390780417299e-135,94,-1.07455137477117851576e-129,66,111,
	  GMP_RNDD, "-b.eae2643497ff6286b@-108");
  check2a(-3.31624349995221499866e-22,107,-8.20150212714204839621e+156,79,99,
	 GMP_RNDD, "-2.63b22b55697e8000000000008@130");
  x = -5943982715394951.0; for (i=0; i<446; i++) x *= 2.0;
  check2a(x, 63, 1.77607317509426332389e+73, 64, 64, GMP_RNDN,
	  "-5.4781549356e1c@124");
  check2a(4.49465557237618783128e+53,108,-2.45103927353799477871e+48,60,105,
	  GMP_RNDN, "4.b14f230f909dc803e@44");
  check2a(2.26531902208967707071e+168,99,-2.67795218510613988524e+168,67,94,
	GMP_RNDU, "-1.bfd7ff2647098@139");
  check2a(-8.88471912490080158206e+253,79,-7.84488427404526918825e+124,95,53,
	  GMP_RNDD, "-c.1e533b8d835@210");
  check2a(-2.18548638152863831959e-125,61,-1.22788940592412363564e+226,71,54,
	  GMP_RNDN, "-8.4b0f99ffa3b58@187");
  check2a(-7.94156823309993162569e+77,74,-5.26820160805275124882e+80,99,101,
	  GMP_RNDD, "-1.1cc90f11d6af26f4@67");
  check2a(-3.85170653452493859064e+189,62,2.18827389706660314090e+158,94,106,
	  GMP_RNDD, "-3.753ac0935b701ffffffffffffd@157");
  check2a(1.07966151149311101950e+46,88,1.13198076934147457400e+46,67,53,
	  GMP_RNDN, "3.dfbc152dd4368@38");
  check2a(3.36768223223409657622e+209,55,-9.61624007357265441884e+219,113,53,
	  GMP_RNDN, "-6.cf7217a451388@182");
  check2a(-6.47376909368979326475e+159,111,5.11127211034490340501e+159,99,62,
	  GMP_RNDD, "-1.8cf3aadf537c@132");
  check2a(-4.95229483271607845549e+220,110,-6.06992115033276044773e+213,109,55,
	  GMP_RNDN, "-2.3129f1f63b31b@183");
  check2a(-6.47376909368979326475e+159,74,5.11127211034490340501e+159,111,75,
	  GMP_RNDU, "-1.8cf3aadf537c@132");
  check2a(2.26531902208967707070e+168,99,-2.67795218510613988525e+168,67,94,
	 GMP_RNDU, "-1.bfd7ff2647098@139");
  check2a(-2.28886326552077479586e-188,67,3.41419438647157839320e-177,60,110,
	  GMP_RNDU, "3.75740b4fe8f17f90258907@-147");
  check2a(-2.66910493504493276454e-52,117,1.61188644159592323415e-52,61,68,
	  GMP_RNDZ, "-a.204acdd25d788@-44");
  check2a(2.90983392714730768886e+50,101,2.31299792168440591870e+50,74,105,
	 GMP_RNDZ, "1.655c53ff5719c8@42");
  check2a(2.72046257722708717791e+243,97,-1.62158447436486437113e+243,83,96,
	  GMP_RNDN, "a.4cc63e002d2e8@201");
  /* Checking double precision (53 bits) */
  check53(-8.22183238641455905806e-19, 7.42227178769761587878e-19, GMP_RNDD, 
	  -7.9956059871694317927e-20);
  check53(5.82106394662028628236e+234, -5.21514064202368477230e+89, GMP_RNDD,
	  5.8210639466202855763e234);
  check53(5.72931679569871602371e+122, -5.72886070363264321230e+122, GMP_RNDN,
	  4.5609206607281141508e118);
  check53(-5.09937369394650450820e+238, 2.70203299854862982387e+250, GMP_RNDD,
	  2.7020329985435301323e250);
  check53(-2.96695924472363684394e+27, 1.22842938251111500000e+16, GMP_RNDD,
	  -2.96695924471135255027e27);
  check53(1.74693641655743793422e-227, -7.71776956366861843469e-229, GMP_RNDN,
	  1.669758720920751867e-227);
  x = -7883040437021647.0; for (i=0; i<468; i++) x = x / 2.0;
  check53(-1.03432206392780011159e-125, 1.30127034799251347548e-133, GMP_RNDN,
	  x);
  check53(1.05824655795525779205e+71, -1.06022698059744327881e+71, GMP_RNDZ,
	  -1.9804226421854867632e68);
  check53(-5.84204911040921732219e+240, 7.26658169050749590763e+240, GMP_RNDD,
	  1.4245325800982785854e240);
  check53(1.00944884131046636376e+221, 2.33809162651471520268e+215, GMP_RNDN,
	  1.0094511794020929787e221);
  x = 7045852550057985.0; for (i=0; i<986; i++) x = x / 2.0;
  check53(4.29232078932667367325e-278, x, GMP_RNDU,
	  4.2933981418314132787e-278);
  check53(5.27584773801377058681e-80, 8.91207657803547196421e-91, GMP_RNDN,
	  5.2758477381028917269e-80);
  check53(2.99280481918991653800e+272, 5.34637717585790933424e+271, GMP_RNDN,
	  3.5274425367757071711e272);
  check53(4.67302514390488041733e-184, 2.18321376145645689945e-190, GMP_RNDN,
	  4.6730273271186420541e-184);
  check53(5.57294120336300389254e+71, 2.60596167942024924040e+65, GMP_RNDZ,
	  5.5729438093246831053e71);
  check53(6.6052588496951015469e24, 4938448004894539.0, GMP_RNDU,
	6.6052588546335505068e24);
  check53(1.23056185051606761523e-190, 1.64589756643433857138e-181, GMP_RNDU, 
	  1.6458975676649006598e-181);
  check53(2.93231171510175981584e-280, 3.26266919161341483877e-273, GMP_RNDU,
	  3.2626694848445867288e-273);
  check53(5.76707395945001907217e-58, 4.74752971449827687074e-51, GMP_RNDD,
	  4.747530291205672325e-51);
  check53(277363943109.0, 11.0, GMP_RNDN, 277363943120.0);
#ifdef HAVE_DENORMS
  /* test denormalized numbers too */
  x = 70360154255223.0; for (i=0; i<1073; i++) x = x / 2.0;
  check53(8.06294740693074521573e-310, x, GMP_RNDU,
          1.5015454417650041761e-309);
#endif
#ifdef HAVE_INFS
  /* the following check double overflow */
  check53(6.27557402141211962228e+307, 1.32141396570101687757e+308,
     GMP_RNDZ, DBL_POS_INF);
  check53(DBL_POS_INF, 6.95250701071929654575e-310, GMP_RNDU, DBL_POS_INF);
  check53(DBL_NEG_INF, 6.95250701071929654575e-310, GMP_RNDU, DBL_NEG_INF);
  check53(6.95250701071929654575e-310, DBL_POS_INF, GMP_RNDU, DBL_POS_INF);
  check53(6.95250701071929654575e-310, DBL_NEG_INF, GMP_RNDU, DBL_NEG_INF);
  check53nan (DBL_POS_INF, DBL_NEG_INF, GMP_RNDN);
#endif
  check53(1.44791789689198883921e-140, -1.90982880222349071284e-121,
	  GMP_RNDN, -1.90982880222349071e-121);


  /* tests for particular cases (Vincent Lefevre, 22 Aug 2001) */
  check53(9007199254740992.0, 1.0, GMP_RNDN, 9007199254740992.0);
  check53(9007199254740994.0, 1.0, GMP_RNDN, 9007199254740996.0);
  check53(9007199254740992.0, -1.0, GMP_RNDN, 9007199254740991.0);
  check53(9007199254740994.0, -1.0, GMP_RNDN, 9007199254740992.0);
  check53(9007199254740996.0, -1.0, GMP_RNDN, 9007199254740996.0);
  
  tests_end_mpfr ();
  return 0;
}
