/*
 * Split string functions
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

#if !defined( _LIBSYSTEM_SPLIT_STRING_H )
#define _LIBSYSTEM_SPLIT_STRING_H

#include <common.h>
#include <types.h>

#include <liberror.h>

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libsystem_split_string libsystem_split_string_t;

struct libsystem_split_string
{
	/* The number of string segments
	 */
	int number_of_segments;

	/* The string
	 */
	libcstring_system_character_t *string;

	/* The string size
	 */
	size_t string_size;

	/* The segments
	 */
	libcstring_system_character_t **segments;

	/* The segment sizes
	 */
	size_t *segment_sizes;
};

int libsystem_split_string_initialize(
     libsystem_split_string_t **split_string,
     const libcstring_system_character_t *string,
     size_t string_size,
     int number_of_segments,
     liberror_error_t **error );

int libsystem_split_string_free(
     libsystem_split_string_t **split_string,
     liberror_error_t **error );

int libsystem_split_string_get_string(
     libsystem_split_string_t *split_string,
     libcstring_system_character_t **string,
     size_t *string_size,
     liberror_error_t **error );

int libsystem_split_string_get_number_of_segments(
     libsystem_split_string_t *split_string,
     int *number_of_segments,
     liberror_error_t **error );

int libsystem_split_string_get_segment_by_index(
     libsystem_split_string_t *split_string,
     int segment_index,
     libcstring_system_character_t **string_segment,
     size_t *string_segment_size,
     liberror_error_t **error );

int libsystem_split_string_set_segment_by_index(
     libsystem_split_string_t *split_string,
     int segment_index,
     libcstring_system_character_t *string_segment,
     size_t string_segment_size,
     liberror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

