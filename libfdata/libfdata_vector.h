/*
 * The vector functions
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

#if !defined( _LIBFDATA_INTERNAL_VECTOR_H )
#define _LIBFDATA_INTERNAL_VECTOR_H

#include <common.h>
#include <types.h>

#include <liberror.h>

#include "libfdata_array_type.h"
#include "libfdata_date_time.h"
#include "libfdata_extern.h"
#include "libfdata_libbfio.h"
#include "libfdata_libfcache.h"
#include "libfdata_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libfdata_internal_vector libfdata_internal_vector_t;

struct libfdata_internal_vector
{
	/* The element size
	 */
	size64_t element_size;

	/* The value size
	 */
	size64_t value_size;

	/* The segments array 
	 */
	libfdata_array_t *segments;

	/* The time stamp
	 */
	time_t timestamp;

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

	/* The read element data function
	 */
	int (*read_element_data)(
	       intptr_t *io_handle,
	       libbfio_handle_t *file_io_handle,
	       libfdata_vector_t *vector,
	       libfcache_cache_t *cache,
	       int element_index,
	       off64_t element_data_offset,
	       size64_t element_data_size,
               uint8_t read_flags,
	       liberror_error_t **error );
};

LIBFDATA_EXTERN \
int libfdata_vector_initialize(
     libfdata_vector_t **vector,
     size64_t element_size,
     intptr_t *io_handle,
     int (*free_io_handle)(
            intptr_t **io_handle,
            liberror_error_t **error ),
     int (*clone_io_handle)(
            intptr_t **destination_io_handle,
            intptr_t *source_io_handle,
            liberror_error_t **error ),
     int (*read_element_data)(
            intptr_t *io_handle,
            libbfio_handle_t *file_io_handle,
            libfdata_vector_t *vector,
            libfcache_cache_t *cache,
            int element_index,
            off64_t element_data_offset,
            size64_t element_data_size,
            uint8_t read_flags,
            liberror_error_t **error ),
     uint8_t flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_vector_free(
     libfdata_vector_t **vector,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_vector_clone(
     libfdata_vector_t **destination_vector,
     libfdata_vector_t *source_vector,
     liberror_error_t **error );

/* Segment functions
 */
LIBFDATA_EXTERN \
int libfdata_vector_append_segment(
     libfdata_vector_t *vector,
     off64_t segment_offset,
     size64_t segment_size,
     uint32_t segment_flags,
     liberror_error_t **error );

/* Vector element functions
 */
LIBFDATA_EXTERN \
int libfdata_vector_get_number_of_elements(
     libfdata_vector_t *vector,
     int *number_of_elements,
     liberror_error_t **error );

int libfdata_vector_get_element_index_at_offset(
     libfdata_vector_t *vector,
     off64_t value_offset,
     int *element_index,
     size_t *element_offset,
     liberror_error_t **error );

/* Vector element value functions
 */
LIBFDATA_EXTERN \
int libfdata_vector_get_element_value_by_index(
     libfdata_vector_t *vector,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     int element_index,
     intptr_t **element_value,
     uint8_t read_flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_vector_get_element_value_at_offset(
     libfdata_vector_t *vector,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     off64_t value_offset,
     intptr_t **element_value,
     uint8_t read_flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_vector_set_element_value_by_index(
     libfdata_vector_t *vector,
     libfcache_cache_t *cache,
     int element_index,
     intptr_t *element_value,
     int (*free_element_value)(
            intptr_t **element_value,
     liberror_error_t **error ),
     uint8_t flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_vector_get_element_size(
     libfdata_vector_t *vector,
     size64_t *element_size,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_vector_get_value_size(
     libfdata_vector_t *vector,
     size64_t *value_size,
     liberror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

