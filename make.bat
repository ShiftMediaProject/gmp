cd mpn
copy msdos\asm-syntax.h asm-syntax.h
copy bsd.h sysdep.h
copy generic\inlines.c inlines.c
copy x86\pentium\add_n.S add_n.S
copy x86\pentium\addmul_1.S addmul_1.S
copy generic\cmp.c cmp.c
copy generic\divmod_1.c divmod_1.c 
copy generic\divrem.c divrem.c
copy generic\divrem_1.c divrem_1.c
copy generic\divrem_n.c divrem_n.c
copy generic\dump.c dump.c
copy x86\pentium\lshift.S lshift.S
copy generic\mod_1.c mod_1.c
copy generic\mul.c mul.c
copy x86\pentium\mul_1.S mul_1.S
copy generic\mul_n.c mul_n.c
copy generic\random.c random.c
copy generic\random2.c random2.c
copy generic\rawrandom.c rawrandom.c
copy x86\pentium\rshift.S rshift.S
copy generic\sqrtrem.c sqrtrem.c
copy x86\pentium\sub_n.S sub_n.S
copy x86\pentium\submul_1.S submul_1.S
copy generic\get_str.c get_str.c
copy generic\set_str.c set_str.c
copy generic\scan0.c scan0.c
copy generic\scan1.c scan1.c
copy generic\popcount.c popcount.c
copy generic\hamdist.c hamdist.c
copy generic\gcd_1.c gcd_1.c
copy generic\pre_mod_.c pre_mod_.c
copy generic\perfsqr.c perfsqr.c
copy generic\bdivmod.c bdivmod.c
copy generic\gcd.c gcd.c
copy generic\gcdext.c gcdext.c
copy generic\gmp-mpar.h gmp-mpar.h
cd ..

cd mpbsd
copy ..\mpz\add.c add.c
copy ..\mpz\cmp.c cmp.c
copy ..\mpz\gcd.c gcd.c
copy ..\mpz\mul.c mul.c
copy ..\mpz\pow_ui.c pow_ui.c
copy ..\mpz\powm.c powm.c
copy ..\mpz\sqrtrem.c sqrtrem.c
copy ..\mpz\sub.c sub.c
cd ..

cd mpn
gcc -c -I. -I.. -g -O mp_bases.c
gcc -c -I. -I.. -g -O inlines.c
gcc -E -I. -I.. -g -O add_n.S | grep -v '^#' >tmp-add_n.s
gcc -c tmp-add_n.s -o add_n.o
del tmp-add_n.s
gcc -E -I. -I.. -g -O addmul_1.S | grep -v '^#' >tmp-addmul_1.s
gcc -c tmp-addmul_1.s -o addmul_1.o
del tmp-addmul_1.s
gcc -c -I. -I.. -g -O cmp.c
gcc -c -I. -I.. -g -O divmod_1.c
gcc -c -I. -I.. -g -O divrem.c
gcc -c -I. -I.. -g -O divrem_1.c
gcc -c -I. -I.. -g -O divrem_2.c
gcc -c -I. -I.. -g -O divrem_n.c
gcc -c -I. -I.. -g -O dump.c
gcc -E -I. -I.. -g -O lshift.S | grep -v '^#' >tmp-lshift.s
gcc -c tmp-lshift.s -o lshift.o
del tmp-lshift.s
gcc -c -I. -I.. -g -O mod_1.c
gcc -c -I. -I.. -g -O mul.c
gcc -E -I. -I.. -g -O mul_1.S | grep -v '^#' >tmp-mul_1.s
gcc -c tmp-mul_1.s -o mul_1.o
del tmp-mul_1.s
gcc -c -I. -I.. -g -O mul_basecase.c
gcc -c -I. -I.. -g -O mul_n.c
gcc -c -I. -I.. -g -O random.c
gcc -c -I. -I.. -g -O random2.c
gcc -E -I. -I.. -g -O rshift.S | grep -v '^#' >tmp-rshift.s
gcc -c tmp-rshift.s -o rshift.o
del tmp-rshift.s
gcc -c -I. -I.. -g -O sqr_basecase.c
gcc -c -I. -I.. -g -O sqrtrem.c
gcc -E -I. -I.. -g -O sub_n.S | grep -v '^#' >tmp-sub_n.s
gcc -c tmp-sub_n.s -o sub_n.o
del tmp-sub_n.s
gcc -E -I. -I.. -g -O submul_1.S | grep -v '^#' >tmp-submul_1.s
gcc -c tmp-submul_1.s -o submul_1.o
del tmp-submul_1.s
gcc -c -I. -I.. -fomit-frame-pointer -O get_str.c
gcc -c -I. -I.. -fomit-frame-pointer -O set_str.c
gcc -c -I. -I.. -fomit-frame-pointer -O scan0.c
gcc -c -I. -I.. -fomit-frame-pointer -O scan1.c
gcc -c -I. -I.. -fomit-frame-pointer -O popcount.c
gcc -c -I. -I.. -fomit-frame-pointer -O hamdist.c
gcc -c -I. -I.. -fomit-frame-pointer -O gcd_1.c
gcc -c -I. -I.. -fomit-frame-pointer -O pre_mod_.c
gcc -c -I. -I.. -fomit-frame-pointer -O perfsqr.c
gcc -c -I. -I.. -fomit-frame-pointer -O bdivmod.c
gcc -c -I. -I.. -fomit-frame-pointer -O gcd.c
gcc -c -I. -I.. -fomit-frame-pointer -O gcdext.c
del libmpn.a
ar rc libmpn.a *.o
cd ..

cd mpz
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O abs.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O add.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O add_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O addmul_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O and.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O array_init.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O bin_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O bin_uiui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O cdiv_q.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O cdiv_q_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O cdiv_qr.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O cdiv_qr_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O cdiv_r.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O cdiv_r_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O cdiv_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O clear.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O clrbit.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O cmp.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O cmp_si.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O cmp_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O cmp_abs.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O cmp_abs_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O com.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O divexact.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O dump.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O fac_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O fdiv_q.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O fdiv_q_2exp.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O fdiv_q_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O fdiv_qr.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O fdiv_qr_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O fdiv_r.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O fdiv_r_2exp.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O fdiv_r_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O fdiv_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O fib_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O gcd.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O gcd_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O gcdext.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O get_d.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O get_si.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O get_str.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O get_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O getlimbn.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O hamdist.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O init.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O inp_raw.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O inp_str.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O invert.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O ior.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O iset.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O iset_d.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O iset_si.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O iset_str.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O iset_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O jacobi.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O lcm.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O legendre.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O mod.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O mul.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O mul_2exp.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O mul_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O neg.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O out_raw.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O out_str.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O perfpow.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O perfsqr.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O popcount.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O pow_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O powm.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O powm_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O pprime_p.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O random.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O random2.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O realloc.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O remove.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O root.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O scan0.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O scan1.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O set.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O set_d.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O set_f.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O set_q.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O set_si.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O set_str.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O set_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O setbit.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O size.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O sizeinbase.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O sqrt.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O sqrtrem.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O sub.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O sub_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O swap.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O tdiv_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O tdiv_q.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O tdiv_q_2exp.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O tdiv_q_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O tdiv_qr.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O tdiv_qr_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O tdiv_r.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O tdiv_r_2exp.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O tdiv_r_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O tstbit.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O ui_pow_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O urandom.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O xor.c
del libmpz.a
ar rc libmpz.a *.o
cd ..

cd mpf
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O abs.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O add.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O add_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O clear.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O cmp.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O cmp_si.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O cmp_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O div.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O div_2exp.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O div_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O dump.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O eq.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O get_d.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O get_prc.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O get_str.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O init.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O init2.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O inp_str.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O iset.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O iset_d.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O iset_si.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O iset_str.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O iset_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O mul.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O mul_2exp.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O mul_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O neg.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O out_str.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O pow_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O random2.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O reldiff.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O set.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O set_d.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O set_dfl_prec.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O set_prc.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O set_prc_raw.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O set_q.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O set_si.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O set_str.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O set_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O set_z.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O size.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O sqrt.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O sqrt_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O sub.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O sub_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O ui_div.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O ui_sub.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O urandom.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O -DMPF_FLOOR integer.c
move/y integer.o floor.o
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O -DMPF_CEIL integer.c
move/y integer.o ceil.o
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O -DMPF_TRUNC integer.c
move/y integer.o trunc.o
del libmpf.a
ar cr libmpf.a *.o
cd ..

cd mpq
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O add.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O canonicalize.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O clear.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O cmp.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O cmp_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O div.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O equal.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O get_d.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O get_den.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O get_num.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O init.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O inv.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O mul.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O neg.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O set.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O set_den.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O set_num.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O set_si.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O set_ui.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O set_z.c
gcc -c -I. -I.. -I../mpn -fomit-frame-pointer -O sub.c
del libmpq.a
ar cr libmpq.a *.o
cd ..

gcc -c -I. -Impn -I.. -fomit-frame-pointer -O extract-dbl.c
gcc -c -I. -Impn -I.. -fomit-frame-pointer -O insert-dbl.c
gcc -c -I. -Impn -I.. -fomit-frame-pointer -O memory.c
gcc -c -I. -Impn -I.. -fomit-frame-pointer -O mp_clz_tab.c
gcc -c -I. -Impn -I.. -fomit-frame-pointer -O mp_set_fns.c
gcc -c -I. -Impn -I.. -fomit-frame-pointer -O stack-alloc.c
gcc -c -I. -Impn -I.. -fomit-frame-pointer -O rand.c
gcc -c -I. -Impn -I.. -fomit-frame-pointer -O randui.c
gcc -c -I. -Impn -I.. -fomit-frame-pointer -O randclr.c
gcc -c -I. -Impn -I.. -fomit-frame-pointer -O randlc.c
gcc -c -I. -Impn -I.. -fomit-frame-pointer -O randlcui.c
gcc -c -I. -Impn -I.. -fomit-frame-pointer -O version.c
deltree/y tmpdir

md tmpdir

md tmpdir\mpn
cd tmpdir\mpn
ar x ../../mpn/libmpn.a
cd ..\..

md tmpdir\mpz
cd tmpdir\mpz
ar x ../../mpz/libmpz.a
cd ..\..

md tmpdir\mpq
cd tmpdir\mpq
ar x ../../mpq/libmpq.a
cd ..\..

md tmpdir\mpf
cd tmpdir\mpf
ar x ../../mpf/libmpf.a
cd ..\..

copy memory.o tmpdir
copy mp_set_fns.o tmpdir
copy mp_clz_tab.o tmpdir
copy version.o tmpdir
copy stack-alloc.o tmpdir

cd tmpdir
ar rc libgmp.a *.o */*.o
ranlib libgmp.a
cd ..

move/y tmpdir\libgmp.a libgmp.a
deltree/y tmpdir
