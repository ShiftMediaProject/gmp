/* Test file for mpfr_add_ui

Copyright (C) 2000, 2001 Free Software Foundation.

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

/* #define DEBUG */
/* #define VERBOSE */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#ifdef __mips
#include <sys/fpu.h>
#endif

void check3 _PROTO((double, unsigned long, unsigned int, double));
void special _PROTO((void));

#define ABS(x) (((x)>0) ? (x) : (-x))

#define check(x,y,r) check3(x,y,r,0.0)

/* checks that x+y gives the same results in double
   and with mpfr with 53 bits of precision */
void
check3 (double x, unsigned long y, unsigned int rnd_mode, double z1)
{
  double z2; mpfr_t xx,zz;

  mpfr_init2(xx, 53);
  mpfr_init2(zz, 53);
  mpfr_set_d(xx, x, rnd_mode);
  mpfr_add_ui(zz, xx, y, rnd_mode);
#ifdef TEST
  mpfr_set_machine_rnd_mode(rnd_mode);
#endif
  if (z1==0.0) z1 = x+y;
  z2 = mpfr_get_d(zz);
  if (z1!=z2 && !(isnan(z1) && isnan(z2))) {
    printf("expected sum is %1.20e, got %1.20e\n",z1,z2);
    printf("mpfr_add_ui failed for x=%1.20e y=%lu with rnd_mode=%s\n",
	   x, y, mpfr_print_rnd_mode(rnd_mode));
    exit(1);
  }
  mpfr_clear(xx); mpfr_clear(zz);
}

void special (void)
{
  mpfr_t x, y;

  mpfr_init2 (x, 63);
  mpfr_init2 (y, 63);
  mpfr_set_str_raw (x, "0.110100000000000001110001110010111111000000000101100011100100011");
  mpfr_add_ui (y, x, 1, GMP_RNDD);
  mpfr_clear (x);
  mpfr_clear (y);
}

int main(argc,argv) int argc; char *argv[];
{
#ifdef TEST
  double x; unsigned long y, N; int i,rnd_mode,rnd;
#ifdef __mips
    /* to get denormalized numbers on IRIX64 */
    union fpc_csr exp;
    exp.fc_word = get_fpc_csr();
    exp.fc_struct.flush = 0;
    set_fpc_csr(exp.fc_word);
#endif

  srand(getpid());
  N = (argc<2) ? 1000000 : atoi(argv[1]);
  rnd_mode = (argc<3) ? -1 : atoi(argv[2]);
  for (i=0;i<1000000;i++) {
    x = drand48();
    y = lrand48();
    if (ABS(x)>2.2e-307 && x+y<1.7e+308 && x+y>-1.7e308) {
      /* avoid denormalized numbers and overflows */
      rnd = (rnd_mode==-1) ? lrand48()%4 : rnd_mode;
      check(x, y, rnd);
    }
  } 
#endif
  special ();
  check3(-1.716113812768534e-140, 1271212614, GMP_RNDZ, 1.27121261399999976e9);
  check3(1.22191250737771397120e+20, 948002822, GMP_RNDN, 
	 122191250738719408128.0);
  check3(-6.72658901114033715233e-165, 2000878121, GMP_RNDZ,
	 2.0008781209999997615e9);
  check3(-2.0769715792901673e-5, 880524, GMP_RNDN, 8.8052399997923023e5);
  check3(1/0., 2394875, GMP_RNDN, 1/0.); 
  check3(-1/0., 2394875, GMP_RNDN, -1/0.); 
  check3(0./0., 2394875, GMP_RNDN, 0./0.); 
  return 0;
}

