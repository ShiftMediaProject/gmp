/* Test file for mpfr_zeta.

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

/* #define DEBUG */

int main()
{mpfr_t p,result,res_p; int succes; size_t t;
mpfr_init2(result, 53);
mpfr_init2(res_p, 53);
mpfr_init2(p, 53); mpfr_set_d(p, 2.0, GMP_RNDN);

mpfr_zeta(result,p,GMP_RNDN);
/*printf("%d\n",succes);*/
#ifdef DEBUG
printf("Valeur de zeta(2) avec prec=53 et arrondi au plus pres:\n");
mpfr_print_raw(result);printf("\n");
t=mpfr_out_str(stdout,10,0,result,GMP_RNDN);printf("\n");
#endif
 if (mpfr_get_d(result) != 1.64493406684822640607e+00) {
   fprintf(stderr, "mpfr_zeta fails for s=2 and rnd_mode=GMP_RNDN\n");
   exit(1);
 }

/*Test de comparaison avec pi^2/6*/
mpfr_set_prec(p, 67);
mpfr_pi(p,GMP_RNDN);
mpfr_mul(p,p,p,GMP_RNDN);
mpfr_set_ui(res_p,6,GMP_RNDN);
mpfr_div(p,p,res_p,GMP_RNDN);
/*mpfr_print_raw(p);printf("\n");
t=mpfr_out_str(stdout,10,0,p,GMP_RNDN);printf("\n");*/
succes=mpfr_can_round(p,63,GMP_RNDN,GMP_RNDN,53);
mpfr_set(res_p,p,GMP_RNDN);
/*printf("%d\n",succes);*/
#ifdef DEBUG
printf("Valeur de pi^2/6 avec prec=53 et arrondi au plus pres:\n");
mpfr_print_raw(res_p);printf("\n");
t=mpfr_out_str(stdout,10,0,res_p,GMP_RNDN);printf("\n");
#endif

mpfr_clear(p);
mpfr_clear(result);
mpfr_clear(res_p);
exit (0);
}
