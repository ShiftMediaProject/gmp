## Process this file with automake to generate Makefile.in

AUTOMAKE_OPTIONS = gnu

INCLUDES = -I$(top_srcdir)
LDADD = ../../libgmp.la

noinst_PROGRAMS = t-add t-sub t-conv t-sqrt t-muldiv t-dm2exp reuse t-get_d

t_add_SOURCES = t-add.c ref.c
t_sub_SOURCES = t-sub.c ref.c

check: Makefile st-add st-sub st-conv st-sqrt st-muldiv st-dm2exp st-reuse \
  st-get_d
	@echo "The tests passed."

st-add: t-add@EXEEXT@
	./t-add
	touch $@
st-sub: t-sub@EXEEXT@
	./t-sub
	touch $@
st-conv: t-conv@EXEEXT@
	./t-conv
	touch $@
st-sqrt: t-sqrt@EXEEXT@
	./t-sqrt
	touch $@
st-muldiv: t-muldiv@EXEEXT@
	./t-muldiv
	touch $@
st-dm2exp: t-dm2exp@EXEEXT@
	./t-dm2exp
	touch $@
st-reuse: reuse@EXEEXT@
	./reuse
	touch $@
st-get_d: t-get_d@EXEEXT@
	./t-get_d
	touch $@
