
AC_DEFUN(AC_MY_LIBS,
[
if ` test "$1" `
then  
  AC_MSG_CHECKING(gmp library)
	if  test -r "$1/lib$2.a"
	then
	  LDADD="$LDADD $1/lib$2.a"
	else
	   AC_MSG_ERROR($2 not found)
	fi
  AC_MSG_RESULT(yes)
else
  AC_CHECK_LIB($2, main, , AC_MSG_ERROR($2 not found))
fi
]
)

AC_DEFUN(AC_MY_HEADERS, 
[
if  test "$1" 
then  
  AC_CHECK_HEADER($1/$2, INCLUDES="$INCLUDES -I$1",AC_MSG_ERROR(echo $2 not found in $1)) 
else
  AC_CHECK_HEADER($2,, 	  AC_MSG_ERROR($2 not found))
fi
])

AC_DEFUN(AC_CHECK_OS, 
[
AC_CACHE_CHECK([OS type], mpfr_cv_os_type, [
  mpfr_cv_os_type=`uname -s`
])
OS_TYPE=$mpfr_cv_os_type
])

AC_DEFUN(AC_CHECK_MACHTYPE,
[
AC_CACHE_CHECK([Mach type], mpfr_cv_mach_type, [
  mpfr_cv_mach_type=`uname -m`
])
MACHTYPE=$mpfr_cv_mach_type
])

dnl ------------------------------------------------------------

AC_DEFUN(MPFR_CONFIGS,
[
case $host in
	*-*-solaris*)
		LM9X="-lm9x"
		;;
esac

AC_REPLACE_FUNCS(strcasecmp)

dnl Check for IEEE-754 switches on Alpha
case $host in
alpha*-*-*)
  saved_CFLAGS="$CFLAGS"
  AC_CACHE_CHECK([for IEEE-754 switches], mpfr_cv_ieee_switches, [
  if test -n "$GCC"; then
    mpfr_cv_ieee_switches="-mfp-rounding-mode=d -mieee-with-inexact"
  else
    mpfr_cv_ieee_switches="-fprm d -ieee_with_inexact"
  fi
  CFLAGS="$CFLAGS $mpfr_cv_ieee_switches"
  AC_TRY_COMPILE(,,, mpfr_cv_ieee_switches="none")
  ])
  if test "$mpfr_cv_ieee_switches" = "none"; then
    CFLAGS="$saved_CFLAGS"
  else
    CFLAGS="$saved_CFLAGS $mpfr_cv_ieee_switches"
  fi
esac

AC_CHECK_HEADERS(fpu_control.h)

dnl Check for fesetround
AC_CACHE_CHECK([for fesetround], mpfr_cv_have_fesetround, [
saved_LIBS="$LIBS"
LIBS="$LIBS $LM9X"
AC_TRY_LINK([#include <fenv.h>], [fesetround(FE_TONEAREST);],
  mpfr_cv_have_fesetround=yes, mpfr_cv_have_fesetround=no)
LIBS="$saved_LIBS"
])
if test "$mpfr_cv_have_fesetround" = "yes"; then
  LIBS="$LIBS $LM9X"
  AC_DEFINE(MPFR_HAVE_FESETROUND,1,[Define if you have the `fesetround' function via the <fenv.h> header file.])
fi

dnl Check random functions
AC_CHECK_FUNCS(lrand48)

dnl Check whether 0/0, 1/0, -1/0, sqrt(-1) are valid expressions
AC_CACHE_CHECK([for valid NaN], mpfr_cv_valid_nan, [
AC_TRY_RUN([
#include <math.h>
int main()
{
  double x = (0.0/0.0) + sqrt(-1.0);
  return x == 1.0/0.0;
}
], mpfr_cv_valid_nan=yes, mpfr_cv_valid_nan=no, mpfr_cv_valid_nan=no)
])
if test "$mpfr_cv_valid_nan" = "yes"; then
   AC_DEFINE(HAVE_INFS,1,[Define if 0/0, 1/0, -1/0 and sqrt(-1) work to generate NaN/infinities.])
fi

dnl Check for gcc float-conversion bug; if need be, -ffloat-store is used to
dnl force the conversion to the destination type when a value is stored to
dnl a variable (see ISO C99 standard 5.1.2.3#13, 6.3.1.5#2, 6.3.1.8#2). This
dnl is important concerning the exponent range. Note that this doesn't solve
dnl the double-rounding problem (x86 processors still have to be set to the
dnl IEEE-754 compatible rounding mode).
if test -n "$GCC"; then
  AC_CACHE_CHECK([for gcc float-conversion bug], mpfr_cv_gcc_floatconv_bug, [
  AC_TRY_RUN([
#include <float.h>
#ifdef MPFR_HAVE_FESETROUND
#include <fenv.h>
#endif
int main()
{
  double x = 0.5;
  int i;
  for (i = 1; i <= 11; i++)
    x *= x;
  if (x != 0)
    return 1;
#ifdef MPFR_HAVE_FESETROUND
  /* Useful test for the G4 PowerPC */
  fesetround(FE_TOWARDZERO);
  x = DBL_MAX;
  x *= 2.0;
  if (x != DBL_MAX)
    return 1;
#endif
  return 0;
}
  ], [mpfr_cv_gcc_floatconv_bug="no"],
     [mpfr_cv_gcc_floatconv_bug="yes, use -ffloat-store"],
     [mpfr_cv_gcc_floatconv_bug="cannot test, use -ffloat-store"])
  ])
  if test "$mpfr_cv_gcc_floatconv_bug" != "no"; then
    CFLAGS="$CFLAGS -ffloat-store"
  fi
fi

dnl Check if denormalized numbers are supported
AC_CACHE_CHECK([for denormalized numbers], mpfr_cv_have_denorms, [
AC_TRY_RUN([
#include <math.h>
#include <stdio.h>
int main()
{
  double x = 2.22507385850720138309e-308;
  fprintf (stderr, "%e\n", x / 2.0);
  return 2.0 * (x / 2.0) != x;
}
], mpfr_cv_have_denorms=yes, mpfr_cv_have_denorms=no, mpfr_cv_have_denorms=no)
])
if test "$mpfr_cv_have_denorms" = "yes"; then
  AC_DEFINE(HAVE_DENORMS,1,[Define if denormalized floats work.])
fi

])
