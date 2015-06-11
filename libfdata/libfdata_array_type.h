/* 
 * Array type functions
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

#if !defined( _LIBFDATA_ARRAY_TYPE_H )
#define _LIBFDATA_ARRAY_TYPE_H

#include <common.h>
#include <types.h>

#include <liberror.h>

#if defined( __cplusplus )
extern "C" {
#endif

/* The array comparison definitions
 */
enum LIBFDATA_ARRAY_COMPARE_DEFINITIONS
{
	/* The first value is less than the second value
	 */
        LIBFDATA_ARRAY_COMPARE_LESS,

	/* The first and second values are equal
	 */
        LIBFDATA_ARRAY_COMPARE_EQUAL,

	/* The first value is greater than the second value
	 */
        LIBFDATA_ARRAY_COMPARE_GREATER
};

/* The array insert flag definitions
 */
enum LIBFDATA_ARRAY_INSERT_FLAGS
{
	/* Allow duplicate entries
	 */
	LIBFDATA_ARRAY_INSERT_FLAG_NON_UNIQUE_ENTRIES	= 0x00,

	/* Only allow unique entries, no duplicates
	 */
	LIBFDATA_ARRAY_INSERT_FLAG_UNIQUE_ENTRIES	= 0x01,
};

typedef struct libfdata_array libfdata_array_t;

struct libfdata_array
{
	/* The number of allocated entries
	 */
	int number_of_allocated_entries;

	/* The number of entries
	 */
	int number_of_entries;

	/* The entries
	 */
	intptr_t **entries;
};

int libfdata_array_initialize(
     libfdata_array_t **array,
     int number_of_entries,
     liberror_error_t **error );

int libfdata_array_free(
     libfdata_array_t **array,
     int (*entry_free_function)(
            intptr_t **entry,
            liberror_error_t **error ),
     liberror_error_t **error );

int libfdata_array_empty(
     libfdata_array_t *array,
     int (*entry_free_function)(
            intptr_t **entry,
            liberror_error_t **error ),
     liberror_error_t **error );

int libfdata_array_clear(
     libfdata_array_t *array,
     int (*entry_free_function)(
            intptr_t **entry,
            liberror_error_t **error ),
     liberror_error_t **error );

int libfdata_array_clone(
     libfdata_array_t **destination_array,
     libfdata_array_t *source_array,
     int (*entry_free_function)(
            intptr_t **entry,
            liberror_error_t **error ),
     int (*entry_clone_function)(
            intptr_t **destination,
            intptr_t *source,
            liberror_error_t **error ),
     liberror_error_t **error );

int libfdata_array_resize(
     libfdata_array_t *array,
     int number_of_entries,
     int (*entry_free_function)(
            intptr_t **entry,
            liberror_error_t **error ),
     liberror_error_t **error );

int libfdata_array_get_number_of_entries(
     libfdata_array_t *array,
     int *number_of_entries,
     liberror_error_t **error );

int libfdata_array_get_entry_by_index(
     libfdata_array_t *array,
     int entry_index,
     intptr_t **entry,
     liberror_error_t **error );

int libfdata_array_set_entry_by_index(
     libfdata_array_t *array,
     int entry_index,
     intptr_t *entry,
     liberror_error_t **error );

int libfdata_array_append_entry(
     libfdata_array_t *array,
     int *entry_index,
     intptr_t *entry,
     liberror_error_t **error );

int libfdata_array_insert_entry(
     libfdata_array_t *array,
     int *entry_index,
     intptr_t *entry,
     int (*entry_compare_function)(
            intptr_t *first_entry,
            intptr_t *second_entry,
            liberror_error_t **error ),
     uint8_t insert_flags,
     liberror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

