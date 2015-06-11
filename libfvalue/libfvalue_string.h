/*
 * String functions
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

#if !defined( _LIBVALUE_STRING_H )
#define _LIBVALUE_STRING_H

#include <common.h>
#include <types.h>

#include <liberror.h>

#include "libfvalue_extern.h"
#include "libfvalue_types.h"

#if defined( _cplusplus )
extern "C" {
#endif

LIBFVALUE_EXTERN \
int libfvalue_utf8_string_decimal_copy_from_8bit(
     uint8_t *utf8_string,
     size_t utf8_string_size,
     size_t *utf8_string_index,
     uint8_t value_8bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_utf8_string_decimal_copy_to_8bit(
     const uint8_t *utf8_string,
     size_t utf8_string_size,
     uint8_t *value_8bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_utf8_string_decimal_copy_from_16bit(
     uint8_t *utf8_string,
     size_t utf8_string_size,
     size_t *utf8_string_index,
     uint16_t value_16bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_utf8_string_decimal_copy_to_16bit(
     const uint8_t *utf8_string,
     size_t utf8_string_size,
     uint16_t *value_16bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_utf8_string_decimal_copy_from_32bit(
     uint8_t *utf8_string,
     size_t utf8_string_size,
     size_t *utf8_string_index,
     uint32_t value_32bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_utf8_string_decimal_copy_to_32bit(
     const uint8_t *utf8_string,
     size_t utf8_string_size,
     uint32_t *value_32bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_utf8_string_decimal_copy_from_64bit(
     uint8_t *utf8_string,
     size_t utf8_string_size,
     size_t *utf8_string_index,
     uint64_t value_64bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_utf8_string_decimal_copy_to_64bit(
     const uint8_t *utf8_string,
     size_t utf8_string_size,
     uint64_t *value_64bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_utf8_string_hexadecimal_copy_from_64bit(
     uint8_t *utf8_string,
     size_t utf8_string_size,
     uint64_t value_64bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_utf8_string_hexadecimal_copy_to_64bit(
     const uint8_t *utf8_string,
     size_t utf8_string_size,
     uint64_t *value_64bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_utf8_string_split(
     const uint8_t *utf8_string,
     size_t utf8_string_size,
     uint8_t delimiter,
     libfvalue_split_utf8_string_t **split_string,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_utf16_string_decimal_copy_from_8bit(
     uint16_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     uint8_t value_8bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_utf16_string_decimal_copy_to_8bit(
     const uint16_t *utf16_string,
     size_t utf16_string_size,
     uint8_t *value_8bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_utf16_string_decimal_copy_from_16bit(
     uint16_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     uint16_t value_16bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_utf16_string_decimal_copy_to_16bit(
     const uint16_t *utf16_string,
     size_t utf16_string_size,
     uint16_t *value_16bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_utf16_string_decimal_copy_from_32bit(
     uint16_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     uint32_t value_32bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_utf16_string_decimal_copy_to_32bit(
     const uint16_t *utf16_string,
     size_t utf16_string_size,
     uint32_t *value_32bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_utf16_string_decimal_copy_from_64bit(
     uint16_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     uint64_t value_64bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_utf16_string_decimal_copy_to_64bit(
     const uint16_t *utf16_string,
     size_t utf16_string_size,
     uint64_t *value_64bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_utf16_string_hexadecimal_copy_from_64bit(
     uint16_t *utf16_string,
     size_t utf16_string_size,
     uint64_t value_64bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_utf16_string_hexadecimal_copy_to_64bit(
     const uint16_t *utf16_string,
     size_t utf16_string_size,
     uint64_t *value_64bit,
     liberror_error_t **error );

LIBFVALUE_EXTERN \
int libfvalue_utf16_string_split(
     const uint16_t *utf16_string,
     size_t utf16_string_size,
     uint16_t delimiter,
     libfvalue_split_utf16_string_t **split_string,
     liberror_error_t **error );

#if defined( _cplusplus )
}
#endif

#endif

