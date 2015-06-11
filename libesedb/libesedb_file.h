/*
 * File functions
 *
 * Copyright (c) 2009-2012, Joachim Metz <jbmetz@users.sourceforge.net>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#if !defined( _LIBESEDB_INTERNAL_FILE_H )
#define _LIBESEDB_INTERNAL_FILE_H

#include <common.h>
#include <types.h>

#include <liberror.h>

#include "libesedb_catalog.h"
#include "libesedb_database.h"
#include "libesedb_extern.h"
#include "libesedb_io_handle.h"
#include "libesedb_libbfio.h"
#include "libesedb_libfcache.h"
#include "libesedb_libfdata.h"

#if defined( _MSC_VER ) || defined( __BORLANDC__ ) || defined( __MINGW32_VERSION ) || defined( __MINGW64_VERSION_MAJOR )

/* This inclusion is needed otherwise some linkers
 * mess up exporting the legacy functions
 */
#include "libesedb_legacy.h"
#endif

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libesedb_internal_file libesedb_internal_file_t;

struct libesedb_internal_file
{
	/* The IO handle
	 */
	libesedb_io_handle_t *io_handle;

	/* The file IO handle
	 */
	libbfio_handle_t *file_io_handle;

	/* Value to indicate if the file IO handle was created inside the library
	 */
	uint8_t file_io_handle_created_in_library;

	/* The pages vector
	 */
	libfdata_vector_t *pages_vector;

	/* The pages cache
	 */
	libfcache_cache_t *pages_cache;

	/* The database
	 */
	libesedb_database_t *database;

	/* The catalog
	 */
	libesedb_catalog_t *catalog;

	/* Value to indicate if abort was signalled
	 */
	int abort;
};

LIBESEDB_EXTERN \
int libesedb_file_initialize(
     libesedb_file_t **file,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_file_free(
     libesedb_file_t **file,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_file_signal_abort(
     libesedb_file_t *file,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_file_open(
     libesedb_file_t *file,
     const char *filename,
     int access_flags,
     liberror_error_t **error );

#if defined( HAVE_WIDE_CHARACTER_TYPE )
LIBESEDB_EXTERN \
int libesedb_file_open_wide(
     libesedb_file_t *file,
     const wchar_t *filename,
     int access_flags,
     liberror_error_t **error );
#endif

LIBESEDB_EXTERN \
int libesedb_file_open_file_io_handle(
     libesedb_file_t *file,
     libbfio_handle_t *file_io_handle,
     int access_flags,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_file_close(
     libesedb_file_t *file,
     liberror_error_t **error );

int libesedb_file_open_read(
     libesedb_internal_file_t *internal_file,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_file_get_type(
     libesedb_file_t *file,
     uint32_t *type,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_file_get_format_version(
     libesedb_file_t *file,
     uint32_t *format_version,
     uint32_t *format_revision,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_file_get_creation_format_version(
     libesedb_file_t *file,
     uint32_t *format_version,
     uint32_t *format_revision,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_file_get_page_size(
     libesedb_file_t *file,
     uint32_t *page_size,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_file_get_number_of_tables(
     libesedb_file_t *file,
     int *number_of_tables,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_file_get_table(
     libesedb_file_t *file,
     int table_entry,
     libesedb_table_t **table,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_file_get_table_by_utf8_name(
     libesedb_file_t *file,
     const uint8_t *utf8_string,
     size_t utf8_string_length,
     libesedb_table_t **table,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_file_get_table_by_utf16_name(
     libesedb_file_t *file,
     const uint16_t *utf16_string,
     size_t utf16_string_length,
     libesedb_table_t **table,
     liberror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

