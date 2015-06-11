AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/common \
	-I$(top_srcdir)/libcstring \
	-I$(top_srcdir)/liberror \
	@MEMWATCH_CPPFLAGS@

noinst_LTLIBRARIES = libnotify.la

libnotify_la_SOURCES = \
	libnotify.c libnotify.h \
	libnotify_print.c libnotify_print.h \
	libnotify_stream.c libnotify_stream.h

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f Makefile

splint:
	@echo "Running splint on libnotify ..." 
	-splint -preproc -redef $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(libnotify_la_SOURCES) 

