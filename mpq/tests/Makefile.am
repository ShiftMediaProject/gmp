## Process this file with automake to generate Makefile.in

AUTOMAKE_OPTIONS = gnu

INCLUDES = -I$(top_srcdir) -I../..
LDADD = ../../libgmp.la

noinst_PROGRAMS = t-cmp t-cmp_ui t-get_d

check: Makefile st-cmp st-cmp_ui st-get_d
	@echo "The tests passed."

st-cmp: t-cmp@EXEEXT@
	./t-cmp
	touch $@
st-cmp_ui: t-cmp_ui@EXEEXT@
	./t-cmp_ui
	touch $@
st-get_d: t-get_d@EXEEXT@
	./t-get_d
	touch $@

