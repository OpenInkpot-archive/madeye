bin_PROGRAMS = madeye
madeye_SOURCES = madeye.cpp keyhandler.cpp choicebox.cpp entrybox.cpp dialogs.cpp database.cpp

# set the include path found by configure
INCLUDES= $(all_includes) @ECORE_CFLAGS@ @EDJE_CFLAGS@ @EVAS_CFLAGS@ @SQLITE_CFLAGS@

# the library search path.
madeye_LDFLAGS = $(all_libraries) @ECORE_LIBS@ @EVAS_LIBS@ @EDJE_LIBS@ @SQLITE_LIBS@ -lecore_evas
