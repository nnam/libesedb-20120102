/*
 * Long value functions
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

#if !defined( _LIBESEDB_INTERNAL_LONG_VALUE_H )
#define _LIBESEDB_INTERNAL_LONG_VALUE_H

#include <common.h>
#include <types.h>

#include <liberror.h>

#include "libesedb_catalog_definition.h"
#include "libesedb_extern.h"
#include "libesedb_io_handle.h"
#include "libesedb_libbfio.h"
#include "libesedb_libfcache.h"
#include "libesedb_libfdata.h"
#include "libesedb_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libesedb_internal_long_value libesedb_internal_long_value_t;

struct libesedb_internal_long_value
{
	/* The file IO handle
	 */
	libbfio_handle_t *file_io_handle;

	/* The column catalog definition
	 */
	libesedb_catalog_definition_t *column_catalog_definition;

	/* The long values tree
	 */
	libfdata_tree_t *long_values_tree;

	/* The long values cache
	 */
	libfcache_cache_t *long_values_cache;

	/* The item flags
	 */
	uint8_t flags;

	/* The data block 
	 */
	libfdata_block_t *data_block;

	/* The data cache 
	 */
	libfcache_cache_t *data_cache;
};

int libesedb_long_value_initialize(
     libesedb_long_value_t **long_value,
     libbfio_handle_t *file_io_handle,
     libesedb_io_handle_t *io_handle,
     libesedb_catalog_definition_t *column_catalog_definition,
     libfdata_vector_t *long_values_pages_vector,
     libfcache_cache_t *long_values_pages_cache,
     libfdata_tree_t *long_values_tree,
     libfcache_cache_t *long_values_cache,
     uint8_t *long_value_key,
     size_t long_value_key_size,
     uint8_t flags,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_long_value_free(
     libesedb_long_value_t **long_value,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_long_value_get_number_of_segments(
     libesedb_long_value_t *long_value,
     int *number_of_segments,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_long_value_get_segment_data(
     libesedb_long_value_t *long_value,
     int data_segment_index,
     uint8_t **segment_data,
     size_t *segment_data_size,
     liberror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

