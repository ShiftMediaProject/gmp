/* Miscellaneous support for test programs.

Copyright 2001, 2002, 2003 Free Software Foundation, Inc.

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

#if HAVE_CONFIG_H
#include "config.h"     /* for a build within gmp */
#endif

#include <stdio.h>
#include <float.h>

#if TIME_WITH_SYS_TIME
# include <sys/time.h>  /* for struct timeval */
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "mpfr-test.h"

#if HAVE_SYS_FPU_H
#include <sys/fpu.h>  /* for mips fpc_csr */
#endif


void tests_rand_start _PROTO ((void));
void tests_rand_end   _PROTO ((void));

void
tests_start_mpfr (void)
{
  /* don't buffer, so output is not lost if a test causes a segv etc */
  setbuf (stdout, NULL);
  setbuf (stderr, NULL);

  tests_memory_start ();
  tests_rand_start ();
}

void
tests_end_mpfr (void)
{
  tests_rand_end ();
  if (__gmpfr_const_pi_prec != 0)
    {
      mpfr_clear (__mpfr_const_pi);
      __gmpfr_const_pi_prec = 0;
    }

  if (__gmpfr_const_log2_prec != 0)
    {
      mpfr_clear (__mpfr_const_log2);
      __gmpfr_const_log2_prec = 0;
    }

  tests_memory_end ();
}

void
tests_rand_start (void)
{
  gmp_randstate_ptr  rands;
  char           *perform_seed;
  unsigned long  seed;

  if (__gmp_rands_initialized)
    {
      printf ("Please let tests_start() initialize the global __gmp_rands.\n");
      printf ("ie. ensure that function is called before the first use of RANDS.\n");
      abort ();
    }
  rands = RANDS;

  perform_seed = getenv ("GMP_CHECK_RANDOMIZE");
  if (perform_seed != NULL)
    {
      seed = atoi (perform_seed);
      if (! (seed == 0 || seed == 1))
        {
          printf ("Re-seeding with GMP_CHECK_RANDOMIZE=%lu\n", seed);
          gmp_randseed_ui (rands, seed);
        }
      else
        {
#if HAVE_GETTIMEOFDAY
          struct timeval  tv;
          gettimeofday (&tv, NULL);
          seed = tv.tv_sec + tv.tv_usec;
#else
          time_t  tv;
          time (&tv);
          seed = tv;
#endif
          gmp_randseed_ui (rands, seed);
          printf ("Seed GMP_CHECK_RANDOMIZE=%lu (include this in bug reports)\n", seed);
        }
    }
}

void
tests_rand_end (void)
{
  RANDS_CLEAR ();
  if (__gmp_rands_initialized)
    {
      gmp_randclear (__gmp_rands);
      __gmp_rands_initialized = 0;
    }
}

/* initialization function for tests using the hardware floats */
void
mpfr_test_init ()
{
  double c, d, eps;
#ifdef __mips
  /* to get denormalized numbers on IRIX64 */
  union fpc_csr exp;

  exp.fc_word = get_fpc_csr();
  exp.fc_struct.flush = 0;
  set_fpc_csr(exp.fc_word);
#endif
#ifdef HAVE_DENORMS
  d = DBL_MIN;
  if (2.0 * (d / 2.0) != d)
    {
      fprintf (stderr, "Warning: no denormalized numbers\n");
      exit (1);
    }
#endif

  tests_machine_prec_double ();

  /* generate DBL_EPSILON with a loop to avoid that the compiler
     optimizes the code below in non-IEEE 754 mode, deciding that
     c = d is always false. */
  for (eps = 1.0; eps != DBL_EPSILON; eps /= 2.0);
  c = 1.0 + eps;
  d = eps * (1.0 - eps) / 2.0;
  d += c;
  if (c != d)
    {
      fprintf (stderr, "Warning: IEEE 754 standard not fully supported\n");
      fprintf (stderr, "         (maybe extended precision not disabled)\n");
      fprintf (stderr, "         Some tests may fail\n");
    }
  
}


/* Set the machine floating point precision, to double or long double.

   On i386 this controls the mantissa precision on the x87 stack, but the
   exponent range is only enforced when storing to memory.

   For reference, on most i386 systems the default is 64-bit "long double"
   precision, but on FreeBSD 3.x it's 53-bit "double".  */

void
tests_machine_prec_double (void)
{
#if MPFR_HAVE_TESTS_x86
  x86_fldcw ((x86_fstcw () & ~0x300) | 0x200);
#endif
}

void
tests_machine_prec_long_double (void)
{
#if MPFR_HAVE_TESTS_x86
  x86_fldcw (x86_fstcw () | 0x300);
#endif
}


/* generate a random limb */
mp_limb_t
randlimb (void)
{
  mp_limb_t limb;

  _gmp_rand (&limb, RANDS, GMP_NUMB_BITS);
  return limb;
}

/* returns ulp(x) for x a 'normal' double-precision number */
double
Ulp (double x)
{
   double y, eps;

   if (x < 0) x = -x;

   y = x * 2.220446049250313080847263336181640625e-16 ; /* x / 2^52 */

   /* as ulp(x) <= y = x/2^52 < 2*ulp(x),
   we have x + ulp(x) <= x + y <= x + 2*ulp(x),
   therefore o(x + y) = x + ulp(x) or x + 2*ulp(x) */

   eps =  x + y;
   eps = eps - x; /* ulp(x) or 2*ulp(x) */

   return (eps > y) ? 0.5 * eps : eps;
}

/* returns the number of ulp's between a and b,
   where a and b can be any floating-point number, except NaN
 */
int
ulp (double a, double b)
{
  double twoa;

  if (a == b) return 0; /* also deals with a=b=inf or -inf */

  twoa = a + a;
  if (twoa == a) /* a is +/-0.0 or +/-Inf */
    return ((b < a) ? INT_MAX : -INT_MAX);

  return (a - b) / Ulp (a);
}

/* return double m*2^e */
double
dbl (double m, int e)
{
  if (e >=0 )
    while (e-- > 0)
      m *= 2.0;
  else
    while (e++ < 0)
      m /= 2.0;
  return m;
}

int
Isnan (double d)
{
  return (d) != (d);
}
