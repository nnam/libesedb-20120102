if HAVE_LOCAL_LIBFWNT
AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/common \
	-I$(top_srcdir)/libcstring \
	-I$(top_srcdir)/liberror \
	@MEMWATCH_CPPFLAGS@

noinst_LTLIBRARIES = libfwnt.la

libfwnt_la_SOURCES = \
	libfwnt_definitions.h \
	libfwnt_extern.h \
	libfwnt_error.c libfwnt_error.h \
	libfwnt_security_identifier.c libfwnt_security_identifier.h \
	libfwnt_support.c libfwnt_support.h \
	libfwnt_types.h
endif

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f Makefile

splint:
	@echo "Running splint on libfwnt ..." 
	-splint -preproc -redef $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(libfwnt_la_SOURCES) 

