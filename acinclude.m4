dnl  autoconf macros specific to gmp


dnl  Copyright (C) 2000 Free Software Foundation, Inc.
dnl
dnl  This file is part of the GNU MP Library.
dnl
dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU Library General Public License as published
dnl  by the Free Software Foundation; either version 2 of the License, or (at
dnl  your option) any later version.
dnl
dnl  The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
dnl  License for more details.
dnl
dnl  You should have received a copy of the GNU Library General Public License
dnl  along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
dnl  the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
dnl  MA 02111-1307, USA.


dnl  GMP_HEADER_GETVAL(NAME,FILE)
dnl  ----------------------------
dnl  Expand to the value of a "#define NAME" from the given FILE.
dnl  The regexps here aren't very rugged, but are enough for gmp.
dnl  /dev/null as a parameter prevents a hang if $2 is accidentally omitted.

define(GMP_HEADER_GETVAL,
[patsubst(patsubst(
esyscmd([grep "^#define $1 " $2 /dev/null 2>/dev/null]),
[^.*$1[ 	]+],[]),
[[
 	]*$],[])])

dnl  GMP_VERSION
dnl  -----------
dnl  The gmp version number, extracted from the #defines in gmp.h.
dnl  Two digits like 3.0 if patchlevel <= 0, or three digits like 3.0.1 if
dnl  patchlevel > 0.

define(GMP_VERSION,
[GMP_HEADER_GETVAL(__GNU_MP_VERSION,gmp.h)[]dnl
.GMP_HEADER_GETVAL(__GNU_MP_VERSION_MINOR,gmp.h)[]dnl
ifelse(eval(GMP_HEADER_GETVAL(__GNU_MP_VERSION_PATCHLEVEL,gmp.h) > 0),1,
[.GMP_HEADER_GETVAL(__GNU_MP_VERSION_PATCHLEVEL,gmp.h)])])

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
dnl  This macro does not test if any of the compilers found is a GNU compiler.
dnl  To do this, when you have finally made up your mind on which one to use, 
dnl  and set CC accordingly, invoke [GMP_PROG_CC_SELECT].  That macro will 
dnl  also make sure that your selection of CFLAGS is valid.
dnl
AC_DEFUN(GMP_PROG_CC_FIND,
[AC_BEFORE([$0], [CC_PROG_CPP])
ifelse([$1], , gmp_cc_list="gcc cc", gmp_cc_list="[$1]")
ifelse([$2], , gmp_req_64bit_cc="no", gmp_req_64bit_cc="[$2]")

CC32=
CC64=
for c in $gmp_cc_list; do
  # Avoid cache hits.
  unset CC
  unset ac_cv_prog_CC
  AC_CHECK_TOOL(CC, $c, $c)
  if test -n "$CC"; then
    eval c_flags=\$gmp_cflags_$c
    GMP_PROG_CC_WORKS($CC, $c_flags,
		      gmp_prog_cc_works=yes, 
		      gmp_prog_cc_works=no)

    if test "$gmp_prog_cc_works" != "yes"; then
      continue
    fi

    # Save first working compiler, whether 32- or 64-bit capable.
    if test -z "$CC32"; then
      CC32="$CC"
    fi
    if test "$gmp_req_64bit_cc" = "yes"; then
      eval c_flags=\$gmp_cflags64_$c
      GMP_CHECK_CC_64BIT($c, $c_flags)
      if test "$gmp_cv_cc_64bit" = "yes"; then
        test -z "$CC64" && CC64="$c"
        test -z "$CFLAGS64" && CFLAGS64="$c_flags"
	# We have CC64 so we're done.
        break
      fi
    else
      # We have CC32, and we don't need a 64-bit compiler so we're done.
      break
    fi
  fi
done
CC="$CC32"
])dnl

dnl  GMP_PROG_CC_SELECT
dnl  Check that `CC' works with `CFLAGS'.  Check if `CC' is a GNU compiler.
dnl  Cache the result as `ac_cv_prog_CC'.
AC_DEFUN(GMP_PROG_CC_SELECT,
[AC_BEFORE([$0], [CC_PROG_CPP])
AC_PROG_CC_WORKS
AC_PROG_CC_GNU

if test "$ac_cv_prog_gcc" = "yes"; then
  GCC=yes
else
  GCC=
fi

# Set CFLAGS if not already set.
if test -z "$CFLAGS"; then
  CFLAGS="-g"
  if test "$GCC" = "yes"; then
    CFLAGS="$CFLAGS -O2"
  fi
fi

AC_SUBST(CC)
AC_CACHE_VAL(ac_cv_prog_CC, ac_cv_prog_CC="$CC")
AC_PROVIDE([AC_PROG_CC])
])dnl

dnl  GMP_CHECK_CC_64BIT(cc, cflags64)
dnl  Find out if `CC' can produce 64-bit code.
dnl  FIXME: Cache result.
AC_DEFUN(GMP_CHECK_CC_64BIT,
[
  gmp_tmp_CC_save="$CC"
  CC="[$1]"
  AC_MSG_CHECKING([whether the C compiler ($CC) is 64-bit capable])
  gmp_tmp_CFLAGS_save="$CFLAGS"
  CFLAGS="[$2]"

  case "$target" in 
    hppa2.0*-*-*)
      # FIXME: If gcc is installed under another name than "gcc", we will 
      # test the wrong thing.
      if test "$CC" != "gcc"; then
        dnl Let compiler version A.10.32.30 or higher be ok.
        dnl Bad compiler output:
        dnl   ccom: HP92453-01 G.10.32.05 HP C Compiler
        dnl Good compiler output:
        dnl   ccom: HP92453-01 A.10.32.30 HP C Compiler
        echo >conftest.c
        gmp_tmp_vs=`$CC $CFLAGS -V -c -o conftest.o conftest.c 2>&1 | grep "^ccom:"`
        rm conftest*
        gmp_tmp_v1=`echo $gmp_tmp_vs | sed 's/.* .\.\(.*\)\..*\..* HP C.*/\1/'`
        gmp_tmp_v2=`echo $gmp_tmp_vs | sed 's/.* .\..*\.\(.*\)\..* HP C.*/\1/'`
        gmp_tmp_v3=`echo $gmp_tmp_vs | sed 's/.* .\..*\..*\.\(.*\) HP C.*/\1/'`
	gmp_cv_cc_64bit=no
	test -n "$gmp_tmp_v1" && test "$gmp_tmp_v1" -ge "10" \
  	  && test -n "$gmp_tmp_v2" && test "$gmp_tmp_v2" -ge "32" \
    	  && test -n "$gmp_tmp_v3" && test "$gmp_tmp_v3" -ge "30" \
	  && gmp_cv_cc_64bit=yes
      else	# gcc
	# FIXME: Compile a minimal file and determine if the resulting object 
	# file is an ELF file.  If so, gcc can produce 64-bit code.
	# Do we have file(1) for target?
	gmp_cv_cc_64bit=no
      fi
      ;;
    mips-sgi-irix6.*)
      # We use `-n32' to cc and `-mabi=n32' to gcc, resulting in 64-bit 
      # arithmetic but not 64-bit pointers, so the general test for sizeof
      # (void *) is not valid.
      # Simply try to compile an empty main.  If that succeeds return
      # true.
      AC_TRY_COMPILE( , ,
                     gmp_cv_cc_64bit=yes, gmp_cv_cc_64bit=no,
                     gmp_cv_cc_64bit=no)
      ;;
    *-*-*)
      # FIXME: Avoid running anything on target at configure time.  Someone
      # may want to cross compile.  Instead, compile something like
      # [int pre; char arr[sizeof (void *)]; int post;]
      # and look at the distance between pre and post.
      AC_TRY_RUN([
        int main() { return (sizeof (void *) != 8); }
      ], gmp_cv_cc_64bit=yes, gmp_cv_cc_64bit=no, gmp_cv_cc_64bit=no)
      ;;
  esac

  CC="$gmp_tmp_CC_save"
  CFLAGS="$gmp_tmp_CFLAGS_save"
  AC_MSG_RESULT($gmp_cv_cc_64bit)
])dnl

dnl  GMP_INIT([M4-DEF-FILE])
dnl  
AC_DEFUN(GMP_INIT,
[ifelse([$1], , gmp_configm4=config.m4, gmp_configm4="[$1]")
gmp_tmpconfigm4=cnfm4.tmp
gmp_tmpconfigm4i=cnfm4i.tmp
gmp_tmpconfigm4p=cnfm4p.tmp
test -f $gmp_tmpconfigm4 && rm $gmp_tmpconfigm4
test -f $gmp_tmpconfigm4i && rm $gmp_tmpconfigm4i
test -f $gmp_tmpconfigm4p && rm $gmp_tmpconfigm4p
])dnl

dnl  GMP_FINISH
dnl  Create 
AC_DEFUN(GMP_FINISH,
[AC_REQUIRE([GMP_INIT])
echo "creating $gmp_configm4"
echo ["dnl $gmp_configm4.  Generated automatically by configure."] > $gmp_configm4
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
if test -f $gmp_tmpconfigm4p; then
  cat $gmp_tmpconfigm4p >> $gmp_configm4
  rm $gmp_tmpconfigm4p
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

dnl GMP_DEFINE(MACRO, DEFINITION [, LOCATION])
dnl [ Define M4 macro MACRO as DEFINITION in temporary file.		]
dnl [ If LOCATION is `POST', the definition will appear after any	]
dnl [ include() directives inserted by GMP_INCLUDE/GMP_SINCLUDE.	]
dnl [ Mind the quoting!  No shell variables will get expanded.		]
dnl [ Don't forget to invoke GMP_FINISH to create file config.m4.	]
dnl [ config.m4 uses `<' and '>' as quote characters for all defines.	]
AC_DEFUN(GMP_DEFINE, 
[AC_REQUIRE([GMP_INIT])
echo ['define(<$1>, <$2>)'] >> ifelse([$3], [POST], $gmp_tmpconfigm4p, $gmp_tmpconfigm4)
])dnl

dnl GMP_DEFINE_RAW(STRING, [, LOCATION])
dnl [ Put STRING in temporary file.					]
dnl [ If LOCATION is `POST', the definition will appear after any	]
dnl [ include() directives inserted by GMP_INCLUDE/GMP_SINCLUDE.	]
dnl [ Don't forget to invoke GMP_FINISH to create file config.m4.	]
AC_DEFUN(GMP_DEFINE_RAW,
[AC_REQUIRE([GMP_INIT])
echo [$1] >> ifelse([$2], [POST], $gmp_tmpconfigm4p, $gmp_tmpconfigm4)
])dnl

dnl  GMP_CHECK_ASM_LABEL_SUFFIX
dnl  Should a label have a colon or not?
AC_DEFUN(GMP_CHECK_ASM_LABEL_SUFFIX,
[AC_CACHE_CHECK([what assembly label suffix to use],
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
int underscore_test() {
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
[AC_REQUIRE([GMP_CHECK_ASM_GLOBL])
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
[AC_CACHE_CHECK([how the .type assembly directive should be used],
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
[AC_CACHE_CHECK([if the .size assembly directive works], gmp_cv_check_asm_size,
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
[AC_REQUIRE([GMP_CHECK_ASM_LABEL_SUFFIX])
AC_CACHE_CHECK([what prefix to use for a local label], 
gmp_cv_check_asm_lsym_prefix,
[if test -z "$NM"; then
  echo; echo ["$0: fatal: need nm"]
  exit 1
fi
ac_assemble="$CCAS $CFLAGS conftest.s 1>&AC_FD_CC"
gmp_cv_check_asm_lsym_prefix="L"
for gmp_tmp_pre in L .L $ L$; do
  cat > conftest.s <<EOF
dummy${gmp_cv_check_asm_label_suffix}
${gmp_tmp_pre}gurkmacka${gmp_cv_check_asm_label_suffix}
	.byte 0
EOF
  if AC_TRY_EVAL(ac_assemble); then
    $NM conftest.o >/dev/null 2>&1
    gmp_rc=$?
    if test "$gmp_rc" != "0"; then
      echo "configure: $NM failure, using default"
      break
    fi
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
[AC_REQUIRE([GMP_CHECK_ASM_DATA])
AC_REQUIRE([GMP_CHECK_ASM_GLOBL])
AC_REQUIRE([GMP_CHECK_ASM_LABEL_SUFFIX])
AC_CACHE_CHECK([how to [define] a 32-bit word],
	       gmp_cv_check_asm_w32,
[if test -z "$NM"; then
  echo; echo ["configure: $0: fatal: need nm"]
  exit 1
fi

# FIXME: HPUX puts first symbol at 0x40000000, breaking our assumption
# that it's at 0x0.  We'll have to declare another symbol before the
# .long/.word and look at the distance between the two symbols.  The
# only problem is that the sed expression(s) barfs (on Solaris, for
# example) for the symbol with value 0.  For now, HPUX uses .word.

case "$target" in 
  *-*-hpux*)
    gmp_cv_check_asm_w32=".word"
    ;;
  *-*-*)
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
    ;;
esac

if test -z "$gmp_cv_check_asm_w32"; then
  echo; echo ["configure: $0: fatal: do not know how to define a 32-bit word"]
  exit 1
fi
rm -f conftest*
])
echo ["define(<W32>, <$gmp_cv_check_asm_w32>)"] >> $gmp_tmpconfigm4
])

dnl  GMP_CHECK_ASM_MMX([ACTION-IF-FOUND, [ACTION-IF-NOT-FOUND]])
dnl  Can we assemble MMX insns?
AC_DEFUN(GMP_CHECK_ASM_MMX,
[AC_REQUIRE([GMP_CHECK_ASM_TEXT])
AC_CACHE_CHECK([if the assembler knows about MMX instructions],
		gmp_cv_check_asm_mmx,
[cat > conftest.s <<EOF
	$gmp_check_asm_text
	por	%mm0, %mm0
EOF
ac_assemble="$CCAS $CFLAGS conftest.s 1>&AC_FD_CC"
if AC_TRY_EVAL(ac_assemble); then
  gmp_cv_check_asm_mmx=yes
else 
  gmp_cv_check_asm_mmx=no
fi
rm -f conftest*
])
if test "$gmp_cv_check_asm_mmx" = "yes"; then
  ifelse([$1], , :, [$1])
else
  AC_MSG_WARN([+----------------------------------------------------------])
  AC_MSG_WARN([| WARNING WARNING WARNING])
  AC_MSG_WARN([| Target CPU has MMX code, but it can't be assembled by])
  AC_MSG_WARN([|     $CCAS $CFLAGS])
  AC_MSG_WARN([| Non-MMX replacements will be used.])
  AC_MSG_WARN([| This will be an inferior build.])
  AC_MSG_WARN([+----------------------------------------------------------])
  ifelse([$2], , :, [$2])
fi
])dnl

dnl  GMP_CHECK_ASM_SHLDL_CL([ACTION-IF-FOUND, [ACTION-IF-NOT-FOUND]])
AC_DEFUN(GMP_CHECK_ASM_SHLDL_CL,
[AC_REQUIRE([GMP_CHECK_ASM_TEXT])
AC_CACHE_CHECK([if the assembler takes cl with shldl],
		gmp_cv_check_asm_shldl_cl,
[cat > conftest.s <<EOF
	$gmp_check_asm_text
	shldl	%cl, %eax, %ebx
EOF
ac_assemble="$CCAS $CFLAGS conftest.s 1>&AC_FD_CC"
if AC_TRY_EVAL(ac_assemble); then
  gmp_cv_check_asm_shldl_cl=yes
else 
  gmp_cv_check_asm_shldl_cl=no
fi
rm -f conftest*
])
if test "$gmp_cv_check_asm_shldl_cl" = "yes"; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
])dnl

dnl  GMP_PROG_CC_WORKS(CC, CFLAGS, ACTION-IF-WORKS, [ACTION-IF-NOT-WORKS])
dnl  Check if CC can compile and link.  Perform various target specific tests.
dnl  FIXME: Require `$target'.
AC_DEFUN(GMP_PROG_CC_WORKS,
[AC_LANG_C	dnl  Note: Destructive.
CC="[$1]"
CFLAGS="[$2]"

# Simple test for all targets.
AC_TRY_COMPILER([int main(){return(0);}],
                tmp_works, tmp_cross)

# Target specific tests.
if test "$tmp_works" = "yes"; then
  case "$target" in 
    *-*-aix*)	# Returning a funcptr.
      AC_MSG_CHECKING([if the C compiler ($CC $CFLAGS) works (returning a function pointer)])
      AC_TRY_COMPILE( , [} void *g(); void *f() { return g(); } int bar(){],
                      tmp_works=yes, tmp_works=no)
      AC_MSG_RESULT($tmp_works)
      ;;
  esac
fi

if test "$tmp_works" = "yes"; then
  [$3]
else
  ifelse([$4], , :, [$4])
fi
])dnl

dnl  Deal with bad synchronization of Autoconf with Libtool.
AC_DEFUN(AC_CANONICAL_BUILD, [_AC_CANONICAL_BUILD])
AC_DEFUN(AC_CHECK_TOOL_PREFIX, [_AC_CHECK_TOOL_PREFIX])
