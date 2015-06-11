/*
 * Buffer data reference functions
 *
 * Copyright (c) 2010-2012, Joachim Metz <jbmetz@users.sourceforge.net>
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

#if !defined( _LIBFDATA_BUFFER_REFERENCE_H )
#define _LIBFDATA_BUFFER_REFERENCE_H

#include <common.h>
#include <types.h>

#include <liberror.h>

#include "libfdata_extern.h"
#include "libfdata_libbfio.h"
#include "libfdata_libfcache.h"
#include "libfdata_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libfdata_buffer_reference libfdata_buffer_reference_t;

struct libfdata_buffer_reference
{
	/* The data offset
	 */
	off64_t data_offset;

	/* The data
	 */
	uint8_t *data;

	/* The data size
	 */
	size_t data_size;

	/* The flags
	 */
	uint8_t flags;
};

LIBFDATA_EXTERN \
int libfdata_buffer_reference_initialize(
     libfdata_reference_t **reference,
     const uint8_t *buffer,
     size_t buffer_size,
     uint8_t flags,
     liberror_error_t **error );

int libfdata_buffer_reference_free(
     libfdata_buffer_reference_t **buffer_reference,
     liberror_error_t **error );

int libfdata_buffer_reference_clone(
     libfdata_buffer_reference_t **destination_buffer_reference,
     libfdata_buffer_reference_t *source_buffer_reference,
     liberror_error_t **error );

/* Segment functions
 */
int libfdata_buffer_reference_get_number_of_segments(
     libfdata_buffer_reference_t *buffer_reference,
     int *number_of_segments,
     liberror_error_t **error );

/* Data functions
 */
int libfdata_buffer_reference_get_size(
     libfdata_buffer_reference_t *buffer_reference,
     size64_t *size,
     liberror_error_t **error );

int libfdata_buffer_reference_get_data(
     libfdata_buffer_reference_t *buffer_reference,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     uint8_t **data,
     size_t *data_size,
     liberror_error_t **error );

int libfdata_buffer_reference_get_segment_data(
     libfdata_buffer_reference_t *buffer_reference,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     int segment_index,
     uint8_t **data,
     size_t *data_size,
     uint8_t read_flags,
     liberror_error_t **error );

int libfdata_buffer_reference_get_segment_data_at_offset(
     libfdata_buffer_reference_t *buffer_reference,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     off64_t data_offset,
     uint8_t **data,
     size_t *data_size,
     uint8_t read_flags,
     liberror_error_t **error );

int libfdata_buffer_reference_get_segment_data_at_value_index(
     libfdata_buffer_reference_t *buffer_reference,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     int value_index,
     size_t value_size,
     uint8_t **data,
     size_t *data_size,
     uint8_t read_flags,
     liberror_error_t **error );

/* IO functions
 */
ssize_t libfdata_buffer_reference_read_buffer(
         libfdata_buffer_reference_t *buffer_reference,
         libbfio_handle_t *file_io_handle,
         libfcache_cache_t *cache,
         uint8_t *buffer,
         size_t buffer_size,
         liberror_error_t **error );

off64_t libfdata_buffer_reference_seek_offset(
         libfdata_buffer_reference_t *buffer_reference,
         off64_t offset,
         int whence,
         liberror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

