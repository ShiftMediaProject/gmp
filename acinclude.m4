dnl  [ GMP_PROG_CC_FIND(cc_list, req_64bit_cc) ]
dnl  Find first working compiler in `cc_list'.
dnl  If `req_64bit_cc' is "yes", the compiler is required to be able to 
dnl  produce 64-bit code.
dnl  NOTE: If a compiler needs any special flags for producing 64-bit code,
dnl  these have to be found in shell variable `gmp_cflags64_{cc}', where `{cc}'
dnl  is the name of the compiler.
dnl  Set CC to the name of the first working compiler.
dnl  If a 64-bit compiler is found, set CC64 to the name of the compiler and
dnl  CFLAGS64 to flags to use.
dnl  This macro does not test that any of the compilers found realy work, 
dnl  nor if it is a GNU compiler.  To do this, when you have finally made
dnl  up your mind on which one to use, invoke [GMP_PROG_CC_SELECT]
dnl
AC_DEFUN(GMP_PROG_CC_FIND, [
AC_BEFORE([$0], [CC_PROG_CPP])dnl
gmp_cc_list=$1
gmp_req_64bit_cc=$2

[echo "***DEBUG*** GMP_PROG_CC_FIND: gmp_cc_list=>$gmp_cc_list<"]
[echo "***DEBUG*** GMP_PROG_CC_FIND: gmp_req_64bit_cc=$gmp_req_64bit_cc"]

for compiler in $gmp_cc_list; do
  AC_CHECK_PROG(CC_TEST, $compiler, $compiler, , , /usr/ucb/cc)
  if test -n "$CC_TEST"; then
    CC=$CC_TEST
    if test "$gmp_req_64bit_cc" = "yes"; then
      eval compiler_flags=\$gmp_cflags64_${compiler}
      GMP_CHECK_CC_64BIT($compiler, $compiler_flags)
      if test "$gmp_cv_cc_64bit" = "yes"; then
        if test -z "$CC64"; then CC64=$compiler; fi
        if test -z "$CFLAGS64"; then CFLAGS64=$compiler_flags; fi
        break
      fi
    else
      break
    fi
  fi
done

[echo "***DEBUG*** GMP_PROG_CC_FIND: CC=$CC, CC64=$CC64, CFLAGS64=$CFLAGS64"]
])dnl

dnl  [GMP_PROG_CC_SELECT]
dnl  Check that `CC' works with `CFLAGS'.
AC_DEFUN(GMP_PROG_CC_SELECT, [
AC_BEFORE([$0], [CC_PROG_CPP])dnl

AC_PROG_CC_WORKS
AC_PROG_CC_GNU

if test $ac_cv_prog_gcc = yes; then
  GCC=yes
else
  GCC=
fi

dnl [Code for setting CFLAGS removed.]

AC_SUBST(CC)
AC_PROVIDE([AC_PROG_CC])
])dnl

dnl  GMP_CHECK_CC_64BIT(cc, cflags64)
dnl  FIXME: Cache result.
AC_DEFUN(GMP_CHECK_CC_64BIT,
[
  AC_MSG_CHECKING([whether the C compiler ($1) is 64-bit capable])
  CC_save=$CC
  CC=$1
  CFLAGS_save=$CFLAGS
  CFLAGS=$2
  AC_TRY_RUN([
    int main() { return (sizeof (void *) != 8); }
  ], gmp_cv_cc_64bit=yes, gmp_cv_cc_64bit=no, gmp_cv_cc_64bit=no)
  CC=$CC_save
  CFLAGS=$CFLAGS_save
  AC_MSG_RESULT($gmp_cv_cc_64bit)
])dnl

dnl  GMP_PROG_LIBTOOL
dnl  Wrap [AC_PROG_LIBTOOL] to avoid the [AC_PROG_CC] problems.
AC_DEFUN(GMP_PROG_LIBTOOL, [
dnl  Save any variable that [AC_PROG_CC] may possibly set, invoke [AC_PROG_CC], and
dnl  restore the variables.  Finally, invoke [AC_PROG_LIBTOOL].
dnl  This is highly dependant on Autoconf version and
dnl  generally a mess.
gmp_save_CC=$CC
gmp_save_CFLAGS=$CFLAGS
gmp_save_ac_cv_prog_gcc=$ac_cv_prog_gcc
gmp_save_GCC=$GCC
gmp_save_ac_cv_prog_cc_g=$ac_cv_prog_cc_g
gmp_save_ac_cv_prog_cc_works=$ac_cv_prog_cc_works
gmp_save_ac_cv_prog_cc_cross=$ac_cv_prog_cc_cross
AC_PROG_CC
CC=$gmp_save_CC
CFLAGS=$gmp_save_CFLAGS
ac_cv_prog_gcc=$gmp_save_ac_cv_prog_gcc
GCC=$gmp_save_GCC
ac_cv_prog_cc_g=$gmp_save_ac_cv_prog_cc_g
ac_cv_prog_cc_works=$gmp_save_ac_cv_prog_cc_works
ac_cv_prog_cc_cross=$gmp_save_ac_cv_prog_cc_cross
AC_PROG_LIBTOOL
])dnl

dnl  GMP_CHECK_ASM_UNDERSCORE([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl  Shamelessly borrowed from glibc.
AC_DEFUN(GMP_CHECK_ASM_UNDERSCORE,
[AC_CACHE_CHECK([if symbols are prefixed by underscore], 
	        gmp_cv_check_asm_underscore,
[cat > conftest.$ac_ext <<EOF
dnl This sometimes fails to find confdefs.h, for some reason.
dnl [#]line __oline__ "[$]0"
[#]line __oline__ "configure"
#include "confdefs.h"
void underscore_test(void) {
return; }
EOF
if AC_TRY_EVAL(ac_compile); then
  if grep _underscore_test conftest* >/dev/null; then
    gmp_cv_check_asm_underscore=yes
    ifelse([$1], , :, [rm -f conftest*
    $1])
  else
    gmp_cv_check_asm_underscore=no
    ifelse([$2], , , [rm -f conftest*
    $2])
  fi
else
  echo "configure: failed program was:" >&AC_FD_CC
  cat conftest.$ac_ext >&AC_FD_CC
  ifelse([$2], , , [rm -f conftest*
  $2])
fi
rm -f conftest*
])dnl
])

dnl  GMP_CHECK_ASM_ALIGN_LOG([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl  Is parameter to `.align' logarithmic?
dnl  Requires CCAS to be set to command for assembling.
AC_DEFUN(GMP_CHECK_ASM_ALIGN_LOG,
[AC_CACHE_CHECK([if .align assembly directive is logarithmic],
		gmp_cv_check_asm_align_log,
[AC_EGREP_CPP(yes,
[#ifdef _AIX
  yes
#endif
],
changequote(,)
gmp_data_sect=".csect .data[[RW]]",
changequote([,])dnl
gmp_data_sect=".data")
cat > conftest.s <<EOF
      	$gmp_data_sect
      	.align  4
	.globl	foo
	.byte	1
	.align	4
foo:	.byte	2
EOF
ac_assemble="$CCAS $CFLAGS conftest.s 1>&AC_FD_CC"
dnl echo "*** DEBUG: ac_assemble=$ac_assemble"
if AC_TRY_EVAL(ac_assemble); then
changequote(<<, >>)
  val=`$NM conftest.o | grep foo | sed -e 's;[[][0-9][]]\(.*\);\1;' \
       -e 's;[^1-9]*\([0-9]*\).*;\1;'`
changequote([,])dnl
  if test "$val" = "10" || test "$val" = "16"; then
    gmp_cv_check_asm_align_log=yes
    ifelse([$1], , :, [rm -f conftest*
    $1])
  else
    gmp_cv_check_asm_align_log=no
    ifelse([$2], , , [rm -f conftest*
    $2])
  fi
else 
  echo "configure: failed program was:" >&AC_FD_CC
  cat conftest.s >&AC_FD_CC
  ifelse([$2], , , [rm -f conftest*
  $2])
fi

rm -f conftest*

])dnl
])


