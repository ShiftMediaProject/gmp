dnl  GMP specific autoconf macros


dnl  Copyright 2000, 2001, 2002 Free Software Foundation, Inc.
dnl
dnl  This file is part of the GNU MP Library.
dnl
dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU Lesser General Public License as published
dnl  by the Free Software Foundation; either version 2.1 of the License, or (at
dnl  your option) any later version.
dnl
dnl  The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
dnl  License for more details.
dnl
dnl  You should have received a copy of the GNU Lesser General Public License
dnl  along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
dnl  the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
dnl  MA 02111-1307, USA.


define(X86_PATTERN,
[[i?86*-*-* | k[5-8]*-*-* | pentium*-*-* | athlon-*-*]])

define(POWERPC64_PATTERN,
[[powerpc64-*-* | powerpc64le-*-* | powerpc620-*-* | powerpc630-*-*]])


dnl  GMP_STRIP_PATH(subdir)
dnl  ----------------------
dnl  Strip entries */subdir from $path.

define(GMP_STRIP_PATH,
[tmp_path=
for i in $path; do
  case $i in
    */$1) ;;
    *) tmp_path="$tmp_path $i" ;;
  esac
done
path="$tmp_path"
])


dnl  GMP_INCLUDE_GMP_H
dnl  -----------------
dnl  Expand to the right way to #include gmp-h.in.  This must be used
dnl  instead of gmp.h, since that file isn't generated until the end of the
dnl  configure.
dnl
dnl  GMP_INCLUDE_GMP_H_BITS_PER_MP_LIMB starts as a dummy, but gets
dnl  redefined in GMP_C_SIZES when the right value is known.

define(GMP_INCLUDE_GMP_H,
[[#define __GMP_WITHIN_CONFIGURE 1   /* ignore template stuff */
#define GMP_NAIL_BITS $GMP_NAIL_BITS]
GMP_INCLUDE_GMP_H_BITS_PER_MP_LIMB
[$DEFN_LONG_LONG_LIMB
#include "$srcdir/gmp-h.in"]
])

define(GMP_INCLUDE_GMP_H_BITS_PER_MP_LIMB,
[[#define __GMP_BITS_PER_MP_LIMB 123 /* dummy for GMP_NUMB_BITS etc */
#define GMP_LIMB_BITS 123]])


dnl  GMP_HEADER_GETVAL(NAME,FILE)
dnl  ----------------------------
dnl  Expand at autoconf time to the value of a "#define NAME" from the given
dnl  FILE.  The regexps here aren't very rugged, but are enough for gmp.
dnl  /dev/null as a parameter prevents a hang if $2 is accidentally omitted.

define(GMP_HEADER_GETVAL,
[patsubst(patsubst(
esyscmd([grep "^#define $1 " $2 /dev/null 2>/dev/null]),
[^.*$1[ 	]+],[]),
[[
 	]*$],[])])


dnl  GMP_VERSION
dnl  -----------
dnl  The gmp version number, extracted from the #defines in gmp-h.in at
dnl  autoconf time.  Two digits like 3.0 if patchlevel <= 0, or three digits
dnl  like 3.0.1 if patchlevel > 0.

define(GMP_VERSION,
[GMP_HEADER_GETVAL(__GNU_MP_VERSION,gmp-h.in)[]dnl
.GMP_HEADER_GETVAL(__GNU_MP_VERSION_MINOR,gmp-h.in)[]dnl
ifelse(m4_eval(GMP_HEADER_GETVAL(__GNU_MP_VERSION_PATCHLEVEL,gmp-h.in) > 0),1,
[.GMP_HEADER_GETVAL(__GNU_MP_VERSION_PATCHLEVEL,gmp-h.in)])])


dnl  GMP_SUBST_CHECK_FUNCS(func,...)
dnl  ------------------------------
dnl  Setup an AC_SUBST of HAVE_FUNC_01 for each argument.

AC_DEFUN(GMP_SUBST_CHECK_FUNCS,
[m4_if([$1],,,
[_GMP_SUBST_CHECK_FUNCS(ac_cv_func_[$1],HAVE_[]m4_translit([$1],[a-z],[A-Z])_01)
GMP_SUBST_CHECK_FUNCS(m4_shift($@))])])

dnl  Called: _GMP_SUBST_CHECK_FUNCS(cachvar,substvar)
AC_DEFUN(_GMP_SUBST_CHECK_FUNCS,
[case $[$1] in
yes) AC_SUBST([$2],1) ;;
no)  [$2]=0 ;;
esac
])


dnl  GMP_SUBST_CHECK_HEADERS(foo.h,...)
dnl  ----------------------------------
dnl  Setup an AC_SUBST of HAVE_FOO_H_01 for each argument.

AC_DEFUN(GMP_SUBST_CHECK_HEADERS,
[m4_if([$1],,,
[_GMP_SUBST_CHECK_HEADERS(ac_cv_header_[]m4_translit([$1],[./],[__]),
HAVE_[]m4_translit([$1],[a-z./],[A-Z__])_01)
GMP_SUBST_CHECK_HEADERS(m4_shift($@))])])

dnl  Called: _GMP_SUBST_CHECK_HEADERS(cachvar,substvar)
AC_DEFUN(_GMP_SUBST_CHECK_HEADERS,
[case $[$1] in
yes) AC_SUBST([$2],1) ;;
no)  [$2]=0 ;;
esac
])


dnl  GMP_COMPARE_GE(A1,B1, A2,B2, ...)
dnl  ---------------------------------
dnl  Compare two version numbers A1.A2.etc and B1.B2.etc.  Set
dnl  $gmp_compare_ge to yes or no accoring to the result.  The A parts
dnl  should be variables, the B parts fixed numbers.  As many parts as
dnl  desired can be included.  An empty string in an A part is taken to be
dnl  zero, the B parts should be non-empty and non-zero.
dnl
dnl  For example,
dnl
dnl      GMP_COMPARE($major,10, $minor,3, $subminor,1)
dnl
dnl  would test whether $major.$minor.$subminor is greater than or equal to
dnl  10.3.1.

AC_DEFUN(GMP_COMPARE_GE,
[gmp_compare_ge=no
GMP_COMPARE_GE_INTERNAL($@)
])

AC_DEFUN(GMP_COMPARE_GE_INTERNAL,
[ifelse(len([$3]),0,
[if test -n "$1" && test "$1" -ge $2; then
  gmp_compare_ge=yes
fi],
[if test -n "$1"; then
  if test "$1" -gt $2; then
    gmp_compare_ge=yes
  else
    if test "$1" -eq $2; then
      GMP_COMPARE_GE_INTERNAL(m4_shift(m4_shift($@)))
    fi
  fi
fi])
])
  

dnl  GMP_PROG_AR
dnl  -----------
dnl  GMP additions to $AR.
dnl
dnl  A cross-"ar" may be necessary when cross-compiling since the build
dnl  system "ar" might try to interpret the object files to build a symbol
dnl  table index, hence the use of AC_CHECK_TOOL.
dnl
dnl  A user-selected $AR is always left unchanged.  AC_CHECK_TOOL is still
dnl  run to get the "checking" message printed though.
dnl
dnl  $AR_FLAGS is set to "cq" rather than leaving it to libtool "cru".  The
dnl  latter fails when libtool goes into piecewise mode and is unlucky
dnl  enough to have two same-named objects in separate pieces, as happens
dnl  for instance to random.o (and others) on vax-dec-ultrix4.5.  Naturally
dnl  a user-selected $AR_FLAGS is left unchanged.

AC_DEFUN(GMP_PROG_AR,
[dnl  Want to establish $AR before libtool initialization.
AC_BEFORE([$0],[AC_PROG_LIBTOOL])
gmp_user_AR=$AR
AC_CHECK_TOOL(AR, ar, ar)
if test -z "$gmp_user_AR"; then
                        eval arflags=\"\$ar${abi1}_flags\"
  test -n "$arflags" || eval arflags=\"\$ar${abi2}_flags\"
  if test -n "$arflags"; then
    AC_MSG_CHECKING([for extra ar flags])
    AR="$AR $arflags"
    AC_MSG_RESULT([$arflags])
  fi
fi
if test -z "$AR_FLAGS"; then
  AR_FLAGS=cq
fi
])


dnl  GMP_PROG_HOST_CC
dnl  ----------------
dnl  Establish a value for $HOST_CC.
dnl
dnl  Any HOST_CC already set is used without testing.  Likewise any
dnl  CC_FOR_BUILD is used without testing.  CC_FOR_BUILD is the new name for
dnl  a build system compiler, see configfsf.guess.

AC_DEFUN(GMP_PROG_HOST_CC,
[AC_BEFORE([$0],[AC_PROG_LIBTOOL])
AC_REQUIRE([AC_PROG_CC])
AC_CACHE_CHECK([for HOST_CC build system compiler],
               gmp_cv_prog_host_cc,
[if test -n "$HOST_CC"; then
  gmp_cv_prog_host_cc=$HOST_CC
else
  if test -n "$CC_FOR_BUILD"; then
    gmp_cv_prog_host_cc=$CC_FOR_BUILD
  else
    cat >conftest.c <<EOF
int main () { exit(0); }
EOF
    for c in "$CC" cc gcc c89; do
      echo "trying $c" >&AC_FD_CC        
      if ($c conftest.c -o conftest) >&AC_FD_CC 2>&1; then
        if (./conftest) >&AC_FD_CC 2>&1; then
          gmp_cv_prog_host_cc=$c
          break
        fi
      fi
    done
    rm -f conftest*
    if test -z "$gmp_cv_prog_host_cc"; then
      AC_MSG_ERROR([cannot find a build system compiler])
    fi
  fi
fi
])
HOST_CC=$gmp_cv_prog_host_cc
])


dnl  GMP_PROG_LEX
dnl  ------------
dnl  AC_PROG_LEX bombs if $LEX is set to ${am_missing_run}flex by
dnl  AM_PROG_LEX.  It needs to see LEX=: if lex is missing.  Avoid this by
dnl  running AC_PROG_LEX first and then using "missing".
dnl
dnl  FIXME: This can be removed and just AM_PROG_LEX used, one that macro
dnl  works properly.

AC_DEFUN(GMP_PROG_LEX,
[AC_REQUIRE([AC_PROG_LEX])
if test "$LEX" = :; then
  LEX=${am_missing_run}flex
fi
])


dnl  GMP_PROG_M4
dnl  -----------
dnl  Find a working m4, either in $PATH or likely locations, and setup $M4
dnl  and an AC_SUBST accordingly.  If $M4 is already set then it's a user
dnl  choice and is accepted with no checks.  GMP_PROG_M4 is like
dnl  AC_PATH_PROG or AC_CHECK_PROG, but tests each m4 found to see if it's
dnl  good enough.
dnl 
dnl  See mpn/asm-defs.m4 for details on the known bad m4s.

AC_DEFUN(GMP_PROG_M4,
[AC_ARG_VAR(M4,[m4 macro processor])
AC_CACHE_CHECK([for suitable m4],
                gmp_cv_prog_m4,
[if test -n "$M4"; then
  gmp_cv_prog_m4="$M4"
else
  cat >conftest.m4 <<\EOF
dnl  Must protect this against being expanded during autoconf m4!
dnl  Dont put "dnl"s in this as autoconf will flag an error for unexpanded
dnl  macros.
[define(dollarhash,``$][#'')ifelse(dollarhash(x),1,`define(t1,Y)',
``bad: $][# not supported (SunOS /usr/bin/m4)
'')ifelse(eval(89),89,`define(t2,Y)',
`bad: eval() doesnt support 8 or 9 in a constant (OpenBSD 2.6 m4)
')ifelse(t1`'t2,YY,`good
')]
EOF
dnl ' <- balance the quotes for emacs sh-mode
  echo "trying m4" >&AC_FD_CC
  gmp_tmp_val=`(m4 conftest.m4) 2>&AC_FD_CC`
  echo "$gmp_tmp_val" >&AC_FD_CC
  if test "$gmp_tmp_val" = good; then
    gmp_cv_prog_m4="m4"
  else
    IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS=":"
dnl $ac_dummy forces splitting on constant user-supplied paths.
dnl POSIX.2 word splitting is done only on the output of word expansions,
dnl not every word.  This closes a longstanding sh security hole.
    ac_dummy="$PATH:/usr/5bin"
    for ac_dir in $ac_dummy; do
      test -z "$ac_dir" && ac_dir=.
      echo "trying $ac_dir/m4" >&AC_FD_CC
      gmp_tmp_val=`($ac_dir/m4 conftest.m4) 2>&AC_FD_CC`
      echo "$gmp_tmp_val" >&AC_FD_CC
      if test "$gmp_tmp_val" = good; then
        gmp_cv_prog_m4="$ac_dir/m4"
        break
      fi
    done
    IFS="$ac_save_ifs"
    if test -z "$gmp_cv_prog_m4"; then
      AC_MSG_ERROR([No usable m4 in \$PATH or /usr/5bin (see config.log for reasons).])
    fi
  fi
  rm -f conftest.m4
fi])
M4="$gmp_cv_prog_m4"
AC_SUBST(M4)
])


dnl  GMP_M4_M4WRAP_SPURIOUS
dnl  ----------------------
dnl  Check for spurious output from m4wrap(), as described in mpn/asm-defs.m4.
dnl
dnl  The following systems have been seen with the problem.
dnl
dnl  - Unicos alpha, but its assembler doesn't seem to mind.
dnl  - MacOS X Darwin, its assembler fails.
dnl  - NetBSD 1.4.1 m68k, and gas 1.92.3 there gives a warning and ignores
dnl    the bad last line since it doesn't have a newline.
dnl  - NetBSD 1.4.2 alpha, but its assembler doesn't seem to mind.
dnl
dnl  Enhancement: Maybe this could be in GMP_PROG_M4, and attempt to prefer
dnl  an m4 with a working m4wrap, if it can be found.

AC_DEFUN(GMP_M4_M4WRAP_SPURIOUS,
[AC_REQUIRE([GMP_PROG_M4])
AC_CACHE_CHECK([if m4wrap produces spurious output],
               gmp_cv_m4_m4wrap_spurious,
[# hide the d-n-l from autoconf's error checking
tmp_d_n_l=d""nl
cat >conftest.m4 <<EOF
[changequote({,})define(x,)m4wrap({x})$tmp_d_n_l]
EOF
echo test input is >&AC_FD_CC
cat conftest.m4 >&AC_FD_CC
tmp_chars=`$M4 conftest.m4 | wc -c`
echo produces $tmp_chars chars output >&AC_FD_CC
rm -f conftest.m4
if test $tmp_chars = 0; then
  gmp_cv_m4_m4wrap_spurious=no
else
  gmp_cv_m4_m4wrap_spurious=yes
fi
])
GMP_DEFINE_RAW(["define(<M4WRAP_SPURIOUS>,<$gmp_cv_m4_m4wrap_spurious>)"])
])


dnl  GMP_PROG_NM
dnl  -----------
dnl  GMP additions to libtool AC_PROG_NM.
dnl
dnl  Note that if AC_PROG_NM can't find a working nm it still leaves
dnl  $NM set to "nm", so $NM can't be assumed to actually work.
dnl
dnl  A user-selected $NM is always left unchanged.  AC_PROG_NM is still run
dnl  to get the "checking" message printed though.

AC_DEFUN(GMP_PROG_NM,
[dnl  Make sure we're the first to call AC_PROG_NM, so our extra flags are
dnl   used by everyone.
AC_BEFORE([$0],[AC_PROG_NM])
gmp_user_NM=$NM
AC_PROG_NM

# FIXME: When cross compiling (ie. $ac_tool_prefix not empty), libtool
# defaults to plain "nm" if a "${ac_tool_prefix}nm" is not found.  In this
# case run it again to try the native "nm", firstly so that likely locations
# are searched, secondly so that -B or -p are added if necessary for BSD
# format.  This is necessary for instance on OSF with "./configure
# --build=alphaev5-dec-osf --host=alphaev6-dec-osf".
#
if test -z "$gmp_user_NM" && test -n "$ac_tool_prefix" && test "$NM" = nm; then
  $as_unset lt_cv_path_NM
  gmp_save_ac_tool_prefix=$ac_tool_prefix
  ac_tool_prefix=
  NM=
  AC_PROG_NM
  ac_tool_prefix=$gmp_save_ac_tool_prefix
fi

if test -z "$gmp_user_NM"; then
                        eval nmflags=\"\$nm${abi1}_flags\"
  test -n "$nmflags" || eval nmflags=\"\$nm${abi2}_flags\"
  if test -n "$nmflags"; then
    AC_MSG_CHECKING([for extra nm flags])
    NM="$NM $nmflags"
    AC_MSG_RESULT([$nmflags])
  fi
fi
])


dnl  GMP_PROG_CC_WORKS(cc+cflags,[ACTION-IF-WORKS][,ACTION-IF-NOT-WORKS])
dnl  --------------------------------------------------------------------
dnl  Check if cc+cflags can compile and link.
dnl
dnl  This test is designed to be run repeatedly with different cc+cflags
dnl  selections, so the result is not cached.
dnl
dnl  For a native build, meaning $cross_compiling == no, we require that the
dnl  generated program will run.  This is the same as AC_PROG_CC does in
dnl  _AC_COMPILER_EXEEXT_WORKS, and checking here will ensure we don't pass
dnl  a CC/CFLAGS combination that it rejects.
dnl
dnl  sparc-*-solaris2.7 can compile ABI=64 but won't run it if the kernel
dnl  was booted in 32-bit mode.  The effect of requiring the compiler output
dnl  will run is that a plain native "./configure" falls back on ABI=32, but
dnl  ABI=64 is still available as a cross-compile.

AC_DEFUN(GMP_PROG_CC_WORKS,
[AC_MSG_CHECKING([compiler $1])
cat >conftest.c <<EOF

/* The following provokes an internal error from gcc 2.95.2 -mpowerpc64
   (without -maix64), hence detecting an unusable compiler */
void *g() { return (void *) 0; }
void *f() { return g(); }

/* The following provokes an invalid instruction syntax from i386 gcc
   -march=pentiumpro on Solaris 2.8.  The native sun assembler
   requires a non-standard syntax for cmov which gcc (as of 2.95.2 at
   least) doesn't know.  */
int n;
int cmov () { return (n >= 0 ? n : 0); }

/* The following provokes a linker invocation problem with gcc 3.0.3
   on AIX 4.3 under "-maix64 -mpowerpc64 -mcpu=630".  The -mcpu=630
   option causes gcc to incorrectly select the 32-bit libgcc.a, not
   the 64-bit one, and consequently it misses out on the __fixunsdfdi
   helper (double -> uint64 conversion).  */
double d;
unsigned long gcc303 () { return (unsigned long) d; }

int main () { return 0; }
EOF
gmp_prog_cc_works=no
gmp_compile="$1 conftest.c >&AC_FD_CC"
if AC_TRY_EVAL(gmp_compile); then
  if test "$cross_compiling" = no; then
    if AC_TRY_COMMAND([./a.out || ./a.exe || ./conftest]); then
      gmp_prog_cc_works=yes
    fi
  else
    gmp_prog_cc_works=yes
  fi
fi
rm -f conftest* a.out a.exe
AC_MSG_RESULT($gmp_prog_cc_works)
if test $gmp_prog_cc_works = yes; then
  ifelse([$2],,:,[$2])
else
  ifelse([$3],,:,[$3])
fi
])


dnl  GMP_PROG_CC_IS_GNU(CC,[ACTIONS-IF-YES][,ACTIONS-IF-NO])
dnl  -------------------------------------------------------
dnl  Determine whether the given compiler is GNU C.
dnl
dnl  This test is the same as autoconf _AC_LANG_COMPILER_GNU, but doesn't
dnl  cache the result.  The same "ifndef" style test is used, to avoid
dnl  problems with syntax checking cpp's used on NeXT and Apple systems.

AC_DEFUN(GMP_PROG_CC_IS_GNU,
[cat >conftest.c <<EOF
#ifndef __GNUC__
  choke me
#endif
EOF
gmp_compile="$1 -c conftest.c >&AC_FD_CC"
if AC_TRY_EVAL(gmp_compile); then
  rm -f conftest*
  AC_MSG_CHECKING([whether $1 is gcc])
  AC_MSG_RESULT(yes)
  ifelse([$2],,:,[$2])
else
  rm -f conftest*
  ifelse([$3],,:,[$3])
fi
])


dnl  GMP_PROG_CC_IS_XLC(CC,[ACTIONS-IF-YES][,ACTIONS-IF-NO])
dnl  -------------------------------------------------------
dnl  Determine whether the given compiler is IBM xlc (on AIX).
dnl
dnl  There doesn't seem to be a preprocessor symbol to test for this, or if
dnl  there is one then it's well hidden in xlc 3.1 on AIX 4.3, so just grep
dnl  the man page printed when xlc is invoked with no arguments.

AC_DEFUN(GMP_PROG_CC_IS_XLC,
[gmp_command="$1 2>&1 | grep xlc >/dev/null"
if AC_TRY_EVAL(gmp_command); then
  AC_MSG_CHECKING([whether $1 is xlc])
  AC_MSG_RESULT(yes)
  ifelse([$2],,:,[$2])
else
  ifelse([$3],,:,[$3])
fi
])


dnl  GMP_HPC_HPPA_2_0(cc,[ACTION-IF-GOOD][,ACTION-IF-BAD])
dnl  ---------------------------------------------------------
dnl  Find out whether a HP compiler is good enough to generate hppa 2.0.
dnl
dnl  This test might be repeated for different compilers, so the result is
dnl  not cached.

AC_DEFUN(GMP_HPC_HPPA_2_0,
[AC_MSG_CHECKING([whether HP compiler $1 is good for 64-bits])
# Bad compiler output:
#   ccom: HP92453-01 G.10.32.05 HP C Compiler
# Good compiler output:
#   ccom: HP92453-01 A.10.32.30 HP C Compiler
# Let A.10.32.30 or higher be ok.
echo >conftest.c
gmp_tmp_vs=`$1 $2 -V -c -o conftest.$OBJEXT conftest.c 2>&1 | grep "^ccom:"`
echo "Version string: $gmp_tmp_vs" >&AC_FD_CC
rm conftest*
gmp_tmp_v1=`echo $gmp_tmp_vs | sed 's/.* .\.\(.*\)\..*\..* HP C.*/\1/'`
gmp_tmp_v2=`echo $gmp_tmp_vs | sed 's/.* .\..*\.\(.*\)\..* HP C.*/\1/'`
gmp_tmp_v3=`echo $gmp_tmp_vs | sed 's/.* .\..*\..*\.\(.*\) HP C.*/\1/'`
echo "Version number: $gmp_tmp_v1.$gmp_tmp_v2.$gmp_tmp_v3" >&AC_FD_CC
if test -z "$gmp_tmp_v1"; then
  gmp_hpc_64bit=not-applicable
else
  GMP_COMPARE_GE($gmp_tmp_v1, 10, $gmp_tmp_v2, 32, $gmp_tmp_v3, 30)
  gmp_hpc_64bit=$gmp_compare_ge
fi
AC_MSG_RESULT($gmp_hpc_64bit)
if test $gmp_hpc_64bit = yes; then
  ifelse([$2],,:,[$2])
else
  ifelse([$3],,:,[$3])
fi
])


dnl  GMP_GCC_VERSION_GE(CC,MAJOR[,MINOR[,SUBMINOR]])
dnl  -----------------------------------------------
dnl  Test whether the version of CC (which must be GNU C) is >=
dnl  MAJOR.MINOR.SUBMINOR.  Set $gmp_compare_ge to "yes" or "no"
dnl  accordingly, or to "error" if the version number string can't be
dnl  parsed.
dnl
dnl  gcc --version is normally just "2.7.2.3" or "2.95.3" or whatever, but
dnl  egcs gives something like "egcs-2.91".  "[a-z-]*" is used to match that
dnl  (Solaris 8 sed doesn't support "?" or "*" of a group, like "\(...\)?"
dnl  or "\(...\)*".)
dnl
dnl  There's no caching here, so that different CC's can be tested.

AC_DEFUN(GMP_GCC_VERSION_GE,
[tmp_version=`($1 --version) 2>&AC_FD_CC`
echo "$1 --version '$tmp_version'" >&AC_FD_CC

major=`(echo "$tmp_version" | sed -n ['s/^[a-z-]*\([0-9][0-9]*\).*/\1/p']) 2>&AC_FD_CC`
echo "    major '$major'" >&AC_FD_CC

ifelse([$3],,,
[minor=`(echo "$tmp_version" | sed -n ['s/^[a-z-]*[0-9][0-9]*\.\([0-9][0-9]*\).*/\1/p']) 2>&AC_FD_CC`
echo "    minor '$minor'" >&AC_FD_CC])

ifelse([$4],,,
[subminor=`(echo "$tmp_version" | sed -n ['s/^[a-z-]*[0-9][0-9]*\.[0-9][0-9]*\.\([0-9][0-9]*\).*/\1/p']) 2>&AC_FD_CC`
echo "    subminor '$subminor'" >&AC_FD_CC])

if test -z "$major"; then
  AC_MSG_WARN([unrecognised gcc version string: $tmp_version])
  gmp_compare_ge=error
else
  ifelse([$3],, [GMP_COMPARE_GE($major, $2)],
  [ifelse([$4],,[GMP_COMPARE_GE($major, $2, $minor, $3)],
                [GMP_COMPARE_GE($major, $2, $minor, $3, $subminor, $4)])])
fi
])


dnl  GMP_GCC_ARM_UMODSI(CC,[ACTIONS-IF-GOOD][,ACTIONS-IF-BAD])
dnl  ---------------------------------------------------------
dnl  gcc 2.95.3 and earlier on arm has a bug in the libgcc __umodsi routine
dnl  making "%" give wrong results for some operands, eg. "0x90000000 % 3".
dnl  We're hoping it'll be fixed in 2.95.4, and we know it'll be fixed in
dnl  gcc 3.
dnl
dnl  There's only a couple of places gmp cares about this, one is the
dnl  size==1 case in mpn/generic/mode1o.c, and this shows up in
dnl  tests/mpz/t-jac.c as a wrong result from mpz_kronecker_ui.

AC_DEFUN(GMP_GCC_ARM_UMODSI,
[AC_MSG_CHECKING([whether gcc unsigned division works])
GMP_GCC_VERSION_GE([$1], 2,95,4)
case $gmp_compare_ge in
yes)
  ifelse([$2],,:,[$2])
  gmp_gcc_arm_umodsi_result=yes ;;
*)
  ifelse([$3],,:,[$3])
  gmp_gcc_arm_umodsi_result="no, gcc <= 2.95.3" ;;
esac
AC_MSG_RESULT([$gmp_gcc_arm_umodsi_result])
])


dnl  GMP_GCC_MARCH_PENTIUMPRO(CC,[ACTIONS-IF-GOOD][,ACTIONS-IF-BAD])
dnl  ---------------------------------------------------------------
dnl  mpz/powm.c swox cvs rev 1.4 tickled a bug in gcc 2.95.2 and 2.95.3 when
dnl  -march=pentiumpro was used.  The bug was wrong handling of the input to
dnl  an ABSIZ(z) expression in mpz_redc().  Fixed in 2.95.4 and pre-release
dnl  3.0, and didn't seem to occur in unofficial 2.96, so test for 2.95.4
dnl  and up.
dnl
dnl  This macro is used only once, after finalizing a choice of CC, so the
dnl  result is cached.

AC_DEFUN(GMP_GCC_MARCH_PENTIUMPRO,
[AC_CACHE_CHECK([whether gcc -march=pentiumpro is good],
                gmp_cv_gcc_march_pentiumpro,
[GMP_GCC_VERSION_GE([$1], 2,95,4)
case $gmp_compare_ge in
yes|no)  gmp_cv_gcc_march_pentiumpro=$gmp_compare_ge ;;
error|*) gmp_cv_gcc_march_pentiumpro=no ;;
esac])
if test $gmp_cv_gcc_march_pentiumpro = yes; then
  ifelse([$2],,:,[$2])
else
  ifelse([$3],,:,[$3])
fi
])


dnl  GMP_GCC_M68K_OPTIMIZE(CCBASE,CC,FLAG-VARIABLE)
dnl  ----------------------------------------------
dnl  m68k gcc 2.95.x gets an internal compiler error when compiling the
dnl  current mpn/generic/gcdext.c (swox cvs rev 1.20) under -O2 or higher,
dnl  so just use -O for the offending gcc versions.  Naturally if gcdext.c
dnl  gets rearranged or rewritten so the ICE doesn't happen then this can be
dnl  removed.

AC_DEFUN(GMP_GCC_M68K_OPTIMIZE,
[case $host in
m68*-*-*)
  if test $1 = gcc; then
    case `$2 --version` in
    2.95*) $3=-O ;;
    esac
  fi
  ;;
esac
])


dnl  GMP_GCC_MIPS_O32(gcc,[actions-yes][,[actions-no]])
dnl  -------------------------------------------------
dnl  Test whether gcc supports o32.
dnl
dnl  gcc 2.7.2.2 only does o32, and doesn't accept -mabi=32.
dnl
dnl  gcc 2.95 accepts -mabi=32 but it only works on irix5, on irix6 it gives
dnl  "cc1: The -mabi=32 support does not work yet".

AC_DEFUN(GMP_GCC_MIPS_O32,
[AC_MSG_CHECKING([whether gcc supports o32])
echo 'int x;' >conftest.c
echo "$1 -mabi=32 -c conftest.c" >&AC_FD_CC
if $1 -mabi=32 -c conftest.c >conftest.out 2>&1; then
  result=yes
else
  cat conftest.out >&AC_FD_CC
  if grep "cc1: Invalid option \`abi=32'" conftest.out >/dev/null; then
    result=yes
  else
    result=no
  fi
fi
rm -f conftest.*
AC_MSG_RESULT($result)
if test $result = yes; then
  ifelse([$2],,:,[$2])
else
  ifelse([$3],,:,[$3])
fi
])


dnl  GMP_GCC_NO_CPP_PRECOMP(CCBASE,CC,CFLAGS,[ACTIONS-YES][,ACTIONS-NO])
dnl  -------------------------------------------------------------------
dnl  Check whether -no-cpp-precomp should be used on this compiler, and
dnl  execute the corresponding ACTIONS-YES or ACTIONS-NO.
dnl
dnl  -no-cpp-precomp is only meant for Apple's hacked version of gcc found
dnl  on powerpc*-*-darwin*, but we can give it a try on any gcc.  Normal gcc
dnl  (as of 3.0 at least) only gives a warning, not an actual error, and we
dnl  watch for that and decide against the option in that case, to avoid
dnl  confusing the user.

AC_DEFUN(GMP_GCC_NO_CPP_PRECOMP,
[if test "$ccbase" = gcc; then
  AC_MSG_CHECKING([compiler $2 $3 -no-cpp-precomp])
  result=no
  cat >conftest.c <<EOF
int main () { return 0; }
EOF
  gmp_compile="$2 $3 -no-cpp-precomp conftest.c >conftest.out 2>&1"
  if AC_TRY_EVAL(gmp_compile); then
    if grep "unrecognized option.*-no-cpp-precomp" conftest.out >/dev/null; then : ;
    else
      result=yes
    fi
  fi
  cat conftest.out >&AC_FD_CC
  rm -f conftest* a.out
  AC_MSG_RESULT($result)
  if test "$result" = yes; then
      ifelse([$4],,:,[$4])
  else
      ifelse([$5],,:,[$5])
  fi
fi
])


dnl  GMP_PROG_CXX_WORKS(cxx/cxxflags [, ACTION-YES [,ACTION-NO]])
dnl  ------------------------------------------------------------
dnl  Check whether cxx/cxxflags can compile and link.
dnl
dnl  This test is designed to be run repeatedly with different cxx/cxxflags
dnl  selections, so the result is not cached.

AC_DEFUN(GMP_PROG_CXX_WORKS,
[AC_MSG_CHECKING([C++ compiler $1])
cat >conftest.cc <<EOF
#include <iostream>
using namespace std;
int
main (void)
{
  cout.setf (ios::hex);
  cout << 123;
  return 0;
}
EOF

gmp_cxxcompile="$1 conftest.cc -o conftest >&AC_FD_CC"
if AC_TRY_EVAL(gmp_cxxcompile); then
  rm -f conftest*
  AC_MSG_RESULT(yes)
  ifelse([$2],,:,[$2])
else
  rm -f conftest*
  AC_MSG_RESULT(no)
  ifelse([$3],,:,[$3])
fi
])


dnl  GMP_INIT([M4-DEF-FILE])
dnl  -----------------------
dnl  Initializations for GMP config.m4 generation.
dnl
dnl  FIXME: The generated config.m4 doesn't get recreated by config.status.
dnl  Maybe the relevant "echo"s should go through AC_CONFIG_COMMANDS.

AC_DEFUN(GMP_INIT,
[ifelse([$1], , gmp_configm4=config.m4, gmp_configm4="[$1]")
gmp_tmpconfigm4=cnfm4.tmp
gmp_tmpconfigm4i=cnfm4i.tmp
gmp_tmpconfigm4p=cnfm4p.tmp
rm -f $gmp_tmpconfigm4 $gmp_tmpconfigm4i $gmp_tmpconfigm4p

# CONFIG_TOP_SRCDIR is a path from the mpn builddir to the top srcdir.
# The pattern here tests for an absolute path the same way as
# _AC_OUTPUT_FILES in autoconf acgeneral.m4.
case $srcdir in
[[\\/]]* | ?:[[\\/]]* )  tmp="$srcdir"    ;;
*)                       tmp="../$srcdir" ;;
esac
echo ["define(<CONFIG_TOP_SRCDIR>,<\`$tmp'>)"] >>$gmp_tmpconfigm4

# All CPUs use asm-defs.m4 
echo ["include(CONFIG_TOP_SRCDIR\`/mpn/asm-defs.m4')"] >>$gmp_tmpconfigm4i
])


dnl  GMP_FINISH
dnl  ----------
dnl  Create config.m4 from its accumulated parts.
dnl
dnl  __CONFIG_M4_INCLUDED__ is used so that a second or subsequent include
dnl  of config.m4 is harmless.
dnl
dnl  A separate ifdef on the angle bracket quoted part ensures the quoting
dnl  style there is respected.  The basic defines from gmp_tmpconfigm4 are
dnl  fully quoted but are still put under an ifdef in case any have been
dnl  redefined by one of the m4 include files.
dnl
dnl  Doing a big ifdef within asm-defs.m4 and/or other macro files wouldn't
dnl  work, since it'd interpret parentheses and quotes in dnl comments, and
dnl  having a whole file as a macro argument would overflow the string space
dnl  on BSD m4.

AC_DEFUN(GMP_FINISH,
[AC_REQUIRE([GMP_INIT])
echo "creating $gmp_configm4"
echo ["d""nl $gmp_configm4.  Generated automatically by configure."] > $gmp_configm4
if test -f $gmp_tmpconfigm4; then
  echo ["changequote(<,>)"] >> $gmp_configm4
  echo ["ifdef(<__CONFIG_M4_INCLUDED__>,,<"] >> $gmp_configm4
  cat $gmp_tmpconfigm4 >> $gmp_configm4
  echo [">)"] >> $gmp_configm4
  echo ["changequote(\`,')"] >> $gmp_configm4
  rm $gmp_tmpconfigm4
fi
echo ["ifdef(\`__CONFIG_M4_INCLUDED__',,\`"] >> $gmp_configm4
if test -f $gmp_tmpconfigm4i; then
  cat $gmp_tmpconfigm4i >> $gmp_configm4
  rm $gmp_tmpconfigm4i
fi
if test -f $gmp_tmpconfigm4p; then
  cat $gmp_tmpconfigm4p >> $gmp_configm4
  rm $gmp_tmpconfigm4p
fi
echo ["')"] >> $gmp_configm4
echo ["define(\`__CONFIG_M4_INCLUDED__')"] >> $gmp_configm4
])


dnl  GMP_INCLUDE_MPN(FILE)
dnl  ---------------------
dnl  Add an include_mpn(`FILE') to config.m4.  FILE should be a path
dnl  relative to the mpn source directory, for example
dnl
dnl      GMP_INCLUDE_MPN(`x86/x86-defs.m4')
dnl

AC_DEFUN(GMP_INCLUDE_MPN,
[AC_REQUIRE([GMP_INIT])
echo ["include_mpn(\`$1')"] >> $gmp_tmpconfigm4i
])


dnl  GMP_DEFINE(MACRO, DEFINITION [, LOCATION])
dnl  ------------------------------------------
dnl  Define M4 macro MACRO as DEFINITION in temporary file.
dnl
dnl  If LOCATION is `POST', the definition will appear after any include()
dnl  directives inserted by GMP_INCLUDE.  Mind the quoting!  No shell
dnl  variables will get expanded.  Don't forget to invoke GMP_FINISH to
dnl  create file config.m4.  config.m4 uses `<' and '>' as quote characters
dnl  for all defines.

AC_DEFUN(GMP_DEFINE, 
[AC_REQUIRE([GMP_INIT])
echo ['define(<$1>, <$2>)'] >>ifelse([$3], [POST],
                              $gmp_tmpconfigm4p, $gmp_tmpconfigm4)
])


dnl  GMP_DEFINE_RAW(STRING, [, LOCATION])
dnl  ------------------------------------
dnl  Put STRING into config.m4 file.
dnl
dnl  If LOCATION is `POST', the definition will appear after any include()
dnl  directives inserted by GMP_INCLUDE.  Don't forget to invoke GMP_FINISH
dnl  to create file config.m4.

AC_DEFUN(GMP_DEFINE_RAW,
[AC_REQUIRE([GMP_INIT])
echo [$1] >> ifelse([$2], [POST], $gmp_tmpconfigm4p, $gmp_tmpconfigm4)
])


dnl  GMP_TRY_ASSEMBLE(asm-code,[action-success][,action-fail])
dnl  ----------------------------------------------------------
dnl  Attempt to assemble the given code.
dnl  Do "action-success" if this succeeds, "action-fail" if not.
dnl
dnl  conftest.o and conftest.out are available for inspection in
dnl  "action-success".  If either action does a "break" out of a loop then
dnl  an explicit "rm -f conftest*" will be necessary.
dnl
dnl  This is not unlike AC_TRY_COMPILE, but there's no default includes or
dnl  anything in "asm-code", everything wanted must be given explicitly.

AC_DEFUN(GMP_TRY_ASSEMBLE,
[cat >conftest.s <<EOF
[$1]
EOF
gmp_assemble="$CCAS $CFLAGS conftest.s >conftest.out 2>&1"
if AC_TRY_EVAL(gmp_assemble); then
  cat conftest.out >&AC_FD_CC
  ifelse([$2],,:,[$2])
else
  cat conftest.out >&AC_FD_CC
  echo "configure: failed program was:" >&AC_FD_CC
  cat conftest.s >&AC_FD_CC
  ifelse([$3],,:,[$3])
fi
rm -f conftest*
])


dnl  GMP_ASM_LABEL_SUFFIX
dnl  --------------------
dnl  Should a label have a colon or not?

AC_DEFUN(GMP_ASM_LABEL_SUFFIX,
[AC_CACHE_CHECK([what assembly label suffix to use],
                gmp_cv_asm_label_suffix,
[case $host in 
  # Empty is only for the HP-UX hppa assembler; hppa gas requires a colon.
  *-*-hpux*) gmp_cv_asm_label_suffix=  ;;
  *)         gmp_cv_asm_label_suffix=: ;;
esac
])
echo ["define(<LABEL_SUFFIX>, <\$][1$gmp_cv_asm_label_suffix>)"] >> $gmp_tmpconfigm4
])


dnl  GMP_ASM_UNDERSCORE
dnl  ------------------
dnl  Determine whether global symbols need to be prefixed with an underscore.
dnl  A test program is linked to an assembler module with or without an
dnl  underscore to see which works.
dnl
dnl  This method should be more reliable than grepping a .o file or using
dnl  nm, since it corresponds to what a real program is going to do.  Note
dnl  in particular that grepping doesn't work with SunOS 4 native grep since
dnl  that grep seems to have trouble with '\0's in files.

AC_DEFUN(GMP_ASM_UNDERSCORE,
[AC_REQUIRE([GMP_ASM_TEXT])
AC_REQUIRE([GMP_ASM_GLOBL])
AC_REQUIRE([GMP_ASM_LABEL_SUFFIX])
AC_CACHE_CHECK([if globals are prefixed by underscore], 
               gmp_cv_asm_underscore,
[cat >conftes1.c <<EOF
#ifdef __cplusplus
extern "C" { void underscore_test(); }
#endif
main () { underscore_test(); }
EOF
for tmp_underscore in "" "_"; do
  cat >conftes2.s <<EOF
      	$gmp_cv_asm_text
	$gmp_cv_asm_globl ${tmp_underscore}underscore_test
${tmp_underscore}underscore_test$gmp_cv_asm_label_suffix
EOF
  case $host in
  *-*-aix*)
    cat >>conftes2.s <<EOF
	$gmp_cv_asm_globl .${tmp_underscore}underscore_test
.${tmp_underscore}underscore_test$gmp_cv_asm_label_suffix
EOF
    ;;
  esac
  gmp_compile="$CC $CFLAGS $CPPFLAGS -c conftes1.c >&AC_FD_CC && $CCAS $CFLAGS conftes2.s >&AC_FD_CC && $CC $CFLAGS conftes1.$OBJEXT conftes2.$OBJEXT >&AC_FD_CC"
  if AC_TRY_EVAL(gmp_compile); then
    eval tmp_result$tmp_underscore=yes
  else
    eval tmp_result$tmp_underscore=no
  fi
done

if test $tmp_result_ = yes; then
  if test $tmp_result = yes; then
    AC_MSG_ERROR([Test program unexpectedly links both with and without underscore.])
  else
    gmp_cv_asm_underscore=yes
  fi
else
  if test $tmp_result = yes; then
    gmp_cv_asm_underscore=no
  else
    AC_MSG_ERROR([Test program links neither with nor without underscore.])
  fi
fi
rm -f conftes1* conftes2* a.out
])
if test "$gmp_cv_asm_underscore" = "yes"; then
  GMP_DEFINE(GSYM_PREFIX, [_])
else
  GMP_DEFINE(GSYM_PREFIX, [])
fi    
])


dnl  GMP_ASM_ALIGN_LOG
dnl  -----------------
dnl  Is parameter to `.align' logarithmic?

AC_DEFUN(GMP_ASM_ALIGN_LOG,
[AC_REQUIRE([GMP_ASM_GLOBL])
AC_REQUIRE([GMP_ASM_DATA])
AC_REQUIRE([GMP_ASM_LABEL_SUFFIX])
AC_REQUIRE([GMP_PROG_NM])
AC_CACHE_CHECK([if .align assembly directive is logarithmic],
               gmp_cv_asm_align_log,
[GMP_TRY_ASSEMBLE(
[      	$gmp_cv_asm_data
      	.align  4
	$gmp_cv_asm_globl	foo
	.byte	1
	.align	4
foo$gmp_cv_asm_label_suffix
	.byte	2],
  [gmp_tmp_val=[`$NM conftest.$OBJEXT | grep foo | \
     sed -e 's;[[][0-9][]]\(.*\);\1;' -e 's;[^1-9]*\([0-9]*\).*;\1;'`]
  if test "$gmp_tmp_val" = "10" || test "$gmp_tmp_val" = "16"; then
    gmp_cv_asm_align_log=yes
  else
    gmp_cv_asm_align_log=no
  fi],
  [AC_MSG_ERROR([cannot assemble alignment test])])])

GMP_DEFINE_RAW(["define(<ALIGN_LOGARITHMIC>,<$gmp_cv_asm_align_log>)"])
])


dnl  GMP_ASM_ALIGN_FILL_0x90
dnl  -----------------------
dnl  Determine whether a ",0x90" suffix works on a .align directive.
dnl  This is only meant for use on x86, 0x90 being a "nop".
dnl
dnl  Old gas, eg. 1.92.3
dnl       Needs ",0x90" or else the fill is 0x00, which can't be executed
dnl       across.
dnl
dnl  New gas, eg. 2.91
dnl       Generates multi-byte nop fills even when ",0x90" is given.
dnl
dnl  Solaris 2.6 as
dnl       ",0x90" is not allowed, causes a fatal error.
dnl
dnl  Solaris 2.8 as
dnl       ",0x90" does nothing, generates a warning that it's being ignored.
dnl
dnl  SCO OpenServer 5 as
dnl       Second parameter is max bytes to fill, not a fill pattern.
dnl       ",0x90" is an error due to being bigger than the first parameter.
dnl       Multi-byte nop fills are generated in text segments.
dnl
dnl  Note that both solaris "as"s only care about ",0x90" if they actually
dnl  have to use it to fill something, hence the .byte in the test.  It's
dnl  the second .align which provokes the error or warning.
dnl
dnl  The warning from solaris 2.8 is supressed to stop anyone worrying that
dnl  something might be wrong.

AC_DEFUN(GMP_ASM_ALIGN_FILL_0x90,
[AC_REQUIRE([GMP_ASM_TEXT])
AC_CACHE_CHECK([if the .align directive accepts an 0x90 fill in .text],
               gmp_cv_asm_align_fill_0x90,
[GMP_TRY_ASSEMBLE(
[      	$gmp_cv_asm_text
      	.align  4, 0x90
	.byte   0
      	.align  4, 0x90],
[if grep "Warning: Fill parameter ignored for executable section" conftest.out >/dev/null; then
  echo "Supressing this warning by omitting 0x90" 1>&AC_FD_CC
  gmp_cv_asm_align_fill_0x90=no
else
  gmp_cv_asm_align_fill_0x90=yes
fi],
[gmp_cv_asm_align_fill_0x90=no])])

GMP_DEFINE_RAW(["define(<ALIGN_FILL_0x90>,<$gmp_cv_asm_align_fill_0x90>)"])
])


dnl  GMP_ASM_TEXT
dnl  ------------

AC_DEFUN(GMP_ASM_TEXT,
[AC_CACHE_CHECK([how to switch to text section],
                gmp_cv_asm_text,
[case $host in
  *-*-aix*)  gmp_cv_asm_text=[".csect .text[PR]"] ;;
  *-*-hpux*) gmp_cv_asm_text=".code" ;;
  *)         gmp_cv_asm_text=".text" ;;
esac
])
echo ["define(<TEXT>, <$gmp_cv_asm_text>)"] >> $gmp_tmpconfigm4
])


dnl  GMP_ASM_DATA
dnl  ------------
dnl  Can we say `.data'?

AC_DEFUN(GMP_ASM_DATA,
[AC_CACHE_CHECK([how to switch to data section],
                gmp_cv_asm_data,
[case $host in
  *-*-aix*) gmp_cv_asm_data=[".csect .data[RW]"] ;;
  *)        gmp_cv_asm_data=".data" ;;
esac
])
echo ["define(<DATA>, <$gmp_cv_asm_data>)"] >> $gmp_tmpconfigm4
])


dnl  GMP_ASM_RODATA
dnl  --------------
dnl  Find out how to switch to the read-only data section.
dnl
dnl  The compiler output is grepped for the right directive.  It's not
dnl  considered wise to just probe for ".section .rodata" or whatever works,
dnl  since arbitrary section names might be accepted, but not necessarily do
dnl  the right thing when they get to the linker.
dnl
dnl  Only a few asm files use RODATA, so this code is perhaps a bit
dnl  excessive right now, but should find more uses in the future.
dnl
dnl  FIXME: gcc on aix generates something like ".csect _foo.ro_c[RO],3"
dnl  where foo is the object file.  Might need to check for that if we use
dnl  RODATA there.

AC_DEFUN(GMP_ASM_RODATA,
[AC_REQUIRE([GMP_ASM_TEXT])
AC_REQUIRE([GMP_ASM_DATA])
AC_REQUIRE([GMP_ASM_LABEL_SUFFIX])
AC_REQUIRE([GMP_ASM_UNDERSCORE])
AC_CACHE_CHECK([how to switch to read-only data section],
               gmp_cv_asm_rodata,
[
dnl Default to DATA on CPUs with split code/data caching, and TEXT
dnl elsewhere.  i386 means generic x86, so use DATA on it.
case $host in
X86_PATTERN) gmp_cv_asm_rodata="$gmp_cv_asm_data" ;;
*)           gmp_cv_asm_rodata="$gmp_cv_asm_text" ;;
esac

cat >conftest.c <<EOF
const int foo = 123;
EOF
echo "Test program:" >&AC_FD_CC
cat conftest.c >&AC_FD_CC
gmp_compile="$CC $CFLAGS $CPPFLAGS -S conftest.c >&AC_FD_CC"
if AC_TRY_EVAL(gmp_compile); then
  echo "Compiler output:" >&AC_FD_CC
  cat conftest.s >&AC_FD_CC
  if test $gmp_cv_asm_underscore = yes; then
    tmp_gsym_prefix=_
  else
    tmp_gsym_prefix=
  fi
  # must see our label
  if grep "^${tmp_gsym_prefix}foo$gmp_cv_asm_label_suffix" conftest.s >/dev/null 2>&AC_FD_CC; then
    # take the last directive before our label (hence skipping segments
    # getting debugging info etc)
    tmp_match=`sed -n ["/^${tmp_gsym_prefix}foo$gmp_cv_asm_label_suffix/q
                        /^[. 	]*data/p
                        /^[. 	]*rdata/p
                        /^[. 	]*text/p
                        /^[. 	]*section/p
                        /^[. 	]*csect/p
                        /^[. 	]*CSECT/p"] conftest.s | sed -n '$p'`
    echo "Match: $tmp_match" >&AC_FD_CC
    if test -n "$tmp_match"; then
      gmp_cv_asm_rodata=$tmp_match
    fi
  else
    echo "Couldn't find label: ^${tmp_gsym_prefix}foo$gmp_cv_asm_label_suffix" >&AC_FD_CC
  fi
fi
])
echo ["define(<RODATA>, <$gmp_cv_asm_rodata>)"] >> $gmp_tmpconfigm4
])


dnl  GMP_ASM_GLOBL
dnl  -------------
dnl  Can we say `.global'?

AC_DEFUN(GMP_ASM_GLOBL,
[AC_CACHE_CHECK([how to export a symbol],
                gmp_cv_asm_globl,
[case $host in
  *-*-hpux*) gmp_cv_asm_globl=".export" ;;
  *)         gmp_cv_asm_globl=".globl" ;;
esac
])
echo ["define(<GLOBL>, <$gmp_cv_asm_globl>)"] >> $gmp_tmpconfigm4
])


dnl  GMP_ASM_GLOBL_ATTR
dnl  ------------------
dnl  Do we need something after `.global symbol'?

AC_DEFUN(GMP_ASM_GLOBL_ATTR,
[AC_CACHE_CHECK([if the export directive needs an attribute],
                gmp_cv_asm_globl_attr,
[case $host in
  *-*-hpux*) gmp_cv_asm_globl_attr=",entry" ;;
  *)         gmp_cv_asm_globl_attr="" ;;
esac
])
echo ["define(<GLOBL_ATTR>, <$gmp_cv_asm_globl_attr>)"] >> $gmp_tmpconfigm4
])


dnl  GMP_ASM_TYPE
dnl  ------------
dnl  Can we say ".type", and how?
dnl
dnl  For i386 GNU/Linux ELF systems, and very likely other ELF systems,
dnl  .type and .size are important on functions in shared libraries.  If
dnl  .type is omitted and the mainline program references that function then
dnl  the code will be copied down to the mainline at load time like a piece
dnl  of data.  If .size is wrong or missing (it defaults to 4 bytes or some
dnl  such) then incorrect bytes will be copied and a segv is the most likely
dnl  result.  In any case such copying is not what's wanted, a .type
dnl  directive will ensure a PLT entry is used.
dnl
dnl  In GMP the assembler functions are normally only used from within the
dnl  library (since most programs are not interested in the low level
dnl  routines), and in those circumstances a missing .type isn't fatal,
dnl  letting the problem go unnoticed.  tests/mpn/t-asmtype.c aims to check
dnl  for it.

AC_DEFUN(GMP_ASM_TYPE,
[AC_CACHE_CHECK([for assembler .type directive],
                gmp_cv_asm_type,
[gmp_cv_asm_type=
for gmp_tmp_prefix in @ \# %; do
  GMP_TRY_ASSEMBLE([	.type	sym,${gmp_tmp_prefix}function],
    [if grep "\.type pseudo-op used outside of \.def/\.endef ignored" conftest.out >/dev/null; then : ;
    else
      gmp_cv_asm_type=".type	\$][1,${gmp_tmp_prefix}\$][2"
      break
    fi])
done
rm -f conftest*
])
echo ["define(<TYPE>, <$gmp_cv_asm_type>)"] >> $gmp_tmpconfigm4
])


dnl  GMP_ASM_SIZE
dnl  ------------
dnl  Can we say `.size'?

AC_DEFUN(GMP_ASM_SIZE,
[AC_CACHE_CHECK([for assembler .size directive],
                gmp_cv_asm_size,
[gmp_cv_asm_size=
GMP_TRY_ASSEMBLE([	.size	sym,1],
  [if grep "\.size pseudo-op used outside of \.def/\.endef ignored" conftest.out >/dev/null; then : ;
  else
    gmp_cv_asm_size=".size	\$][1,\$][2"
  fi])
])
echo ["define(<SIZE>, <$gmp_cv_asm_size>)"] >> $gmp_tmpconfigm4
])


dnl  GMP_ASM_LSYM_PREFIX
dnl  -------------------
dnl  What is the prefix for a local label?
dnl
dnl  The prefixes tested are,
dnl
dnl      L  - usual for underscore systems
dnl      .L - usual for non-underscore systems
dnl      $  - alpha (gas and OSF system assembler)
dnl      L$ - hppa (gas and HP-UX system assembler)
dnl
dnl  The default is "L" if the tests fail for any reason.  There's a good
dnl  chance this will be adequate, since on most systems labels are local
dnl  anyway unless given a ".globl", and an "L" will avoid clashes with
dnl  other identifers.
dnl
dnl  For gas, ".L" is normally purely local to the assembler, it doesn't get
dnl  put into the object file at all.  This style is preferred, to keep the
dnl  object files nice and clean.
dnl
dnl  BSD format nm produces a line like the following.  The lower case "t"
dnl  indicates a local text segment label.  On OSF with "nm -B", an "N" is
dnl  printed instead.
dnl
dnl      00000000 t Lgurkmacka
dnl
dnl  HP-UX nm prints an error message (though seems to give a 0 exit) if
dnl  there's no symbols at all in an object file, hence the use of "dummy".

AC_DEFUN(GMP_ASM_LSYM_PREFIX,
[AC_REQUIRE([GMP_ASM_LABEL_SUFFIX])
AC_REQUIRE([GMP_PROG_NM])
AC_CACHE_CHECK([what prefix to use for a local label], 
               gmp_cv_asm_lsym_prefix,
[for gmp_tmp_pre in L .L $ L$; do
  echo "Trying $gmp_tmp_pre" >&AC_FD_CC
  GMP_TRY_ASSEMBLE(
[dummy${gmp_cv_asm_label_suffix}
${gmp_tmp_pre}gurkmacka${gmp_cv_asm_label_suffix}],
  [if $NM conftest.$OBJEXT >conftest.nm 2>&AC_FD_CC; then : ; else
    cat conftest.nm >&AC_FD_CC
    AC_MSG_WARN(["$NM" failure])
    break
  fi
  cat conftest.nm >&AC_FD_CC
  if grep gurkmacka conftest.nm >/dev/null; then : ; else
    # no mention of the symbol, this is good
    gmp_cv_asm_lsym_prefix="$gmp_tmp_pre"
    break
  fi
  if grep [' [Nt] .*gurkmacka'] conftest.nm >/dev/null; then
    # symbol mentioned as a local, use this if nothing better
    if test -z "$gmp_cv_asm_lsym_prefix"; then
      gmp_cv_asm_lsym_prefix="$gmp_tmp_pre"
    fi
  fi
  ])
done
rm -f conftest*
if test -z "$gmp_cv_asm_lsym_prefix"; then
  gmp_cv_asm_lsym_prefix=L
  AC_MSG_WARN([cannot determine local label, using default $gmp_cv_asm_lsym_prefix])
fi
])
echo ["define(<LSYM_PREFIX>, <${gmp_cv_asm_lsym_prefix}>)"] >> $gmp_tmpconfigm4
AC_DEFINE_UNQUOTED(LSYM_PREFIX, "$gmp_cv_asm_lsym_prefix",
                   [Assembler local label prefix])
])


dnl  GMP_ASM_W32
dnl  -----------
dnl  How to define a 32-bit word.

AC_DEFUN(GMP_ASM_W32,
[AC_REQUIRE([GMP_ASM_DATA])
AC_REQUIRE([GMP_ASM_GLOBL])
AC_REQUIRE([GMP_ASM_LABEL_SUFFIX])
AC_REQUIRE([GMP_PROG_NM])
AC_CACHE_CHECK([how to define a 32-bit word],
	       gmp_cv_asm_w32,
[case $host in 
  *-*-hpux*)
    # FIXME: HPUX puts first symbol at 0x40000000, breaking our assumption
    # that it's at 0x0.  We'll have to declare another symbol before the
    # .long/.word and look at the distance between the two symbols.  The
    # only problem is that the sed expression(s) barfs (on Solaris, for
    # example) for the symbol with value 0.  For now, HPUX uses .word.
    gmp_cv_asm_w32=".word"
    ;;
  *-*-*)
    gmp_tmp_val=
    for gmp_tmp_op in .long .word; do
      GMP_TRY_ASSEMBLE(
[	$gmp_cv_asm_data
	$gmp_cv_asm_globl	foo
	$gmp_tmp_op	0
foo$gmp_cv_asm_label_suffix
	.byte	0],
        [gmp_tmp_val=[`$NM conftest.$OBJEXT | grep foo | \
          sed -e 's;[[][0-9][]]\(.*\);\1;' -e 's;[^1-9]*\([0-9]*\).*;\1;'`]
        if test "$gmp_tmp_val" = 4; then
          gmp_cv_asm_w32="$gmp_tmp_op"
          break
        fi])
    done
    rm -f conftest*
    ;;
esac
if test -z "$gmp_cv_asm_w32"; then
  AC_MSG_ERROR([cannot determine how to define a 32-bit word])
fi
])
echo ["define(<W32>, <$gmp_cv_asm_w32>)"] >> $gmp_tmpconfigm4
])


dnl  GMP_ASM_X86_MMX([ACTION-IF-YES][,ACTION-IF-NO])
dnl  -----------------------------------------------
dnl  Determine whether the assembler supports MMX instructions.
dnl
dnl  This macro is wanted before GMP_ASM_TEXT, so ".text" is hard coded
dnl  here.  ".text" is believed to be correct on all x86 systems, certainly
dnl  it's all GMP_ASM_TEXT gives currently.  Actually ".text" probably isn't
dnl  needed at all, at least for just checking instruction syntax.
dnl
dnl  "movq %mm0, %mm1" should assemble to "0f 6f c8", but Solaris 2.6 and
dnl  2.7 wrongly assemble it to "0f 6f c1" (that being the reverse "movq
dnl  %mm1, %mm0").  It seems more trouble than it's worth to work around
dnl  this in the code, so just detect and reject.

AC_DEFUN(GMP_ASM_X86_MMX,
[AC_CACHE_CHECK([if the assembler knows about MMX instructions],
		gmp_cv_asm_x86_mmx,
[GMP_TRY_ASSEMBLE(
[	.text
	movq	%mm0, %mm1],
[gmp_cv_asm_x86_mmx=yes
case $host in
*-*-solaris*)
  if (dis conftest.$OBJEXT >conftest.out) 2>/dev/null; then
    if grep "0f 6f c1" conftest.out >/dev/null; then
      gmp_cv_asm_x86_mmx=movq-bug
    fi
  else
    AC_MSG_WARN(["dis" not available to check for "as" movq bug])
  fi
esac],
[gmp_cv_asm_x86_mmx=no])])

case $gmp_cv_asm_x86_mmx in
movq-bug)
  AC_MSG_WARN([+----------------------------------------------------------])
  AC_MSG_WARN([| WARNING WARNING WARNING])
  AC_MSG_WARN([| Host CPU has MMX code, but the assembler])
  AC_MSG_WARN([|     $CCAS $CFLAGS])
  AC_MSG_WARN([| has the Solaris 2.6 and 2.7 bug where register to register])
  AC_MSG_WARN([| movq operands are reversed.])
  AC_MSG_WARN([| Non-MMX replacements will be used.])
  AC_MSG_WARN([| This will be an inferior build.])
  AC_MSG_WARN([+----------------------------------------------------------])
  ;;
no)
  AC_MSG_WARN([+----------------------------------------------------------])
  AC_MSG_WARN([| WARNING WARNING WARNING])
  AC_MSG_WARN([| Host CPU has MMX code, but it can't be assembled by])
  AC_MSG_WARN([|     $CCAS $CFLAGS])
  AC_MSG_WARN([| Non-MMX replacements will be used.])
  AC_MSG_WARN([| This will be an inferior build.])
  AC_MSG_WARN([+----------------------------------------------------------])
  ;;
esac
if test "$gmp_cv_asm_x86_mmx" = yes; then
  ifelse([$1],,:,[$1])
else
  ifelse([$2],,:,[$2])
fi
])


dnl  GMP_ASM_X86_SHLDL_CL
dnl  --------------------

AC_DEFUN(GMP_ASM_X86_SHLDL_CL,
[AC_REQUIRE([GMP_ASM_TEXT])
AC_CACHE_CHECK([if the assembler takes cl with shldl],
		gmp_cv_asm_x86_shldl_cl,
[GMP_TRY_ASSEMBLE(
[	$gmp_cv_asm_text
	shldl	%cl, %eax, %ebx],
  gmp_cv_asm_x86_shldl_cl=yes,
  gmp_cv_asm_x86_shldl_cl=no)
])
if test "$gmp_cv_asm_x86_shldl_cl" = "yes"; then
  GMP_DEFINE(WANT_SHLDL_CL,1)
else
  GMP_DEFINE(WANT_SHLDL_CL,0)
fi
])


dnl  GMP_ASM_X86_SSE2([ACTION-IF-YES][,ACTION-IF-NO])
dnl  ------------------------------------------------
dnl  Determine whether the assembler supports SSE2 instructions.
dnl
dnl  This macro is wanted before GMP_ASM_TEXT, so ".text" is hard coded
dnl  here.  ".text" is believed to be correct on all x86 systems, certainly
dnl  it's all GMP_ASM_TEXT gives currently.  Actually ".text" probably isn't
dnl  needed at all, at least for just checking instruction syntax.

AC_DEFUN(GMP_ASM_X86_SSE2,
[AC_CACHE_CHECK([if the assembler knows about SSE2 instructions],
		gmp_cv_asm_x86_sse2,
[GMP_TRY_ASSEMBLE(
[	.text
	paddq	%mm0, %mm1],
  [gmp_cv_asm_x86_sse2=yes],
  [gmp_cv_asm_x86_sse2=no])
])
case $gmp_cv_asm_x86_sse2 in
yes)
  ifelse([$1],,:,[$1])
  ;;
*)
  AC_MSG_WARN([+----------------------------------------------------------])
  AC_MSG_WARN([| WARNING WARNING WARNING])
  AC_MSG_WARN([| Host CPU has SSE2 code, but it can't be assembled by])
  AC_MSG_WARN([|     $CCAS $CFLAGS])
  AC_MSG_WARN([| Non-SSE2 replacements will be used.])
  AC_MSG_WARN([| This will be an inferior build.])
  AC_MSG_WARN([+----------------------------------------------------------])
  ifelse([$2],,:,[$2])
  ;;
esac
])


dnl  GMP_ASM_X86_MCOUNT
dnl  ------------------
dnl  Find out how to call mcount for profiling on an x86 system.
dnl
dnl  A dummy function is compiled and the ".s" output examined.  The pattern
dnl  matching might be a bit fragile, but should work at least with gcc on
dnl  sensible systems.  Certainly it's better than hard coding a table of
dnl  conventions.
dnl
dnl  For non-PIC, any ".data" is taken to mean a counter might be passed.
dnl  It's assumed a movl will set it up, and the right register is taken
dnl  from that movl.  Any movl involving %esp is ignored (a frame pointer
dnl  setup normally).
dnl
dnl  For PIC, any ".data" is similarly interpreted, but a GOTOFF identifies
dnl  the line setting up the right register.
dnl
dnl  In both cases a line with "mcount" identifies the call and that line is
dnl  used literally.
dnl
dnl  On some systems (eg. FreeBSD 3.5) gcc emits ".data" but doesn't use it,
dnl  so it's not an error to have .data but then not find a register.
dnl
dnl  Variations in mcount conventions on different x86 systems can be found
dnl  in gcc config/i386.  mcount can have a "_" prefix or be .mcount or
dnl  _mcount_ptr, and for PIC it can be called through a GOT entry, or via
dnl  the PLT.  If a pointer to a counter is required it's passed in %eax or
dnl  %edx.
dnl
dnl  Flags to specify PIC are taken from $lt_prog_compiler_pic set by
dnl  AC_PROG_LIBTOOL.
dnl
dnl  Enhancement: Cache the values determined here. But what's the right way
dnl  to get two variables (mcount_nonpic_reg and mcount_nonpic_call say) set
dnl  from one block of commands?

AC_DEFUN(GMP_ASM_X86_MCOUNT,
[AC_REQUIRE([AC_ENABLE_SHARED])
AC_REQUIRE([AC_PROG_LIBTOOL])
AC_MSG_CHECKING([how to call x86 mcount])
cat >conftest.c <<EOF
foo(){bar();}
EOF

if test "$enable_static" = yes; then
  gmp_asmout_compile="$CC $CFLAGS $CPPFLAGS -S conftest.c 1>&AC_FD_CC"
  if AC_TRY_EVAL(gmp_asmout_compile); then
    if grep '\.data' conftest.s >/dev/null; then
      mcount_nonpic_reg=`sed -n ['/esp/!s/.*movl.*,\(%[a-z]*\).*$/\1/p'] conftest.s`
    else
      mcount_nonpic_reg=
    fi
    mcount_nonpic_call=`grep 'call.*mcount' conftest.s`
    if test -z "$mcount_nonpic_call"; then
      AC_MSG_ERROR([Cannot find mcount call for non-PIC])
    fi
  else
    AC_MSG_ERROR([Cannot compile test program for non-PIC])
  fi
fi

if test "$enable_shared" = yes; then
  gmp_asmout_compile="$CC $CFLAGS $CPPFLAGS $lt_prog_compiler_pic -S conftest.c 1>&AC_FD_CC"
  if AC_TRY_EVAL(gmp_asmout_compile); then
    if grep '\.data' conftest.s >/dev/null; then
      case $lt_prog_compiler_pic in
        *-DDLL_EXPORT*)
          # Windows DLLs have non-PIC style mcount
          mcount_pic_reg=`sed -n ['/esp/!s/.*movl.*,\(%[a-z]*\).*$/\1/p'] conftest.s`
          ;;
        *)
          mcount_pic_reg=`sed -n ['s/.*GOTOFF.*,\(%[a-z]*\).*$/\1/p'] conftest.s`
          ;;
      esac
    else
      mcount_pic_reg=
    fi
    mcount_pic_call=`grep 'call.*mcount' conftest.s`
    if test -z "$mcount_pic_call"; then
      AC_MSG_ERROR([Cannot find mcount call for PIC])
    fi
  else
    AC_MSG_ERROR([Cannot compile test program for PIC])
  fi
fi

GMP_DEFINE_RAW(["define(<MCOUNT_NONPIC_REG>, <\`$mcount_nonpic_reg'>)"])
GMP_DEFINE_RAW(["define(<MCOUNT_NONPIC_CALL>,<\`$mcount_nonpic_call'>)"])
GMP_DEFINE_RAW(["define(<MCOUNT_PIC_REG>,    <\`$mcount_pic_reg'>)"])
GMP_DEFINE_RAW(["define(<MCOUNT_PIC_CALL>,   <\`$mcount_pic_call'>)"])

rm -f conftest.*
AC_MSG_RESULT([determined])
])


dnl  GMP_ASM_M68K_INSTRUCTION
dnl  ------------------------
dnl  Not sure if ".l" and "%" are independent settings, but it doesn't hurt
dnl  to try all four possibilities.  Note that the % ones must be first, so
dnl  "d0" won't be interpreted as a label.
dnl
dnl  gas 1.92.3 on NetBSD 1.4 needs to be tested with a two operand
dnl  instruction.  It takes registers without "%", but a single operand
dnl  "clrl %d0" only gives a warning, not an error.

AC_DEFUN(GMP_ASM_M68K_INSTRUCTION,
[AC_REQUIRE([GMP_ASM_TEXT])
AC_CACHE_CHECK([assembler instruction and register style],
		gmp_cv_asm_m68k_instruction,
[gmp_cv_asm_m68k_instruction=unknown
for i in "addl %d0,%d1" "add.l %d0,%d1" "addl d0,d1" "add.l d0,d1"; do
  GMP_TRY_ASSEMBLE(
    [	$gmp_cv_asm_text
	$i],
    [gmp_cv_asm_m68k_instruction=$i
    rm -f conftest*
    break])
done
])
case $gmp_cv_asm_m68k_instruction in
"addl d0,d1")    want_dot_size=no;  want_register_percent=no  ;;
"addl %d0,%d1")  want_dot_size=no;  want_register_percent=yes ;;
"add.l d0,d1")   want_dot_size=yes; want_register_percent=no  ;;
"add.l %d0,%d1") want_dot_size=yes; want_register_percent=yes ;;
*) AC_MSG_ERROR([cannot determine assembler instruction and register style]) ;;
esac
GMP_DEFINE_RAW(["define(<WANT_REGISTER_PERCENT>, <\`$want_register_percent'>)"])
GMP_DEFINE_RAW(["define(<WANT_DOT_SIZE>, <\`$want_dot_size'>)"])
])


dnl  GMP_ASM_M68K_ADDRESSING
dnl  -----------------------

AC_DEFUN(GMP_ASM_M68K_ADDRESSING,
[AC_REQUIRE([GMP_ASM_TEXT])
AC_REQUIRE([GMP_ASM_M68K_INSTRUCTION])
AC_CACHE_CHECK([assembler addressing style],
		gmp_cv_asm_m68k_addressing,
[case $gmp_cv_asm_m68k_instruction in
addl*)  movel=movel ;;
add.l*) movel=move.l ;;
*) AC_MSG_ERROR([oops, unrecognised gmp_cv_asm_m68k_instruction]) ;;
esac
case $gmp_cv_asm_m68k_instruction in
*"%d0,%d1") dreg=%d0; areg=%a0 ;;
*"d0,d1")   dreg=d0;  areg=a0  ;;
*) AC_MSG_ERROR([oops, unrecognised gmp_cv_asm_m68k_instruction]) ;;
esac
GMP_TRY_ASSEMBLE(
[	$gmp_cv_asm_text
	$movel	$dreg, $areg@-],
  [gmp_cv_asm_m68k_addressing=mit],
[GMP_TRY_ASSEMBLE(
[	$gmp_cv_asm_text
	$movel	$dreg, -($areg)],
  [gmp_cv_asm_m68k_addressing=motorola],
[AC_MSG_ERROR([cannot determine assembler addressing style])])])
])
GMP_DEFINE_RAW(["define(<WANT_ADDRESSING>, <\`$gmp_cv_asm_m68k_addressing'>)"])
])


dnl  GMP_ASM_M68K_BRANCHES
dnl  ---------------------
dnl  "bra" is the standard branch instruction.  "jra" or "jbra" are
dnl  preferred where available, since on gas for instance they give a
dnl  displacement only as big as it needs to be, whereas "bra" is always
dnl  16-bits.  This applies to the conditional branches "bcc" etc too.
dnl  However "dbcc" etc on gas are already only as big as they need to be.

AC_DEFUN(GMP_ASM_M68K_BRANCHES,
[AC_REQUIRE([GMP_ASM_TEXT])
AC_CACHE_CHECK([assembler shortest branches],
		gmp_cv_asm_m68k_branches,
[gmp_cv_asm_m68k_branches=unknown
for i in jra jbra bra; do
  GMP_TRY_ASSEMBLE(
[	$gmp_cv_asm_text
foo$gmp_cv_asm_label_suffix
	$i	foo],
  [gmp_cv_asm_m68k_branches=$i
  rm -f conftest*
  break])
done
])
if test "$gmp_cv_asm_m68k_branches" = unknown; then
  AC_MSG_ERROR([cannot determine assembler branching style])
fi
GMP_DEFINE_RAW(["define(<WANT_BRANCHES>, <\`$gmp_cv_asm_m68k_branches'>)"])
])


dnl  GMP_ASM_POWERPC_R_REGISTERS
dnl  ---------------------------
dnl  Determine whether the assembler takes powerpc registers with an "r" as
dnl  in "r6", or as plain "6".  The latter is standard, but NeXT, Rhapsody,
dnl  and MacOS-X require the "r" forms.
dnl
dnl  See also mpn/powerpc32/powerpc-defs.m4 which uses the result of this
dnl  test.

AC_DEFUN(GMP_ASM_POWERPC_R_REGISTERS,
[AC_REQUIRE([GMP_ASM_TEXT])
AC_CACHE_CHECK([if the assembler needs r on registers],
               gmp_cv_asm_powerpc_r_registers,
[GMP_TRY_ASSEMBLE(
[	$gmp_cv_asm_text
	mtctr	6],
[gmp_cv_asm_powerpc_r_registers=no],
[GMP_TRY_ASSEMBLE(
[	$gmp_cv_asm_text
	mtctr	r6],
[gmp_cv_asm_powerpc_r_registers=yes],
[AC_MSG_ERROR([neither "mtctr 6" nor "mtctr r6" works])])])])

GMP_DEFINE_RAW(["define(<WANT_R_REGISTERS>,<$gmp_cv_asm_powerpc_r_registers>)"])
])


dnl  GMP_C_ATTRIBUTE_CONST
dnl  ---------------------

AC_DEFUN(GMP_C_ATTRIBUTE_CONST,
[AC_CACHE_CHECK([whether gcc __attribute__ ((const)) works],
                gmp_cv_c_attribute_const,
[AC_TRY_COMPILE([int foo (int x) __attribute__ ((const));], ,
  gmp_cv_c_attribute_const=yes, gmp_cv_c_attribute_const=no)
])
if test $gmp_cv_c_attribute_const = yes; then
  AC_DEFINE(HAVE_ATTRIBUTE_CONST, 1,
  [Define if the compiler accepts gcc style __attribute__ ((const))])
fi
])


dnl  GMP_C_ATTRIBUTE_MALLOC
dnl  ----------------------
dnl  gcc 2.95.x accepts __attribute__ ((malloc)) but with a warning that
dnl  it's ignored.  Pretend it doesn't exist in this case, to avoid that
dnl  warning.

AC_DEFUN(GMP_C_ATTRIBUTE_MALLOC,
[AC_CACHE_CHECK([whether gcc __attribute__ ((malloc)) works],
                gmp_cv_c_attribute_malloc,
[cat >conftest.c <<EOF
void *foo (int x) __attribute__ ((malloc));
EOF
gmp_compile="$CC $CFLAGS $CPPFLAGS -c conftest.c >conftest.out 2>&1"
if AC_TRY_EVAL(gmp_compile); then
  if grep "attribute directive ignored" conftest.out >/dev/null; then
    gmp_cv_c_attribute_malloc=no
  else
    gmp_cv_c_attribute_malloc=yes
  fi
else
  gmp_cv_c_attribute_malloc=no
fi
cat conftest.out >&AC_FD_CC
rm -f conftest*
])
if test $gmp_cv_c_attribute_malloc = yes; then
  AC_DEFINE(HAVE_ATTRIBUTE_MALLOC, 1,
  [Define if the compiler accepts gcc style __attribute__ ((malloc))])
fi
])


dnl  GMP_C_ATTRIBUTE_MODE
dnl  --------------------
dnl  Introduced in gcc 2.2, but perhaps not in all Apple derived versions.

AC_DEFUN(GMP_C_ATTRIBUTE_MODE,
[AC_CACHE_CHECK([whether gcc __attribute__ ((mode (XX))) works],
                gmp_cv_c_attribute_mode,
[AC_TRY_COMPILE([typedef int SItype __attribute__ ((mode (SI)));], ,
  gmp_cv_c_attribute_mode=yes, gmp_cv_c_attribute_mode=no)
])
if test $gmp_cv_c_attribute_mode = yes; then
  AC_DEFINE(HAVE_ATTRIBUTE_MODE, 1,
  [Define if the compiler accepts gcc style __attribute__ ((mode (XX)))])
fi
])


dnl  GMP_C_ATTRIBUTE_NORETURN
dnl  ------------------------

AC_DEFUN(GMP_C_ATTRIBUTE_NORETURN,
[AC_CACHE_CHECK([whether gcc __attribute__ ((noreturn)) works],
                gmp_cv_c_attribute_noreturn,
[AC_TRY_COMPILE([void foo (int x) __attribute__ ((noreturn));], ,
  gmp_cv_c_attribute_noreturn=yes, gmp_cv_c_attribute_noreturn=no)
])
if test $gmp_cv_c_attribute_noreturn = yes; then
  AC_DEFINE(HAVE_ATTRIBUTE_NORETURN, 1,
  [Define if the compiler accepts gcc style __attribute__ ((noreturn))])
fi
])


dnl  GMP_C_DOUBLE_FORMAT
dnl  -------------------
dnl  Determine the floating point format.
dnl
dnl  The object file is grepped, in order to work when cross compiling.  A
dnl  start and end sequence is included to avoid false matches, and
dnl  allowance is made for the desired data crossing an "od -b" line
dnl  boundary.  The test number is a small integer so it should appear
dnl  exactly, no rounding or truncation etc.
dnl
dnl  "od -b", incidentally, is supported even by Unix V7, and the awk script
dnl  used doesn't have functions or anything, so even an "old" awk should
dnl  suffice.

AC_DEFUN(GMP_C_DOUBLE_FORMAT,
[AC_REQUIRE([AC_PROG_CC])
AC_REQUIRE([AC_PROG_AWK])
AC_CACHE_CHECK([format of `double' floating point],
                gmp_cv_c_double_format,
[gmp_cv_c_double_format=unknown
cat >conftest.c <<\EOF
[struct {
  char    before[8];
  double  x;
  char    after[8];
} foo = {
  { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF },
  -123456789.0,
  { 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10 },
};]
EOF
gmp_compile="$CC $CFLAGS $CPPFLAGS -c conftest.c >&AC_FD_CC 2>&1"
if AC_TRY_EVAL(gmp_compile); then
cat >conftest.awk <<\EOF
[
BEGIN {
  found = 0
}

{
  for (f = 2; f <= NF; f++)
    {
      for (i = 0; i < 23; i++)
        got[i] = got[i+1];
      got[23] = $f;

      # match the special begin and end sequences
      if (got[0] != "001") continue
      if (got[1] != "043") continue
      if (got[2] != "105") continue
      if (got[3] != "147") continue
      if (got[4] != "211") continue
      if (got[5] != "253") continue
      if (got[6] != "315") continue
      if (got[7] != "357") continue
      if (got[16] != "376") continue
      if (got[17] != "334") continue
      if (got[18] != "272") continue
      if (got[19] != "230") continue
      if (got[20] != "166") continue
      if (got[21] != "124") continue
      if (got[22] != "062") continue
      if (got[23] != "020") continue

      saw = " (" got[8] " " got[9] " " got[10] " " got[11] " " got[12] " " got[13] " " got[14] " " got[15] ")"

      if (got[8]  == "000" &&  \
          got[9]  == "000" &&  \
          got[10] == "000" &&  \
          got[11] == "124" &&  \
          got[12] == "064" &&  \
          got[13] == "157" &&  \
          got[14] == "235" &&  \
          got[15] == "301")
        {
          print "IEEE little endian"
          found = 1
          exit
        }

      # ARM style little endian but with the two 4-byte halves swapped
      if (got[8]  == "064" &&  \
          got[9]  == "157" &&  \
          got[10] == "235" &&  \
          got[11] == "301" &&  \
          got[12] == "000" &&  \
          got[13] == "000" &&  \
          got[14] == "000" &&  \
          got[15] == "124")
        {
          print "IEEE little endian, swapped halves"
          found = 1
          exit
        }

      if (got[8]  == "301" &&  \
          got[9]  == "235" &&  \
          got[10] == "157" &&  \
          got[11] == "064" &&  \
          got[12] == "124" &&  \
          got[13] == "000" &&  \
          got[14] == "000" &&  \
	  got[15] == "000")
        {
          print "IEEE big endian"
          found = 1
          exit
        }

      if (got[8]  == "353" &&  \
          got[9]  == "315" &&  \
          got[10] == "242" &&  \
          got[11] == "171" &&  \
          got[12] == "000" &&  \
          got[13] == "240" &&  \
          got[14] == "000" &&  \
          got[15] == "000")
        {
          print "VAX D"
          found = 1
          exit
        }

      if (got[8]  == "275" &&  \
          got[9]  == "301" &&  \
          got[10] == "064" &&  \
          got[11] == "157" &&  \
          got[12] == "000" &&  \
          got[13] == "124" &&  \
          got[14] == "000" &&  \
          got[15] == "000")
        {
          print "VAX G"
          found = 1
          exit
        }

      if (got[8]  == "300" &&  \
          got[9]  == "033" &&  \
          got[10] == "353" &&  \
          got[11] == "171" &&  \
          got[12] == "242" &&  \
          got[13] == "240" &&  \
          got[14] == "000" &&  \
          got[15] == "000")
        {
          print "Cray CFP"
          found = 1
          exit
        }
    }
}

END {
  if (! found)
    print "unknown", saw
}
]
EOF
  gmp_cv_c_double_format=`od -b conftest.$OBJEXT | $AWK -f conftest.awk`
  case $gmp_cv_c_double_format in
  unknown*)
    echo "cannot match anything, conftest.$OBJEXT contains" >&AC_FD_CC
    od -b conftest.$OBJEXT >&AC_FD_CC
    ;;
  esac
else
  AC_MSG_WARN([oops, cannot compile test program])
fi
])

AH_VERBATIM([HAVE_DOUBLE],
[/* Define one (and only one) of the following for the format of a `double'.
   If your format is not among these choices, or you don't know what it is,
   then leave all of them undefined.
   "IEEE_LITTLE_SWAPPED" means little endian, but with the two 4-byte halves
   swapped, as used by ARM CPUs in little endian mode.  */
#undef HAVE_DOUBLE_IEEE_BIG_ENDIAN
#undef HAVE_DOUBLE_IEEE_LITTLE_ENDIAN
#undef HAVE_DOUBLE_IEEE_LITTLE_SWAPPED
#undef HAVE_DOUBLE_VAX_D
#undef HAVE_DOUBLE_VAX_G
#undef HAVE_DOUBLE_CRAY_CFP])

case $gmp_cv_c_double_format in
  "IEEE big endian")
    AC_DEFINE(HAVE_DOUBLE_IEEE_BIG_ENDIAN, 1) ;;
  "IEEE little endian")
    AC_DEFINE(HAVE_DOUBLE_IEEE_LITTLE_ENDIAN, 1) ;;
  "IEEE little endian, swapped halves")
    AC_DEFINE(HAVE_DOUBLE_IEEE_LITTLE_SWAPPED, 1) ;;
  "VAX D")
    AC_DEFINE(HAVE_DOUBLE_VAX_D, 1) ;;
  "VAX G")
    AC_DEFINE(HAVE_DOUBLE_VAX_G, 1) ;;
  "Cray CFP")
    AC_DEFINE(HAVE_DOUBLE_CRAY_CFP, 1) ;;
  unknown*)
    ;;
  *) 
    AC_MSG_WARN([oops, unrecognised float format: $gmp_cv_c_double_format])
    ;;
esac
])


dnl  GMP_C_INLINE
dnl  ------------
dnl  Establish an "inline" keyword, if possible.
dnl
dnl  This is the same as AC_C_INLINE, but introduing a typedef into the test
dnl  program.  Some versions of HP C succeed when the return value is a
dnl  plain builtin type like "int", but fail when it's a typedef.
dnl
dnl  FIXME: Hopefully autoconf will do this extra itself one day.

AC_DEFUN(GMP_C_INLINE,
[AC_CACHE_CHECK([for inline], gmp_cv_c_inline,
[gmp_cv_c_inline=no
for i in inline __inline__ __inline; do
  AC_TRY_COMPILE(
[#ifndef __cplusplus
typedef int foo_t;
$i foo_t func1 () { return 0; }
static $i foo_t func2 () { return 0; }
#endif], ,
    [gmp_cv_c_inline=$i
    break])
done
if test $gmp_cv_c_inline = inline; then
  gmp_cv_c_inline=yes
fi
])
case $gmp_cv_c_inline in
  inline | yes) ;;
  no) AC_DEFINE(inline,,
                [Define as `__inline' if that's what the C compiler calls it,
                 or to nothing if it is not supported.]) ;;
  *)  AC_DEFINE_UNQUOTED(inline, $gmp_cv_c_inline) ;;
esac
])


dnl  GMP_C_RESTRICT
dnl  --------------
dnl  Establish a "restrict" keyword, if possible, like AC_C_INLINE.
dnl
dnl  If restrict works already, then do nothing.  Otherwise AC_DEFINE it to
dnl  __restrict__ or __restrict if they work, or to empty otherwise.
dnl
dnl  "restrict" is in C99.  GCC 2.95 and up has it in c99 mode, and also as
dnl  __restrict__ and __restrict in all modes.
dnl
dnl  "yes" is used in the cache variable if plain "restrict" works, to make
dnl  the configure message look nicer.

AC_DEFUN(GMP_C_RESTRICT,
[AC_CACHE_CHECK([for restrict], gmp_cv_c_restrict,
[gmp_cv_c_restrict=no
for r in restrict __restrict__ __restrict; do
  AC_TRY_COMPILE(, [char * $r foo;],
    [gmp_cv_c_restrict=$r
    break])
done
if test $gmp_cv_c_restrict = restrict; then
  gmp_cv_c_restrict=yes
fi
])
case $gmp_cv_c_restrict in
  restrict | yes) ;;
  no) AC_DEFINE(restrict,,
                [Define as `__restrict' if that's what the C compiler calls it,
                 or to nothing if it is not supported.]) ;;
  *)  AC_DEFINE_UNQUOTED(restrict, $gmp_cv_c_restrict) ;;
esac
])


dnl  GMP_C_SIZES
dnl  -----------
dnl  Determine some sizes, if not alredy provided by gmp-mparam.h.
dnl  $gmp_mparam_source is the selected gmp-mparam.h.
dnl
dnl  BITS_PER_MP_LIMB, BYTES_PER_MP_LIMB and BITS_PER_ULONG are needed at
dnl  preprocessing time when building the library, for use in #if
dnl  conditionals.
dnl
dnl  BITS_PER_MP_LIMB is also wanted as a plain constant for some macros in
dnl  the generated gmp.h, and is instantiated as BITS_PER_MP_LIMB.
dnl
dnl  If some assembler code depends on a particular type size it's probably
dnl  best to put explicit #defines for these in gmp-mparam.h.  That way if
dnl  strange compiler options change the size then a mismatch will be
dnl  detected by t-constants.c rather than only by the code crashing or
dnl  giving wrong results.
dnl
dnl  None of the assembler code depends on BITS_PER_ULONG currently, so it's
dnl  just as easy to let configure find its size as to put explicit values.
dnl
dnl  The tests here assume bits=8*sizeof, but that might not be universally
dnl  true.  It'd be better to probe for how many bits seem to work, like
dnl  t-constants does.  But all currently supported systems have limbs and
dnl  ulongs with bits=8*sizeof, so it's academic.  Strange systems can
dnl  always have the right values put in gmp-mparam.h explicitly.

AC_DEFUN(GMP_C_SIZES,
[BITS_PER_MP_LIMB=[`sed -n 's/^#define BITS_PER_MP_LIMB[ 	][ 	]*\([0-9]*\).*$/\1/p' $gmp_mparam_source`]
if test -n "$BITS_PER_MP_LIMB" \
   && grep "^#define BYTES_PER_MP_LIMB" $gmp_mparam_source >/dev/null; then : ;
else
  AC_CHECK_SIZEOF(mp_limb_t,,
[#include <stdio.h>]
GMP_INCLUDE_GMP_H)
  if test "$ac_cv_sizeof_mp_limb_t" = 0; then
    AC_MSG_ERROR([some sort of compiler problem, mp_limb_t doesn't seem to work])
  fi
  if test -z "$BITS_PER_MP_LIMB"; then
    BITS_PER_MP_LIMB="(8*$ac_cv_sizeof_mp_limb_t)"
  fi
  if grep "^#define BYTES_PER_MP_LIMB" $gmp_mparam_source >/dev/null; then : ;
  else
    AC_DEFINE_UNQUOTED(BYTES_PER_MP_LIMB, $ac_cv_sizeof_mp_limb_t,
                       [bytes per mp_limb_t, if not in gmp-mparam.h])
  fi
fi
AC_SUBST(BITS_PER_MP_LIMB)
define([GMP_INCLUDE_GMP_H_BITS_PER_MP_LIMB],
[[#define __GMP_BITS_PER_MP_LIMB $BITS_PER_MP_LIMB
#define GMP_LIMB_BITS $BITS_PER_MP_LIMB]])

if grep "^#define BITS_PER_ULONG" $gmp_mparam_source >/dev/null; then : ;
else
  case $limb_chosen in
  longlong)
    AC_CHECK_SIZEOF(unsigned long)
    AC_DEFINE_UNQUOTED(BITS_PER_ULONG, (8 * $ac_cv_sizeof_unsigned_long),
                       [bits per unsigned long, if not in gmp-mparam.h])
    ;;
  *)
    # Copy the limb size when a limb is a ulong
    AC_DEFINE(BITS_PER_ULONG, BITS_PER_MP_LIMB)
    ;;
  esac
fi
])


dnl  GMP_C_STDARG
dnl  ------------
dnl  Test whether to use <stdarg.h> or <varargs.h>.
dnl
dnl  Notice the AC_DEFINE here is HAVE_STDARG to avoid clashing with
dnl  HAVE_STDARG_H which could arise from AC_CHECK_HEADERS.
dnl
dnl  This test might be slight overkill, after all there's really only going
dnl  to be ANSI or K&R and the two can be differentiated by AC_PROG_CC_STDC
dnl  or very likely by the setups for _PROTO in gmp.h.  On the other hand
dnl  this test is nice and direct, being what we're going to actually use.

AC_DEFUN(GMP_C_STDARG,
[AC_CACHE_CHECK([whether <stdarg.h> exists and works],
                gmp_cv_c_stdarg,
[AC_TRY_COMPILE(
[#include <stdarg.h>
int foo (int x, ...)
{
  va_list  ap;
  int      y;
  va_start (ap, x);
  y = va_arg (ap, int);
  va_end (ap);
  return y;
}],,
gmp_cv_c_stdarg=yes, gmp_cv_c_stdarg=no)
])
if test $gmp_cv_c_stdarg = yes; then
  AC_DEFINE(HAVE_STDARG, 1, [Define if <stdarg.h> exists and works])
fi
])


dnl  GMP_FUNC_ALLOCA
dnl  ---------------
dnl  Determine whether "alloca" is available.  This is AC_FUNC_ALLOCA from
dnl  autoconf, but changed so it doesn't use alloca.c if alloca() isn't
dnl  available, and also to use gmp-impl.h for the conditionals detecting
dnl  compiler builtin alloca's.

AC_DEFUN(GMP_FUNC_ALLOCA,
[AC_REQUIRE([GMP_HEADER_ALLOCA])
AC_CACHE_CHECK([for alloca (via gmp-impl.h)],
               gmp_cv_func_alloca,
[AC_TRY_LINK(
GMP_INCLUDE_GMP_H
[#include "$srcdir/gmp-impl.h"
],
  [char *p = (char *) alloca (1);],
  gmp_cv_func_alloca=yes,
  gmp_cv_func_alloca=no)])
if test $gmp_cv_func_alloca = yes; then
  AC_DEFINE(HAVE_ALLOCA, 1,
    [Define if alloca() works (via gmp-impl.h).])
fi
])

AC_DEFUN(GMP_HEADER_ALLOCA,
[# The Ultrix 4.2 mips builtin alloca declared by alloca.h only works
# for constant arguments.  Useless!
AC_CACHE_CHECK([for working alloca.h],
               gmp_cv_header_alloca,
[AC_TRY_LINK([#include <alloca.h>],
  [char *p = (char *) alloca (2 * sizeof (int));],
  gmp_cv_header_alloca=yes,
  gmp_cv_header_alloca=no)])
if test $gmp_cv_header_alloca = yes; then
  AC_DEFINE(HAVE_ALLOCA_H, 1,
    [Define if you have <alloca.h> and it should be used (not on Ultrix).])
fi
])


dnl  GMP_OPTION_ALLOCA
dnl  -----------------
dnl  Decide what to do about --enable-alloca from the user.
dnl  This is a macro so it can require GMP_FUNC_ALLOCA.

AC_DEFUN(GMP_OPTION_ALLOCA,
[AC_REQUIRE([GMP_FUNC_ALLOCA])
AC_CACHE_CHECK([how to allocate temporary memory],
               gmp_cv_option_alloca,
[case $enable_alloca in
  yes)
    gmp_cv_option_alloca=alloca
    ;;
  no)
    gmp_cv_option_alloca=malloc-reentrant
    ;;
  reentrant | notreentrant)
    case $gmp_cv_func_alloca in
    yes)  gmp_cv_option_alloca=alloca ;;
    *)    gmp_cv_option_alloca=malloc-$enable_alloca ;;
    esac
    ;;
  *)
    gmp_cv_option_alloca=$enable_alloca
    ;;
esac
])
case $gmp_cv_option_alloca in
  alloca)
    if test $gmp_cv_func_alloca = no; then
      AC_MSG_ERROR([--enable-alloca=alloca specified, but alloca not available])
    fi
    AC_DEFINE(WANT_TMP_ALLOCA, 1, [--enable-alloca=yes])
    ;;
  malloc-reentrant)
    AC_DEFINE(WANT_TMP_REENTRANT, 1, [--enable-alloca=malloc-reentrant])
    TAL_OBJECT=tal-reent$U.lo
    ;;
  malloc-notreentrant)
    AC_DEFINE(WANT_TMP_NOTREENTRANT, 1, [--enable-alloca=malloc-notreentrant])
    TAL_OBJECT=tal-notreent$U.lo
    ;;
  debug)
    AC_DEFINE(WANT_TMP_DEBUG, 1, [--enable-alloca=debug])
    TAL_OBJECT=tal-debug$U.lo
    ;;
  *)
    # checks at the start of configure.in should protect us
    AC_MSG_ERROR([unrecognised --enable-alloca=$gmp_cv_option_alloca])
    ;;
esac
AC_SUBST(TAL_OBJECT)
])


dnl  GMP_FUNC_SSCANF_WRITABLE_INPUT
dnl  ------------------------------
dnl  Determine whether sscanf requires a writable input string.
dnl
dnl  FIXME: Run a program to try this, when doing a native build.

AC_DEFUN(GMP_FUNC_SSCANF_WRITABLE_INPUT,
[AC_CACHE_CHECK([whether sscanf needs writable input],
                 gmp_cv_func_sscanf_writable_input,
[case $host in
  *-*-hpux9 | *-*-hpux9.*)
     gmp_cv_func_sscanf_writable_input=yes ;;
  *) gmp_cv_func_sscanf_writable_input=no  ;;
esac
])
case $gmp_cv_func_sscanf_writable_input in
  yes) AC_DEFINE(SSCANF_WRITABLE_INPUT, 1,
                 [Define if sscanf requires writable inputs]) ;;
  no)  ;;
  *)   AC_MSG_ERROR([unrecognised \$gmp_cv_func_sscanf_writable_input]) ;;
esac
])


dnl  GMP_FUNC_VSNPRINTF
dnl  ------------------
dnl  Check whether vsnprintf exists, and works properly.
dnl
dnl  Sparc Solaris 2.7 in 64-bit mode doesn't always truncate, making
dnl  vsnprintf like vsprintf, and hence completely useless.  On one system a
dnl  literal string is enough to provoke the problem, on another a "%n" was
dnl  needed.  There seems to be something weird going on with the optimizer
dnl  or something, since on the first system adding a second check with
dnl  "%n", or even just an initialized local variable, makes it work.  In
dnl  any case, without bothering to get to the bottom of this, the two
dnl  program runs in the code below end up successfully detecting the
dnl  problem.
dnl
dnl  glibc 2.0.x returns either -1 or bufsize-1 for an overflow (both seen,
dnl  not sure which 2.0.x does which), but still puts the correct null
dnl  terminated result into the buffer.

AC_DEFUN(GMP_FUNC_VSNPRINTF,
[AC_REQUIRE([GMP_C_STDARG])
AC_CHECK_FUNC(vsnprintf,
              [gmp_vsnprintf_exists=yes],
              [gmp_vsnprintf_exists=no])
if test "$gmp_vsnprintf_exists" = no; then
  gmp_cv_func_vsnprintf=no
else
  AC_CACHE_CHECK([whether vsnprintf works],
                 gmp_cv_func_vsnprintf,
  [gmp_cv_func_vsnprintf=yes
   for i in 'check ("hello world");' 'int n; check ("%nhello world", &n);'; do
     AC_TRY_RUN([
#include <string.h>  /* for strcmp */
#include <stdio.h>   /* for vsnprintf */

#if HAVE_STDARG
#include <stdarg.h>
#else
#include <varargs.h>
#endif

int
#if HAVE_STDARG
check (const char *fmt, ...)
#else
check (va_alist)
     va_dcl
#endif
{
  static char  buf[128];
  va_list  ap;
  int      ret;

#if HAVE_STDARG
  va_start (ap, fmt);
#else
  char *fmt;
  va_start (ap);
  fmt = va_arg (ap, char *);
#endif

  ret = vsnprintf (buf, 4, fmt, ap);

  if (strcmp (buf, "hel") != 0)
    exit (1);

  /* allowed return values */
  if (ret != -1 && ret != 3 && ret != 11)
    exit (2);

  return 0;
}

int
main ()
{
$i
  exit (0);
}
],
      [:],
      [gmp_cv_func_vsnprintf=no; break],
      [gmp_cv_func_vsnprintf=probably; break])
  done
  ])
  if test "$gmp_cv_func_vsnprintf" = probably; then
    AC_MSG_WARN([cannot check for properly working vsnprintf when cross compiling, will assume it's ok])
  fi
  if test "$gmp_cv_func_vsnprintf" != no; then
    AC_DEFINE(HAVE_VSNPRINTF,1,
              [Define if you have vsnprintf and it works properly.])
  fi
fi
])


dnl  GMP_H_ANSI
dnl  ----------
dnl  Check whether gmp.h recognises the compiler as ANSI capable.

AC_DEFUN(GMP_H_ANSI,
[AC_REQUIRE([AC_PROG_CC_STDC])
case $ac_cv_prog_cc_stdc in
  no)
    ;;
  *)
    AC_TRY_COMPILE(
GMP_INCLUDE_GMP_H
[#if ! __GMP_HAVE_PROTOTYPES
die die die
#endif
],,,
    [AC_MSG_WARN([gmp.h doesnt recognise compiler as ANSI, prototypes and "const" will be unavailable])])
    ;;
esac
])


dnl  GMP_H_EXTERN_INLINE
dnl  -------------------
dnl  If the compiler has an "inline" of some sort, check whether the
dnl  #ifdef's in gmp.h recognise it.

AC_DEFUN(GMP_H_EXTERN_INLINE,
[AC_REQUIRE([GMP_C_INLINE])
case $gmp_cv_c_inline in
no) ;;
*)
  AC_TRY_COMPILE(
GMP_INCLUDE_GMP_H
[#ifndef __GMP_EXTERN_INLINE
die die die
#endif
],,,
  [case $gmp_cv_c_inline in
  yes) tmp_inline=inline ;;
  *)   tmp_inline=$gmp_cv_c_inline ;;
  esac    
  AC_MSG_WARN([gmp.h doesnt recognise compiler "$tmp_inline", inlines will be unavailable])])
  ;;
esac
])


dnl  GMP_H_HAVE_FILE
dnl  ---------------
dnl  Check whether the #ifdef's in gmp.h recognise when stdio.h has been
dnl  included to get FILE.

AC_DEFUN(GMP_H_HAVE_FILE,
[AC_TRY_COMPILE(
[#include <stdio.h>]
GMP_INCLUDE_GMP_H
[#if ! _GMP_H_HAVE_FILE
die die die
#endif
],,,
  [AC_MSG_WARN([gmp.h doesnt recognise <stdio.h>, FILE prototypes will be unavailable])])
])


dnl  GMP_IMPL_H_IEEE_FLOATS
dnl  ----------------------
dnl  Check whether the #ifdef's in gmp-impl.h recognise IEEE format and
dnl  endianness.

AC_DEFUN(GMP_IMPL_H_IEEE_FLOATS,
[case $host in
  vax*-*-*)
    # not IEEE (neither D nor G formats are IEEE)
    ;;
  none-*-*)
    # don't worry about this when CPU "none"
    ;;
  *)
    case $path in
      *cray/cfp*)
        # not IEEE
        ;;
      *)
        AC_TRY_COMPILE(
[#include <stdio.h>]
GMP_INCLUDE_GMP_H
[#include "$srcdir/gmp-impl.h"
#ifndef _GMP_IEEE_FLOATS
die die die
#endif
],,,[
          AC_MSG_WARN([gmp-impl.h doesnt recognise "double" as IEEE.])
          AC_MSG_WARN([If your CPU floats are in fact IEEE then you])
	  AC_MSG_WARN([might like to augment the tests there.])
        ])
        ;;
    esac
    ;;
esac
])
