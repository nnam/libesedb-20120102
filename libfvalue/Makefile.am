if HAVE_LOCAL_LIBFVALUE
AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/common \
	-I$(top_srcdir)/libcstring \
	-I$(top_srcdir)/liberror \
	-I$(top_srcdir)/libnotify \
	@LIBUNA_CPPFLAGS@ \
	@LIBFDATETIME_CPPFLAGS@ \
	@LIBFGUID_CPPFLAGS@ \
	@MEMWATCH_CPPFLAGS@

noinst_LTLIBRARIES = libfvalue.la

libfvalue_la_SOURCES = \
	libfvalue_array_type.c libfvalue_array_type.h \
	libfvalue_codepage.h \
	libfvalue_debug.c libfvalue_debug.h \
	libfvalue_definitions.h \
	libfvalue_extern.h \
	libfvalue_libfdatetime.h \
	libfvalue_libfguid.h \
	libfvalue_libuna.h \
	libfvalue_split_string.c libfvalue_split_string.h \
	libfvalue_string.c libfvalue_string.h \
	libfvalue_types.h \
	libfvalue_table.c libfvalue_table.h \
	libfvalue_value.c libfvalue_value.h \
	libfvalue_value_entry.c libfvalue_value_entry.h
endif

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f Makefile

splint:
	@echo "Running splint on libfvalue ..." 
	-splint -preproc -redef $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(libfvalue_la_SOURCES) 

