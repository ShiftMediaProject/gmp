
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
	AC_MSG_CHECKING(OS type)
	OS_TYPE=`uname -a | awk '{print $ 1}' `
	AC_MSG_RESULT($OS_TYPE)
])

AC_DEFUN(AC_CHECK_MACHTYPE,
[
	AC_MSG_CHECKING(Mach type)
	MACHTYPE=`uname -m`
	AC_MSG_RESULT($MACHTYPE)
])

dnl ------------------------------------------------------------

AC_DEFUN(MPFR_CONFIGS,
[
AC_CHECK_HEADERS(fpu_control.h)

dnl Check for fesetround
AC_MSG_CHECKING(for fesetround)
saved_LIBS="$LIBS"
LIBS="$LIBS $LM9X"
AC_TRY_LINK([#include <fenv.h>], [fesetround(FE_TONEAREST);],
  [AC_MSG_RESULT(yes)
   AC_DEFINE(MPFR_HAVE_FESETROUND,1,[Define if you have the `fesetround' function via the <fenv.h> header file.])],
  [AC_MSG_RESULT(no)
   LIBS="$saved_LIBS"]
)

dnl Check random functions
AC_CHECK_FUNCS(lrand48)

dnl Check whether 0/0, 1/0, -1/0, sqrt(-1) are valid expressions
AC_MSG_CHECKING(for valid NaN)
AC_TRY_RUN([
#include <math.h>
int main()
{
  double x = (0.0/0.0) + sqrt(-1.0);
  return x == 1.0/0.0;
}
],
  [AC_MSG_RESULT(yes)
   AC_DEFINE(HAVE_INFS,1,[Define if 0/0, 1/0, -1/0 and sqrt(-1) work to generate NaN/infinities.])],
  AC_MSG_RESULT(no),
  AC_MSG_RESULT(no)
)

dnl Check for gcc float-conversion bug; if need be, -ffloat-store is used to
dnl force the conversion to the destination type when a value is stored to
dnl a variable (see ISO C99 standard 5.1.2.3#13, 6.3.1.5#2, 6.3.1.8#2). This
dnl is important concerning the exponent range. Note that this doesn't solve
dnl the double-rounding problem (x86 processors still have to be set to the
dnl IEEE-754 compatible rounding mode).
if test -n "$GCC"; then
  AC_MSG_CHECKING(for gcc float-conversion bug)
  AC_TRY_RUN([
int main()
{
  double x = 0.5;
  int i;
  for (i = 1; i <= 11; i++)
    x *= x;
  return x == 0;
}
  ],
    [AC_MSG_RESULT([yes, use -ffloat-store])
     CFLAGS="$CFLAGS -ffloat-store"],
    AC_MSG_RESULT(no),
    [AC_MSG_RESULT([can't test, use -ffloat-store])
     CFLAGS="$CFLAGS -ffloat-store"]
  )
fi

dnl Check if denormalized numbers are supported
AC_MSG_CHECKING(for denormalized numbers)
AC_TRY_RUN([
#include <math.h>
#include <stdio.h>
int main()
{
  double x = 2.22507385850720138309e-308;
  fprintf (stderr, "%e\n", x / 2.0);
  return 2.0 * (x / 2.0) != x;
}
],
  [AC_MSG_RESULT(yes)
   AC_DEFINE(HAVE_DENORMS,1,[Define if denormalized floats work.])],
  AC_MSG_RESULT(no),
  AC_MSG_RESULT(no)
)

])
