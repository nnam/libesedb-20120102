/*
 * Value functions
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

#if !defined( _LIBFVALUE_INTERNAL_VALUE_H )
#define _LIBFVALUE_INTERNAL_VALUE_H

#include <common.h>
#include <types.h>

#include <liberror.h>

#include <stdio.h>

#include "libfvalue_array_type.h"
#include "libfvalue_extern.h"
#include "libfvalue_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libfvalue_internal_value libfvalue_internal_value_t;

struct libfvalue_internal_value
{
	/* The type
	 */
	uint8_t type;

	/* The identifier
	 */
	uint8_t *identifier;

	/* The identifier size
	 */
	size_t identifier_size;

	/* The value entries array
	 */
	libfvalue_array_t *value_entries;

	/* The data
	 */
	uint8_t *data;

	/* The data size
	 */
	size_t data_size;

	/* The metadata
	 */
	uint8_t *metadata;

	/* The metadata size
	 */
	size_t metadata_size;

	/* The byte order
	 */
	uint8_t byte_order;

	/* The format
	 */
	uint8_t format;

	/* The byte stream string codepage
	 */
	int codepage;

	/* The flags
	 */
	uint8_t flags;
};

LIBFVALUE_EXTERN \
int libfvalue_value_initialize(
     libfvalue_value_t **value,
     uint8_t type,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_free(
     libfvalue_value_t **value,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_clone(
     libfvalue_value_t **destination_value,
     libfvalue_value_t *source_value,
     liberror_error_t **error );

int libfvalue_compare_identifier(
     intptr_t *first_value,
     intptr_t *second_value,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_get_type(
     libfvalue_value_t *value,
     uint8_t *value_type,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_get_identifier(
     libfvalue_value_t *value,
     uint8_t **identifier,
     size_t *identifier_size,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_set_identifier(
     libfvalue_value_t *value,
     const uint8_t *identifier,
     size_t identifier_size,
     uint8_t flags,
     liberror_error_t **error );

int libfvalue_value_get_data_type_size(
     libfvalue_internal_value_t *internal_value,
     size_t *data_type_size,
     liberror_error_t **error );

int libfvalue_value_initialize_data(
     libfvalue_internal_value_t *internal_value,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_has_data(
     libfvalue_value_t *value,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_get_data(
     libfvalue_value_t *value,
     uint8_t **data,
     size_t *data_size,
     uint8_t *byte_order,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_get_data_size(
     libfvalue_value_t *value,
     size_t *data_size,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_set_data(
     libfvalue_value_t *value,
     const uint8_t *data,
     size_t data_size,
     uint8_t byte_order,
     uint8_t flags,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_set_data_string(
     libfvalue_value_t *value,
     const uint8_t *data,
     size_t data_size,
     uint8_t byte_order,
     uint8_t flags,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_set_data_strings_array(
     libfvalue_value_t *value,
     const uint8_t *data,
     size_t data_size,
     uint8_t byte_order,
     uint8_t flags,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_copy_data(
     libfvalue_value_t *value,
     uint8_t *data,
     size_t data_size,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_get_entry_data(
     libfvalue_value_t *value,
     int value_entry_index,
     uint8_t **entry_data,
     size_t *entry_data_size,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_copy_entry_data(
     libfvalue_value_t *value,
     int value_entry_index,
     uint8_t *entry_data,
     size_t entry_data_size,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_get_metadata(
     libfvalue_value_t *value,
     uint8_t **metadata,
     size_t *metadata_size,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_set_metadata(
     libfvalue_value_t *value,
     const uint8_t *metadata,
     size_t metadata_size,
     uint8_t flags,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_get_codepage(
     libfvalue_value_t *value,
     int *codepage,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_set_codepage(
     libfvalue_value_t *value,
     int codepage,
     liberror_error_t **error );

/* Value entry functions
 */

LIBFVALUE_EXTERN \
int libfvalue_value_resize_value_entries(
     libfvalue_value_t *value,
     int number_of_value_entries,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_get_number_of_value_entries(
     libfvalue_value_t *value,
     int *number_of_value_entries,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_set_value_entry(
     libfvalue_value_t *value,
     int value_entry_index,
     size_t value_entry_offset,
     size_t value_entry_size,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_append_value_entry(
     libfvalue_value_t *value,
     size_t value_entry_offset,
     size_t value_entry_size,
     liberror_error_t **error );

/* Marshalling functions
 */

/* TODO add get/set format functions, or get/set data_with_format */

LIBFVALUE_EXTERN \
ssize_t libfvalue_value_read_from_file_stream(
         libfvalue_value_t *value,
         FILE *file_stream,
         liberror_error_t **error );

LIBFVALUE_EXTERN \
ssize_t libfvalue_value_write_to_file_stream(
         libfvalue_value_t *value,
         FILE *file_stream,
         liberror_error_t **error );

/* Boolean value functions
 */
LIBFVALUE_EXTERN \
int libfvalue_value_copy_from_boolean(
     libfvalue_value_t *value,
     int value_entry_index,
     uint8_t value_boolean,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_copy_to_boolean(
     libfvalue_value_t *value,
     int value_entry_index,
     uint8_t *value_boolean,
     liberror_error_t **error );

/* Integer value functions
 */
LIBFVALUE_EXTERN \
int libfvalue_value_copy_from_8bit(
     libfvalue_value_t *value,
     int value_entry_index,
     uint8_t value_8bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_copy_to_8bit(
     libfvalue_value_t *value,
     int value_entry_index,
     uint8_t *value_8bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_copy_from_16bit(
     libfvalue_value_t *value,
     int value_entry_index,
     uint16_t value_16bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_copy_to_16bit(
     libfvalue_value_t *value,
     int value_entry_index,
     uint16_t *value_16bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_copy_from_32bit(
     libfvalue_value_t *value,
     int value_entry_index,
     uint32_t value_32bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_copy_to_32bit(
     libfvalue_value_t *value,
     int value_entry_index,
     uint32_t *value_32bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_copy_from_64bit(
     libfvalue_value_t *value,
     int value_entry_index,
     uint64_t value_64bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_copy_to_64bit(
     libfvalue_value_t *value,
     int value_entry_index,
     uint64_t *value_64bit,
     liberror_error_t **error );

/* Floating point value functions
 */
LIBFVALUE_EXTERN \
int libfvalue_value_copy_from_float(
     libfvalue_value_t *value,
     int value_entry_index,
     float value_float,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_copy_to_float(
     libfvalue_value_t *value,
     int value_entry_index,
     float *value_float,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_copy_from_double(
     libfvalue_value_t *value,
     int value_entry_index,
     double value_double,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_copy_to_double(
     libfvalue_value_t *value,
     int value_entry_index,
     double *value_double,
     liberror_error_t **error );

/* String value functions
 */
LIBFVALUE_EXTERN \
int libfvalue_value_copy_from_utf8_string(
     libfvalue_value_t *value,
     int value_entry_index,
     const uint8_t *utf8_string,
     size_t utf8_string_size,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_get_utf8_string_size(
     libfvalue_value_t *value,
     int value_entry_index,
     size_t *utf8_string_size,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_copy_to_utf8_string(
     libfvalue_value_t *value,
     int value_entry_index,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_copy_from_utf16_string(
     libfvalue_value_t *value,
     int value_entry_index,
     const uint16_t *utf16_string,
     size_t utf16_string_size,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_get_utf16_string_size(
     libfvalue_value_t *value,
     int value_entry_index,
     size_t *utf16_string_size,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_copy_to_utf16_string(
     libfvalue_value_t *value,
     int value_entry_index,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_copy_from_utf32_string(
     libfvalue_value_t *value,
     int value_entry_index,
     const uint32_t *utf32_string,
     size_t utf32_string_size,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_get_utf32_string_size(
     libfvalue_value_t *value,
     int value_entry_index,
     size_t *utf32_string_size,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_value_copy_to_utf32_string(
     libfvalue_value_t *value,
     int value_entry_index,
     uint32_t *utf32_string,
     size_t utf32_string_size,
     liberror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

