/*
 * The block functions
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

#if !defined( _LIBFDATA_INTERNAL_BLOCK_H )
#define _LIBFDATA_INTERNAL_BLOCK_H

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

typedef struct libfdata_internal_block libfdata_internal_block_t;

struct libfdata_internal_block
{
	/* The data offset
	 */
	off64_t data_offset;

	/* The segment index
	 */
	int segment_index;

	/* The segment data offset
	 */
	size_t segment_data_offset;

	/* The segments list
	 */
	libfdata_list_t *segments_list;

	/* The flags
	 */
	uint8_t flags;

	/* The io handle
	 */
	intptr_t *io_handle;

	/* The free io handle function
	 */
	int (*free_io_handle)(
	       intptr_t **io_handle,
	       liberror_error_t **error );

	/* The clone (duplicate) io handle function
	 */
	int (*clone_io_handle)(
	       intptr_t **destination_io_handle,
	       intptr_t *source_io_handle,
	       liberror_error_t **error );

	/* The read segment data function
	 */
	ssize_t (*read_segment_data)(
	           intptr_t *io_handle,
	           libbfio_handle_t *file_io_handle,
                   int segment_index,
	           uint8_t *data,
	           size_t data_size,
                   uint8_t read_flags,
	           liberror_error_t **error );
};

LIBFDATA_EXTERN \
int libfdata_block_initialize(
     libfdata_block_t **block,
     intptr_t *io_handle,
     int (*free_io_handle)(
            intptr_t **io_handle,
            liberror_error_t **error ),
     int (*clone_io_handle)(
            intptr_t **destination_io_handle,
            intptr_t *source_io_handle,
            liberror_error_t **error ),
     ssize_t (*read_segment_data)(
                intptr_t *io_handle,
                libbfio_handle_t *file_io_handle,
                int segment_index,
                uint8_t *data,
                size_t data_size,
                uint8_t read_flags,
                liberror_error_t **error ),
     uint8_t flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_block_free(
     libfdata_block_t **block,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_block_clone(
     libfdata_block_t **destination_block,
     libfdata_block_t *source_block,
     liberror_error_t **error );

/* Segment functions
 */
LIBFDATA_EXTERN \
int libfdata_block_resize_segments(
     libfdata_block_t *block,
     int number_of_segments,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_block_get_number_of_segments(
     libfdata_block_t *block,
     int *number_of_segments,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_block_get_segment_by_index(
     libfdata_block_t *block,
     int segment_index,
     off64_t *segment_offset,
     size64_t *segment_size,
     uint32_t *segment_flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_block_set_segment_by_index(
     libfdata_block_t *block,
     int segment_index,
     off64_t segment_offset,
     size64_t segment_size,
     uint32_t segment_flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_block_append_segment(
     libfdata_block_t *block,
     off64_t segment_offset,
     size64_t segment_size,
     uint32_t segment_flags,
     liberror_error_t **error );

/* Data functions
 */
LIBFDATA_EXTERN \
int libfdata_block_get_size(
     libfdata_block_t *block,
     size64_t *size,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_block_get_data(
     libfdata_block_t *block,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     uint8_t **data,
     size_t *data_size,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_block_get_segment_data(
     libfdata_block_t *block,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     int segment_index,
     uint8_t **data,
     size_t *data_size,
     uint8_t read_flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_block_get_segment_data_at_offset(
     libfdata_block_t *block,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     off64_t data_offset,
     uint8_t **data,
     size_t *data_size,
     uint8_t read_flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_block_get_segment_data_at_value_index(
     libfdata_block_t *block,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     int value_index,
     size_t value_size,
     uint8_t **data,
     size_t *data_size,
     uint8_t read_flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_block_set_segment_data(
     libfdata_block_t *block,
     libfcache_cache_t *cache,
     int segment_index,
     uint8_t *data,
     size_t data_size,
     uint8_t flags,
     liberror_error_t **error );

/* IO functions
 */
int libfdata_block_read_element_data(
     intptr_t *io_handle,
     libbfio_handle_t *file_io_handle,
     libfdata_list_element_t *list_element,
     libfcache_cache_t *cache,
     off64_t element_data_offset,
     size64_t element_data_size,
     uint32_t element_data_range_flags,
     uint8_t read_flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
ssize_t libfdata_block_read_segment_data(
         intptr_t *io_handle,
         libbfio_handle_t *file_io_handle,
         int segment_index,
         uint8_t *data,
         size_t data_size,
         uint8_t read_flags,
         liberror_error_t **error );

LIBFDATA_EXTERN \
ssize_t libfdata_block_read_buffer(
         libfdata_block_t *block,
         libbfio_handle_t *file_io_handle,
         libfcache_cache_t *cache,
         uint8_t *buffer,
         size_t buffer_size,
         liberror_error_t **error );

LIBFDATA_EXTERN \
off64_t libfdata_block_seek_offset(
         libfdata_block_t *block,
         off64_t offset,
         int whence,
         liberror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

