## Process this file with automake to generate Makefile.in

AUTOMAKE_OPTIONS = gnu

INCLUDES = -I$(top_srcdir)
LDADD = ../../libgmp.la
##LDFLAGS = -static

noinst_PROGRAMS = t-mul t-tdiv t-tdiv_ui t-fdiv t-fdiv_ui t-gcd \
  dive t-sqrtrem convert io logic bit t-powm t-powm_ui t-pow_ui t-2exp reuse

check: Makefile st-mul st-tdiv st-tdiv_ui st-fdiv st-fdiv_ui st-gcd st-dive \
  st-sqrtrem st-convert st-io st-logic st-bit st-powm st-powm_ui st-pow_ui \
  st-2exp st-reuse
	@echo "The tests passed."

st-mul: t-mul@EXEEXT@
	./t-mul
	touch $@
st-tdiv: t-tdiv@EXEEXT@
	./t-tdiv
	touch $@
st-tdiv_ui: t-tdiv_ui@EXEEXT@
	./t-tdiv_ui
	touch $@
st-fdiv: t-fdiv@EXEEXT@
	./t-fdiv
	touch $@
st-fdiv_ui: t-fdiv_ui@EXEEXT@
	./t-fdiv_ui
	touch $@
st-gcd: t-gcd@EXEEXT@
	./t-gcd
	touch $@
st-dive: dive@EXEEXT@
	./dive
	touch $@
st-sqrtrem: t-sqrtrem@EXEEXT@
	./t-sqrtrem
	touch $@
st-convert: convert@EXEEXT@
	./convert
	touch $@
st-io: io@EXEEXT@
	./io
	touch $@
st-logic: logic@EXEEXT@
	./logic
	touch $@
st-bit: bit@EXEEXT@
	./bit
	touch $@
st-powm: t-powm@EXEEXT@
	./t-powm
	touch $@
st-powm_ui: t-powm_ui@EXEEXT@
	./t-powm_ui
	touch $@
st-pow_ui: t-pow_ui@EXEEXT@
	./t-pow_ui
	touch $@
st-2exp: t-2exp@EXEEXT@
	./t-2exp
	touch $@
st-reuse: reuse@EXEEXT@
	./reuse
	touch $@
