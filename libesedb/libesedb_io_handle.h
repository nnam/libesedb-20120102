/*
 * Input/Output (IO) handle functions
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

#if !defined( _LIBESEDB_IO_HANDLE_H )
#define _LIBESEDB_IO_HANDLE_H

#include <common.h>
#include <types.h>

#include <liberror.h>

#include "libesedb_libbfio.h"
#include "libesedb_libfcache.h"
#include "libesedb_libfdata.h"

#if defined( __cplusplus )
extern "C" {
#endif

extern const uint8_t esedb_file_signature[ 4 ];

typedef struct libesedb_io_handle libesedb_io_handle_t;

struct libesedb_io_handle
{
	/* The file type
	 */
	uint32_t file_type;

	/* The format version
	 */
	uint32_t format_version;

	/* The format revision number
	 */
	uint32_t format_revision;

	/* The creation format version
	 */
	uint32_t creation_format_version;

	/* The creation format revision number
	 */
	uint32_t creation_format_revision;

	/* The pages data offset
	 */
	off64_t pages_data_offset;

	/* The pages data size
	 */
	size64_t pages_data_size;

	/* The page size
	 */
	uint32_t page_size;

	/* The last page number
	 */
	uint32_t last_page_number;

	/* The codepage of the ASCII strings
	 */
	int ascii_codepage;
};

int libesedb_io_handle_initialize(
     libesedb_io_handle_t **io_handle,
     liberror_error_t **error );

int libesedb_io_handle_free(
     libesedb_io_handle_t **io_handle,
     liberror_error_t **error );

int libesedb_io_handle_set_pages_data_range(
     libesedb_io_handle_t *io_handle,
     size64_t file_size,
     liberror_error_t **error );

int libesedb_io_handle_read_file_header(
     libesedb_io_handle_t *io_handle,
     libbfio_handle_t *file_io_handle,
     off64_t file_offset,
     liberror_error_t **error );

int libesedb_io_handle_read_page(
     intptr_t *io_handle,
     libbfio_handle_t *file_io_handle,
     libfdata_vector_t *vector,
     libfcache_cache_t *cache,
     int element_index,
     off64_t element_data_offset,
     size64_t element_data_size,
     uint8_t read_flags,
     liberror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

