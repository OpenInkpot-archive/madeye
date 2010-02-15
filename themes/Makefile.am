MAINTAINERCLEANFILES = Makefile.in

EDC= ${srcdir}/madeye.edc
EDJ = $(EDC:%.edc=%.edj)

EXTRA_DIST = $(EDC)
CLEANFILES = $(EDJ)

themedir = $(pkgdatadir)/themes
theme_DATA = $(EDJ)

all: $(EDJ)
%.edj: %.edc
	edje_cc -id ${srcdir} -id  ${srcdir}/.. -o $@ $<
