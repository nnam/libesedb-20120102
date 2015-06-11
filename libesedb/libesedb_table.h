/*
 * Table functions
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

#if !defined( _LIBESEDB_TABLE_H )
#define _LIBESEDB_TABLE_H

#include <common.h>
#include <types.h>

#include <liberror.h>

#include "libesedb_extern.h"
#include "libesedb_io_handle.h"
#include "libesedb_libbfio.h"
#include "libesedb_libfcache.h"
#include "libesedb_libfdata.h"
#include "libesedb_list_type.h"
#include "libesedb_table_definition.h"
#include "libesedb_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libesedb_internal_table libesedb_internal_table_t;

struct libesedb_internal_table
{
	/* The file IO handle
	 */
	libbfio_handle_t *file_io_handle;

	/* The IO handle
	 */
	libesedb_io_handle_t *io_handle;

	/* The table definition
	 */
	libesedb_table_definition_t *table_definition;

	/* The template table definition
	 */
	libesedb_table_definition_t *template_table_definition;

	/* The item flags
	 */
	uint8_t flags;

	/* The pages vector
	 */
	libfdata_vector_t *pages_vector;

	/* The pages cache
	 */
	libfcache_cache_t *pages_cache;

	/* The long values pages vector
	 */
	libfdata_vector_t *long_values_pages_vector;

	/* The long values pages cache
	 */
	libfcache_cache_t *long_values_pages_cache;

	/* The table values tree
	 */
	libfdata_tree_t *table_values_tree;

	/* The table values cache
	 */
	libfcache_cache_t *table_values_cache;

	/* The long values tree
	 */
	libfdata_tree_t *long_values_tree;

	/* The long values cache
	 */
	libfcache_cache_t *long_values_cache;
};

int libesedb_table_initialize(
     libesedb_table_t **table,
     libbfio_handle_t *file_io_handle,
     libesedb_io_handle_t *io_handle,
     libesedb_table_definition_t *table_definition,
     libesedb_table_definition_t *template_table_definition,
     uint8_t flags,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_table_free(
     libesedb_table_t **table,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_table_get_identifier(
     libesedb_table_t *table,
     uint32_t *identifier,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_table_get_utf8_name_size(
     libesedb_table_t *table,
     size_t *utf8_string_size,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_table_get_utf8_name(
     libesedb_table_t *table,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_table_get_utf16_name_size(
     libesedb_table_t *table,
     size_t *utf16_string_size,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_table_get_utf16_name(
     libesedb_table_t *table,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_table_get_utf8_template_name_size(
     libesedb_table_t *table,
     size_t *utf8_string_size,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_table_get_utf8_template_name(
     libesedb_table_t *table,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_table_get_utf16_template_name_size(
     libesedb_table_t *table,
     size_t *utf16_string_size,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_table_get_utf16_template_name(
     libesedb_table_t *table,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_table_get_number_of_columns(
     libesedb_table_t *table,
     int *number_of_columns,
     uint8_t flags,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_table_get_column(
     libesedb_table_t *table,
     int column_entry,
     libesedb_column_t **column,
     uint8_t flags,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_table_get_number_of_indexes(
     libesedb_table_t *table,
     int *number_of_indexes,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_table_get_index(
     libesedb_table_t *table,
     int index_entry,
     libesedb_index_t **index,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_table_get_number_of_records(
     libesedb_table_t *table,
     int *number_of_records,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_table_get_record(
     libesedb_table_t *table,
     int record_entry,
     libesedb_record_t **record,
     liberror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

