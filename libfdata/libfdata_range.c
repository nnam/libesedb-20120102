/*
 * The data range functions
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

#include <common.h>
#include <memory.h>
#include <types.h>

#include <liberror.h>

#include "libfdata_range.h"

/* Initializes the range
 * Returns 1 if successful or -1 on error
 */
int libfdata_range_initialize(
     libfdata_range_t **range,
     liberror_error_t **error )
{
	static char *function = "libfdata_range_initialize";

	if( range == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid range.",
		 function );

		return( -1 );
	}
	if( *range != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid range value already set.",
		 function );

		return( -1 );
	}
	*range = memory_allocate_structure(
	          libfdata_range_t );

	if( *range == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create range.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *range,
	     0,
	     sizeof( libfdata_range_t ) ) == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear range.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( *range != NULL )
	{
		memory_free(
		 *range );

		*range = NULL;
	}
	return( -1 );
}

/* Frees the range
 * Returns 1 if successful or -1 on error
 */
int libfdata_range_free(
     libfdata_range_t **range,
     liberror_error_t **error )
{
	static char *function = "libfdata_range_free";

	if( range == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid range.",
		 function );

		return( -1 );
	}
	if( *range != NULL )
	{
		memory_free(
		 *range );

		*range = NULL;
	}
	return( 1 );
}

/* Clones (duplicates) the range
 * Returns 1 if successful or -1 on error
 */
int libfdata_range_clone(
     libfdata_range_t **destination_range,
     libfdata_range_t *source_range,
     liberror_error_t **error )
{
	static char *function = "libfdata_range_clone";

	if( destination_range == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid destination range.",
		 function );

		return( -1 );
	}
	if( *destination_range != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: destination range already set.",
		 function );

		return( -1 );
	}
	if( source_range == NULL )
	{
		*destination_range = NULL;

		return( 1 );
	}
	*destination_range = memory_allocate_structure(
	                      libfdata_range_t );

	if( *destination_range == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create destination range.",
		 function );

		goto on_error;
	}
	if( memory_copy(
	     *destination_range,
	     source_range,
	     sizeof( libfdata_range_t ) ) == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_COPY_FAILED,
		 "%s: unable to copy source to destination range.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( *destination_range != NULL )
	{
		memory_free(
		 *destination_range );

		*destination_range = NULL;
	}
	return( -1 );
}

/* Retrieves the range values
 * Returns 1 if successful or -1 on error
 */
int libfdata_range_get_values(
     libfdata_range_t *range,
     off64_t *offset,
     size64_t *size,
     uint32_t *flags,
     liberror_error_t **error )
{
	static char *function = "libfdata_range_get_values";

	if( range == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid range.",
		 function );

		return( -1 );
	}
	if( offset == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid offset.",
		 function );

		return( -1 );
	}
	if( size == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid size.",
		 function );

		return( -1 );
	}
	if( flags == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: nt data flags.",
		 function );

		return( -1 );
	}
	*offset = range->offset;
	*size   = range->size;
	*flags  = range->flags;

	return( 1 );
}

/* Sets the range values
 * Returns 1 if successful or -1 on error
 */
int libfdata_range_set_values(
     libfdata_range_t *range,
     off64_t offset,
     size64_t size,
     uint32_t flags,
     liberror_error_t **error )
{
	static char *function = "libfdata_range_set_values";

	if( range == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid range.",
		 function );

		return( -1 );
	}
	if( offset < 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_LESS_THAN_ZERO,
		 "%s: invalid offset value less than zero.",
		 function );

		return( -1 );
	}
	if( size > (size64_t) INT64_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid size value exceeds maximum.",
		 function );

		return( -1 );
	}
	range->offset = offset;
	range->size   = size;
	range->flags  = flags;

	return( 1 );
}

