/* Test file for hyperbolic function : mpfr_cosh, mpfr_sinh, mpfr_tanh, mpfr_acosh, mpfr_asinh, mpfr_atanh.
 
Copyright 2001, 2002 Free Software Foundation, Inc.
 
This file is part of the MPFR Library.
 
The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published
by
the Free Software Foundation; either version 2.1 of the License, or (at youroption) any later version.
 
The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITYor FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.
 
You should have received a copy of the GNU Lesser General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */
                   
#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"


int check_O _PROTO((void));
int check_NAN _PROTO((void));
int check_zero _PROTO((void));
int check_INF _PROTO((void));



int
check_NAN (void)
{

 mpfr_t t, ch,sh,th,ach,ash,ath;
 int tester;

 mpfr_init2(t,200);
 mpfr_init2(ch,200);
 mpfr_init2(sh,200);
 mpfr_init2(th,200);
 mpfr_init2(ach,200);
 mpfr_init2(ash,200);
 mpfr_init2(ath,200);

 MPFR_SET_NAN(t);

  /******cosh********/

  tester=mpfr_cosh(ch,t,GMP_RNDD);
  if (!MPFR_IS_NAN(ch) || tester!=0 )
  {
    printf("cosh NAN \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
    return(1);
  } 

  /******sinh********/

  tester=mpfr_sinh(sh,t,GMP_RNDD);
  if (!MPFR_IS_NAN(sh) || tester!=0 )
  {
    printf("sinh NAN \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
    return(1);
  } 

  /******tanh********/

  tester=mpfr_tanh(th,t,GMP_RNDD);
  if (!MPFR_IS_NAN(th) || tester!=0 )
  {
    printf("tanh NAN \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
    return(1);
  } 

  /******acosh********/

  tester=mpfr_acosh(ach,t,GMP_RNDD);
  if (!MPFR_IS_NAN(ach) || tester!=0 )
  {
    printf("acosh NAN \n");
    return(1);
  } 

  /******asinh********/

  tester=mpfr_asinh(ash,t,GMP_RNDD);
  if (!MPFR_IS_NAN(ash) || tester!=0 )
  {
    printf("asinh NAN \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
    return(1);
  } 

  /******atanh********/

  tester=mpfr_atanh(ath,t,GMP_RNDD);
  if (!MPFR_IS_NAN(ath) || tester!=0 )
  {
    printf("atanh NAN \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
    return(1);
  } 

  mpfr_clear(t);
  mpfr_clear(ch);
  mpfr_clear(sh);
  mpfr_clear(th);
  mpfr_clear(ach);
  mpfr_clear(ash);
  mpfr_clear(ath);

  return(0);

}
int
check_zero (void)
{

 mpfr_t t, ch,sh,th,ach,ash,ath;
 int tester;

 mpfr_init2(t,200);
 mpfr_init2(ch,200);
 mpfr_init2(sh,200);
 mpfr_init2(th,200);
 mpfr_init2(ach,200);
 mpfr_init2(ash,200);
 mpfr_init2(ath,200);


 mpfr_set_ui(t,0,GMP_RNDD);

  /******cosh********/

  tester=mpfr_cosh(ch,t,GMP_RNDD);
  if (!mpfr_cmp_ui(ch,1) || !tester )
  {
    printf("cosh(0) \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
    return(1);
  } 

  /******sinh********/

  tester=mpfr_sinh(sh,t,GMP_RNDD);
  if (!MPFR_IS_ZERO(sh) || !tester )
  {
    printf("sinh(0) \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
    return(1);
  } 

  /******tanh********/

  tester=mpfr_tanh(th,t,GMP_RNDD);
  if (!MPFR_IS_ZERO(th) || !tester )
  {
    printf("tanh(0) \n");
   mpfr_clear(t);
   mpfr_clear(ch);
   mpfr_clear(sh);
   mpfr_clear(th);
   mpfr_clear(ach);
   mpfr_clear(ash);
   mpfr_clear(ath);
   return(1);
  } 

  /******acosh********/

  tester=mpfr_acosh(ach,t,GMP_RNDD);
  if (!MPFR_IS_NAN(ach) || tester != -1 )
  {
    printf("acosh(0) \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
    return(1);
  } 

  /******asinh********/

  tester=mpfr_asinh(ash,t,GMP_RNDD);
  if (!MPFR_IS_ZERO(ash) || !tester )
  {
    printf("asinh(0) \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
    return(1);
  } 

  /******atanh********/

  tester=mpfr_atanh(ath,t,GMP_RNDD);
  if (!MPFR_IS_ZERO(ath) || !tester )
  {
    printf("atanh(0) \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
    return(1);
  } 

  mpfr_clear(t);
  mpfr_clear(ch);
  mpfr_clear(sh);
  mpfr_clear(th);
  mpfr_clear(ach);
  mpfr_clear(ash);
  mpfr_clear(ath);

  return(0);

}

int
check_INF (void)
{

 mpfr_t t, ch,sh,th,ach,ash,ath;
 int tester;


 mpfr_init2(t,200);
 mpfr_init2(ch,200);
 mpfr_init2(sh,200);
 mpfr_init2(th,200);
 mpfr_init2(ach,200);
 mpfr_init2(ash,200);
 mpfr_init2(ath,200);

 MPFR_CLEAR_NAN(t);
 MPFR_SET_INF(t);

 if(MPFR_SIGN(t)<0)
   MPFR_CHANGE_SIGN(t);

  /******cosh********/

  tester = mpfr_cosh(ch,t,GMP_RNDD);
  if (!MPFR_IS_INF(ch) || MPFR_SIGN(ch) < 0 || tester!=0 )
  {
    printf("cosh(INF) \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
    return(1);
  } 

  /******sinh********/

  tester=mpfr_sinh(sh,t,GMP_RNDD);
  if (!MPFR_IS_INF(sh) || MPFR_SIGN(sh) < 0  || tester!=0 )
  {
    printf("sinh(INF) \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
    return(1);
  } 

  /******tanh********/

  tester=mpfr_tanh(th,t,GMP_RNDD);
  if (mpfr_cmp_ui(th,1) != 0 || tester!=0 )
  {
    printf("tanh(INF) \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
    return(1);
  } 

  /******acosh********/

  tester=mpfr_acosh(ach,t,GMP_RNDD);
  if (!MPFR_IS_INF(ach) || MPFR_SIGN(ach) < 0  || tester!=0 )
  {
    printf("acosh(INF) \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
    return(1);
  } 

  /******asinh********/

  tester=mpfr_asinh(ash,t,GMP_RNDD);
  if (!MPFR_IS_INF(ash) || MPFR_SIGN(ash) < 0  || tester!=0 )
  {
    printf("asinh(INF) \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
    return(1);
  } 

  /******atanh********/

  tester=mpfr_atanh(ath,t,GMP_RNDD);  
  if (!MPFR_IS_INF(ath) || tester != 0)
  {
    printf("atanh(INF) \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
    return(1);
  } 

 MPFR_CHANGE_SIGN(t);

  /******cosh********/

  tester=mpfr_cosh(ch,t,GMP_RNDD);
  if (!MPFR_IS_INF(ch) || MPFR_SIGN(ch) < 0  || tester!=0 )
  {
    printf("cosh(-INF) \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
    return(1);
  } 

  /******sinh********/

  tester=mpfr_sinh(sh,t,GMP_RNDD);
  if (!MPFR_IS_INF(sh)  || MPFR_SIGN(sh) > 0 || tester!=0 )
  {
    printf("sinh(-INF) \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
    return(1);
  } 

  /******tanh********/

  tester=mpfr_tanh(th,t,GMP_RNDD);
  if (!mpfr_cmp_ui(th,-1) || tester!=0 )
  {
    printf("tanh(-INF) \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
    return(1);
  } 

  /******acosh********/

  tester=mpfr_acosh(ach,t,GMP_RNDD);
  if (!MPFR_IS_INF(ach) || MPFR_SIGN(ach) < 0  || tester!=0 )
  {
    printf("acosh(-INF) \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
    return(1);
  } 

  /******asinh********/

  tester=mpfr_asinh(ash,t,GMP_RNDD);
  if (!MPFR_IS_INF(ash) || MPFR_SIGN(ash) > 0  || tester!=0 )
  {
    printf("asinh(-INF) \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
    return(1);
  } 

  /******atanh********/

  tester=mpfr_atanh(ath,t,GMP_RNDD);
  if (!MPFR_IS_INF(ath) || MPFR_SIGN(ath) > 0 || tester != 0 )
  {
    printf("atanh(-INF) \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
   return(1);
  } 

  mpfr_clear(t);
  mpfr_clear(ch);
  mpfr_clear(sh);
  mpfr_clear(th);
  mpfr_clear(ach);
  mpfr_clear(ash);
  mpfr_clear(ath);


  return(0);

}

int
check_O (void)
{

 mpfr_t t, ch,sh,th,ach,ash,ath;

 mpfr_init2(t,20);
 mpfr_init2(ch,40);
 mpfr_init2(sh,40);
 mpfr_init2(th,40);
 mpfr_init2(ach,40);
 mpfr_init2(ash,40);
 mpfr_init2(ath,40);

 mpfr_set_ui(t,2,GMP_RNDD);

 /******acosh o cosh********/

  mpfr_cosh(ch,t,GMP_RNDN);
  mpfr_acosh(ach,ch,GMP_RNDN);
  if(mpfr_cmp_ui(ach,2)!=0)
  {
    printf("cosh o acosh \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
    return(1);
  }

  /******asinh o sinh********/

  mpfr_sinh(sh,t,GMP_RNDN);
  mpfr_asinh(ash,sh,GMP_RNDN);
  if(mpfr_cmp_ui(ash,2)!=0)
  {
    printf("sinh o asinh \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
    return(1);
  }
  

  /******atanh o tanh********/

  mpfr_tanh(th,t,GMP_RNDN);
  mpfr_atanh(ath,th,GMP_RNDU);

  /*
  mpfr_out_str(stdout, 2,40,th,GMP_RNDN);
  putchar('\n');

  mpfr_out_str(stdout, 2,40,ath,GMP_RNDU);
  putchar('\n');
  */

  if(mpfr_cmp_ui(ath,2)!=0)
  {
    printf("tanh o atanh \n");
    mpfr_clear(t);
    mpfr_clear(ch);
    mpfr_clear(sh);
    mpfr_clear(th);
    mpfr_clear(ach);
    mpfr_clear(ash);
    mpfr_clear(ath);
    return(1);
  }

  mpfr_clear(t);
  mpfr_clear(ch);
  mpfr_clear(sh);
  mpfr_clear(th);
  mpfr_clear(ach);
  mpfr_clear(ash);
  mpfr_clear(ath);

 return(0);

}

int
main(void)
{

if (check_INF())printf("Error in evaluation of INF\n");
if (check_NAN())printf("Error in evaluation of NAN\n");
/*if (check_O())printf("Error in evaluation of composition hyperbolic function\n");*/

return(0);


}
