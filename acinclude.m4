dnl  GMP_PROG_CC_FIND([CC_LIST], [REQ_64BIT_CC])
dnl  Find first working compiler in CC_LIST.
dnl  If REQ_64BIT_CC is "yes", the compiler is required to be able to 
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
AC_DEFUN(GMP_PROG_CC_FIND,
[AC_BEFORE([$0], [CC_PROG_CPP])
ifelse([$1], , gmp_cc_list="gcc cc", gmp_cc_list="[$1]")
ifelse([$2], , gmp_req_64bit_cc="no", gmp_req_64bit_cc="[$2]")

[echo "***DEBUG*** GMP_PROG_CC_FIND: gmp_cc_list=>$gmp_cc_list<"]
[echo "***DEBUG*** GMP_PROG_CC_FIND: gmp_req_64bit_cc=$gmp_req_64bit_cc"]

for gmp_tmp_compiler in $gmp_cc_list; do
  AC_CHECK_PROG(CC_TEST, $gmp_tmp_compiler, $gmp_tmp_compiler, , , /usr/ucb/cc)
  if test -n "$CC_TEST"; then
    CC=$CC_TEST
    if test "$gmp_req_64bit_cc" = "yes"; then
      eval gmp_tmp_compiler_flags=\$gmp_cflags64_${gmp_tmp_compiler}
      GMP_CHECK_CC_64BIT($gmp_tmp_compiler, $gmp_tmp_compiler_flags)
      if test "$gmp_cv_cc_64bit" = "yes"; then
        if test -z "$CC64"; then CC64=$gmp_tmp_compiler; fi
        if test -z "$CFLAGS64"; then CFLAGS64=$gmp_tmp_compiler_flags; fi
        break
      fi
    else
      break
    fi
  fi
done
[echo "***DEBUG*** GMP_PROG_CC_FIND: CC=$CC, CC64=$CC64, CFLAGS64=$CFLAGS64"]
])dnl

dnl  GMP_PROG_CC_SELECT
dnl  Check that `CC' works with `CFLAGS'.
AC_DEFUN(GMP_PROG_CC_SELECT,
[AC_BEFORE([$0], [CC_PROG_CPP])
AC_PROG_CC_WORKS
AC_PROG_CC_GNU

if test "$ac_cv_prog_gcc" = "yes"; then
  GCC=yes
else
  GCC=
fi
dnl [Code for setting CFLAGS removed.]
AC_SUBST(CC)
AC_PROVIDE([AC_PROG_CC])
])dnl

dnl  GMP_CHECK_CC_64BIT(cc, cflags64)
dnl  Find out if `CC' can produce 64-bit code.
AC_DEFUN(GMP_CHECK_CC_64BIT,
[
  gmp_tmp_CC_save="$CC"
  CC="[$1]"
  AC_MSG_CHECKING([whether the C compiler ($CC) is 64-bit capable])
  gmp_tmp_CFLAGS_save="$CFLAGS"
  CFLAGS="[$2]"
  AC_TRY_RUN([
    int main() { return (sizeof (void *) != 8); }
  ], gmp_cv_cc_64bit=yes, gmp_cv_cc_64bit=no, gmp_cv_cc_64bit=no)
  CC="$gmp_tmp_CC_save"
  CFLAGS="$gmp_tmp_CFLAGS_save"
  AC_MSG_RESULT($gmp_cv_cc_64bit)
])dnl

dnl  GMP_PROG_CCAS
dnl  Find C compiler invoking an assembler.
dnl  Set CCAS and [AC_SUBST] it.
AC_DEFUN(GMP_PROG_CCAS,
[AC_REQUIRE([GMP_PROG_CC_SELECT])
AC_CACHE_CHECK([for CC ($CC) invoking assembler],
               gmp_cv_prog_ccas,
[ac_assemble="$CC -c $CFLAGS conftest.s 1>&AC_FD_CC"
rm -f conftest*
echo "	.byte	1" > conftest.s
if AC_TRY_EVAL(ac_assemble); then
  if test -f conftest.o; then
    gmp_cv_prog_ccas="$CC -c $CFLAGS"
  else
    dnl  FIXME: Disable all assembly code instead of fatal exit.
    dnl  FIXME: Better fatal exit.
    echo "configure: can't find a C compiler able to assemble"
    exit 1
  fi
else
  echo "configure: failed program was:" >&AC_FD_CC
  cat conftest.s >&AC_FD_CC
  dnl  FIXME: Disable all assembly code instead of fatal exit.
  dnl  FIXME: Better fatal exit.
  echo "configure: can't find a C compiler able to assemble"
  exit 1
fi
rm -f conftest*
])
CCAS="$gmp_cv_prog_ccas"
AC_SUBST(CCAS)
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

dnl  GMP_INIT([M4-DEF-FILE])
dnl  
AC_DEFUN(GMP_INIT,
[ifelse([$1], , gmp_configm4=config.m4, gmp_configm4="[$1]")
gmp_tmpconfigm4=cnfm4.tmp
gmp_tmpconfigm4i=cnfm4i.tmp
test -f $gmp_tmpconfigm4 && rm $gmp_tmpconfigm4
test -f $gmp_tmpconfigm4i && rm $gmp_tmpconfigm4i
])dnl

dnl  GMP_FINISH
dnl  Create 
AC_DEFUN(GMP_FINISH,
[AC_REQUIRE([GMP_INIT])
echo "creating $gmp_configm4"
echo [dnl $gmp_configm4.  Generated automatically by configure.] > $gmp_configm4
if test -f $gmp_tmpconfigm4; then
  echo ["changequote(<,>)dnl"] >> $gmp_configm4
  cat $gmp_tmpconfigm4 >> $gmp_configm4
  echo ["changequote(\`,')dnl"] >> $gmp_configm4
  rm $gmp_tmpconfigm4
fi
if test -f $gmp_tmpconfigm4i; then
  cat $gmp_tmpconfigm4i >> $gmp_configm4
  rm $gmp_tmpconfigm4i
fi
])dnl

dnl  GMP_INCLUDE(FILE)
AC_DEFUN(GMP_INCLUDE,
[AC_REQUIRE([GMP_INIT])
echo ["include(\`$1')"] >> $gmp_tmpconfigm4i
])dnl

dnl  GMP_SINCLUDE(FILE)
AC_DEFUN(GMP_SINCLUDE,
[AC_REQUIRE([GMP_INIT])
echo ["sinclude(\`$1')"] >> $gmp_tmpconfigm4i
])dnl

dnl  GMP_DEFINE(MACRO, DEFINITION)
dnl  Define M4 macro MACRO to DEFINITION in temporary file.  Invoke 
dnl  [GMP_FINISH] to create file config.m4 containing all GMP_DEFINEd
dnl  macros.  config.m4 uses `<' and '>' as quote characters.
AC_DEFUN(GMP_DEFINE, 
[AC_REQUIRE([GMP_INIT])
echo ['define(<$1>, <$2>)'] >> $gmp_tmpconfigm4
])dnl

dnl  GMP_CHECK_ASM_LABEL_SUFFIX
dnl  Should a label have a colon or not?
AC_DEFUN(GMP_CHECK_ASM_LABEL_SUFFIX,
[AC_CACHE_CHECK([if assembly label needs a suffix],
               gmp_cv_check_asm_label_suffix,
[case "$target" in 
  *-*-hpux*) gmp_cv_check_asm_label_suffix=[""] ;;
  *) gmp_cv_check_asm_label_suffix=[":"] ;;
esac
])
echo ["define(<LABEL_SUFFIX>, <\$][1$gmp_cv_check_asm_label_suffix>)"] >> $gmp_tmpconfigm4
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
  else
    gmp_cv_check_asm_underscore=no
  fi
else
  echo "configure: failed program was:" >&AC_FD_CC
  cat conftest.$ac_ext >&AC_FD_CC
fi
rm -f conftest*
])
if test "$gmp_cv_check_asm_underscore" = "yes"; then
  GMP_DEFINE(GSYM_PREFIX, [_])
  ifelse([$1], , :, [$1])
else
  GMP_DEFINE(GSYM_PREFIX, [])
  ifelse([$2], , :, [$2])
fi    
])dnl

dnl  GMP_CHECK_ASM_ALIGN_LOG([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl  Is parameter to `.align' logarithmic?
dnl  Requires NM to be set to nm for target.
AC_DEFUN(GMP_CHECK_ASM_ALIGN_LOG,
[AC_REQUIRE([GMP_PROG_CCAS])
AC_REQUIRE([GMP_CHECK_ASM_GLOBL])
AC_REQUIRE([GMP_CHECK_ASM_DATA])
AC_REQUIRE([GMP_CHECK_ASM_LABEL_SUFFIX])
AC_CACHE_CHECK([if .align assembly directive is logarithmic],
		gmp_cv_check_asm_align_log,
[if test -z "$NM"; then
  echo; echo ["configure: $0: fatal: need nm"]
  exit 1
fi
cat > conftest.s <<EOF
      	$gmp_cv_check_asm_data
      	.align  4
	$gmp_cv_check_asm_globl	foo
	.byte	1
	.align	4
foo$gmp_cv_check_asm_label_suffix
	.byte	2
EOF
ac_assemble="$CCAS $CFLAGS conftest.s 1>&AC_FD_CC"
if AC_TRY_EVAL(ac_assemble); then
  changequote(<,>)
  gmp_tmp_val=`$NM conftest.o | grep foo | sed -e 's;[[][0-9][]]\(.*\);\1;' \
       -e 's;[^1-9]*\([0-9]*\).*;\1;'`
  changequote([, ])dnl
  if test "$gmp_tmp_val" = "10" || test "$gmp_tmp_val" = "16"; then
    gmp_cv_check_asm_align_log=yes
  else
    gmp_cv_check_asm_align_log=no
  fi
else 
  echo "configure: failed program was:" >&AC_FD_CC
  cat conftest.s >&AC_FD_CC
fi
rm -f conftest*
])
if test "$gmp_cv_check_asm_align_log" = "yes"; then
  GMP_DEFINE(ALIGN, [.align m4_log2($][1)])
  ifelse([$1], , :, [$1])
else
  GMP_DEFINE(ALIGN, [.align $][1])
  ifelse([$2], , :, [$2])
fi  
])dnl

dnl  GMP_CHECK_ASM_TEXT
AC_DEFUN(GMP_CHECK_ASM_TEXT,
[AC_CACHE_CHECK([how to switch to text section], gmp_cv_check_asm_text,
[case "$target" in
  *-*-aix*)
    changequote({, })
    gmp_cv_check_asm_text={".csect .text[PR]"}
    changequote([, ])
    ;;
  *-*-hpux*) gmp_cv_check_asm_text=[".code"] ;;
  *) gmp_cv_check_asm_text=[".text"] ;;
esac
])
echo ["define(<TEXT>, <$gmp_cv_check_asm_text>)"] >> $gmp_tmpconfigm4
])dnl

dnl  GMP_CHECK_ASM_DATA
dnl  Can we say `.data'?
AC_DEFUN(GMP_CHECK_ASM_DATA,
[AC_CACHE_CHECK([how to switch to data section], gmp_cv_check_asm_data,
[case "$target" in
  *-*-aix*)
    changequote({, })
    gmp_cv_check_asm_data={".csect .data[RW]"}
    changequote([, ])
    ;;
  *-*-hpux*) gmp_cv_check_asm_data=[".code"] ;;
  *) gmp_cv_check_asm_data=[".data"] ;;
esac
])
echo ["define(<DATA>, <$gmp_cv_check_asm_data>)"] >> $gmp_tmpconfigm4
])dnl

dnl  GMP_CHECK_ASM_GLOBL
dnl  Can we say `.global'?
AC_DEFUN(GMP_CHECK_ASM_GLOBL,
[AC_CACHE_CHECK([how to export a symbol], gmp_cv_check_asm_globl,
[case "$target" in
  *-*-hpux*) gmp_cv_check_asm_globl=[".export"] ;;
  *) gmp_cv_check_asm_globl=[".globl"] ;;
esac
])
echo ["define(<GLOBL>, <$gmp_cv_check_asm_globl>)"] >> $gmp_tmpconfigm4
])dnl

dnl  GMP_CHECK_ASM_TYPE
dnl  Can we say `.type'?
AC_DEFUN(GMP_CHECK_ASM_TYPE,
[AC_REQUIRE([GMP_PROG_CCAS])
AC_CACHE_CHECK([how the .type assembly directive should be used],
gmp_cv_check_asm_type,
[ac_assemble="$CCAS $CFLAGS conftest.s 1>&AC_FD_CC"
for gmp_tmp_prefix in @ \# %; do
  echo "	.type	sym,${gmp_tmp_prefix}function" > conftest.s
  if AC_TRY_EVAL(ac_assemble); then
    gmp_cv_check_asm_type="[.type	\$][1,${gmp_tmp_prefix}\$][2]"
    break
  fi
done
if test -z "$gmp_cv_check_asm_type"; then
  gmp_cv_check_asm_type="[dnl]"
fi
])
echo ["define(<TYPE>, <$gmp_cv_check_asm_type>)"] >> $gmp_tmpconfigm4
])dnl

dnl  GMP_CHECK_ASM_SIZE
dnl  Can we say `.size'?
AC_DEFUN(GMP_CHECK_ASM_SIZE,
[AC_REQUIRE([GMP_PROG_CCAS])
AC_CACHE_CHECK([if the .size assembly directive works], gmp_cv_check_asm_size,
[ac_assemble="$CCAS $CFLAGS conftest.s 1>&AC_FD_CC"
echo '	.size	sym,1' > conftest.s
if AC_TRY_EVAL(ac_assemble); then
  gmp_cv_check_asm_size="[.size	\$][1,\$][2]"
else
  gmp_cv_check_asm_size="[dnl]"
fi
])
echo ["define(<SIZE>, <$gmp_cv_check_asm_size>)"] >> $gmp_tmpconfigm4
])dnl

dnl  GMP_CHECK_ASM_LSYM_PREFIX
dnl  What is the prefix for a local label?
dnl  Requires NM to be set to nm for target.
AC_DEFUN(GMP_CHECK_ASM_LSYM_PREFIX,
[AC_REQUIRE([GMP_PROG_CCAS])
AC_REQUIRE([GMP_CHECK_ASM_LABEL_SUFFIX])
AC_CACHE_CHECK([what prefix to use for a local label], 
gmp_cv_check_asm_lsym_prefix,
[if test -z "$NM"; then
  echo; echo ["$0: fatal: need nm"]
  exit 1
fi
ac_assemble="$CCAS $CFLAGS conftest.s 1>&AC_FD_CC"
gmp_cv_check_asm_lsym_prefix="L"
for gmp_tmp_pre in .L $ L$; do
  cat > conftest.s <<EOF
${gmp_tmp_pre}gurkmacka${gmp_cv_check_asm_label_suffix}
	.byte 0
EOF
  if AC_TRY_EVAL(ac_assemble); then
    if $NM conftest.o | grep gurkmacka >/dev/null; then true; else
      gmp_cv_check_asm_lsym_prefix="$gmp_tmp_pre"
      break
    fi
  else
    echo "configure: failed program was:" >&AC_FD_CC
    cat conftest.s >&AC_FD_CC
    # Use default.
  fi
done
rm -f conftest*
])
echo ["define(<LSYM_PREFIX>, <${gmp_cv_check_asm_lsym_prefix}>)"] >> $gmp_tmpconfigm4
])

dnl  GMP_CHECK_ASM_W32
dnl  How to [define] a 32-bit word.
dnl  Requires NM to be set to nm for target.
AC_DEFUN(GMP_CHECK_ASM_W32,
[AC_REQUIRE([GMP_PROG_CCAS])
AC_REQUIRE([GMP_CHECK_ASM_DATA])
AC_REQUIRE([GMP_CHECK_ASM_GLOBL])
AC_REQUIRE([GMP_CHECK_ASM_LABEL_SUFFIX])
AC_CACHE_CHECK([how to [define] a 32-bit word],
	       gmp_cv_check_asm_w32,
[if test -z "$NM"; then
  echo; echo ["configure: $0: fatal: need nm"]
  exit 1
fi
ac_assemble="$CCAS $CFLAGS conftest.s 1>&AC_FD_CC"
for gmp_tmp_op in .long .word; do
  cat > conftest.s <<EOF
      	$gmp_cv_check_asm_data
	$gmp_cv_check_asm_globl	foo
	$gmp_tmp_op	0
foo${gmp_cv_check_asm_label_suffix}
	.byte	0
EOF
  if AC_TRY_EVAL(ac_assemble); then
    changequote(<,>)
    gmp_tmp_val=`$NM conftest.o | grep foo | sed -e 's;[[][0-9][]]\(.*\);\1;' \
         -e 's;[^1-9]*\([0-9]*\).*;\1;'`
    changequote([, ])dnl
    if test "$gmp_tmp_val" = "4"; then
      gmp_cv_check_asm_w32="$gmp_tmp_op"
      break
    fi
  fi
done
if test -z "$gmp_cv_check_asm_w32"; then
  echo; echo ["configure: $0: fatal: do not know how to define a 32-bit word"]
  exit 1
fi
rm -f conftest*
])
echo ["define(<W32>, <$gmp_cv_check_asm_w32>)"] >> $gmp_tmpconfigm4
])
