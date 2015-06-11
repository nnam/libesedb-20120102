/*
 * Values table functions
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

#if !defined( _LIBFVALUE_INTERNAL_TABLE_H )
#define _LIBFVALUE_INTERNAL_TABLE_H

#include <common.h>
#include <types.h>

#include <liberror.h>

#include "libfvalue_array_type.h"
#include "libfvalue_extern.h"
#include "libfvalue_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libfvalue_internal_table libfvalue_internal_table_t;

struct libfvalue_internal_table
{
	/* The values array
	 */
	libfvalue_array_t *values;
};

LIBFVALUE_EXTERN \
int libfvalue_table_initialize(
     libfvalue_table_t **table,
     int number_of_values,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_table_free(
     libfvalue_table_t **table,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_table_empty(
     libfvalue_table_t *table,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_table_resize(
     libfvalue_table_t *table,
     int number_of_values,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_table_clone(
     libfvalue_table_t **destination_table,
     libfvalue_table_t *source_table,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_table_get_number_of_values(
     libfvalue_table_t *table,
     int *number_of_values,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_table_get_index_by_identifier(
     libfvalue_table_t *table,
     const uint8_t *identifier,
     size_t identifier_size,
     int *value_index,
     uint8_t flags,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_table_get_value_by_index(
     libfvalue_table_t *table,
     int value_index,
     libfvalue_value_t **value,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_table_get_value_by_identifier(
     libfvalue_table_t *table,
     const uint8_t *identifier,
     size_t identifier_size,
     libfvalue_value_t **value,
     uint8_t flags,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_table_set_value_by_index(
     libfvalue_table_t *table,
     int value_index,
     libfvalue_value_t *value,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_table_set_value(
     libfvalue_table_t *table,
     libfvalue_value_t *value,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_table_copy_from_utf8_xml_string(
     libfvalue_table_t *table,
     const uint8_t *utf8_string,
     size_t utf8_string_size,
     const uint8_t *table_name,
     size_t table_name_length,
     liberror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

