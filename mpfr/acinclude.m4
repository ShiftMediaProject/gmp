
AC_DEFUN(AC_MY_LIBS,
[
if ` test "$1" `
then  
  AC_MSG_CHECKING(gmp library)
	if  test -r "$1/lib$2.a"
	then
	  LDADD="$LDADD $1/lib$2.a"
	else
	   AC_MSG_ERROR($1/lib$2.a not found)
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

# CPU-dependent objects for the test programs
case $host in
  X86_PATTERN)
    AC_SUBST(TESTS_ASM_OBJECTS, x86.$OBJEXT)
    AC_DEFINE(MPFR_HAVE_TESTS_x86, 1,
              [Define to 1 if mpfr x86 test routines are available.])
    ;;
esac


AC_REPLACE_FUNCS(strcasecmp strncasecmp)

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

# Reasons for testing:
#
#   sys/fpu.h - MIPS specific
#
AC_CHECK_HEADERS(sys/fpu.h)

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

dnl Check for isnan
AC_CACHE_CHECK([for isnan], mpfr_cv_have_isnan, [
AC_TRY_LINK([#include <math.h>], [isnan(0.0);],
  mpfr_cv_have_isnan=yes, mpfr_cv_have_isnan=no)
])
if test "$mpfr_cv_have_isnan" = "yes"; then
  AC_DEFINE(MPFR_HAVE_ISNAN,1,[Define if you have the `isnan' function via the <math.h> header file.])
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

dnl Check if HUGE_VAL is supported without the need of a specific library
AC_CACHE_CHECK([for HUGE_VAL], mpfr_cv_have_huge_val, [
AC_TRY_LINK([#include <math.h>], [HUGE_VAL;],
  mpfr_cv_have_huge_val=yes, mpfr_cv_have_huge_val=no)
])
if test "$mpfr_cv_have_huge_val" = "yes"; then
  AC_DEFINE(HAVE_HUGE_VAL,1,[Define if HUGE_VAL can be used without the need of a specific library.])
fi

MPFR_C_LONG_DOUBLE_FORMAT
])


dnl  MPFR_C_LONG_DOUBLE_FORMAT
dnl  -------------------------
dnl  Determine the format of a long double.
dnl
dnl  The object file is grepped, so as to work when cross compiling.  A
dnl  start and end sequence is included to avoid false matches, and
dnl  allowance is made for the desired data crossing an "od -b" line
dnl  boundary.  The test number is a small integer so it should appear
dnl  exactly, no rounding or truncation etc.
dnl
dnl  "od -b" is supported even by Unix V7, and the awk script used doesn't
dnl  have functions or anything, so even an "old" awk should suffice.
dnl
dnl  The 10-byte IEEE extended format is normally padded to either 12 or 16
dnl  bytes, for alignment purposes.  The SVR4 i386 ABI is 12 bytes, or i386
dnl  gcc -m128bit-long-double can select 16 bytes.  IA-64 is 16 bytes in
dnl  LP64 mode, or 12 bytes in ILP32 mode.  The first 10 bytes are the
dnl  relevant data, even in IA-64 big-endian mode too.
dnl
dnl  Enhancements:
dnl
dnl  Could match more formats (IEEE quad, or big endian IEEE), but no need
dnl  to worry until there's code wanting to use them.
dnl
dnl  Don't want to duplicate the double matching from GMP_C_DOUBLE_FORMAT,
dnl  perhaps we should merge with that macro, to match data formats
dnl  irrespective of the C type in question.  Or perhaps just let the code
dnl  use DOUBLE macros when sizeof(double)==sizeof(long double).

AC_DEFUN(MPFR_C_LONG_DOUBLE_FORMAT,
[AC_REQUIRE([AC_PROG_CC])
AC_REQUIRE([AC_PROG_AWK])
AC_CACHE_CHECK([format of `long double' floating point],
                mpfr_cv_c_long_double_format,
[mpfr_cv_c_long_double_format=unknown
cat >conftest.c <<\EOF
[
/* "before" is 16 bytes to ensure there's no padding between it and "x".
   We're not expecting any "long double" bigger than 16 bytes or with
   alignment requirements stricter than 16 bytes.  */
struct {
  char         before[16];
  long double  x;
  char         after[8];
} foo = {
  { '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\x01', '\x23', '\x45', '\x67', '\x89', '\xAB', '\xCD', '\xEF' },
  -123456789.0,
  { '\xFE', '\xDC', '\xBA', '\x98', '\x76', '\x54', '\x32', '\x10' },
};
]
EOF
mpfr_compile="$CC $CFLAGS $CPPFLAGS -c conftest.c >&AC_FD_CC 2>&1"
if AC_TRY_EVAL(mpfr_compile); then
cat >conftest.awk <<\EOF
[
BEGIN {
  found = 0
}

# got[] holds a sliding window of bytes read the input.  got[0] is the most
# recent byte read, and got[31] the oldest byte read, so when looking to
# match some data the indices are "reversed".
#
{
  for (f = 2; f <= NF; f++)
    {
      # new byte, shift others up
      for (i = 31; i >= 0; i--)
        got[i+1] = got[i];
      got[0] = $f;

      # end sequence
      if (got[7] != "376") continue
      if (got[6] != "334") continue
      if (got[5] != "272") continue
      if (got[4] != "230") continue
      if (got[3] != "166") continue
      if (got[2] != "124") continue
      if (got[1] != "062") continue
      if (got[0] != "020") continue

      # start sequence, with 12-byte body
      if (got[27] == "001" && \
          got[26] == "043" && \
          got[25] == "105" && \
          got[24] == "147" && \
          got[23] == "211" && \
          got[22] == "253" && \
          got[21] == "315" && \
          got[20] == "357")
        {
          saw = " (" got[19] \
                 " " got[18] \
                 " " got[17] \
                 " " got[16] \
                 " " got[15] \
                 " " got[14] \
                 " " got[13] \
                 " " got[12] \
                 " " got[11] \
                 " " got[10] \
                 " " got[9]  \
                 " " got[8] ")"

          if (got[19] == "000" && \
              got[18] == "000" && \
              got[17] == "000" && \
              got[16] == "000" && \
              got[15] == "240" && \
              got[14] == "242" && \
              got[13] == "171" && \
              got[12] == "353" && \
              got[11] == "031" && \
              got[10] == "300")
            {
              print "IEEE extended, little endian"
              found = 1
              exit
            }
        }

      # start sequence, with 16-byte body
      if (got[31] == "001" && \
          got[30] == "043" && \
          got[29] == "105" && \
          got[28] == "147" && \
          got[27] == "211" && \
          got[26] == "253" && \
          got[25] == "315" && \
          got[24] == "357")
        {
          saw = " (" got[23] \
                 " " got[22] \
                 " " got[21] \
                 " " got[20] \
                 " " got[19] \
                 " " got[18] \
                 " " got[17] \
                 " " got[16] \
                 " " got[15] \
                 " " got[14] \
                 " " got[13] \
                 " " got[12] \
                 " " got[11] \
                 " " got[10] \
                 " " got[9]  \
                 " " got[8] ")"

          if (got[23] == "000" && \
              got[22] == "000" && \
              got[21] == "000" && \
              got[20] == "000" && \
              got[19] == "240" && \
              got[18] == "242" && \
              got[17] == "171" && \
              got[16] == "353" && \
              got[15] == "031" && \
              got[14] == "300")
            {
              print "IEEE extended, little endian"
              found = 1
              exit
            }
        }
    }
}

END {
  if (! found)
    print "unknown", saw
}
]
EOF
  mpfr_cv_c_long_double_format=`od -b conftest.$OBJEXT | $AWK -f conftest.awk`
  case $mpfr_cv_c_long_double_format in
  unknown*)
    echo "cannot match anything, conftest.$OBJEXT contains" >&AC_FD_CC
    od -b conftest.$OBJEXT >&AC_FD_CC
    ;;
  esac
else
  AC_MSG_WARN([oops, cannot compile test program])
fi
])

AH_VERBATIM([HAVE_LDOUBLE],
[/* Define one of the following to 1 for the format of a `long double'.
   If your format is not among these choices, or you don't know what it is,
   then leave all undefined.
   IEEE_EXT is the 10-byte IEEE extended format.
   LITTLE or BIG is the endianness.  */
#undef HAVE_LDOUBLE_IEEE_EXT_LITTLE])

case $mpfr_cv_c_long_double_format in
  "IEEE extended, little endian")
    AC_DEFINE(HAVE_LDOUBLE_IEEE_EXT_LITTLE, 1)
    ;;
  unknown*)
    ;;
  *) 
    AC_MSG_WARN([oops, unrecognised float format: $mpfr_cv_c_long_double_format])
    ;;
esac
])
