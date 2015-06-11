/*
 * Entry identifier (ENTRYID) functions
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

#include <common.h>
#include <memory.h>
#include <types.h>

#include <liberror.h>

#include "libfmapi_definitions.h"
#include "libfmapi_entry_identifier.h"
#include "libfmapi_libfguid.h"

/* Initialize an entry identifier
 * Returns 1 if successful or -1 on error
 */
int libfmapi_entry_identifier_initialize(
     libfmapi_entry_identifier_t **entry_identifier,
     liberror_error_t **error )
{
	libfmapi_internal_entry_identifier_t *internal_entry_identifier = NULL;
	static char *function                                           = "libfmapi_entry_identifier_initialize";

	if( entry_identifier == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid entry identifier.",
		 function );

		return( -1 );
	}
	if( *entry_identifier != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid entry identifier value already set.",
		 function );

		return( -1 );
	}
	internal_entry_identifier = memory_allocate_structure(
	                             libfmapi_internal_entry_identifier_t );

	if( internal_entry_identifier == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create entry identifier.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_entry_identifier,
	     0,
	     sizeof( libfmapi_internal_entry_identifier_t ) ) == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear entry identifier.",
		 function );

		goto on_error;
	}
	if( libfguid_identifier_initialize(
	     &( internal_entry_identifier->service_provider_identifier ),
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create service provider identifier.",
		 function );

		goto on_error;
	}
	*entry_identifier = (libfmapi_entry_identifier_t *) internal_entry_identifier;

	return( 1 );

on_error:
	if( internal_entry_identifier != NULL )
	{
		memory_free(
		 internal_entry_identifier );
	}
	return( -1 );
}

/* Frees an entry identifier
 * Returns 1 if successful or -1 on error
 */
int libfmapi_entry_identifier_free(
     libfmapi_entry_identifier_t **entry_identifier,
     liberror_error_t **error )
{
	libfmapi_internal_entry_identifier_t *internal_entry_identifier = NULL;
	static char *function                                           = "libfmapi_entry_identifier_free";
	int result                                                      = 1;

	if( entry_identifier == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid entry identifier.",
		 function );

		return( -1 );
	}
	if( *entry_identifier != NULL )
	{
		internal_entry_identifier = (libfmapi_internal_entry_identifier_t *) *entry_identifier;
		*entry_identifier         = NULL;

		if( libfguid_identifier_free(
		     &( internal_entry_identifier->service_provider_identifier ),
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free service provider identifier.",
			 function );

			result = -1;
		}
		memory_free(
		 internal_entry_identifier );
	}
	return( result );
}

/* Converts a byte stream into an entry identifier
 * Returns 1 if successful or -1 on error
 */
int libfmapi_entry_identifier_copy_from_byte_stream(
     libfmapi_entry_identifier_t *entry_identifier,
     uint8_t *byte_stream,
     size_t byte_stream_size,
     liberror_error_t **error )
{
	libfmapi_internal_entry_identifier_t *internal_entry_identifier = NULL;
	static char *function                                           = "libfmapi_entry_identifier_copy_from_byte_stream";

	if( entry_identifier == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid entry identifier.",
		 function );

		return( -1 );
	}
	internal_entry_identifier = (libfmapi_internal_entry_identifier_t *) entry_identifier;

	if( byte_stream == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid byte stream.",
		 function );

		return( -1 );
	}
	if( byte_stream_size < 20 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: byte stream too small.",
		 function );

		return( -1 );
	}
	if( byte_stream_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: byte stream size exceeds maximum.",
		 function );

		return( -1 );
	}
	if( memory_copy(
	     internal_entry_identifier->flags,
	     byte_stream,
	     4 ) == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_COPY_FAILED,
		 "%s: unable to copy flags.",
		 function );

		return( -1 );
	}
	byte_stream += 4;

	if( libfguid_identifier_copy_from_byte_stream(
	     internal_entry_identifier->service_provider_identifier,
	     byte_stream,
	     16,
	     LIBFGUID_ENDIAN_LITTLE,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy service provider identifier from byte stream.",
		 function );

		return( -1 );
	}
	byte_stream += 16;

	return( 1 );
}

