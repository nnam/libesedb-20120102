/*
 * The list functions
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

#if !defined( _LIBFDATA_INTERNAL_LIST_H )
#define _LIBFDATA_INTERNAL_LIST_H

#include <common.h>
#include <types.h>

#include <liberror.h>

#include "libfdata_array_type.h"
#include "libfdata_extern.h"
#include "libfdata_libbfio.h"
#include "libfdata_libfcache.h"
#include "libfdata_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libfdata_internal_list libfdata_internal_list_t;

struct libfdata_internal_list
{
	/* The value size
	 */
	size64_t value_size;

	/* The (list) elements (array)
	 */
	libfdata_array_t *elements;

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
	       libfdata_list_element_t *list_element,
	       libfcache_cache_t *cache,
	       off64_t element_data_offset,
	       size64_t element_data_size,
	       uint32_t element_data_flags,
               uint8_t read_flags,
	       liberror_error_t **error );
};

LIBFDATA_EXTERN \
int libfdata_list_initialize(
     libfdata_list_t **list,
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
            libfdata_list_element_t *list_element,
            libfcache_cache_t *cache,
            off64_t element_data_offset,
            size64_t element_data_size,
            uint32_t element_data_flags,
            uint8_t read_flags,
            liberror_error_t **error ),
     uint8_t flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_list_free(
     libfdata_list_t **list,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_list_clone(
     libfdata_list_t **destination_list,
     libfdata_list_t *source_list,
     liberror_error_t **error );

int libfdata_list_clone_elements(
     libfdata_list_t *destination_list,
     libfdata_list_t *source_list,
     liberror_error_t **error );

/* List element functions
 */
LIBFDATA_EXTERN \
int libfdata_list_resize(
     libfdata_list_t *list,
     int number_of_elements,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_list_get_number_of_elements(
     libfdata_list_t *list,
     int *number_of_elements,
     liberror_error_t **error );

int libfdata_list_calculate_value_offsets(
     libfdata_internal_list_t *internal_list,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_list_get_element_by_index(
     libfdata_list_t *list,
     int element_index,
     libfdata_list_element_t **element,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_list_get_element_by_offset(
     libfdata_list_t *list,
     off64_t element_data_offset,
     int *element_index,
     libfdata_list_element_t **element,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_list_set_element_by_index(
     libfdata_list_t *list,
     int element_index,
     off64_t offset,
     size64_t size,
     uint32_t flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_list_append_element(
     libfdata_list_t *list,
     int *element_index,
     off64_t offset,
     size64_t size,
     uint32_t flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_list_is_element_set(
     libfdata_list_t *list,
     int element_index,
     liberror_error_t **error );

/* Data range functions
 */
LIBFDATA_EXTERN \
int libfdata_list_get_data_range_by_index(
     libfdata_list_t *list,
     int element_index,
     off64_t *offset,
     size64_t *size,
     uint32_t *flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_list_set_data_range_by_index(
     libfdata_list_t *list,
     int element_index,
     off64_t offset,
     size64_t size,
     uint32_t flags,
     liberror_error_t **error );

/* List element value functions
 */
int libfdata_list_get_element_index_at_value_offset(
     libfdata_list_t *list,
     off64_t value_offset,
     int *element_index,
     size_t *element_offset,
     liberror_error_t **error );

int libfdata_list_get_element_index_at_value_index(
     libfdata_list_t *list,
     int value_index,
     size_t value_size,
     int *element_index,
     size_t *element_offset,
     liberror_error_t **error );

int libfdata_list_get_element_value(
     libfdata_list_t *list,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     libfdata_list_element_t *element,
     intptr_t **element_value,
     uint8_t read_flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_list_get_element_value_by_index(
     libfdata_list_t *list,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     int element_index,
     intptr_t **element_value,
     uint8_t read_flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_list_get_element_value_at_offset(
     libfdata_list_t *list,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     off64_t value_offset,
     intptr_t **element_value,
     uint8_t read_flags,
     liberror_error_t **error );

int libfdata_list_set_element_value(
     libfdata_list_t *list,
     libfcache_cache_t *cache,
     libfdata_list_element_t *element,
     intptr_t *element_value,
     int (*free_element_value)(
            intptr_t **element_value,
            liberror_error_t **error ),
     uint8_t flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_list_set_element_value_by_index(
     libfdata_list_t *list,
     libfcache_cache_t *cache,
     int element_index,
     intptr_t *element_value,
     int (*free_element_value)(
            intptr_t **element_value,
            liberror_error_t **error ),
     uint8_t flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_list_get_value_size(
     libfdata_list_t *list,
     size64_t *value_size,
     liberror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

