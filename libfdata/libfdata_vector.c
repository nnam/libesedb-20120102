/*
 * The vector functions
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
#include <libnotify.h>

#include "libfdata_array_type.h"
#include "libfdata_date_time.h"
#include "libfdata_definitions.h"
#include "libfdata_libbfio.h"
#include "libfdata_libfcache.h"
#include "libfdata_range.h"
#include "libfdata_types.h"
#include "libfdata_vector.h"

#define libfdata_vector_calculate_cache_entry_index( element_index, number_of_cache_entries ) \
	element_index % number_of_cache_entries

/* Initializes the vector
 *
 * If the flag LIBFDATA_FLAG_IO_HANDLE_MANAGED is set the vector
 * takes over management of the IO handle and the IO handle is freed when
 * no longer needed
 *
 * Returns 1 if successful or -1 on error
 */
int libfdata_vector_initialize(
     libfdata_vector_t **vector,
     size64_t element_size,
     intptr_t *io_handle,
     int (*free_io_handle)(
            intptr_t **io_handle,
            liberror_error_t **error ),
     int (*clone_io_handle)(
            intptr_t **destination_io_handle,
            intptr_t *source_io_handle,
            liberror_error_t **error ),
     int (*read_element_data)(
            intptr_t *io_handle,
            libbfio_handle_t *file_io_handle,
            libfdata_vector_t *vector,
            libfcache_cache_t *cache,
            int element_index,
            off64_t element_data_offset,
            size64_t element_data_size,
            uint8_t read_flags,
            liberror_error_t **error ),
     uint8_t flags,
     liberror_error_t **error )
{
	libfdata_internal_vector_t *internal_vector = NULL;
	static char *function                       = "libfdata_vector_initialize";

	if( vector == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid vector.",
		 function );

		return( -1 );
	}
	if( *vector != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid vector value already set.",
		 function );

		return( -1 );
	}
	if( element_size == 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_ZERO_OR_LESS,
		 "%s: invalid element size value zero or less.",
		 function );

		return( -1 );
	}
	if( read_element_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid read element data function.",
		 function );

		return( -1 );
	}
	internal_vector = memory_allocate_structure(
	                   libfdata_internal_vector_t );

	if( internal_vector == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create vector.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_vector,
	     0,
	     sizeof( libfdata_internal_vector_t ) ) == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear vector.",
		 function );

		goto on_error;
	}
	if( libfdata_array_initialize(
	     &( internal_vector->segments ),
	     0,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create segments array.",
		 function );

		goto on_error;
	}
	internal_vector->element_size      = element_size;
	internal_vector->timestamp         = libfdata_date_time_get_timestamp();
	internal_vector->flags            |= flags;
	internal_vector->io_handle         = io_handle;
	internal_vector->free_io_handle    = free_io_handle;
	internal_vector->clone_io_handle   = clone_io_handle;
	internal_vector->read_element_data = read_element_data;

	*vector = (libfdata_vector_t *) internal_vector;

	return( 1 );

on_error:
	if( internal_vector != NULL )
	{
		memory_free(
		 internal_vector );
	}
	return( -1 );
}

/* Frees the vector
 * Returns 1 if successful or -1 on error
 */
int libfdata_vector_free(
     libfdata_vector_t **vector,
     liberror_error_t **error )
{
	libfdata_internal_vector_t *internal_vector = NULL;
	static char *function                       = "libfdata_vector_free";
	int result                                  = 1;

	if( vector == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid vector.",
		 function );

		return( -1 );
	}
	if( *vector != NULL )
	{
		internal_vector = (libfdata_internal_vector_t *) *vector;
		*vector         = NULL;

		if( libfdata_array_free(
		     &( internal_vector->segments ),
		     (int (*)(intptr_t **, liberror_error_t **)) &libfdata_range_free,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free the segments array.",
			 function );

			result = -1;
		}
		if( ( internal_vector->flags & LIBFDATA_FLAG_IO_HANDLE_MANAGED ) != 0 )
		{
			if( internal_vector->io_handle != NULL )
			{
				if( internal_vector->free_io_handle != NULL )
				{
					if( internal_vector->free_io_handle(
					     &( internal_vector->io_handle ),
					     error ) != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
						 "%s: unable to free IO handle.",
						 function );

						result = -1;
					}
				}
			}
		}
		memory_free(
		 internal_vector );
	}
	return( result );
}

/* Clones (duplicates) the vector
 * Returns 1 if successful or -1 on error
 */
int libfdata_vector_clone(
     libfdata_vector_t **destination_vector,
     libfdata_vector_t *source_vector,
     liberror_error_t **error )
{
	libfdata_internal_vector_t *internal_source_vector = NULL;
	intptr_t *destination_io_handle                    = NULL;
	static char *function                              = "libfdata_vector_clone";

	if( destination_vector == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid destination vector.",
		 function );

		return( -1 );
	}
	if( *destination_vector != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: destination vector already set.",
		 function );

		return( -1 );
	}
	if( source_vector == NULL )
	{
		*destination_vector = source_vector;

		return( 1 );
	}
	internal_source_vector = (libfdata_internal_vector_t *) source_vector;

	if( internal_source_vector->io_handle != NULL )
	{
		if( internal_source_vector->free_io_handle == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid source vector - missing free IO handle function.",
			 function );

			goto on_error;
		}
		if( internal_source_vector->clone_io_handle == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid source vector - missing clone IO handle function.",
			 function );

			goto on_error;
		}
		if( internal_source_vector->clone_io_handle(
		     &destination_io_handle,
		     internal_source_vector->io_handle,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to clone IO handle.",
			 function );

			goto on_error;
		}
	}
	if( libfdata_vector_initialize(
	     destination_vector,
	     internal_source_vector->element_size,
	     destination_io_handle,
	     internal_source_vector->free_io_handle,
	     internal_source_vector->clone_io_handle,
	     internal_source_vector->read_element_data,
	     LIBFDATA_FLAG_IO_HANDLE_MANAGED,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create destination vector.",
		 function );

		goto on_error;
	}
/* TODO clone data ranges */
	return( 1 );

on_error:
	if( destination_io_handle != NULL )
	{
		internal_source_vector->free_io_handle(
		 &destination_io_handle,
		 NULL );
	}
	return( -1 );
}

/* Retrieves the number of elements of the vector
 * Returns 1 if successful or -1 on error
 */
int libfdata_vector_get_number_of_elements(
     libfdata_vector_t *vector,
     int *number_of_elements,
     liberror_error_t **error )
{
	libfdata_internal_vector_t *internal_vector = NULL;
	static char *function                       = "libfdata_vector_get_number_of_elements";

	if( vector == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid vector.",
		 function );

		return( -1 );
	}
	internal_vector = (libfdata_internal_vector_t *) vector;

	if( internal_vector->element_size == 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid vector - element size value out of bounds.",
		 function );

		return( -1 );
	}
	if( number_of_elements == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid number of elements.",
		 function );

		return( -1 );
	}
	*number_of_elements = (int) ( internal_vector->value_size / internal_vector->element_size );

	return( 1 );
}

/* Retrieves the element index for a specific offset
 * Returns 1 if successful or -1 on error
 */
int libfdata_vector_get_element_index_at_offset(
     libfdata_vector_t *vector,
     off64_t value_offset,
     int *element_index,
     size_t *element_offset,
     liberror_error_t **error )
{
	libfdata_internal_vector_t *internal_vector = NULL;
	static char *function                       = "libfdata_vector_get_element_index_at_offset";
	int calculated_element_index                = 0;

	if( vector == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid vector.",
		 function );

		return( -1 );
	}
	internal_vector = (libfdata_internal_vector_t *) vector;

	if( internal_vector->element_size == 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid vector - element size value out of bounds.",
		 function );

		return( -1 );
	}
	if( value_offset < 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_LESS_THAN_ZERO,
		 "%s: invalid value offset value less than zero.",
		 function );

		return( -1 );
	}
	if( element_index == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid element index.",
		 function );

		return( -1 );
	}
	if( element_offset == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid element offset.",
		 function );

		return( -1 );
	}
	calculated_element_index = (int) ( value_offset / internal_vector->element_size );

	value_offset -= (off64_t) ( calculated_element_index * internal_vector->element_size );

	if( ( value_offset < 0 )
	 || ( value_offset > (off64_t) SSIZE_MAX ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid element offset value out of bounds.",
		 function );

		return( -1 );
	}
	*element_index  = calculated_element_index;
	*element_offset = (size_t) value_offset;

	return( 1 );
}

/* Appends a segment offset and size
 * Returns 1 if successful or -1 on error
 */
int libfdata_vector_append_segment(
     libfdata_vector_t *vector,
     off64_t segment_offset,
     size64_t segment_size,
     uint32_t segment_flags,
     liberror_error_t **error )
{
	libfdata_internal_vector_t *internal_vector = NULL;
	libfdata_range_t *segment_data_range        = NULL;
	static char *function                       = "libfdata_vector_append_segment";
	int segment_index                           = 0;

	if( vector == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid vector.",
		 function );

		return( -1 );
	}
	internal_vector = (libfdata_internal_vector_t *) vector;

	if( libfdata_range_initialize(
	     &segment_data_range,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create segment data range.",
		 function );

		goto on_error;
	}
	if( libfdata_range_set_values(
	     segment_data_range,
	     segment_offset,
	     segment_size,
	     segment_flags,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set segment data range.",
		 function );

		goto on_error;
	}
	if( libfdata_array_append_entry(
	     internal_vector->segments,
	     &segment_index,
	     (intptr_t *) segment_data_range,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to append data range to segments array.",
		 function );

		goto on_error;
	}
	internal_vector->value_size += segment_size;

	return( 1 );

on_error:
	if( segment_data_range != NULL )
	{
		libfdata_range_free(
		 &segment_data_range,
		 NULL );
	}
	return( -1 );
}

/* Retrieves the value of a specific element
 * Returns 1 if successful or -1 on error
 */
int libfdata_vector_get_element_value_by_index(
     libfdata_vector_t *vector,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     int element_index,
     intptr_t **element_value,
     uint8_t read_flags,
     liberror_error_t **error )
{
	libfcache_cache_value_t *cache_value         = NULL;
	libfdata_internal_vector_t *internal_vector = NULL;
	libfdata_range_t *segment_data_range        = NULL;
	static char *function                       = "libfdata_vector_get_element_value_by_index";
	off64_t cache_value_offset                  = (off64_t) -1;
	off64_t element_data_offset                 = 0;
	time_t cache_value_timestamp                = 0;
	int cache_entry_index                       = -1;
	int number_of_cache_entries                 = 0;
	int number_of_segments                      = 0;
	int result                                  = 0;
	int segment_index                           = 0;

	if( vector == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid vector.",
		 function );

		return( -1 );
	}
	internal_vector = (libfdata_internal_vector_t *) vector;

	if( internal_vector->element_size == 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid vector - element size value out of bounds.",
		 function );

		return( -1 );
	}
	if( ( internal_vector->value_size == 0 )
	 || ( internal_vector->value_size > (off64_t) INT64_MAX ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid vector - value size value out of bounds.",
		 function );

		return( -1 );
	}
	if( element_index < 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid element index value out of bounds.",
		 function );

		return( -1 );
	}
	element_data_offset = (off64_t) ( element_index * internal_vector->element_size );

	if( element_data_offset > (off64_t) internal_vector->value_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid element index value out of bounds.",
		 function );

		return( -1 );
	}
	if( libfdata_array_get_number_of_entries(
	     internal_vector->segments,
	     &number_of_segments,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of segments.",
		 function );

		return( -1 );
	}
	if( number_of_segments <= 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid number of segments value out of bounds.",
		 function );

		return( -1 );
	}
	for( segment_index = 0;
	     segment_index < number_of_segments;
	     segment_index++ )
	{
		if( libfdata_array_get_entry_by_index(
		     internal_vector->segments,
		     segment_index,
		     (intptr_t **) &segment_data_range,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve segment data range: %d from array.",
			 function,
			 segment_index );

			return( -1 );
		}
		if( segment_data_range == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing segment data range: %d.",
			 function,
			 segment_index );

			return( -1 );
		}
/* TODO what about compressed data ranges */
		if( element_data_offset < (off64_t) segment_data_range->size )
		{
			element_data_offset += segment_data_range->offset;

			break;
		}
		element_data_offset -= segment_data_range->size;
	}
	if( segment_index >= number_of_segments )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid segment index value out of bounds.",
		 function );

		return( -1 );
	}
	if( libfcache_cache_get_number_of_entries(
	     cache,
	     &number_of_cache_entries,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of cache entries.",
		 function );

		return( -1 );
	}
	if( number_of_cache_entries <= 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid number of cache entries value out of bounds.",
		 function );

		return( -1 );
	}
	if( ( read_flags & LIBFDATA_READ_FLAG_IGNORE_CACHE ) == 0 )
	{
		cache_entry_index = libfdata_vector_calculate_cache_entry_index(
		                     element_index,
		                     number_of_cache_entries );

		if( libfcache_cache_get_value_by_index(
		     cache,
		     cache_entry_index,
		     &cache_value,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve cache entry: %d from cache.",
			 function,
			 cache_entry_index );

			return( -1 );
		}
		if( cache_value != NULL )
		{
			if( libfcache_cache_value_get_identifier(
			     cache_value,
			     &cache_value_offset,
			     &cache_value_timestamp,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve cache value identifier.",
				 function );

				return( -1 );
			}
		}
		if( ( element_data_offset == cache_value_offset )
		 && ( internal_vector->timestamp == cache_value_timestamp ) )
		{
			result = 1;
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libnotify_verbose != 0 )
		{
			if( result == 0 )
			{
				libnotify_printf(
				 "%s: cache: 0x%08" PRIjx " miss (%d out of %d)\n",
				 function,
				 (intptr_t) cache,
				 cache_entry_index,
				 number_of_cache_entries );
			}
			else
			{
				libnotify_printf(
				 "%s: cache: 0x%08" PRIjx " hit (%d out of %d)\n",
				 function,
				 (intptr_t) cache,
				 cache_entry_index,
				 number_of_cache_entries );
			}
		}
#endif
	}
	if( result == 0 )
	{
#if defined( HAVE_DEBUG_OUTPUT )
		if( libnotify_verbose != 0 )
		{
			libnotify_printf(
			 "%s: reading element data at offset: %" PRIi64 " (0x%08" PRIx64 ") of size: %" PRIu64 "\n",
			 function,
			 element_data_offset,
			 element_data_offset,
			 internal_vector->element_size );
		}
#endif
		/* Read the element data from the file IO handle
		 */
		if( internal_vector->read_element_data(
		     internal_vector->io_handle,
		     file_io_handle,
		     vector,
		     cache,
		     element_index,
		     element_data_offset,
		     internal_vector->element_size,
		     read_flags,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read element data at offset: %" PRIi64 ".",
			 function,
			 element_data_offset );

			return( -1 );
		}
		cache_entry_index = libfdata_vector_calculate_cache_entry_index(
		                     element_index,
		                     number_of_cache_entries );

		if( libfcache_cache_get_value_by_index(
		     cache,
		     cache_entry_index,
		     &cache_value,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve cache entry: %d from cache.",
			 function,
			 cache_entry_index );

			return( -1 );
		}
		if( cache_value != NULL )
		{
			if( libfcache_cache_value_get_identifier(
			     cache_value,
			     &cache_value_offset,
			     &cache_value_timestamp,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve cache value identifier.",
				 function );

				return( -1 );
			}
		}
		if( ( element_data_offset != cache_value_offset )
		 || ( internal_vector->timestamp != cache_value_timestamp ) )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing cache value.",
			 function );

			return( -1 );
		}
	}
	if( libfcache_cache_value_get_value(
	     cache_value,
	     element_value,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve element value.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the value an element at a specific offset
 * Returns 1 if successful or -1 on error
 */
int libfdata_vector_get_element_value_at_offset(
     libfdata_vector_t *vector,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     off64_t value_offset,
     intptr_t **element_value,
     uint8_t read_flags,
     liberror_error_t **error )
{
	static char *function = "libfdata_vector_get_element_value_at_offset";
	size_t element_offset = 0;
	int element_index     = 0;

	if( libfdata_vector_get_element_index_at_offset(
	     vector,
	     value_offset,
	     &element_index,
	     &element_offset,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve element index at offset: %" PRIi64 ".",
		 function,
		 value_offset );

		return( -1 );
	}
	if( libfdata_vector_get_element_value_by_index(
	     vector,
	     file_io_handle,
	     cache,
	     element_index,
	     element_value,
	     read_flags,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve element: %d value.",
		 function,
		 element_index );

		return( -1 );
	}
	return( 1 );
}

/* Sets the value of a specific element
 *
 * If the flag LIBFDATA_VECTOR_ELEMENT_VALUE_FLAG_MANAGED is set the vector
 * takes over management of the value and the value is freed when
 * no longer needed.
 *
 * Returns 1 if successful or -1 on error
 */
int libfdata_vector_set_element_value_by_index(
     libfdata_vector_t *vector,
     libfcache_cache_t *cache,
     int element_index,
     intptr_t *element_value,
     int (*free_element_value)(
            intptr_t **element_value,
            liberror_error_t **error ),
     uint8_t flags,
     liberror_error_t **error )
{
	libfdata_internal_vector_t *internal_vector = NULL;
	libfdata_range_t *segment_data_range        = NULL;
	static char *function                       = "libfdata_vector_set_element_value_by_index";
	off64_t element_data_offset                 = 0;
	int cache_entry_index                       = -1;
	int number_of_cache_entries                 = 0;
	int number_of_segments                      = 0;
	int segment_index                           = 0;

	if( vector == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid vector.",
		 function );

		return( -1 );
	}
	internal_vector = (libfdata_internal_vector_t *) vector;

	if( internal_vector->element_size == 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid vector - element size value out of bounds.",
		 function );

		return( -1 );
	}
	if( ( internal_vector->value_size == 0 )
	 || ( internal_vector->value_size > (off64_t) INT64_MAX ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid vector - value size value out of bounds.",
		 function );

		return( -1 );
	}
	if( element_index < 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid element index value out of bounds.",
		 function );

		return( -1 );
	}
	element_data_offset = (off64_t) ( element_index * internal_vector->element_size );

	if( element_data_offset > (off64_t) internal_vector->value_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid element index value out of bounds.",
		 function );

		return( -1 );
	}
	if( libfdata_array_get_number_of_entries(
	     internal_vector->segments,
	     &number_of_segments,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of segments.",
		 function );

		return( -1 );
	}
	if( number_of_segments <= 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid number of segments value out of bounds.",
		 function );

		return( -1 );
	}
	for( segment_index = 0;
	     segment_index < number_of_segments;
	     segment_index++ )
	{
		if( libfdata_array_get_entry_by_index(
		     internal_vector->segments,
		     segment_index,
		     (intptr_t **) &segment_data_range,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve segment data range: %d from array.",
			 function,
			 segment_index );

			return( -1 );
		}
		if( segment_data_range == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing segment data range: %d.",
			 function,
			 segment_index );

			return( -1 );
		}
/* TODO what about compressed data ranges */
		if( element_data_offset < (off64_t) segment_data_range->size )
		{
			element_data_offset += segment_data_range->offset;

			break;
		}
		element_data_offset -= segment_data_range->size;
	}
	if( segment_index >= number_of_segments )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid segment index value out of bounds.",
		 function );

		return( -1 );
	}
	if( libfcache_cache_get_number_of_entries(
	     cache,
	     &number_of_cache_entries,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of cache entries.",
		 function );

		return( -1 );
	}
	if( number_of_cache_entries <= 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid number of cache entries value out of bounds.",
		 function );

		return( -1 );
	}
	cache_entry_index = libfdata_vector_calculate_cache_entry_index(
	                     element_index,
	                     number_of_cache_entries );

	if( libfcache_cache_set_value_by_index(
	     cache,
	     cache_entry_index,
	     (int64_t) element_data_offset,
	     internal_vector->timestamp,
	     element_value,
	     free_element_value,
	     flags,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set value in cache entry: %d.",
		 function,
		 cache_entry_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the element size of the vector
 * Returns 1 if successful or -1 on error
 */
int libfdata_vector_get_element_size(
     libfdata_vector_t *vector,
     size64_t *element_size,
     liberror_error_t **error )
{
	libfdata_internal_vector_t *internal_vector = NULL;
	static char *function                       = "libfdata_vector_get_element_size";

	if( vector == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid vector.",
		 function );

		return( -1 );
	}
	internal_vector = (libfdata_internal_vector_t *) vector;

	if( element_size == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid element size.",
		 function );

		return( -1 );
	}
	*element_size = internal_vector->element_size;

	return( 1 );
}

/* Retrieves the value size of the vector
 * Returns 1 if successful or -1 on error
 */
int libfdata_vector_get_value_size(
     libfdata_vector_t *vector,
     size64_t *value_size,
     liberror_error_t **error )
{
	libfdata_internal_vector_t *internal_vector = NULL;
	static char *function                       = "libfdata_vector_get_value_size";

	if( vector == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid vector.",
		 function );

		return( -1 );
	}
	internal_vector = (libfdata_internal_vector_t *) vector;

	if( value_size == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value size.",
		 function );

		return( -1 );
	}
	*value_size = internal_vector->value_size;

	return( 1 );
}

