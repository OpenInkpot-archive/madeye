AC_INIT(configure.in)

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(madeye, 0.1)

AC_PROG_CC_C99
AM_PROG_LIBTOOL

PKG_CHECK_MODULES(EFL,[ecore ecore-file evas ecore-evas efreet-mime])

AC_OUTPUT(Makefile src/Makefile data/Makefile)
