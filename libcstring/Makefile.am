AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/common \
	@MEMWATCH_CPPFLAGS@

noinst_LTLIBRARIES = libcstring.la

libcstring_la_SOURCES = \
	libcstring.h \
	libcstring_codepage.h \
	libcstring_definitions.h \
	libcstring_locale.c libcstring_locale.h \
	libcstring_narrow_string.h \
	libcstring_string.h \
	libcstring_system_string.c libcstring_system_string.h \
	libcstring_types.h \
	libcstring_wide_string.c libcstring_wide_string.h

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f Makefile

splint:
	@echo "Running splint on libcstring ..." 
	-splint -preproc -redef $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(libcstring_la_SOURCES) 

