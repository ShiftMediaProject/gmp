/* mpfr_zeta -- Riemann Zeta function at a floating-point number

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
#include <math.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "mpfr.h"

int
#if __STDC__
mpfr_zeta(mpfr_ptr result, mpfr_srcptr op, unsigned char rnd_mode) 
#else
mpfr_zeta(result, op, rnd_mode)
     mpfr_ptr result; 
     mpfr_srcptr op; 
     unsigned char rnd_mode; 
#endif
{
	mpfr_t s,s2,x,y,u,b,v,nn,z,z2; 
	int i,n,succes; 

	/* first version */
	if (mpfr_get_d(op) != 2.0 || rnd_mode != GMP_RNDN 
	|| PREC(result) != 53) {
	   fprintf(stderr, "not yet implemented\n"); exit(1);
	}

mpfr_set_default_prec(67);
mpfr_init(x);
mpfr_init(y);
mpfr_init(s);
mpfr_init(s2);
mpfr_init(u);
mpfr_init(b);
mpfr_init(v);
mpfr_init(nn);
mpfr_init(z);
mpfr_init(z2);
mpfr_set_ui(u,1,GMP_RNDN);
mpfr_set_ui(s,0,GMP_RNDN);
/*s=Somme des 1/i^2 (i=100...2)*/
n=100;
for (i=n; i>1; i--)
{
mpfr_set_ui(x,i*i,GMP_RNDN);
mpfr_div(y,u,x,GMP_RNDN);
mpfr_add(s,s,y,GMP_RNDN);
};
/*mpfr_print_raw(s);printf("\n");
t=mpfr_out_str(stdout,10,0,s,GMP_RNDN);printf("\n");*/
/*Application d'Euler-Maclaurin, jusqu'au terme 1/n^7 - n=100)*/
mpfr_set_ui(nn,n,GMP_RNDN);
mpfr_div(z,u,nn,GMP_RNDN);
mpfr_set(s2,z,GMP_RNDN);
mpfr_mul(z2,z,z,GMP_RNDN);
mpfr_div_2exp(v,z2,1,GMP_RNDN);
mpfr_sub(s2,s2,v,GMP_RNDN);
mpfr_set_ui(b,6,GMP_RNDN);
mpfr_mul(z,z,z2,GMP_RNDN);
mpfr_div(v,z,b,GMP_RNDN);
mpfr_add(s2,s2,v,GMP_RNDN);
mpfr_set_si(b,-30,GMP_RNDN);
mpfr_mul(z,z,z2,GMP_RNDN);
mpfr_div(v,z,b,GMP_RNDN);
mpfr_add(s2,s2,v,GMP_RNDN);
mpfr_set_si(b,42,GMP_RNDN);
mpfr_mul(z,z,z2,GMP_RNDN);
mpfr_div(v,z,b,GMP_RNDN);
mpfr_add(s2,s2,v,GMP_RNDN);
/*mpfr_print_raw(s2);printf("\n");
t=mpfr_out_str(stdout,10,0,s2,GMP_RNDN);printf("\n");*/
mpfr_add(s,s,s2,GMP_RNDN);
/*mpfr_print_raw(s);printf("\n");
t=mpfr_out_str(stdout,10,0,s,GMP_RNDN);printf("\n");*/
mpfr_add(s,s,u,GMP_RNDN);
/*mpfr_print_raw(s);printf("\n");
t=mpfr_out_str(stdout,10,0,s,GMP_RNDN);printf("\n");*/
/*Peut-on arrondir ? La reponse est oui*/
succes=mpfr_can_round(s,57,GMP_RNDN,GMP_RNDN,53);
if (succes) mpfr_set(result,s,GMP_RNDN);
else {
  fprintf(stderr, "can't round in mpfr_zeta\n"); exit(1);
}

mpfr_clear(x);
mpfr_clear(y);
mpfr_clear(s);
mpfr_clear(s2);
mpfr_clear(u);
mpfr_clear(b);
mpfr_clear(v);
mpfr_clear(nn);
mpfr_clear(z);
mpfr_clear(z2);
return 1; /* result is inexact */
}
