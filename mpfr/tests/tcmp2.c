/* Test file for mpfr_cmp2.

Copyright (C) 1999 PolKA project, Inria Lorraine and Loria

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "gmp.h"
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#ifdef IRIX64
#include <sys/fpu.h>
#endif

extern int isnan();

void tcmp2(x, y, i) double x, y; int i;
{
  mpfr_t xx,yy; int j;

  if (i==-1) {
    if (x==y) i=53;
    else i = (int) floor(log(x)/log(2.0)) - (int) floor(log(x-y)/log(2.0));
  }
  mpfr_init2(xx, 53); mpfr_init2(yy, 53);
  mpfr_set_d(xx, x, 0);
  mpfr_set_d(yy, y, 0);
  if ((j=mpfr_cmp2(xx, yy)) != i) {
    printf("Error in mpfr_cmp2: x=%1.20e y=%1.20e mpfr_cmp2(x,y)=%d instead of %d\n",x,y,j,i); 
    exit(1);
  }
  mpfr_set_prec(xx, 127); mpfr_set_prec(yy, 127);
  mpfr_set_str_raw(xx, "0.1011010000110111111000000101011110110001000101101011011110010010011110010000101101000010011001100110010000000010110000101000101E6");
  mpfr_set_str_raw(yy, "0.1011010000110111111000000101011011111100011101000011001111000010100010100110110100110010011001100110010000110010010110000010110E6");
  if ((j=mpfr_cmp2(xx, yy)) != 32) {
    printf("Error in mpfr_cmp2:\n");
    printf("x="); mpfr_print_raw(xx); putchar('\n');
    printf("y="); mpfr_print_raw(yy); putchar('\n');
    printf("got %d, expected 32\n", j);
    exit(1);
  }
  mpfr_clear(xx); mpfr_clear(yy);
}

int main()
{
  int i,j; double x=1.0, y, z;
#ifdef IRIX64
    /* to get denormalized numbers on IRIX64 */
    union fpc_csr exp;
    exp.fc_word = get_fpc_csr();
    exp.fc_struct.flush = 0;
    set_fpc_csr(exp.fc_word);
#endif

  tcmp2(1.06022698059744327881e+71, 1.05824655795525779205e+71, -1);
  tcmp2(1.0, 1.0, 53);
  for (i=0;i<54;i++) {
    tcmp2(1.0, 1.0-x, i);
    x /= 2.0;
  }
  for (j=0;j<1000000;j++) {
    x = drand48();
    y = drand48();
    if (x<y) { z=x; x=y; y=z; }
    if (y != 0.0 && y != -0.0) tcmp2(x, y, -1);
  }
  exit (0);
}

