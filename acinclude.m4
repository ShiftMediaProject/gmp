dnl  GMP specific autoconf macros


dnl  Copyright 2000 Free Software Foundation, Inc.
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


dnl  GMP_HEADER_GETVAL(NAME,FILE)
dnl  ----------------------------
dnl
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
dnl
dnl  The gmp version number, extracted from the #defines in gmp.h at
dnl  autoconf time.  Two digits like 3.0 if patchlevel <= 0, or three digits
dnl  like 3.0.1 if patchlevel > 0.

define(GMP_VERSION,
[GMP_HEADER_GETVAL(__GNU_MP_VERSION,gmp.h)[]dnl
.GMP_HEADER_GETVAL(__GNU_MP_VERSION_MINOR,gmp.h)[]dnl
ifelse(m4_eval(GMP_HEADER_GETVAL(__GNU_MP_VERSION_PATCHLEVEL,gmp.h) > 0),1,
[.GMP_HEADER_GETVAL(__GNU_MP_VERSION_PATCHLEVEL,gmp.h)])])


dnl  GMP_PROG_M4
dnl  -----------
dnl
dnl  Find a working m4, either in $PATH or likely locations, and setup $M4
dnl  and an AC_SUBST accordingly.  If $M4 is already set then it's a user
dnl  choice and is accepted with no checks.  GMP_PROG_M4 is like
dnl  AC_PATH_PROG or AC_CHECK_PROG, but it tests each m4 found to see if
dnl  it's good enough.
dnl 
dnl  See mpn/asm-defs.m4 for details on the known bad m4s.

AC_DEFUN(GMP_PROG_M4,
[AC_CACHE_CHECK([for suitable m4],
                gmp_cv_prog_m4,
[if test -n "$M4"; then
  gmp_cv_prog_m4="$M4"
else
  cat >conftest.m4 <<\EOF
dnl  must protect this against being expanded during autoconf m4!
[define(dollarhash,``$][#'')dnl
ifelse(dollarhash(x),1,`define(t1,Y)',
``bad: $][# not supported (SunOS /usr/bin/m4)
'')dnl
ifelse(eval(89),89,`define(t2,Y)',
`bad: eval() doesnt support 8 or 9 in a constant (OpenBSD 2.6 m4)
')dnl
ifelse(t1`'t2,YY,`good
')dnl]
EOF
  echo "trying m4" 1>&AC_FD_CC
  gmp_tmp_val="`(m4 conftest.m4) 2>&AC_FD_CC`"
  echo "$gmp_tmp_val" 1>&AC_FD_CC
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
      echo "trying $ac_dir/m4" 1>&AC_FD_CC
      gmp_tmp_val="`($ac_dir/m4 conftest.m4) 2>&AC_FD_CC`"
      echo "$gmp_tmp_val" 1>&AC_FD_CC
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


dnl  GMP_PROG_NM
dnl  -----------
dnl  A small addition to libtool AC_PROG_NM.
dnl
dnl  FIXME: Is this -X specific to what GMP does with powerpc64, or
dnl  should it be folded into libtool somehow?
dnl
dnl  Note that if AC_PROG_NM can't find a working nm it still leaves
dnl  $NM set to "nm", so $NM can't be assumed to actually work.

AC_DEFUN(GMP_PROG_NM,
[AC_REQUIRE([AC_PROG_NM])
case "$target" in
  powerpc64*-*-aix*)
    # nm on 64-bit AIX needs to know the object file format
    NM="$NM -X 64" ;;
esac
])


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

      # Verify that the compiler works in 64-bit mode as well.
      # /usr/ucb/cc on Solaris 7 can *compile* in 64-bit mode, but not link.
      GMP_PROG_CC_WORKS($c, $c_flags,
		      	gmp_prog_cc_works=yes, 
		      	gmp_prog_cc_works=no)

      if test "$gmp_prog_cc_works" = "yes"; then
        GMP_CHECK_CC_64BIT($c, $c_flags)
        if test "$gmp_cv_cc_64bit" = "yes"; then
          test -z "$CC64" && CC64="$c"
          test -z "$CFLAGS64" && CFLAGS64="$c_flags"
	  # We have CC64 so we're done.
          break
        fi
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
dnl  Requires NM to be set to nm for target.
dnl  FIXME: Cache result.
AC_DEFUN(GMP_CHECK_CC_64BIT,
[
  gmp_tmp_CC_save="$CC"
  CC="[$1]"
  AC_MSG_CHECKING([whether the C compiler ($CC) is 64-bit capable])
  if test -z "$NM"; then
    echo; echo ["configure: $0: fatal: need nm"]
    exit 1
  fi
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
      # Allocate an array of size sizeof (void *) and use nm to determine its 
      # size.  We depend on the first declared variable being put at address 0.
      cat >conftest.c <<EOF
[char arr[sizeof (void *)]={0};
char post=0;]
EOF
      gmp_compile="$CC $CFLAGS -c conftest.c 1>&AC_FD_CC"
      if AC_TRY_EVAL(gmp_compile); then
        changequote(<,>)dnl
	gmp_tmp_val=`$NM conftest.o | grep post | sed -e 's;[[][0-9][]]\(.*\);\1;' \
          -e 's;[^1-9]*\([0-9]*\).*;\1;'`
        changequote([, ])dnl
        if test "$gmp_tmp_val" = "8"; then
	  gmp_cv_cc_64bit=yes
	else
	  gmp_cv_cc_64bit=no
        fi
      else
        echo "configure: failed program was:" >&AC_FD_CC
        cat conftest.$ac_ext >&AC_FD_CC
        gmp_cv_cc_64bit=no
      fi
      rm -f conftest*
      ;;
  esac

  CC="$gmp_tmp_CC_save"
  CFLAGS="$gmp_tmp_CFLAGS_save"
  AC_MSG_RESULT($gmp_cv_cc_64bit)
])dnl


dnl  GMP_INIT([M4-DEF-FILE])
dnl  -----------------------

AC_DEFUN(GMP_INIT,
[ifelse([$1], , gmp_configm4=config.m4, gmp_configm4="[$1]")
gmp_tmpconfigm4=cnfm4.tmp
gmp_tmpconfigm4i=cnfm4i.tmp
gmp_tmpconfigm4p=cnfm4p.tmp
test -f $gmp_tmpconfigm4 && rm $gmp_tmpconfigm4
test -f $gmp_tmpconfigm4i && rm $gmp_tmpconfigm4i
test -f $gmp_tmpconfigm4p && rm $gmp_tmpconfigm4p
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
echo ["dnl $gmp_configm4.  Generated automatically by configure."] > $gmp_configm4
if test -f $gmp_tmpconfigm4; then
  echo ["changequote(<,>)dnl"] >> $gmp_configm4
  echo ["ifdef(<__CONFIG_M4_INCLUDED__>,,<"] >> $gmp_configm4
  cat $gmp_tmpconfigm4 >> $gmp_configm4
  echo [">)"] >> $gmp_configm4
  echo ["changequote(\`,')dnl"] >> $gmp_configm4
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


dnl  GMP_INCLUDE(FILE)
dnl  -----------------

AC_DEFUN(GMP_INCLUDE,
[AC_REQUIRE([GMP_INIT])
echo ["include(\`$1')"] >> $gmp_tmpconfigm4i
])


dnl  GMP_SINCLUDE(FILE)
dnl  ------------------

AC_DEFUN(GMP_SINCLUDE,
[AC_REQUIRE([GMP_INIT])
echo ["sinclude(\`$1')"] >> $gmp_tmpconfigm4i
])


dnl  GMP_DEFINE(MACRO, DEFINITION [, LOCATION])
dnl  ------------------------------------------
dnl  Define M4 macro MACRO as DEFINITION in temporary file.
dnl
dnl  If LOCATION is `POST', the definition will appear after any include()
dnl  directives inserted by GMP_INCLUDE/GMP_SINCLUDE.  Mind the quoting!  No
dnl  shell variables will get expanded.  Don't forget to invoke GMP_FINISH
dnl  to create file config.m4.  config.m4 uses `<' and '>' as quote
dnl  characters for all defines.

AC_DEFUN(GMP_DEFINE, 
[AC_REQUIRE([GMP_INIT])
echo ['define(<$1>, <$2>)'] >> ifelse([$3], [POST], $gmp_tmpconfigm4p, $gmp_tmpconfigm4)
])


dnl  GMP_DEFINE_RAW(STRING, [, LOCATION])
dnl  ------------------------------------
dnl  Put STRING in temporary file.
dnl
dnl  If LOCATION is `POST', the definition will appear after any include()
dnl  directives inserted by GMP_INCLUDE/GMP_SINCLUDE.  Don't forget to
dnl  invoke GMP_FINISH to create file config.m4.

AC_DEFUN(GMP_DEFINE_RAW,
[AC_REQUIRE([GMP_INIT])
echo [$1] >> ifelse([$2], [POST], $gmp_tmpconfigm4p, $gmp_tmpconfigm4)
])


dnl  GMP_ASM_LABEL_SUFFIX
dnl  --------------------
dnl  Should a label have a colon or not?

AC_DEFUN(GMP_ASM_LABEL_SUFFIX,
[AC_CACHE_CHECK([what assembly label suffix to use],
                gmp_cv_asm_label_suffix,
[case "$target" in 
  *-*-hpux*) gmp_cv_asm_label_suffix=[""] ;;
  *) gmp_cv_asm_label_suffix=[":"] ;;
esac
])
echo ["define(<LABEL_SUFFIX>, <\$][1$gmp_cv_asm_label_suffix>)"] >> $gmp_tmpconfigm4
])


dnl  GMP_ASM_UNDERSCORE([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl  -------------------------------------------------------------
dnl
dnl  Deterine whether global symbols need to be prefixed with an underscore.
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
[cat > conftes1.c <<EOF
main () { underscore_test(); }
EOF
for tmp_underscore in "" "_"; do
  cat > conftes2.s <<EOF
      	$gmp_cv_asm_text
	$gmp_cv_asm_globl ${tmp_underscore}underscore_test
${tmp_underscore}underscore_test$gmp_cv_asm_label_suffix
EOF
  case "$target" in
  *-*-aix*)
    cat >> conftes2.s <<EOF
	$gmp_cv_asm_globl .${tmp_underscore}underscore_test
.${tmp_underscore}underscore_test$gmp_cv_asm_label_suffix
EOF
    ;;
  esac
  tmp_compile="$CC $CFLAGS $CPPFLAGS conftes1.c conftes2.s 1>&AC_FD_CC"
  if AC_TRY_EVAL(tmp_compile); then
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
rm -f conftes* a.out
])
if test "$gmp_cv_asm_underscore" = "yes"; then
  GMP_DEFINE(GSYM_PREFIX, [_])
  ifelse([$1], , :, [$1])
else
  GMP_DEFINE(GSYM_PREFIX, [])
  ifelse([$2], , :, [$2])
fi    
])


dnl  GMP_ASM_ALIGN_LOG([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl  ------------------------------------------------------------
dnl  Is parameter to `.align' logarithmic?

AC_DEFUN(GMP_ASM_ALIGN_LOG,
[AC_REQUIRE([GMP_ASM_GLOBL])
AC_REQUIRE([GMP_ASM_DATA])
AC_REQUIRE([GMP_ASM_LABEL_SUFFIX])
AC_REQUIRE([GMP_PROG_NM])
AC_CACHE_CHECK([if .align assembly directive is logarithmic],
               gmp_cv_asm_align_log,
[cat > conftest.s <<EOF
      	$gmp_cv_asm_data
      	.align  4
	$gmp_cv_asm_globl	foo
	.byte	1
	.align	4
foo$gmp_cv_asm_label_suffix
	.byte	2
EOF
ac_assemble="$CCAS $CFLAGS conftest.s 1>&AC_FD_CC"
if AC_TRY_EVAL(ac_assemble); then
  changequote(<,>)
  gmp_tmp_val=`$NM conftest.o | grep foo | sed -e 's;[[][0-9][]]\(.*\);\1;' \
       -e 's;[^1-9]*\([0-9]*\).*;\1;'`
  changequote([, ])dnl
  if test "$gmp_tmp_val" = "10" || test "$gmp_tmp_val" = "16"; then
    gmp_cv_asm_align_log=yes
  else
    gmp_cv_asm_align_log=no
  fi
else 
  echo "configure: failed program was:" >&AC_FD_CC
  cat conftest.s >&AC_FD_CC
fi
rm -f conftest*
])
GMP_DEFINE_RAW(["define(<ALIGN_LOGARITHMIC>,<$gmp_cv_asm_align_log>)"])
if test "$gmp_cv_asm_align_log" = "yes"; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi  
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
dnl  SCO OpenServer as
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
[cat > conftest.s <<EOF
      	$gmp_cv_asm_text
      	.align  4, 0x90
	.byte   0
      	.align  4, 0x90
EOF
if $CCAS $CFLAGS conftest.s >conftest.out 2>&1; then
  cat conftest.out 1>&AC_FD_CC
  if grep "Warning: Fill parameter ignored for executable section" conftest.out >/dev/null; then
    echo "Supressing this warning by omitting 0x90" 1>&AC_FD_CC
    gmp_cv_asm_align_fill_0x90=no
  else
    gmp_cv_asm_align_fill_0x90=yes
  fi
else
  cat conftest.out 1>&AC_FD_CC
  echo "Non-zero exit code" 1>&AC_FD_CC
  gmp_cv_asm_align_fill_0x90=no
fi
rm -f conftest*
])
GMP_DEFINE_RAW(
["define(<ALIGN_FILL_0x90>,<$gmp_cv_asm_align_fill_0x90>)"])
])


dnl  GMP_ASM_TEXT
dnl  ------------

AC_DEFUN(GMP_ASM_TEXT,
[AC_CACHE_CHECK([how to switch to text section],
                gmp_cv_asm_text,
[case "$target" in
  *-*-aix*)
    changequote({, })
    gmp_cv_asm_text={".csect .text[PR]"}
    changequote([, ])
    ;;
  *-*-hpux*) gmp_cv_asm_text=[".code"] ;;
  *) gmp_cv_asm_text=[".text"] ;;
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
[case "$target" in
  *-*-aix*)
    changequote({, })
    gmp_cv_asm_data={".csect .data[RW]"}
    changequote([, ])
    ;;
  *) gmp_cv_asm_data=[".data"] ;;
esac
])
echo ["define(<DATA>, <$gmp_cv_asm_data>)"] >> $gmp_tmpconfigm4
])


dnl  GMP_ASM_RODATA
dnl  --------------
dnl
dnl  ELF uses `.section .rodata', possibly with a `,"a"' though in gas the
dnl  flags default from the section name.
dnl
dnl  COFF looks like it might use `.section .rdata', possibly with `,"dr"'
dnl  though again gas uses defaults based on the section name.
dnl
dnl  a.out has only text, data and bss.
dnl
dnl  FIXME: It's not quite clear how to tell the difference between ELF and
dnl  COFF, so for the moment RODATA is a synonym for DATA on CPUs with split
dnl  code and data caching, or TEXT elsewhere.
dnl
dnl  i386 and i486 don't have caching but are treated the same as newer x86s
dnl  since i386 in particular is used to mean generic x86.

AC_DEFUN(GMP_ASM_RODATA,
[AC_CACHE_CHECK([how to switch to read-only data section],
                gmp_cv_asm_rodata,
[AC_REQUIRE([GMP_ASM_TEXT])
AC_REQUIRE([GMP_ASM_DATA])
case "$target" in
X86_PATTERN)
  gmp_cv_asm_rodata="$gmp_cv_asm_data" ;;
*)
  gmp_cv_asm_rodata="$gmp_cv_asm_text" ;;
esac
])
echo ["define(<RODATA>, <$gmp_cv_asm_rodata>)"] >> $gmp_tmpconfigm4
])


dnl  GMP_ASM_GLOBL
dnl  -------------
dnl  Can we say `.global'?

AC_DEFUN(GMP_ASM_GLOBL,
[AC_CACHE_CHECK([how to export a symbol],
                gmp_cv_asm_globl,
[case "$target" in
  *-*-hpux*) gmp_cv_asm_globl=[".export"] ;;
  *) gmp_cv_asm_globl=[".globl"] ;;
esac
])
echo ["define(<GLOBL>, <$gmp_cv_asm_globl>)"] >> $gmp_tmpconfigm4
])


dnl  GMP_ASM_TYPE
dnl  ------------
dnl  Can we say `.type'?

AC_DEFUN(GMP_ASM_TYPE,
[AC_CACHE_CHECK([how the .type assembly directive should be used],
                gmp_cv_asm_type,
[ac_assemble="$CCAS $CFLAGS conftest.s 1>&AC_FD_CC"
for gmp_tmp_prefix in @ \# %; do
  echo "	.type	sym,${gmp_tmp_prefix}function" > conftest.s
  if AC_TRY_EVAL(ac_assemble); then
    gmp_cv_asm_type="[.type	\$][1,${gmp_tmp_prefix}\$][2]"
    break
  fi
done
if test -z "$gmp_cv_asm_type"; then
  gmp_cv_asm_type="[dnl]"
fi
])
echo ["define(<TYPE>, <$gmp_cv_asm_type>)"] >> $gmp_tmpconfigm4
])


dnl  GMP_ASM_SIZE
dnl  ------------
dnl  Can we say `.size'?

AC_DEFUN(GMP_ASM_SIZE,
[AC_CACHE_CHECK([if the .size assembly directive works],
                 gmp_cv_asm_size,
[ac_assemble="$CCAS $CFLAGS conftest.s 1>&AC_FD_CC"
echo '	.size	sym,1' > conftest.s
if AC_TRY_EVAL(ac_assemble); then
  gmp_cv_asm_size="[.size	\$][1,\$][2]"
else
  gmp_cv_asm_size="[dnl]"
fi
])
echo ["define(<SIZE>, <$gmp_cv_asm_size>)"] >> $gmp_tmpconfigm4
])


dnl  GMP_ASM_LSYM_PREFIX
dnl  -------------------
dnl  What is the prefix for a local label?

AC_DEFUN(GMP_ASM_LSYM_PREFIX,
[AC_REQUIRE([GMP_ASM_LABEL_SUFFIX])
AC_REQUIRE([GMP_PROG_NM])
AC_CACHE_CHECK([what prefix to use for a local label], 
               gmp_cv_asm_lsym_prefix,
[ac_assemble="$CCAS $CFLAGS conftest.s 1>&AC_FD_CC"
gmp_cv_asm_lsym_prefix="L"
for gmp_tmp_pre in L .L $ L$; do
  cat > conftest.s <<EOF
dummy${gmp_cv_asm_label_suffix}
${gmp_tmp_pre}gurkmacka${gmp_cv_asm_label_suffix}
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
      gmp_cv_asm_lsym_prefix="$gmp_tmp_pre"
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
echo ["define(<LSYM_PREFIX>, <${gmp_cv_asm_lsym_prefix}>)"] >> $gmp_tmpconfigm4
])


dnl  GMP_ASM_W32
dnl  -----------
dnl  How to define a 32-bit word.

AC_DEFUN(GMP_ASM_W32,
[AC_REQUIRE([GMP_ASM_DATA])
AC_REQUIRE([GMP_ASM_GLOBL])
AC_REQUIRE([GMP_ASM_LABEL_SUFFIX])
AC_REQUIRE([GMP_PROG_NM])
AC_CACHE_CHECK([how to [define] a 32-bit word],
	       gmp_cv_asm_w32,
[# FIXME: HPUX puts first symbol at 0x40000000, breaking our assumption
# that it's at 0x0.  We'll have to declare another symbol before the
# .long/.word and look at the distance between the two symbols.  The
# only problem is that the sed expression(s) barfs (on Solaris, for
# example) for the symbol with value 0.  For now, HPUX uses .word.

case "$target" in 
  *-*-hpux*)
    gmp_cv_asm_w32=".word"
    ;;
  *-*-*)
    ac_assemble="$CCAS $CFLAGS conftest.s 1>&AC_FD_CC"
    for gmp_tmp_op in .long .word; do
      cat > conftest.s <<EOF
	$gmp_cv_asm_data
	$gmp_cv_asm_globl	foo
	$gmp_tmp_op	0
foo${gmp_cv_asm_label_suffix}
	.byte	0
EOF
      if AC_TRY_EVAL(ac_assemble); then
        changequote(<,>)
        gmp_tmp_val=`$NM conftest.o | grep foo | sed -e 's;[[][0-9][]]\(.*\);\1;' \
             -e 's;[^1-9]*\([0-9]*\).*;\1;'`
        changequote([, ])dnl
        if test "$gmp_tmp_val" = "4"; then
          gmp_cv_asm_w32="$gmp_tmp_op"
          break
        fi
      fi
    done
    ;;
esac

if test -z "$gmp_cv_asm_w32"; then
  echo; echo ["configure: $0: fatal: do not know how to define a 32-bit word"]
  exit 1
fi
rm -f conftest*
])
echo ["define(<W32>, <$gmp_cv_asm_w32>)"] >> $gmp_tmpconfigm4
])


dnl  GMP_ASM_MMX([ACTION-IF-FOUND, [ACTION-IF-NOT-FOUND]])
dnl  -----------------------------------------------------
dnl  Determine whether the assembler supports MMX instructions.
dnl
dnl  This macro is wanted before GMP_ASM_TEXT, so ".text" is hard coded
dnl  here.  ".text" is believed to be correct on all x86 systems, certainly
dnl  it's all GMP_ASM_TEXT gives currently.  Actually ".text" probably isn't
dnl  needed at all, at least for just checking instruction syntax.

AC_DEFUN(GMP_ASM_MMX,
[AC_CACHE_CHECK([if the assembler knows about MMX instructions],
		gmp_cv_asm_mmx,
[cat > conftest.s <<EOF
	.text
	por	%mm0, %mm0
EOF
ac_assemble="$CCAS $CFLAGS conftest.s 1>&AC_FD_CC"
if AC_TRY_EVAL(ac_assemble); then
  gmp_cv_asm_mmx=yes
else 
  gmp_cv_asm_mmx=no
fi
rm -f conftest*
])
if test "$gmp_cv_asm_mmx" = "yes"; then
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
])


dnl  GMP_ASM_SHLDL_CL([ACTION-IF-FOUND, [ACTION-IF-NOT-FOUND]])
dnl  ----------------------------------------------------------

AC_DEFUN(GMP_ASM_SHLDL_CL,
[AC_REQUIRE([GMP_ASM_TEXT])
AC_CACHE_CHECK([if the assembler takes cl with shldl],
               gmp_cv_asm_shldl_cl,
[cat > conftest.s <<EOF
	$gmp_cv_asm_text
	shldl	%cl, %eax, %ebx
EOF
ac_assemble="$CCAS $CFLAGS conftest.s 1>&AC_FD_CC"
if AC_TRY_EVAL(ac_assemble); then
  gmp_cv_asm_shldl_cl=yes
else 
  gmp_cv_asm_shldl_cl=no
fi
rm -f conftest*
])
if test "$gmp_cv_asm_shldl_cl" = "yes"; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
])


dnl  GMP_GCC_MARCH_PENTIUMPRO([ACTIONS-IF-GOOD][,ACTIONS-IF-BAD])
dnl  ------------------------------------------------------------
dnl
dnl  mpz/powm.c swox cvs rev 1.4 tickles a bug in gcc 2.95.2 when
dnl  -march=pentiumpro is used.  The problem appears to be fixed in 2.96, so
dnl  that option is used only on 2.96 and up.
dnl
dnl  The bug is incorrect code generated for a simple ABSIZ(z) expression in
dnl  mpz_redc(), some registers being clobbered near a cmov.  There's no
dnl  obvious reason for this, and there's many similar or identical
dnl  expressions throughout the library, so it seems wisest to disable the
dnl  option until 2.96.

AC_DEFUN(GMP_GCC_MARCH_PENTIUMPRO,
[AC_CACHE_CHECK([whether gcc -march=pentiumpro is good],
                gmp_cv_gcc_march_pentiumpro,
[tmp_major="`(gcc --version | sed -n ['s/^\([0-9][0-9]*\).*/\1/p']) 2>&AC_FD_CC`"
 tmp_minor="`(gcc --version | sed -n ['s/^[0-9][0-9]*\.\([0-9][0-9]*\).*/\1/p']) 2>&AC_FD_CC`"
echo "gcc major '$tmp_major', minor '$tmp_minor'" 1>&AC_FD_CC

gmp_cv_gcc_march_pentiumpro=no
if test -n "$tmp_major"; then
  if test "$tmp_major" -gt 2; then
    gmp_cv_gcc_march_pentiumpro=yes
  else
    if test "$tmp_major" -eq 2; then
      if test -n "$tmp_minor"; then
        if test "$tmp_minor" -ge 96; then
          gmp_cv_gcc_march_pentiumpro=yes
        fi
      fi
    fi
  fi
fi
])
if test $gmp_cv_gcc_march_pentiumpro = yes; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
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
dnl  Flags to specify PIC are taken from $ac_cv_prog_cc_pic set by
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
      mcount_nonpic_reg="`sed -n ['/esp/!s/.*movl.*,\(%[a-z]*\).*$/\1/p'] conftest.s`"
    else
      mcount_nonpic_reg=
    fi
    mcount_nonpic_call="`grep 'call.*mcount' conftest.s`"
    if test -z "$mcount_nonpic_call"; then
      AC_MSG_ERROR([Cannot find mcount call for non-PIC])
    fi
  else
    AC_MSG_ERROR([Cannot compile test program for non-PIC])
  fi
fi

if test "$enable_shared" = yes; then
  gmp_asmout_compile="$CC $CFLAGS $ac_cv_prog_cc_pic -S conftest.c 1>&AC_FD_CC"
  if AC_TRY_EVAL(gmp_asmout_compile); then
    if grep '\.data' conftest.s >/dev/null; then
      mcount_pic_reg="`sed -n ['s/.*GOTOFF.*,\(%[a-z]*\).*$/\1/p'] conftest.s`"
    else
      mcount_pic_reg=
    fi
    mcount_pic_call="`grep 'call.*mcount' conftest.s`"
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


dnl  GMP_ASM_POWERPC_REGISTERS
dnl  -------------------------
dnl
dnl  Determine whether the assembler takes powerpc registers with an "r" as
dnl  in "r6", or as plain "6".  The latter is standard, but NeXT, Rhapsody,
dnl  and MacOS-X require the "r" forms.
dnl
dnl  See also mpn/powerpc32/powerpc-defs.m4 which uses the result of this
dnl  test.

AC_DEFUN(GMP_ASM_POWERPC_REGISTERS,
[AC_REQUIRE([GMP_ASM_TEXT])
AC_CACHE_CHECK([if the assembler needs r on registers],
               gmp_cv_asm_powerpc_registers,
[cat >conftest.s <<EOF
      	$gmp_cv_asm_text
	mtctr	6
EOF
gmp_assemble="$CCAS $CFLAGS conftest.s 1>&AC_FD_CC"
if AC_TRY_EVAL(gmp_assemble); then
  gmp_cv_asm_powerpc_registers=no
else 
  gmp_cv_asm_powerpc_registers=yes
fi
rm -f conftest*
])
GMP_DEFINE_RAW(["define(<WANT_REGISTERS_R>,<$gmp_cv_asm_powerpc_registers>)"])
])


dnl  GMP_PROG_CC_WORKS(CC, CFLAGS, ACTION-IF-WORKS, [ACTION-IF-NOT-WORKS])
dnl  Check if CC can compile and link.  Perform various target specific tests.
dnl  FIXME: Require `$target'.
AC_DEFUN(GMP_PROG_CC_WORKS,
[AC_LANG_C	dnl  Note: Destructive.
CC="[$1]"
CFLAGS="[$2]"
AC_MSG_CHECKING([if the C compiler ($CC) works with flags $CFLAGS])

# Simple test for all targets.
AC_TRY_COMPILER([int main(){return(0);}],
                tmp_works, tmp_cross)

# Target specific tests.
if test "$tmp_works" = "yes"; then
  case "$target" in 
    *-*-aix*)	# Returning a funcptr.
      AC_TRY_COMPILE( , [} void *g(); void *f() { return g(); } int bar(){],
                      tmp_works=yes, tmp_works=no)
      ;;
  esac
fi

if test "$tmp_works" = "yes"; then
  [$3]
else
  ifelse([$4], , :, [$4])
fi

AC_MSG_RESULT($tmp_works)
])


dnl  GMP_C_ANSI2KNR
dnl  --------------
dnl  Setup to use ansi2knr if necessary.
dnl
dnl  The test here is simply that if an ANSI style function works then
dnl  ansi2knr isn't needed.  The normal tests for whether $CC works mean we
dnl  don't need to worry here about anything badly broken.
dnl
dnl  AM_C_PROTOTYPES is the normal way to set up ansi2knr, but (in automake
dnl  March 2000) it gives the wrong answer on a C++ compiler because its
dnl  test requires that the compiler accept both ANSI and K&R, or otherwise
dnl  ansi2knr is used.  A C++ compiler fails on the K&R part, which makes
dnl  AM_C_PROTOTYPES think it needs ansi2knr!  GMP has no bare K&R so we
dnl  only need ANSI or K&R to work, not both.

AC_DEFUN(GMP_C_ANSI2KNR,
[AC_CACHE_CHECK([if ansi2knr should be used],
                gmp_cv_c_ansi2knr,
[cat >conftest.c <<EOF
int main (int argc, char *argv[]) { return 0; }
EOF
if AC_TRY_EVAL(ac_compile); then
  gmp_cv_c_ansi2knr=no
else
  gmp_cv_c_ansi2knr=yes
fi
rm -f conftest.*
])
if test $gmp_cv_c_ansi2knr = no; then
  U= ANSI2KNR=
else
  U=_ ANSI2KNR=./ansi2knr
  # Ensure some checks needed by ansi2knr itself.
  AC_HEADER_STDC
  AC_CHECK_HEADERS(string.h)
fi
AC_SUBST(U)
AC_SUBST(ANSI2KNR)
])


dnl  GMP_C_SIZES
dnl  -----------
dnl  Probe for the sizes of various data types, if they aren't already
dnl  provided by gmp-mparam.h.  $gmp_mparam_source is the selected
dnl  gmp-mparam.h.
dnl
dnl  These sizes need to be known at pre-processing time, for use in #if
dnl  conditionals.  Most preprocessors won't accept sizeof(), GNU cpp
dnl  doesn't for instance.
dnl
dnl  The test probes by creating an array which will have a negative size if
dnl  $tmp_try doesn't equal sizeof($tmp_type), thereby inducing an error.
dnl
dnl  This method won't work if some dumb compiler treats array sizes as
dnl  unsigned or doesn't test for negatives, but requiring that exactly one
dnl  of the test values succeeds should protect against this.  Values are
dnl  also double checked by tests/t-constants.c.
dnl
dnl  When assembler code depends on particular sizes it's probably best to
dnl  put explicit #defines in gmp-mparam.h.  That way if strange compiler
dnl  options change the size of some type then the mismatch will be detected
dnl  by t-constants.c rather than only by the code crashing or giving wrong
dnl  results.

AC_DEFUN(GMP_C_SIZES,
[for tmp_pair in BITS_PER_MP_LIMB:mp_limb_t \
                 BITS_PER_LONGINT:long \
                 BITS_PER_INT:int \
                 BITS_PER_SHORTINT:short \
                 BITS_PER_CHAR:char; do
  tmp_def=`echo $tmp_pair | sed 's/:.*//'`
  tmp_type=`echo $tmp_pair | sed 's/.*://'`
  if grep "#define $tmp_def" $gmp_mparam_source >/dev/null; then
    continue
  fi
  AC_CACHE_CHECK([sizeof $tmp_type],
                 gmp_cv_c_sizes_$tmp_type,
    [tmp_val=
    rm -f conftest.out
    for tmp_try in 1 2 4 8 16 32; do
      echo "Trying $tmp_try:" >>conftest.out
      cat >conftest.c <<EOF
#include "$srcdir/gmp.h"
[int test [2*(sizeof($tmp_type) == $tmp_try) - 1];]
EOF
      cat conftest.c >>conftest.out
      echo "$CC $CFLAGS $CPPFLAGS -c conftest.c" >>conftest.out
      if ($CC $CFLAGS $CPPFLAGS -c conftest.c) >>conftest.out 2>&1; then
        if test -n "$tmp_val"; then
          cat conftest.out 1>&AC_FD_CC
          AC_MSG_ERROR([$tmp_def $tmp_type passes both $tmp_val and $tmp_try])
        fi
        tmp_val=$tmp_try
      fi
    done
    if test -z "$tmp_val"; then
      cat conftest.out 1>&AC_FD_CC
      AC_MSG_ERROR([$tmp_def $tmp_type: size not determined])
    fi
    eval gmp_cv_c_sizes_$tmp_type=$tmp_val
    ])
  eval tmp_val=\$gmp_cv_c_sizes_$tmp_type
  AC_DEFINE_UNQUOTED($tmp_def, (8*$tmp_val))
  if test $tmp_def = BITS_PER_MP_LIMB; then
    AC_DEFINE_UNQUOTED(BYTES_PER_MP_LIMB, $tmp_val)
  fi
done
rm -f conftest.*
])


dnl  Deal with bad synchronization of Autoconf with Libtool.
AC_DEFUN(AC_CANONICAL_BUILD, [_AC_CANONICAL_BUILD])
AC_DEFUN(AC_CHECK_TOOL_PREFIX, [_AC_CHECK_TOOL_PREFIX])
