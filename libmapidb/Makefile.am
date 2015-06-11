if HAVE_LOCAL_LIBMAPIDB
AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/common \
	-I$(top_srcdir)/libcstring \
	-I$(top_srcdir)/liberror \
	-I$(top_srcdir)/libesedb \
	@MEMWATCH_CPPFLAGS@

noinst_LTLIBRARIES = libmapidb.la

libmapidb_la_SOURCES = \
	libmapidb_definitions.h \
	libmapidb_extern.h \
	libmapidb_database.c libmapidb_database.h \
	libmapidb_error.c libmapidb_error.h \
	libmapidb_folder.c libmapidb_folder.h \
	libmapidb_support.c libmapidb_support.h \
	libmapidb_types.h
endif

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f Makefile

splint:
	@echo "Running splint on libmapidb ..." 
	-splint -preproc -redef $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(libmapidb_la_SOURCES) 

