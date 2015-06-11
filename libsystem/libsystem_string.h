/*
 * System character string functions
 *
 * Copyright (c) 2008-2012, Joachim Metz <jbmetz@users.sourceforge.net>
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

#if !defined( _LIBSYSTEM_STRING_H )
#define _LIBSYSTEM_STRING_H

#include <common.h>
#include <types.h>

#include <libcstring.h>
#include <liberror.h>

#include "libsystem_split_string.h"

#if defined( __cplusplus )
extern "C" {
#endif

int libsystem_string_decimal_copy_from_64_bit(
     libcstring_system_character_t *string,
     size_t string_size,
     size_t *string_index,
     uint64_t value_64bit,
     liberror_error_t **error );

int libsystem_string_decimal_copy_to_64_bit(
     const libcstring_system_character_t *string,
     size_t string_size,
     uint64_t *value_64bit,
     liberror_error_t **error );

int libsystem_string_size_from_utf8_string(
     const uint8_t *utf8_string,
     size_t utf8_string_size,
     size_t *string_size,
     liberror_error_t **error );

int libsystem_string_copy_from_utf8_string(
     libcstring_system_character_t *string,
     size_t string_size,
     const uint8_t *utf8_string,
     size_t utf8_string_size,
     liberror_error_t **error );

int libsystem_string_size_to_utf8_string(
     const libcstring_system_character_t *string,
     size_t string_size,
     size_t *utf8_string_size,
     liberror_error_t **error );

int libsystem_string_copy_to_utf8_string(
     const libcstring_system_character_t *string,
     size_t string_size,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     liberror_error_t **error );

int libsystem_string_split(
     const libcstring_system_character_t *string,
     size_t string_size,
     libcstring_system_character_t delimiter,
     libsystem_split_string_t **split_string,
     liberror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

