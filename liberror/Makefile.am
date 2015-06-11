AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/common \
	-I$(top_srcdir)/libcstring \
	@MEMWATCH_CPPFLAGS@

noinst_LTLIBRARIES = liberror.la

liberror_la_SOURCES = \
	liberror.h \
	liberror_definitions.h \
	liberror_error.c liberror_error.h \
	liberror_types.h

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f Makefile

splint:
	@echo "Running splint on liberror ..." 
	-splint -preproc -redef $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(liberror_la_SOURCES) 

