/*
 * The list element functions
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

#if !defined( _LIBFDATA_INTERNAL_LIST_ELEMENT_H )
#define _LIBFDATA_INTERNAL_LIST_ELEMENT_H

#include <common.h>
#include <types.h>

#include <liberror.h>

#include "libfdata_date_time.h"
#include "libfdata_extern.h"
#include "libfdata_libbfio.h"
#include "libfdata_libfcache.h"
#include "libfdata_range.h"
#include "libfdata_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libfdata_internal_list_element libfdata_internal_list_element_t;

struct libfdata_internal_list_element
{
	/* The list reference
	 */
	libfdata_list_t *list;

	/* The (list) element index
	 */
	int element_index;

	/* The (element) data range
	 */
	libfdata_range_t *data_range;

	/* The time stamp
	 */
	time_t timestamp;

	/* The (element) value offset
	 */
	off64_t value_offset;

	/* The (element) value size
	 */
	size64_t value_size;

	/* The flags
	 */
	uint8_t flags;
};

int libfdata_list_element_initialize(
     libfdata_list_element_t **element,
     libfdata_list_t *list,
     int element_index,
     liberror_error_t **error );

int libfdata_list_element_free(
     libfdata_list_element_t **element,
     liberror_error_t **error );

int libfdata_list_element_clone(
     libfdata_list_element_t **destination_element,
     libfdata_list_element_t *source_element,
     libfdata_list_t *list,
     int element_index,
     liberror_error_t **error );

int libfdata_list_element_get_element_index(
     libfdata_list_element_t *element,
     int *element_index,
     liberror_error_t **error );

int libfdata_list_element_set_element_index(
     libfdata_list_element_t *element,
     int element_index,
     liberror_error_t **error );

int libfdata_list_element_get_timestamp(
     libfdata_list_element_t *element,
     time_t *timestamp,
     liberror_error_t **error );

int libfdata_list_element_get_value_offset(
     libfdata_list_element_t *element,
     off64_t *value_offset,
     liberror_error_t **error );

int libfdata_list_element_set_value_offset(
     libfdata_list_element_t *element,
     off64_t value_offset,
     liberror_error_t **error );

int libfdata_list_element_get_value_size(
     libfdata_list_element_t *element,
     size64_t *value_size,
     liberror_error_t **error );

int libfdata_list_element_set_value_size(
     libfdata_list_element_t *element,
     size64_t value_size,
     liberror_error_t **error );

/* Data range functions
 */
int libfdata_list_element_get_data_range(
     libfdata_list_element_t *element,
     off64_t *offset,
     size64_t *size,
     uint32_t *flags,
     liberror_error_t **error );

int libfdata_list_element_set_data_range(
     libfdata_list_element_t *element,
     off64_t offset,
     size64_t size,
     uint32_t flags,
     liberror_error_t **error );

/* Element value functions
 */
LIBFDATA_EXTERN \
int libfdata_list_element_get_element_value(
     libfdata_list_element_t *element,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     intptr_t **element_value,
     uint8_t read_flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_list_element_set_element_value(
     libfdata_list_element_t *element,
     libfcache_cache_t *cache,
     intptr_t *element_value,
     int (*free_element_value)(
            intptr_t **element_value,
            liberror_error_t **error ),
     uint8_t flags,
     liberror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

