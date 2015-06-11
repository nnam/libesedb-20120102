if HAVE_LOCAL_LIBBFIO
AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/common \
	-I$(top_srcdir)/libcstring \
	-I$(top_srcdir)/liberror \
	@LIBUNA_CPPFLAGS@ \
	@MEMWATCH_CPPFLAGS@

noinst_LTLIBRARIES = libbfio.la

libbfio_la_SOURCES = \
	libbfio_codepage.h \
	libbfio_definitions.h \
	libbfio_error.c libbfio_error.h \
	libbfio_error_string.c libbfio_error_string.h \
	libbfio_extern.h \
	libbfio_file.c libbfio_file.h \
	libbfio_file_pool.c libbfio_file_pool.h \
	libbfio_file_range.c libbfio_file_range.h \
	libbfio_handle.c libbfio_handle.h \
	libbfio_libuna.h \
	libbfio_list_type.c libbfio_list_type.h \
	libbfio_memory_range.c libbfio_memory_range.h \
	libbfio_narrow_split_string.c libbfio_narrow_split_string.h \
	libbfio_narrow_string.c libbfio_narrow_string.h \
	libbfio_offset_list.c libbfio_offset_list.h \
	libbfio_path.c libbfio_path.h \
	libbfio_pool.c libbfio_pool.h \
	libbfio_support.c libbfio_support.h \
	libbfio_types.h \
	libbfio_unused.h \
	libbfio_wide_split_string.c libbfio_wide_split_string.h \
	libbfio_wide_string.c libbfio_wide_string.h
endif

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f Makefile

splint:
	@echo "Running splint on libbfio ..." 
	-splint -preproc -redef $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(libbfio_la_SOURCES) 

