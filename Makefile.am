## Process this file with automake to generate Makefile.in

## FIXME: We disable dependencies (`no-dependencies') for now, since 
## it doesn't owrk.  (Our idea of overriding [AC_PROG_CC] may have 
## something to do with it.)
#AUTOMAKE_OPTIONS = gnu no-dependencies
AUTOMAKE_OPTIONS = gnu

SUBDIRS = mpf mpz mpn mpq

## FIXME: For now, each object file going into libgmp has to be listed here.
##        The accompanying sources are listed in their appropriate Makefile.am.
MPF_OBJECTS = \
  mpf/init.lo \
  mpf/init2.lo \
  mpf/set.lo \
  mpf/set_ui.lo \
  mpf/set_si.lo \
  mpf/set_str.lo \
  mpf/set_d.lo \
  mpf/set_z.lo \
  mpf/iset.lo \
  mpf/iset_ui.lo \
  mpf/iset_si.lo \
  mpf/iset_str.lo \
  mpf/iset_d.lo \
  mpf/clear.lo \
  mpf/get_str.lo \
  mpf/dump.lo \
  mpf/size.lo \
  mpf/eq.lo \
  mpf/reldiff.lo \
  mpf/sqrt.lo \
  mpf/random2.lo \
  mpf/inp_str.lo \
  mpf/out_str.lo \
  mpf/add.lo \
  mpf/add_ui.lo \
  mpf/sub.lo \
  mpf/sub_ui.lo \
  mpf/ui_sub.lo \
  mpf/mul.lo \
  mpf/mul_ui.lo \
  mpf/div.lo \
  mpf/div_ui.lo \
  mpf/cmp.lo \
  mpf/cmp_ui.lo \
  mpf/cmp_si.lo \
  mpf/mul_2exp.lo \
  mpf/div_2exp.lo \
  mpf/abs.lo \
  mpf/neg.lo \
  mpf/set_q.lo \
  mpf/get_d.lo \
  mpf/set_dfl_prec.lo \
  mpf/set_prc.lo \
  mpf/set_prc_raw.lo \
  mpf/get_prc.lo \
  mpf/ui_div.lo \
  mpf/sqrt_ui.lo \
  mpf/floor.lo \
  mpf/ceil.lo \
  mpf/trunc.lo \
  mpf/pow_ui.lo \
  mpf/urandom.lo
MPZ_OBJECTS = \
  mpz/abs.lo \
  mpz/add.lo \
  mpz/add_ui.lo \
  mpz/addmul_ui.lo \
  mpz/and.lo \
  mpz/array_init.lo \
  mpz/bin_ui.lo \
  mpz/bin_uiui.lo \
  mpz/cdiv_q.lo \
  mpz/cdiv_q_ui.lo \
  mpz/cdiv_qr.lo \
  mpz/cdiv_qr_ui.lo \
  mpz/cdiv_r.lo \
  mpz/cdiv_r_ui.lo \
  mpz/cdiv_ui.lo \
  mpz/clear.lo \
  mpz/clrbit.lo \
  mpz/cmp.lo \
  mpz/cmp_si.lo \
  mpz/cmp_ui.lo \
  mpz/cmp_abs.lo \
  mpz/cmp_abs_ui.lo \
  mpz/com.lo \
  mpz/divexact.lo \
  mpz/dump.lo \
  mpz/fac_ui.lo \
  mpz/fdiv_q.lo \
  mpz/fdiv_q_2exp.lo \
  mpz/fdiv_q_ui.lo \
  mpz/fdiv_qr.lo \
  mpz/fdiv_qr_ui.lo \
  mpz/fdiv_r.lo \
  mpz/fdiv_r_2exp.lo \
  mpz/fdiv_r_ui.lo \
  mpz/fdiv_ui.lo \
  mpz/fib_ui.lo \
  mpz/gcd.lo \
  mpz/gcd_ui.lo \
  mpz/gcdext.lo \
  mpz/get_d.lo \
  mpz/get_si.lo \
  mpz/get_str.lo \
  mpz/get_ui.lo \
  mpz/getlimbn.lo \
  mpz/hamdist.lo \
  mpz/init.lo \
  mpz/inp_raw.lo \
  mpz/inp_str.lo \
  mpz/invert.lo \
  mpz/ior.lo \
  mpz/iset.lo \
  mpz/iset_d.lo \
  mpz/iset_si.lo \
  mpz/iset_str.lo \
  mpz/iset_ui.lo \
  mpz/jacobi.lo \
  mpz/lcm.lo \
  mpz/legendre.lo \
  mpz/mod.lo \
  mpz/mul.lo \
  mpz/mul_2exp.lo \
  mpz/mul_ui.lo \
  mpz/neg.lo \
  mpz/out_raw.lo \
  mpz/out_str.lo \
  mpz/perfpow.lo \
  mpz/perfsqr.lo \
  mpz/popcount.lo \
  mpz/pow_ui.lo \
  mpz/powm.lo \
  mpz/powm_ui.lo \
  mpz/pprime_p.lo \
  mpz/random.lo \
  mpz/random2.lo \
  mpz/realloc.lo \
  mpz/remove.lo \
  mpz/root.lo \
  mpz/scan0.lo \
  mpz/scan1.lo \
  mpz/set.lo \
  mpz/set_d.lo \
  mpz/set_f.lo \
  mpz/set_q.lo \
  mpz/set_si.lo \
  mpz/set_str.lo \
  mpz/set_ui.lo \
  mpz/setbit.lo \
  mpz/size.lo \
  mpz/sizeinbase.lo \
  mpz/sqrt.lo \
  mpz/sqrtrem.lo \
  mpz/sub.lo \
  mpz/sub_ui.lo \
  mpz/swap.lo \
  mpz/tdiv_ui.lo \
  mpz/tdiv_q.lo \
  mpz/tdiv_q_2exp.lo \
  mpz/tdiv_q_ui.lo \
  mpz/tdiv_qr.lo \
  mpz/tdiv_qr_ui.lo \
  mpz/tdiv_r.lo \
  mpz/tdiv_r_2exp.lo \
  mpz/tdiv_r_ui.lo \
  mpz/tstbit.lo \
  mpz/ui_pow_ui.lo \
  mpz/urandom.lo \
  mpz/xor.lo
MPQ_OBJECTS = \
  mpq/add.lo \
  mpq/canonicalize.lo \
  mpq/clear.lo \
  mpq/cmp.lo \
  mpq/cmp_ui.lo \
  mpq/div.lo \
  mpq/get_d.lo \
  mpq/get_den.lo \
  mpq/get_num.lo \
  mpq/init.lo \
  mpq/inv.lo \
  mpq/mul.lo \
  mpq/neg.lo \
  mpq/set.lo \
  mpq/set_den.lo \
  mpq/set_num.lo \
  mpq/set_si.lo \
  mpq/set_ui.lo \
  mpq/sub.lo \
  mpq/equal.lo \
  mpq/set_z.lo
MPN_OBJECTS = @mpn_objs_in_libgmp@

## FIXME: Remove config.in when AM_CONFIG_HEADER is used in configure.in.
EXTRA_DIST = demos gmp-impl.h gmp.h longlong.h mp.h stack-alloc.h urandom.h \
  config.in

info_TEXINFOS = gmp.texi

lib_LTLIBRARIES = libgmp.la
libgmp_la_SOURCES = \
  assert.c memory.c mp_set_fns.c mp_clz_tab.c rand.c randui.c randclr.c \
  randlc.c randlcui.c version.c stack-alloc.c mp_bpl.c extract-dbl.c \
  insert-dbl.c
libgmp_la_LIBADD = \
	$(MPF_OBJECTS) $(MPZ_OBJECTS) $(MPN_OBJECTS) $(MPQ_OBJECTS)
libgmp_la_DEPENDENCIES = \
	$(MPF_OBJECTS) $(MPZ_OBJECTS) $(MPN_OBJECTS) $(MPQ_OBJECTS)

## FIXME: --version-info is on the form `current[:revision[:age]]' (see libtool doc)
## Hint: See binutils/bfd/Makefile.am
## libmpn_la_LDFLAGS = -version-info @VERSION@

# FIXME: Build libmp.a?

check: all
	cd mpz/tests && $(MAKE) $(FLAGS_TO_PASS) check
	cd mpq/tests && $(MAKE) $(FLAGS_TO_PASS) check
	cd mpf/tests && $(MAKE) $(FLAGS_TO_PASS) check

# Don't ship CVS directories.
dist-hook:
	-find $(distdir) \( -name CVS -type d \) -o -name "*.~*" \
		| xargs rm -rf
