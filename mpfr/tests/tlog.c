/* Test file for mpfr_log.

Copyright 1999, 2001, 2002 Free Software Foundation, Inc.

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
#include "mpfr-test.h"

double drand_log _PROTO((void)); 
int check1 _PROTO((double, mp_rnd_t, double, int, int)); 
void check3 _PROTO((double, unsigned long, mp_rnd_t)); 
void check4 _PROTO((int)); 
void slave _PROTO((int, int)); 
void check_worst_cases _PROTO((void)); 
void special _PROTO((void));

#if (BITS_PER_LONGINT == 32)
#define INT32 long int
#else
#define INT32 int
#endif


double
drand_log (void)
{
  double d; INT32 *i;

  i = (INT32*) &d;
  do {
    i[0] = LONG_RAND();
    i[1] = LONG_RAND();
   } while ((d<1e-153) || (d>1e153));    /* to avoid underflow or overflow
					 in double calculus in sqrt(u*v) */
  return d;
}

#define check2(a,rnd,res) check1(a,rnd,res,1,0)
#define check(a,r) check2(a,r,0.0)

int
check1 (double a, mp_rnd_t rnd_mode, double res1, int ck, int max_ulp)
{
  mpfr_t ta, tres;
  double res2;
  int diff=0;
  /* ck=1 iff res1 is certified correct */

#ifdef MPFR_HAVE_FESETROUND
  mpfr_set_machine_rnd_mode(rnd_mode);
#endif
  if (ck==0 && res1==0.0) res1=log(a); 
  mpfr_init2(ta, 53);
  mpfr_init2(tres, 53);
  mpfr_set_d(ta, a, GMP_RNDN);
  mpfr_log(tres, ta, rnd_mode);
  res2=mpfr_get_d1 (tres);
  mpfr_clear(ta); mpfr_clear(tres); 

  if (res1!=res2 && (!isnan(res1) || !isnan(res2))) {
      diff = ulp(res1,res2);
      if (ck) { 
	printf("mpfr_log failed for    a=%1.20e, rnd_mode=%s\n", a,
	       mpfr_print_rnd_mode(rnd_mode));
	printf("correct result is        %1.20e\n mpfr_log gives          %1.20e (%d ulp)\n",res1,res2,ulp(res1,res2));
	exit(1);
      }
      else if (diff>max_ulp) {
	printf("mpfr_log differs from libm.a for a=%1.20e, rnd_mode=%s\n", a,
	       mpfr_print_rnd_mode(rnd_mode));
	printf(" double calculus gives %1.20e\n mpfr_log        gives %1.20e (%d ulp)\n",res1,res2,ulp(res1,res2));
      }
  }
  if (!isnan(res1) || !isnan(res2))
    return diff;
  else
    return 0;
}

void
check3 (double d, unsigned long prec, mp_rnd_t rnd)
{
  mpfr_t x, y;
  
  mpfr_init2(x, prec); mpfr_init2(y, prec);
  mpfr_set_d(x, d, rnd);
  mpfr_log(y, x, rnd);
  mpfr_out_str(stdout, 10, 0, y, rnd); putchar('\n');
  mpfr_print_binary(y); putchar('\n');
  mpfr_clear(x); mpfr_clear(y);
}

void
check4 (int N)
{
  int i, max=0, sum=0, cur;
  double d;
  mp_rnd_t rnd;

  for(i=0;i<N;i++) {
    d = drand_log ();
    rnd = LONG_RAND() % 4;
    cur=check1 (d, rnd, 0.0, 0, max);
    if (cur<0)
      cur = -cur;
    if (cur > max)
      max=cur;
    sum+=cur;
  }
  d=(double)sum / (double)N;
  fprintf(stderr, "max error : %i \t mean error : %f   (in ulps)\n",max,d);
}

void
slave (int N, int p)
{
  int i;
  double d;
  mpfr_t ta, tres;

  mpfr_init2(ta, 53);
  mpfr_init2(tres, p);
  for(i=0;i<N;i++) {
    d = drand_log(); 
    mpfr_set_d (ta, d, GMP_RNDN);
    mpfr_log (tres, ta, LONG_RAND() % 4 );
  }
  mpfr_clear(ta); mpfr_clear(tres); 
  printf("fin\n");
}

/* examples from Jean-Michel Muller and Vincent Lefevre 
   Cf http://www.ens-lyon.fr/~jmmuller/Intro-to-TMD.htm
*/

void
check_worst_cases (void)
{ 
  check2(1.00089971802309629645, GMP_RNDD, 8.99313519443722736088e-04); 
  check2(1.00089971802309629645, GMP_RNDN, 8.99313519443722844508e-04);
  check2(1.00089971802309629645, GMP_RNDU, 8.99313519443722844508e-04); 

  check2(1.01979300812244555452, GMP_RNDD, 1.95996734891603630047e-02); 
  check2(1.01979300812244555452, GMP_RNDN, 1.95996734891603664741e-02);
  check2(1.01979300812244555452, GMP_RNDU, 1.95996734891603664741e-02);

  check2(1.02900871924604464525, GMP_RNDD, 2.85959303301472726744e-02);
  check2(1.02900871924604464525, GMP_RNDN, 2.85959303301472761438e-02);
  check2(1.02900871924604464525, GMP_RNDU, 2.85959303301472761438e-02);

  check2(1.27832870030418943585, GMP_RNDD, 2.45553521871417795852e-01);
  check2(1.27832870030418943585, GMP_RNDN, 2.45553521871417823608e-01);
  check2(1.27832870030418943585, GMP_RNDU, 2.45553521871417823608e-01);

  check2(1.31706530746788241792, GMP_RNDD, 2.75406009586277422674e-01);
  check2(1.31706530746788241792, GMP_RNDN, 2.75406009586277478185e-01);
  check2(1.31706530746788241792, GMP_RNDU, 2.75406009586277478185e-01);

  check2(1.47116981099449883885, GMP_RNDD, 3.86057874110010412760e-01);
  check2(1.47116981099449883885, GMP_RNDN, 3.86057874110010412760e-01);
  check2(1.47116981099449883885, GMP_RNDU, 3.86057874110010468272e-01);

  check2(1.58405446812987782401, GMP_RNDD, 4.59987679246663727639e-01);
  check2(1.58405446812987782401, GMP_RNDN, 4.59987679246663783150e-01);
  check2(1.58405446812987782401, GMP_RNDU, 4.59987679246663783150e-01);

  check2(1.67192331263391547047, GMP_RNDD, 5.13974647961076613889e-01);
  check2(1.67192331263391547047, GMP_RNDN, 5.13974647961076724911e-01);
  check2(1.67192331263391547047, GMP_RNDU, 5.13974647961076724911e-01);

  check2(1.71101198068990645318, GMP_RNDD, 5.37084997042120315669e-01);
  check2(1.71101198068990645318, GMP_RNDN, 5.37084997042120315669e-01);
  check2(1.71101198068990645318, GMP_RNDU, 5.37084997042120426691e-01);

  check2(1.72634853551388700588, GMP_RNDD, 5.46008504786553605648e-01);
  check2(1.72634853551388700588, GMP_RNDN, 5.46008504786553716670e-01);
  check2(1.72634853551388700588, GMP_RNDU, 5.46008504786553716670e-01);

  check2(2.00028876593004323325, GMP_RNDD, 6.93291553102749702475e-01);
  check2(2.00028876593004323325, GMP_RNDN, 6.93291553102749813497e-01);
  check2(2.00028876593004323325, GMP_RNDU, 6.93291553102749813497e-01);

  check2(6.27593230200363105808, GMP_RNDD, 1.83672204800630312072);
  check2(6.27593230200363105808, GMP_RNDN, 1.83672204800630334276);
  check2(6.27593230200363105808, GMP_RNDU, 1.83672204800630334276);

  check2(7.47216682321367997588, GMP_RNDD, 2.01118502712453661729);
  check2(7.47216682321367997588, GMP_RNDN, 2.01118502712453706138);
  check2(7.47216682321367997588, GMP_RNDU, 2.01118502712453706138);

  check2(9.34589857718275318632, GMP_RNDD, 2.23493759221664944903);
  check2(9.34589857718275318632, GMP_RNDN, 2.23493759221664989312);
  check2(9.34589857718275318632, GMP_RNDU, 2.23493759221664989312);

  check2(10.6856587560831854944, GMP_RNDD, 2.36890253928838445674);
  check2(10.6856587560831854944, GMP_RNDN, 2.36890253928838445674);
  check2(10.6856587560831854944, GMP_RNDU, 2.36890253928838490083);

  check2(12.4646345033981766903, GMP_RNDD, 2.52289539471636015122);
  check2(12.4646345033981766903, GMP_RNDN, 2.52289539471636015122);
  check2(12.4646345033981766903, GMP_RNDU, 2.52289539471636059531);

  check2(17.0953275851761752335, GMP_RNDD, 2.83880518553861849185);
  check2(17.0953275851761752335, GMP_RNDN, 2.83880518553861893594); 
  check2(17.0953275851761752335, GMP_RNDU, 2.83880518553861893594);

  check2(19.8509496207496916043, GMP_RNDD, 2.98825184582516722998);
  check2(19.8509496207496916043, GMP_RNDN, 2.98825184582516722998);
  check2(19.8509496207496916043, GMP_RNDU, 2.98825184582516767406);

  check2(23.9512076062771335216, GMP_RNDD, 3.17601874455977206679);
  check2(23.9512076062771335216, GMP_RNDN, 3.17601874455977206679);
  check2(23.9512076062771335216, GMP_RNDU, 3.17601874455977251088);

  check2(428.315247165198229595, GMP_RNDD, 6.05985948325268264369);
  check2(428.315247165198229595, GMP_RNDN, 6.05985948325268353187);
  check2(428.315247165198229595, GMP_RNDU, 6.05985948325268353187); 
}

void
special (void)
{
  mpfr_t x, y;

  mpfr_init2 (x, 53);
  mpfr_init2 (y, 53);
  mpfr_set_ui (x, 3, GMP_RNDD);
  mpfr_log (y, x, GMP_RNDD);
  if (mpfr_get_d1 (y) != 1.09861228866810956) {
    fprintf (stderr, "Error in mpfr_log(3) for GMP_RNDD\n");
    exit (1);
  }

  /* check large precision */
  mpfr_set_prec (x, 3322);
  mpfr_set_prec (y, 3322);
  mpfr_set_d (x, 3.0, GMP_RNDN);
  mpfr_sqrt (x, x, GMP_RNDN);
  mpfr_log (y, x, GMP_RNDN);

  mpfr_clear (x);
  mpfr_clear (y);
}

#define TEST_FUNCTION mpfr_log
#include "tgeneric.c"

int
main (int argc, char *argv[])
{
  int N = 0;
  double d;

  SEED_RAND (time(NULL));
  if (argc==4) {   /* tlog x prec rnd */
    check3(atof(argv[1]), atoi(argv[2]), atoi(argv[3]));
    return 0;
  }

  if (argc==3) {   /* tlog N p : N calculus with precision p*/
  printf("Doing %d random tests in %d precision\n",atoi(argv[1]),atoi(argv[2]));
    slave(atoi(argv[1]),atoi(argv[2]));
     return 0;
  }

  if (argc==2) { /* tlog N: N tests with random double's */
    N=atoi(argv[1]);
    printf("Doing %d random tests in double precision\n", N);
    check4(N);
  }
  else {
    special ();
    check_worst_cases();

  check2(1.01979300812244555452, GMP_RNDN, 1.95996734891603664741e-02);
  check2(10.0,GMP_RNDU,2.30258509299404590110e+00);
  check2(6.0,GMP_RNDU,1.79175946922805517936);  
  check2(1.0,GMP_RNDZ,0.0);  
  check2(62.0,GMP_RNDU,4.12713438504509166905);
  check2(0.5,GMP_RNDZ,-6.93147180559945286226e-01);   
  check2(3.0,GMP_RNDZ,1.09861228866810956006e+00); 
  check2(234375765.0,GMP_RNDU,1.92724362186836231104e+01);
  check2(8.0,GMP_RNDZ,2.07944154167983574765e+00);  
  check2(44.0,GMP_RNDU,3.78418963391826146392e+00); 
  check2(1.01979300812244555452, GMP_RNDN, 1.95996734891603664741e-02);
  /* bugs found by Vincent Lefe`vre */
  d = -4723773766428415.0 / 1180591620717411303424.0;
  check2(0.99999599881598921769, GMP_RNDN, d);
  check2(9.99995576063808955247e-01, GMP_RNDZ, -4.42394597667932383816e-06);
  check2(9.99993687357856209097e-01, GMP_RNDN, -6.31266206860017342601e-06);
  check2(9.99995223520736886691e-01, GMP_RNDN, -4.77649067052670982220e-06);
  check2(9.99993025794720935551e-01, GMP_RNDN, -6.97422959894716163837e-06);
  check2(9.99987549017837484833e-01, GMP_RNDN, -1.24510596766369924330e-05);
  check2(9.99985901426543311032e-01, GMP_RNDN, -1.40986728425098585229e-05);
  d = -8232353813100321.0 / 590295810358705651712.0;
  check2(9.99986053947420794330e-01, GMP_RNDN, d);
  check2(9.99971938247442126979e-01, GMP_RNDN, -2.80621462962173414790e-05);
  /* other bugs found by Vincent Lefe`vre */
  check2(1.18615436389927785905e+77, GMP_RNDN, 1.77469768607706015473e+02);
  check2(9.48868723578399476187e+77, GMP_RNDZ, 1.79549152432275803903e+02);
  check2(2.31822210096938820854e+89, GMP_RNDN, 2.05770873832573869322e+02);
  /* further bugs found by Vincent Lefe`vre */
  check2(9.99999989485669482647e-01, GMP_RNDZ, -1.05143305726283042331e-08);
  check2(9.99999989237970177136e-01, GMP_RNDZ, -1.07620298807745377934e-08);
  check2(9.99999989239339082125e-01, GMP_RNDN, -1.07606609757704445430e-08);

  check2(7.3890560989306504,GMP_RNDU,2.0000000000000004); /* exp(2.0) */
  check2(7.3890560989306495,GMP_RNDU,2.0); /* exp(2.0) */
  check2(7.53428236571286402512e+34,GMP_RNDZ,8.03073567492226345621e+01);
  check2(6.18784121531737948160e+19,GMP_RNDZ,4.55717030391710693493e+01); 
  check2(1.02560267603047283735e+00,GMP_RNDD,2.52804164149448735987e-02);
  check2(7.53428236571286402512e+34,GMP_RNDZ,8.03073567492226345621e+01);
  d = 497773706319601.0 / 4398046511104.0;
  check2(1.42470900831881198052e+49, GMP_RNDZ, d);
  
  check2(1.08013816255293777466e+11,GMP_RNDN,2.54055249841782604392e+01);
  check2(6.72783635300509015581e-37,GMP_RNDU,-8.32893948416799503320e+01);
  check2(2.25904918906057891180e-52,GMP_RNDU,-1.18919480823735682406e+02);
  check2(1.48901209246462951085e+00,GMP_RNDD,3.98112874867437460668e-01);
  check2(1.70322470467612341327e-01,GMP_RNDN,-1.77006175364294615626);
  check2(1.94572026316065240791e+01,GMP_RNDD,2.96821731676437838842);
  check2(4.01419512207026418764e+04,GMP_RNDD,1.06001772315501128218e+01);
  check2(9.47077365236487591672e-04,GMP_RNDZ,-6.96212977303956748187e+00);
  check2(3.95906157687589643802e-109,GMP_RNDD,-2.49605768114704119399e+02);
  check2(2.73874914516503004113e-02,GMP_RNDD,-3.59766888618655977794e+00);
  check2(9.18989072589566467669e-17,GMP_RNDZ,-3.69258425351464083519e+01);
  check2(dbl(2830750724514701.0,131),GMP_RNDZ,dbl(1111664301085491.0,-43));
  check2(1.74827399630587801934e-23,GMP_RNDZ,-5.24008281254547156891e+01);
  check2(4.35302958401482307665e+22,GMP_RNDD,5.21277441046519527390e+01);
  check2(9.70791868689332915209e+00,GMP_RNDD,2.27294191194272210410e+00);
  check2(2.22183639799464011100e-01,GMP_RNDN,-1.50425103275253957413e+00);
  check2(2.27313466156682375540e+00,GMP_RNDD,8.21159787095675608448e-01);
  check2(6.58057413965851156767e-01,GMP_RNDZ,-4.18463096196088235600e-01);
  d = 7107588635148285.0 / 70368744177664.0;
  check2 (7.34302197248998461006e+43, GMP_RNDZ, d);
  check2(6.09969788341579732815e+00,GMP_RNDD,1.80823924264386204363e+00);
  }

  test_generic (2, 100, 40);

  return 0;
}
