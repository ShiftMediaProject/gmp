divert(-1)

dnl  m4 macros for x86 assembler.


dnl  Copyright (C) 1999-2000 Free Software Foundation, Inc.
dnl 
dnl  This file is part of the GNU MP Library.
dnl
dnl  The GNU MP Library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Library General Public License as
dnl  published by the Free Software Foundation; either version 2 of the
dnl  License, or (at your option) any later version.
dnl
dnl  The GNU MP Library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Library General Public License for more details.
dnl
dnl  You should have received a copy of the GNU Library General Public
dnl  License along with the GNU MP Library; see the file COPYING.LIB.  If
dnl  not, write to the Free Software Foundation, Inc., 675 Mass Ave,
dnl  Cambridge, MA 02139, USA.


dnl  Notes:
dnl
dnl  m4 isn't perfect for processing BSD style x86 assembler code, the main
dnl  problems are,
dnl
dnl  1. Doing define(foo,123) and then using foo in an addressing mode like
dnl     foo(%ebx) expands as a macro rather than a constant.  This is worked
dnl     around by using deflit() from asm-defs.m4, instead of define().
dnl
dnl  2. Immediates in macro definitions need a space or `' to stop the $
dnl     looking like a macro parameter.  For example,
dnl
dnl  	        define(foo, `mov $ 123, %eax')
dnl
dnl     This is only a problem in macro definitions, not in ordinary text,
dnl     and not in macro parameters like text passed to forloop() or
dnl     ifdef().


deflit(BYTES_PER_MP_LIMB, 4)


dnl  --------------------------------------------------------------------------
dnl  Replacement PROLOGUE/EPILOGUE with more sophisticated error checking.
dnl  Nesting and overlapping not allowed.
dnl


dnl  Usage: PROLOGUE(functionname)
dnl
dnl  Generate a function prologue.  functionname gets GSYM_PREFIX added.
dnl  Examples,
dnl
dnl         PROLOGUE(mpn_add_n)
dnl         PROLOGUE(somefun)

define(`PROLOGUE',
m4_assert_numargs(1)
m4_assert_defined(`PROLOGUE_cpu')
`ifdef(`PROLOGUE_current_function',
`m4_error(`PROLOGUE'(`PROLOGUE_current_function') needs an `EPILOGUE'() before `PROLOGUE'($1)
)')dnl
m4_file_seen()dnl
define(`PROLOGUE_current_function',`$1')dnl
PROLOGUE_cpu(GSYM_PREFIX`'$1)')


dnl  Usage: EPILOGUE()
dnl
dnl  Notice the function name is passed to EPILOGUE_cpu(), letting it use $1
dnl  instead of the long PROLOGUE_current_function symbol.

define(`EPILOGUE',
m4_assert_numargs(0)
m4_assert_defined(`EPILOGUE_cpu')
`ifdef(`PROLOGUE_current_function',,
`m4_error(`EPILOGUE'() with no `PROLOGUE'()
)')dnl
EPILOGUE_cpu(GSYM_PREFIX`'PROLOGUE_current_function)`'dnl
undefine(`PROLOGUE_current_function')')

m4wrap(
`ifdef(`PROLOGUE_current_function',
`m4_error(`EPILOGUE() for PROLOGUE('PROLOGUE_current_function`) never seen
')')')


dnl  Usage: PROLOGUE_assert_inside()
dnl
dnl  Use this unquoted on a line on its own at the start of a macro
dnl  definition to add some code to check the macro is only used inside a
dnl  PROLOGUE/EPILOGUE pair, and that hence PROLOGUE_current_function is
dnl  defined.

define(PROLOGUE_assert_inside,
m4_assert_numargs(0)
``PROLOGUE_assert_inside_internal'(m4_doublequote($`'0))`dnl '')

define(PROLOGUE_assert_inside_internal,
m4_assert_numargs(1)
`ifdef(`PROLOGUE_current_function',,
`m4_error(`$1 used outside a PROLOGUE / EPILOGUE pair
')')')


dnl  Usage: L(labelname)
dnl         LF(functionname,labelname)
dnl
dnl  Generate a local label in the current or given function.  For LF(),
dnl  functionname gets GSYM_PREFIX added, the same as with PROLOGUE().
dnl
dnl  For example, in a function mpn_add_n (and with MPN_PREFIX __mpn),
dnl
dnl         L(bar)          => L__mpn_add_n__bar
dnl         LF(somefun,bar) => Lsomefun__bar
dnl
dnl  The funtion name and label name get two underscores between them rather
dnl  than one to guard against clashing with a separate external symbol that
dnl  happened to be called functionname_labelname.  (Though this would only
dnl  happen if the local label prefix is is empty.)  Underscores are used so
dnl  the whole label will still be a valid C identifier and so can be easily
dnl  used in gdb.

dnl  LSYM_PREFIX can be L$, so defn() is used to prevent L expanding as the
dnl  L macro and making an infinite recursion.
define(LF,
m4_assert_numargs(2)
m4_assert_defined(`LSYM_PREFIX')
`defn(`LSYM_PREFIX')GSYM_PREFIX`'$1`'__$2')

define(`L',
m4_assert_numargs(1)
PROLOGUE_assert_inside()
`LF(PROLOGUE_current_function,`$1')')


dnl  Called: PROLOGUE_cpu(gsym)
dnl          EPILOGUE_cpu(gsym)

define(PROLOGUE_cpu,
m4_assert_numargs(1)
	`GLOBL	$1
	TYPE($1,`function')
$1:')

define(EPILOGUE_cpu,
m4_assert_numargs(1)
`	SIZE($1,.-$1)')



dnl  --------------------------------------------------------------------------
dnl  Various x86 macros.
dnl


dnl  Usage: ALIGN_OFFSET(bytes,offset)
dnl
dnl  Align to `offset' away from a multiple of `bytes'.
dnl
dnl  This is useful for testing, for example align to something very strict
dnl  and see what effect offsets from it have, "ALIGN_OFFSET(256,32)".
dnl
dnl  Generally you wouldn't execute across the padding, but it's done with
dnl  nop's so it'll work.

define(ALIGN_OFFSET,
m4_assert_numargs(2)
`ALIGN($1)
forloop(`i',1,$2,`	nop
')')


dnl  Usage: defframe(name,offset)
dnl
dnl  Made a definition like the following with which to access a parameter
dnl  or variable on the stack.
dnl
dnl         define(name,`FRAME+offset(%esp)')
dnl
dnl  Actually m4_empty_if_zero(eval(FRAME+offset)) is used, which will save
dnl  one byte if FRAME+offset is zero, by putting (%esp) rather than
dnl  0(%esp).  Do define(`defframe_empty_if_zero_disabled',1) if for some
dnl  reason the zero offset is wanted.
dnl
dnl  The new definition gets a check that FRAME is actually defined when the
dnl  new macro is used, and that the final %esp offset isn't negative, which
dnl  would mean an attempt to access something below the current %esp.
dnl
dnl  Also, deflit() is used rather than a plain define(), so the new macro
dnl  won't delete any following parenthesized expression.  name(%edi) will
dnl  come out say as 16(%esp)(%edi).  This isn't valid assembler and should
dnl  provoke an error, which is better than silently giving just 16(%esp).
dnl
dnl  See README.family for more on the suggested way to access the stack
dnl  frame.

define(defframe_empty_if_zero,
`ifelse(defframe_empty_if_zero_disabled,1,`$1',`m4_empty_if_zero(`$1')')')

define(defframe_check_notbelow,
`ifelse(eval(($3)+($2)<0),1,
`m4_error(`$1 at frame offset $2 used when FRAME is only $3 bytes
')')')

define(defframe,
m4_assert_numargs(2)
`deflit(`$1',
m4_assert_defined(`FRAME')
`defframe_check_notbelow(`$1',`$2',FRAME)dnl
defframe_empty_if_zero(eval(FRAME+($2)))(%esp)')')


dnl  Usage: FRAME_pushl()
dnl         FRAME_addl_esp(n)
dnl         FRAME_subl_esp(n)
dnl
dnl  Adjust FRAME appropriately for a pushl, or for an addl or subl %esp of
dnl  n bytes.
dnl
dnl  Using these macros is completely optional.  Sometimes it makes more
dnl  sense to put explicit deflit(`FRAME',N) forms, especially when there's
dnl  jumps and different sequences of FRAME values need to be used in
dnl  different places.

define(FRAME_pushl,
m4_assert_numargs(0)
m4_assert_defined(`FRAME')
`deflit(`FRAME',eval(FRAME+4))')

define(FRAME_addl_esp,
m4_assert_numargs(1)
m4_assert_defined(`FRAME')
`deflit(`FRAME',eval(FRAME-($1)))')

define(FRAME_subl_esp,
m4_assert_numargs(1)
m4_assert_defined(`FRAME')
`deflit(`FRAME',eval(FRAME+($1)))')


dnl  Usage: defframe_pushl(name)
dnl
dnl  Do a combination of a FRAME_pushl() and a defframe() to name the stack
dnl  location just pushed.  This should come after a pushl instruction.
dnl  Putting it on the same line works and avoids lengthening the code.  For
dnl  example,
dnl
dnl         pushl   %eax    defframe_pushl(VAR_COUNTER)
dnl
dnl  Notice the defframe() is done with an unquoted -FRAME thus giving its
dnl  current value without tracking future changes.

define(defframe_pushl,
`FRAME_pushl()defframe(`$1',-FRAME)')


dnl  --------------------------------------------------------------------------
dnl  Assembler instruction macros.
dnl


dnl  Usage: femms
dnl
dnl  The gas 2.91 that comes with FreeBSD 3.4 doesn't support femms, so the
dnl  following is a replacement using .byte.

define(femms,
m4_assert_numargs(-1)
`.byte	15,14	# AMD 3DNow femms')


dnl  Usage: emms_or_femms
dnl         femms_available_p
dnl
dnl  femms_available_p expands to 1 or 0 according to whether the AMD 3DNow
dnl  femms instruction is available.  emms_or_femms expands to femms if
dnl  available, or emms if not.
dnl
dnl  This is meant for use in the K6 directory where plain K6 (without
dnl  3DNow) and K6-2 and K6-3 (with 3DNow) are supported together.  In the
dnl  K7 directory femms can be used unconditionally since all K7s have
dnl  3DNow.

define(femms_available_p,
m4_assert_numargs(-1)
`m4_ifdef_anyof_p(
	`HAVE_TARGET_CPU_k62',
	`HAVE_TARGET_CPU_k63',
	`HAVE_TARGET_CPU_athlon')')

define(emms_or_femms,
m4_assert_numargs(-1)
`ifelse(femms_available_p,1,`femms',`emms')')


dnl  Usage: jadcl0(op)
dnl
dnl  Issue a jnc/incl as a substitute for adcl $0,op.  This isn't an exact
dnl  replacement, since it doesn't set the flags like adcl does.
dnl
dnl  This finds a use in K6 mpn_addmul_1, mpn_submul_1 and mpn_mul_basecase,
dnl  because on K6 an adcl is slow, the branch misprediction penalty is
dnl  small, and the multiply algorithm used leads to a carry bit on average
dnl  only 1/4 of the time.
dnl
dnl  jadcl0_disabled can be set to 1 to instead issue an ordinary adcl for
dnl  comparison.  For example,
dnl
dnl		define(`jadcl0_disabled',1)
dnl
dnl  When using a register operand, eg. "jadcl0(%edx)", the jnc/incl code is
dnl  the same size as an adcl.  This makes it possible to use the exact same
dnl  computed jump code when testing the relative speed of jnc/incl and adcl
dnl  with jadcl0_disabled.

define(jadcl0,
m4_assert_numargs(1)
`ifelse(jadcl0_disabled,1,
	`adcl	$`'0, $1',
	`jnc	1f
	incl	$1
1:dnl')')


dnl  Usage: cmov_available_p
dnl
dnl  Expand to 1 if cmov is available, 0 if not.
dnl
dnl  The list here is all the chips that don't have cmov, on the assumption
dnl  that new chips will have it.

define(cmov_available_p,
`ifelse(m4_ifdef_anyof_p(
	`HAVE_TARGET_CPU_i386',
	`HAVE_TARGET_CPU_i486',
	`HAVE_TARGET_CPU_pentium',
	`HAVE_TARGET_CPU_pentiummmx',
	`HAVE_TARGET_CPU_k5',
	`HAVE_TARGET_CPU_k6',
	`HAVE_TARGET_CPU_k62',
	`HAVE_TARGET_CPU_k63'),1,
0,1)')


dnl  Usage: cmov_bytes(cond,src,dst,bytes)
dnl
dnl  Generate a cmov from a list of bytes, or simulated with a mov and
dnl  conditional jump if cmov isn't available.  The bytes argument should be
dnl  quoted to protect commas in it.

define(cmov_bytes,
m4_assert_numargs(4)
`ifelse(cmov_available_p,1,
	`.byte	$4	# cmov$1 $2, $3',
	`j`'cmov_bytes_oppcond(`$1')	1f	# simulated cmov$1 $2, $3
	mov	$2, $3
1:`'dnl
m4_warning(`warning, simulating cmov with jump, use for testing only
')')')

dnl  Expand to an opposite condition, eg. cmov_bytes_oppcond(z) -> nz,
dnl  or cmov_bytes_oppcond(ns) -> s.
dnl
define(cmov_bytes_oppcond,
m4_assert_numargs(1)
`ifelse(substr(`$1',0,1),n,
`substr(`$1',1)',
`n$1')')


dnl  Usage: cmovnz_ecx_ebx
dnl         ...
dnl
dnl  The gas 2.91 that comes with FreeBSD 3.4 doesn't support cmov, so the
dnl  following are replacements using .byte.  The byte values are best found
dnl  from an assembler that does know about cmov, eg. gas 2.95.  Setting
dnl  these up is tedious, so only what's needed is here.

define(`cmovnz_ecx_ebx',
m4_assert_numargs(-1)
`cmov_bytes(nz,%ecx,%ebx,`15,69,217')')

define(`cmovnz_edx_ecx',
m4_assert_numargs(-1)
`cmov_bytes(nz,%edx,%ecx,`15,69,202')')

define(`cmovz_eax_ecx',
m4_assert_numargs(-1)
`cmov_bytes(z,%eax,%ecx,`15,68,200')')

define(`cmovnz_eax_ebx',
m4_assert_numargs(-1)
`cmov_bytes(nz,%eax,%ebx,`15,69,216')')


dnl  Usage: loop_or_decljnz label
dnl
dnl  Generate either a "loop" instruction or a "decl %ecx / jnz", whichever
dnl  is better.  "loop" is better on K6 and probably on 386, on other chips
dnl  separate decl/jnz is better.
dnl
dnl  This macro is just for the x86/divmod_1.asm, divrem_1.asm and mod_1.asm
dnl  where this loop_or_decljnz variation is enough to allow the code to be
dnl  shared by all chips.

define(loop_or_decljnz,
`ifelse(loop_is_better_p,1,
	`loop',
	`decl	%ecx
	jnz')')

define(loop_is_better_p,
`m4_ifdef_anyof_p(`HAVE_TARGET_CPU_k6',
                  `HAVE_TARGET_CPU_k62',
                  `HAVE_TARGET_CPU_k63',
                  `HAVE_TARGET_CPU_i386')')

divert`'dnl
