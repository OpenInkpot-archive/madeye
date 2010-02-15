bin_PROGRAMS = madeye
madeye_SOURCES = madeye.c keyboard.c

AM_CFLAGS = --std=gnu99 -W -Wall \
    -DTHEME_DIR='"$(pkgdatadir)/themes"'

INCLUDES= $(all_includes) @EFL_CFLAGS@ @LIBKEYS_CFLAGS@ @LIBEOI_CFLAGS@ @LIBCHOICEBOX_CFLAGS@

madeye_LDFLAGS = $(all_libraries) @EFL_LIBS@ @LIBKEYS_LIBS@ @LIBEOI_LIBS@ @LIBCHOICEBOX_LIBS@
