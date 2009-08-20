bin_PROGRAMS = madeye
madeye_SOURCES = madeye.c 
#keyhandler.cpp choicebox.cpp entrybox.cpp dialogs.cpp database.cpp

# set the include path found by configure
INCLUDES= $(all_includes) @ECORE_CFLAGS@ @EDJE_CFLAGS@ @EVAS_CFLAGS@ @SQLITE_CFLAGS@ @ECORE_FILE_CFLAGS@ @ECORE_EVAS_CFLAGS@ 
#@EFREET_MIME_CFLAGS@

# the library search path.
madeye_LDFLAGS = $(all_libraries) @ECORE_LIBS@ @EVAS_LIBS@ @EDJE_LIBS@ @SQLITE_LIBS@ @ECORE_FILE_LIBS@ @ECORE_EVAS_LIBS@ 
#@EFREET_MIME_LIBS@
