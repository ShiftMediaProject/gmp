/* Test file for mpfr_add and mpfr_sub.

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

/* #define DEBUG */
/* #define VERBOSE */

#define N 100000

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#ifdef IRIX64
#include <sys/fpu.h>
#endif

extern int isnan();
extern int getpid();

#define ABS(x) (((x)>0) ? (x) : (-x))

/* checks that x+y gives the same results in double
   and with mpfr with 53 bits of precision */
void check(double x, double y, unsigned int rnd_mode, unsigned int px, 
unsigned int py, unsigned int pz, double res)
{
  double z1,z2; mpfr_t xx,yy,zz;

  mpfr_init2(xx, px);
  mpfr_init2(yy, py);
  mpfr_init2(zz, pz);
  mpfr_set_d(xx, x, rnd_mode);
  mpfr_set_d(yy, y, rnd_mode);
  mpfr_add(zz, xx, yy, rnd_mode);
  mpfr_set_machine_rnd_mode(rnd_mode);
  z1 = (res==0.0) ? x+y : res;
  z2 = mpfr_get_d(zz);
  if (px==53 && py==53 && pz==53) res=1.0;
  if (res!=0.0 && z1!=z2 && !(isnan(z1) && isnan(z2))) {
    printf("expected sum is %1.20e, got %1.20e\n",z1,z2);
    printf("mpfr_add failed for x=%1.20e y=%1.20e with rnd_mode=%u\n",x,y,rnd_mode);
    exit(1);
  }
  mpfr_clear(xx); mpfr_clear(yy); mpfr_clear(zz);
}

/* idem than check for mpfr_add(x, x, y) */
void check3(double x, double y, unsigned int rnd_mode)
{
  double z1,z2; mpfr_t xx,yy; int neg;

  neg = rand() % 2;
  mpfr_init2(xx, 53);
  mpfr_init2(yy, 53);
  mpfr_set_d(xx, x, rnd_mode);
  mpfr_set_d(yy, y, rnd_mode);
  if (neg) mpfr_sub(xx, xx, yy, rnd_mode);
  else mpfr_add(xx, xx, yy, rnd_mode);
  mpfr_set_machine_rnd_mode(rnd_mode);
  z1 = (neg) ? x-y : x+y;
  z2 = mpfr_get_d(xx);
  if (z1!=z2 && !(isnan(z1) && isnan(z2))) {
    printf("expected result is %1.20e, got %1.20e\n",z1,z2);
    printf("mpfr_%s(x,x,y) failed for x=%1.20e y=%1.20e with rnd_mode=%u\n",
	   (neg) ? "sub" : "add",x,y,rnd_mode);
    exit(1);
  }
  mpfr_clear(xx); mpfr_clear(yy);
}

/* idem than check for mpfr_add(x, y, x) */
void check4(double x, double y, unsigned int rnd_mode)
{
  double z1,z2; mpfr_t xx,yy; int neg;

  neg = rand() % 2;
  mpfr_init2(xx, 53);
  mpfr_init2(yy, 53);
  mpfr_set_d(xx, x, rnd_mode);
  mpfr_set_d(yy, y, rnd_mode);
  if (neg) mpfr_sub(xx, yy, xx, rnd_mode);
  else mpfr_add(xx, yy, xx, rnd_mode);
  mpfr_set_machine_rnd_mode(rnd_mode);
  z1 = (neg) ? y-x : x+y;
  z2 = mpfr_get_d(xx);
  if (z1!=z2 && !(isnan(z1) && isnan(z2))) {
    printf("expected result is %1.20e, got %1.20e\n",z1,z2);
    printf("mpfr_%s(x,y,x) failed for x=%1.20e y=%1.20e with rnd_mode=%u\n",
	   (neg) ? "sub" : "add",x,y,rnd_mode);
    exit(1);
  }
  mpfr_clear(xx); mpfr_clear(yy);
}

/* idem than check for mpfr_add(x, x, x) */
void check5(double x, unsigned int rnd_mode)
{
  double z1,z2; mpfr_t xx; int neg;

  mpfr_init2(xx, 53); neg = rand() % 2;
  mpfr_set_d(xx, x, rnd_mode);
  if (neg) mpfr_sub(xx, xx, xx, rnd_mode);
  else mpfr_add(xx, xx, xx, rnd_mode);
  mpfr_set_machine_rnd_mode(rnd_mode);
  z1 = (neg) ? x-x : x+x;
  z2 = mpfr_get_d(xx);
  if (z1!=z2 && !(isnan(z1) && isnan(z2))) {
    printf("expected result is %1.20e, got %1.20e\n",z1,z2);
    printf("mpfr_%s(x,x,x) failed for x=%1.20e with rnd_mode=%u\n",
	   (neg) ? "sub" : "add",x,rnd_mode);
    exit(1);
  }
  mpfr_clear(xx);
}

void check2(x,px,y,py,pz,rnd_mode) double x,y; int px,py,pz,rnd_mode;
{
  mpfr_t xx, yy, zz; double z,z2; int u;

#ifdef DEBUG
printf("x=%1.20e,%d y=%1.20e,%d pz=%d,rnd=%d\n",x,px,y,py,pz,rnd_mode);
#endif
  mpfr_init2(xx,px); mpfr_init2(yy,py); mpfr_init2(zz,pz);
  mpfr_set_d(xx, x, rnd_mode);
  mpfr_set_d(yy, y, rnd_mode);
  mpfr_add(zz, xx, yy, rnd_mode);
  mpfr_set_machine_rnd_mode(rnd_mode);
  z = x+y; z2=mpfr_get_d(zz); u=ulp(z,z2);
#ifdef DEBUG
  printf("x+y=%1.20e,%d (%d ulp) rnd_mode=%d\n",z2,pz,u,rnd_mode);
  mpfr_set_d(zz, z2, rnd_mode); 
  printf("i.e."); mpfr_print_raw(zz); putchar('\n');
#endif
    /* one ulp difference is possible due to composed rounding */
  if (px>=53 && py>=53 && pz>=53 && ABS(u)>1) { 
    printf("x=%1.20e,%d y=%1.20e,%d pz=%d,rnd=%d\n",x,px,y,py,pz,rnd_mode);
    printf("got %1.20e\n",z2);
    printf("result should be %1.20e (diff=%d ulp)\n",z,u);
    mpfr_set_d(zz, z, rnd_mode);
    printf("i.e."); mpfr_print_raw(zz); putchar('\n');
    exit(1); }
  mpfr_clear(xx); mpfr_clear(yy); mpfr_clear(zz);
}

void check64()
{
  mpfr_t x, t, u;

  mpfr_init(x); mpfr_init(t); mpfr_init(u);
  mpfr_set_prec(x, 53); mpfr_set_prec(t, 76); mpfr_set_prec(u, 76);
  mpfr_set_str_raw(x, "-0.10010010001001011011110000000000001010011011011110001E-32");
  mpfr_set_str_raw(t, "-0.1011000101110010000101111111011111010001110011110111100110101011110010011111");
  mpfr_sub(u, x, t, GMP_RNDU);
  mpfr_set_str_raw(t, "0.1011000101110010000101111111011100111111101010011011110110101011101000000100");
  if (mpfr_cmp(u,t)) {
    printf("expect "); mpfr_print_raw(t); putchar('\n');
    fprintf(stderr, "mpfr_add failed for precisions 53-76\n"); exit(1);
  }
  mpfr_set_prec(x, 53); mpfr_set_prec(t, 108); mpfr_set_prec(u, 108);
  mpfr_set_str_raw(x, "-0.10010010001001011011110000000000001010011011011110001E-32");
  mpfr_set_str_raw(t, "-0.101100010111001000010111111101111101000111001111011110011010101111001001111000111011001110011000000000111111");
  mpfr_sub(u, x, t, GMP_RNDU);
  mpfr_set_str_raw(t, "0.101100010111001000010111111101110011111110101001101111011010101110100000001011000010101110011000000000111111");
  if (mpfr_cmp(u,t)) {
    printf("expect "); mpfr_print_raw(t); putchar('\n');
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
  if ((MANT(u)[(PREC(u)-1)/mp_bits_per_limb] & 
      ((mp_limb_t)1<<(mp_bits_per_limb-1)))==0) {
    printf("Error in mpfr_sub: result is not msb-normalized\n"); exit(1);
  }
  mpfr_set_prec(x, 65); mpfr_set_prec(t, 65); mpfr_set_prec(u, 65);
  mpfr_set_str_raw(x, "0.10011010101000110101010000000011001001001110001011101011111011101E623");
  mpfr_set_str_raw(t, "0.10011010101000110101010000000011001001001110001011101011111011100E623");
  mpfr_sub(u, x, t, GMP_RNDU);
  if (mpfr_get_d(u) != 9.4349060620538533806e167) { /* 2^558 */
    printf("Error (1) in mpfr_sub\n"); exit(1);
  }
  mpfr_set_prec(x, 64); mpfr_set_prec(t, 64); mpfr_set_prec(u, 64);
  mpfr_set_str_raw(x, "0.1000011110101111011110111111000011101011101111101101101100000100E-220");
  mpfr_set_str_raw(t, "0.1000011110101111011110111111000011101011101111101101010011111101E-220");
  mpfr_add(u, x, t, GMP_RNDU);
  if ((MANT(u)[0] & 1) != 1) { 
    printf("error in mpfr_add with rnd_mode=GMP_RNDU\n");
    printf("b=  "); mpfr_print_raw(x); putchar('\n');
    printf("c=  "); mpfr_print_raw(t); putchar('\n');
    printf("b+c="); mpfr_print_raw(u); putchar('\n');
    exit(1);
  }
  mpfr_clear(x); mpfr_clear(t); mpfr_clear(u);
}

/* checks when source and destination are equal */
void check_same()
{
  mpfr_t x;

  mpfr_init(x); mpfr_set_d(x, 1.0, GMP_RNDZ);
  mpfr_add(x, x, x, GMP_RNDZ);
  if (mpfr_get_d(x) != 2.0) {
    printf("Error when all 3 operands are equal\n"); exit(1);
  }
  mpfr_clear(x);
}

int main(argc,argv) int argc; char *argv[];
{
  double x,y; int i,prec,rnd_mode,px,py,pz,rnd;
#ifdef IRIX64
    /* to get denormalized numbers on IRIX64 */
    union fpc_csr exp;
    exp.fc_word = get_fpc_csr();
    exp.fc_struct.flush = 0;
    set_fpc_csr(exp.fc_word);
#endif

  check64();
  check_same();
  check(6.14384195492641560499e-02, -6.14384195401037683237e-02,
	GMP_RNDU, 53, 53, 53, 0.0);
  check(1.16809465359248765399e+196, 7.92883212101990665259e+196,
	GMP_RNDU, 53, 53, 53, 0.0);
  check(3.14553393112021279444e-67, 3.14553401015952024126e-67,
	GMP_RNDU, 53, 53, 53, 0.0);
#ifdef VERBOSE
  printf("Checking random precisions\n");
#endif
  srand(getpid());
  check2(5.43885304644369509058e+185,53,-1.87427265794105342763e-57,53,53,0);
  check2(5.43885304644369509058e+185,53,-1.87427265794105342763e-57,53,53,1);
  check2(5.43885304644369509058e+185,53,-1.87427265794105342763e-57,53,53,2);
  check2(5.43885304644369509058e+185,53,-1.87427265794105342763e-57,53,53,3);
  check2(2.26531902208967707071e+168,99,-2.67795218510613988524e+168,67,94,2);
  check2(-1.21510626304662318398e+145,70,1.21367733647758957118e+145,65,61,3);
  check2(2.73028857032080744543e+155,83,-1.16446121423113355603e+163,59,125,1);
  check2(-4.38589520019641698848e+78,155,-1.09923643769309483415e+72,15,159,3);
  check2(-1.49963910666191123860e+265,76,-2.30915090591874527520e-191,8,75,1);
  check2(5.17945380930936917508e+112,119,1.11369077158813567738e+108,15,150,1);
  check2(-2.66910493504493276454e-52,117,1.61188644159592323415e-52,61,68,1);
  check2(-1.87427265794105342764e-57,175,1.76570844587489516446e+190,2,115,1);
  check2(6.85523243386777784171e+107,187,-2.78148588123699111146e+48,87,178,3);
  check2(-1.15706375390780417299e-135,94,-1.07455137477117851576e-129,66,111,2);
  check2(-1.15706375390780417299e-135,94,-1.07455137477117851576e-129,66,111,3);
  check2(-3.31624349995221499866e-22,107,-8.20150212714204839621e+156,79,99,3);
  check2(-1.15706375390780417299e-135,94,-1.07455137477117851576e-129,66,111,3);
  check2(-1.08007920352320089721e+150,63,1.77607317509426332389e+73,64,64,0);
  check2(4.49465557237618783128e+53,108,-2.45103927353799477871e+48,60,105,0);
  check2(3.25471707846623300604e-160,81,-7.93846654265839958715e-274,58,54,0);
  check2(-8.88471912490080158206e+253,79,-7.84488427404526918825e+124,95,53,3);
  check2(-2.18548638152863831959e-125,61,-1.22788940592412363564e+226,71,54,0);
  check2(-7.94156823309993162569e+77,74,-5.26820160805275124882e+80,99,101,3);
  check2(-3.85170653452493859064e+189,62,2.18827389706660314090e+158,94,106,3);
  check2(1.07966151149311101950e+46,88,1.13198076934147457400e+46,67,53,0);
  check2(3.36768223223409657622e+209,55,-9.61624007357265441884e+219,113,53,0);
  check2(-6.47376909368979326475e+159,111,5.11127211034490340501e+159,99,62,3);
  check2(-4.95229483271607845549e+220,110,-6.06992115033276044773e+213,109,55,0);
  check2(-6.47376909368979326475e+159,74,5.11127211034490340501e+159,111,75,2);
  check2(2.26531902208967707070e+168,99,-2.67795218510613988525e+168,67,94,2);
  check2(-2.28886326552077479586e-188,67,3.41419438647157839320e-177,60,110,2);
  check2(-2.66910493504493276454e-52,117,1.61188644159592323415e-52,61,68,1);
  check2(2.90983392714730768886e+50,101,2.31299792168440591870e+50,74,105,1);
  check2(2.72046257722708717791e+243,97,-1.62158447436486437113e+243,83,96,0);
  for (i=0;i<N;i++) {
#ifdef DEBUG
printf("\nTest i=%d\n",i);
#endif
    px = 53 + (rand() % 64); 
    py = 53 + (rand() % 64); 
    pz = 53 + (rand() % 64); 
    rnd_mode = rand() % 4;
    do { x = drand(); } while (isnan(x));
    do { y = drand(); } while (isnan(y));
    check2(x,px,y,py,pz,rnd_mode);
  }
#ifdef VERBOSE
  printf("Checking double precision (53 bits)\n");
#endif
  prec = (argc<2) ? 53 : atoi(argv[1]);
  rnd_mode = (argc<3) ? -1 : atoi(argv[2]);
  check(-8.22183238641455905806e-19, 7.42227178769761587878e-19,
	GMP_RNDD, 53, 53, 53, 0.0);
  check(5.82106394662028628236e+234, -5.21514064202368477230e+89,
	GMP_RNDD, 53, 53, 53, 0.0);
  check(5.72931679569871602371e+122, -5.72886070363264321230e+122,
	GMP_RNDN, 53, 53, 53, 0.0);
  check(-5.09937369394650450820e+238, 2.70203299854862982387e+250,
	GMP_RNDD, 53, 53, 53, 0.0);
  check(-2.96695924472363684394e+27, 1.22842938251111500000e+16,
	GMP_RNDD, 53, 53, 53, 0.0);
  check(1.74693641655743793422e-227, -7.71776956366861843469e-229,
	GMP_RNDN, 53, 53, 53, 0.0);
  check(-1.03432206392780011159e-125, 1.30127034799251347548e-133,
	GMP_RNDN, 53, 53, 53, 0.0);
  check(1.05824655795525779205e+71, -1.06022698059744327881e+71,
	GMP_RNDZ, 53, 53, 53, 0.0);
  check(-5.84204911040921732219e+240, 7.26658169050749590763e+240,
	GMP_RNDD, 53, 53, 53, 0.0);
  /* the following check double overflow */
  /*  check(6.27557402141211962228e+307, 1.32141396570101687757e+308,
      GMP_RNDZ, 53, 53, 53, 0.0); */
  check(1.00944884131046636376e+221, 2.33809162651471520268e+215,
	GMP_RNDN, 53, 53, 53, 0.0);
  check(4.29232078932667367325e-278, 1.07735250473897938332e-281,
	GMP_RNDU, 53, 53, 53, 0.0);
  check(5.27584773801377058681e-80, 8.91207657803547196421e-91,
	GMP_RNDN, 53, 53, 53, 0.0);
  check(2.99280481918991653800e+272, 5.34637717585790933424e+271,
	GMP_RNDN, 53, 53, 53, 0.0);
  check(4.67302514390488041733e-184, 2.18321376145645689945e-190,
	GMP_RNDN, 53, 53, 53, 0.0);
  check(5.57294120336300389254e+71, 2.60596167942024924040e+65,
	GMP_RNDZ, 53, 53, 53, 0.0);
  x=6151626677899716.0; for (i=0;i<30;i++) x = 2.0*x;
  check(x, 4938448004894539.0, GMP_RNDU, 53, 53, 53, 0.0);
  check(1.23056185051606761523e-190, 1.64589756643433857138e-181,
	GMP_RNDU, 53, 53, 53, 0.0);
  check(2.93231171510175981584e-280, 3.26266919161341483877e-273,
	GMP_RNDU, 53, 53, 53, 0.0);
  check(5.76707395945001907217e-58, 4.74752971449827687074e-51,
	GMP_RNDD, 53, 53, 53, 0.0);
  check(277363943109.0, 11.0, GMP_RNDN, 53, 53, 53, 0.0);
  /* test denormalized numbers too */
  check(8.06294740693074521573e-310, 6.95250701071929654575e-310,
	GMP_RNDU, 53, 53, 53, 0.0);
#ifdef VERBOSE
  printf("Comparing to double precision using machine arithmetic\n");
#endif
  for (i=0;i<N;i++) {
    x = drand(); 
    y = drand();
    if (ABS(x)>2.2e-307 && ABS(y)>2.2e-307 && x+y<1.7e+308 && x+y>-1.7e308) 
      /* avoid denormalized numbers and overflows */
      rnd = (rnd_mode==-1) ? lrand48()%4 : rnd_mode;
      check(x, y, rnd, prec, prec, prec, 0.0);
  } 
#ifdef VERBOSE
  printf("Checking mpfr_add(x, x, y) with prec=53\n");
#endif
  for (i=0;i<N;i++) {
    x = drand(); 
    y = drand();
    if (ABS(x)>2.2e-307 && ABS(y)>2.2e-307 && x+y<1.7e+308 && x+y>-1.7e308) 
      /* avoid denormalized numbers and overflows */
      rnd = (rnd_mode==-1) ? lrand48()%4 : rnd_mode;
      check3(x, y, rnd);
  } 
#ifdef VERBOSE
  printf("Checking mpfr_add(x, y, x) with prec=53\n");
#endif
  for (i=0;i<N;i++) {
    x = drand(); 
    y = drand();
    if (ABS(x)>2.2e-307 && ABS(y)>2.2e-307 && x+y<1.7e+308 && x+y>-1.7e308) 
      /* avoid denormalized numbers and overflows */
      rnd = (rnd_mode==-1) ? lrand48()%4 : rnd_mode;
      check4(x, y, rnd);
  } 
#ifdef VERBOSE
  printf("Checking mpfr_add(x, x, x) with prec=53\n");
#endif
  for (i=0;i<N;i++) {
    do { x = drand(); } while ((ABS(x)<2.2e-307) || (ABS(x)>0.8e308));
    /* avoid denormalized numbers and overflows */
    rnd = (rnd_mode==-1) ? lrand48()%4 : rnd_mode;
    check5(x, rnd);
  } 
  exit (0);
}

