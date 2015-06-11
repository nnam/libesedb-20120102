/*
 * Debug functions
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

#if !defined( _LIBFMAPI_DEBUG_H )
#define _LIBFMAPI_DEBUG_H

#include <common.h>
#include <types.h>

#include <liberror.h>

#include <stdio.h>

#if defined( __cplusplus )
extern "C" {
#endif

#if defined( HAVE_DEBUG_OUTPUT )

int libfmapi_debug_string_contains_zero_bytes(
     uint8_t *buffer,
     size_t buffer_size,
     liberror_error_t **error );

int libfmapi_debug_print_entry_identifier(
     uint8_t *value_data,
     size_t value_data_size,
     int ascii_codepage,
     liberror_error_t **error );

int libfmapi_debug_print_one_off_entry_identifier(
     uint8_t *value_data,
     size_t value_data_size,
     int ascii_codepage,
     liberror_error_t **error );

int libfmapi_debug_print_value(
     uint32_t entry_type,
     uint32_t value_type,
     uint8_t *value_data,
     size_t value_data_size,
     int ascii_codepage,
     liberror_error_t **error );

int libfmapi_debug_print_integer_32bit_value(
     uint32_t entry_type,
     uint32_t value_type,
     uint8_t *value_data,
     size_t value_data_size,
     liberror_error_t **error );

int libfmapi_debug_print_binary_data_value(
     uint32_t entry_type,
     uint32_t value_type,
     uint8_t *value_data,
     size_t value_data_size,
     int ascii_codepage,
     liberror_error_t **error );

int libfmapi_debug_print_array_multi_value(
     uint32_t entry_type,
     uint32_t value_type,
     uint8_t *value_data,
     size_t value_data_size,
     int ascii_codepage,
     liberror_error_t **error );

int libfmapi_debug_print_element_multi_value(
     uint32_t entry_type,
     uint32_t value_type,
     uint8_t *value_data,
     size_t value_data_size,
     int ascii_codepage,
     liberror_error_t **error );

#endif

#if defined( __cplusplus )
}
#endif

#endif

