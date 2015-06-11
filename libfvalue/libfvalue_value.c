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

#include <common.h>
#include <byte_stream.h>
#include <file_stream.h>
#include <memory.h>
#include <types.h>

#include <libcstring.h>
#include <liberror.h>
#include <libnotify.h>

#include "libfvalue_array_type.h"
#include "libfvalue_codepage.h"
#include "libfvalue_definitions.h"
#include "libfvalue_libuna.h"
#include "libfvalue_string.h"
#include "libfvalue_types.h"
#include "libfvalue_value.h"
#include "libfvalue_value_entry.h"

/* Initialize a value
 * Returns 1 if successful or -1 on error
 */
int libfvalue_value_initialize(
     libfvalue_value_t **value,
     uint8_t type,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	static char *function                      = "libfvalue_value_initialize";

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	if( *value != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid value value already set.",
		 function );

		return( -1 );
	}
	internal_value = memory_allocate_structure(
	                  libfvalue_internal_value_t );

	if( internal_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create value.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_value,
	     0,
	     sizeof( libfvalue_internal_value_t ) ) == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear value.",
		 function );

		goto on_error;
	}
	internal_value->type = type;

	*value = (libfvalue_value_t *) internal_value;

	return( 1 );

on_error:
	if( internal_value != NULL )
	{
		memory_free(
		 internal_value );
	}
	return( -1 );
}

/* Frees a value
 * Returns 1 if successful or -1 on error
 */
int libfvalue_value_free(
     libfvalue_value_t **value,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	static char *function                      = "libfvalue_value_free";
	int result                                 = 1;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	if( *value != NULL )
	{
		internal_value = (libfvalue_internal_value_t *) *value;
		*value         = NULL;

		if( ( internal_value->flags & LIBFVALUE_VALUE_FLAG_IDENTIFIER_MANAGED ) != 0 )
		{
			if( internal_value->identifier != NULL )
			{
				memory_free(
				 internal_value->identifier );
			}
		}
		if( internal_value->value_entries != NULL )
		{
			if( libfvalue_array_free(
			     &( internal_value->value_entries ),
			     (int (*)(intptr_t **, liberror_error_t **)) &libfvalue_value_entry_free,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free value entries array.",
				 function );

				result = -1;
			}
		}
		if( ( internal_value->flags & LIBFVALUE_VALUE_FLAG_DATA_MANAGED ) != 0 )
		{
			if( internal_value->data != NULL )
			{
				memory_free(
				 internal_value->data );
			}
		}
		if( ( internal_value->flags & LIBFVALUE_VALUE_FLAG_METADATA_MANAGED ) != 0 )
		{
			if( internal_value->metadata != NULL )
			{
				memory_free(
				 internal_value->metadata );
			}
		}
		memory_free(
		 internal_value );
	}
	return( result );
}

/* Clones a value
 * Returns 1 if successful or -1 on error
 */
int libfvalue_value_clone(
     libfvalue_value_t **destination_value,
     libfvalue_value_t *source_value,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_source_value = NULL;
	static char *function                             = "libfvalue_value_clone";

	if( destination_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid destination value.",
		 function );

		return( -1 );
	}
	if( *destination_value != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: destination value already set.",
		 function );

		return( -1 );
	}
	if( source_value == NULL )
	{
		*destination_value = NULL;

		return( 1 );
	}
	internal_source_value = (libfvalue_internal_value_t *) source_value;

	if( libfvalue_value_initialize(
	     destination_value,
	     internal_source_value->type,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create destination value.",
		 function );

		return( -1 );
	}
	if( *destination_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing destination value.",
		 function );

		return( -1 );
	}
	if( internal_source_value->identifier != NULL )
	{
		if( libfvalue_value_set_identifier(
		     *destination_value,
		     internal_source_value->identifier,
		     internal_source_value->identifier_size,
		     LIBFVALUE_VALUE_FLAG_IDENTIFIER_MANAGED,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set identifier in destination value.",
			 function );

			goto on_error;
		}
	}
	if( internal_source_value->value_entries != NULL )
	{
		if( libfvalue_array_clone(
		     &( ( (libfvalue_internal_value_t *) *destination_value )->value_entries ),
		     internal_source_value->value_entries,
		     (int (*)(intptr_t **, liberror_error_t **)) &libfvalue_value_entry_free,
		     (int (*)(intptr_t **, intptr_t *, liberror_error_t **)) &libfvalue_value_entry_clone,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create destination value entries array.",
			 function );

			goto on_error;
		}
	}
	if( internal_source_value->data != NULL )
	{
		if( libfvalue_value_set_data(
		     *destination_value,
		     internal_source_value->data,
		     internal_source_value->data_size,
		     internal_source_value->byte_order,
		     LIBFVALUE_VALUE_DATA_FLAG_MANAGED,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set data in destination value.",
			 function );

			goto on_error;
		}
	}
	if( internal_source_value->metadata != NULL )
	{
		if( libfvalue_value_set_metadata(
		     *destination_value,
		     internal_source_value->metadata,
		     internal_source_value->metadata_size,
		     LIBFVALUE_VALUE_FLAG_METADATA_MANAGED,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set metadata in destination value.",
			 function );

			goto on_error;
		}
	}
	if( internal_source_value->codepage != 0 )
	{
		if( libfvalue_value_set_codepage(
		     *destination_value,
		     internal_source_value->codepage,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set codepage in destination value.",
			 function );

			goto on_error;
		}
	}
	return( 1 );

on_error:
	if( *destination_value != NULL )
	{
		libfvalue_value_free(
		 destination_value,
		 NULL );
	}
	return( -1 );
}

/* Retrieves the value type
 * Returns 1 if successful or -1 on error
 */
int libfvalue_value_get_type(
     libfvalue_value_t *value,
     uint8_t *value_type,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	static char *function                      = "libfvalue_value_get_type";

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( value_type == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value type.",
		 function );

		return( -1 );
	}
	*value_type = internal_value->type;

	return( 1 );
}

/* Compares two index values
 * Returns LIBFVALUE_ARRAY_COMPARE_LESS, LIBFVALUE_ARRAY_COMPARE_EQUAL, LIBFVALUE_ARRAY_COMPARE_GREATER if successful or -1 on error
 */
int libfvalue_compare_identifier(
     intptr_t *first_value,
     intptr_t *second_value,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_first_value  = NULL;
	libfvalue_internal_value_t *internal_second_value = NULL;
	static char *function                             = "libfvalue_compare_identifier";
	size_t compare_size                               = 0;
	int result                                        = 0;

	if( first_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid first value.",
		 function );

		return( -1 );
	}
	internal_first_value = (libfvalue_internal_value_t *) first_value;

	if( second_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid second value.",
		 function );

		return( -1 );
	}
	internal_second_value = (libfvalue_internal_value_t *) second_value;

	if( internal_first_value->identifier == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid first value - missing identifier.",
		 function );

		return( -1 );
	}
	if( internal_second_value->identifier == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid second value - missing identifier.",
		 function );

		return( -1 );
	}
	if( internal_first_value->identifier_size <= internal_second_value->identifier_size )
	{
		compare_size = internal_first_value->identifier_size;
	}
	else
	{
		compare_size = internal_second_value->identifier_size;
	}
	result = memory_compare(
	          internal_first_value->identifier,
	          internal_second_value->identifier,
	          compare_size );

	if( result < 0 )
	{
		return( LIBFVALUE_ARRAY_COMPARE_LESS );
	}
	else if( result > 0 )
	{
		return( LIBFVALUE_ARRAY_COMPARE_GREATER );
	}
	if( internal_first_value->identifier_size < internal_second_value->identifier_size )
	{
		return( LIBFVALUE_ARRAY_COMPARE_LESS );
	}
	else if( internal_first_value->identifier_size > internal_second_value->identifier_size )
	{
		return( LIBFVALUE_ARRAY_COMPARE_GREATER );
	}
	return( LIBFVALUE_ARRAY_COMPARE_EQUAL );
}

/* Retrieves the identifier
 * Returns 1 if successful or -1 on error
 */
int libfvalue_value_get_identifier(
     libfvalue_value_t *value,
     uint8_t **identifier,
     size_t *identifier_size,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	static char *function                      = "libfvalue_value_get_identifier";

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( identifier == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid identifier.",
		 function );

		return( -1 );
	}
	if( identifier_size == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid identifier size.",
		 function );

		return( -1 );
	}
	*identifier      = internal_value->identifier;
	*identifier_size = internal_value->identifier_size;

	return( 1 );
}

/* Sets the identifier
 * Returns 1 if successful or -1 on error
 */
int libfvalue_value_set_identifier(
     libfvalue_value_t *value,
     const uint8_t *identifier,
     size_t identifier_size,
     uint8_t flags,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	static char *function                      = "libfvalue_value_set_identifier";

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( internal_value->identifier != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid value - identifier already set.",
		 function );

		return( -1 );
	}
	if( identifier == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid identifier.",
		 function );

		return( -1 );
	}
	if( ( identifier_size == 0 )
	 || ( identifier_size > (size_t) SSIZE_MAX ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid identifier length value out of bounds.",
		 function );

		return( -1 );
	}
	if( ( flags & ~( LIBFVALUE_VALUE_IDENTIFIER_FLAG_MANAGED | LIBFVALUE_VALUE_IDENTIFIER_FLAG_CLONE_BY_REFERENCE ) ) != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported flags: 0x%02" PRIx8 ".",
		 function,
		 flags );

		return( -1 );
	}
	if( ( internal_value->flags & LIBFVALUE_VALUE_FLAG_IDENTIFIER_MANAGED ) != 0 )
	{
		if( internal_value->identifier != NULL )
		{
			memory_free(
			 internal_value->identifier );

			internal_value->identifier      = NULL;
			internal_value->identifier_size = 0;
		}
		internal_value->flags &= ~( LIBFVALUE_VALUE_FLAG_IDENTIFIER_MANAGED );
	}
	if( ( flags & LIBFVALUE_VALUE_IDENTIFIER_FLAG_CLONE_BY_REFERENCE ) != 0 )
	{
		internal_value->identifier = (uint8_t *) identifier;

		if( ( flags & LIBFVALUE_VALUE_IDENTIFIER_FLAG_MANAGED ) != 0 )
		{
			internal_value->flags |= LIBFVALUE_VALUE_FLAG_IDENTIFIER_MANAGED;
		}
	}
	else
	{
		internal_value->identifier = (uint8_t *) memory_allocate(
		                                          sizeof( uint8_t ) * identifier_size );

		if( internal_value->identifier == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create identifier.",
			 function );

			goto on_error;
		}
		if( memory_copy(
		     internal_value->identifier,
		     identifier,
		     identifier_size ) == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_COPY_FAILED,
			 "%s: unable to copy identifier.",
			 function );

			goto on_error;
		}
		internal_value->flags |= LIBFVALUE_VALUE_FLAG_IDENTIFIER_MANAGED;
	}
	internal_value->identifier_size = identifier_size;

	return( 1 );

on_error:
	if( internal_value->identifier != NULL )
	{
		memory_free(
		 internal_value->identifier );

		internal_value->identifier = NULL;
	}
	return( -1 );
}

/* Retrieves the value data type size
 * Returns 1 if successful, 0 if data type is not supported or -1 on error
 */
int libfvalue_value_get_data_type_size(
     libfvalue_internal_value_t *internal_value,
     size_t *data_type_size,
     liberror_error_t **error )
{
	static char *function = "libfvalue_get_value_data_type_size";
	int result            = 1;

	if( internal_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid internal value.",
		 function );

		return( -1 );
	}
	if( data_type_size == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid data type size.",
		 function );

		return( -1 );
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_BOOLEAN:
		case LIBFVALUE_VALUE_TYPE_INTEGER_8BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_8BIT:
			*data_type_size = 1;

			break;

		case LIBFVALUE_VALUE_TYPE_FATDATE:
		case LIBFVALUE_VALUE_TYPE_FATTIME:
		case LIBFVALUE_VALUE_TYPE_INTEGER_16BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_16BIT:
			*data_type_size = 2;

			break;

		case LIBFVALUE_VALUE_TYPE_FATDATETIME:
		case LIBFVALUE_VALUE_TYPE_INTEGER_32BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_32BIT:
		case LIBFVALUE_VALUE_TYPE_FLOATING_POINT_32BIT:
			*data_type_size = 4;

			break;

		case LIBFVALUE_VALUE_TYPE_FILETIME:
		case LIBFVALUE_VALUE_TYPE_NSF_TIMEDATE:
		case LIBFVALUE_VALUE_TYPE_INTEGER_64BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_64BIT:
		case LIBFVALUE_VALUE_TYPE_FLOATING_POINT_64BIT:
			*data_type_size = 8;

			break;

		default:
			result = 0;

			break;
	}
	return( result );
}

/* Initializes the data
 * Returns 1 if successful or -1 on error
 */
int libfvalue_value_initialize_data(
     libfvalue_internal_value_t *internal_value,
     liberror_error_t **error )
{
	static char *function       = "libfvalue_value_initialize_data";
	size_t data_size            = 0;
	int number_of_value_entries = 0;
	int result                  = 0;

	if( internal_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid internal value.",
		 function );

		return( -1 );
	}
	if( internal_value->data != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid value - data already set.",
		 function );

		return( -1 );
	}
	result = libfvalue_value_get_data_type_size(
		  internal_value,
		  &( internal_value->data_size ),
		  error );

	if( result == -1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve data type size.",
		 function );

		return( -1 );
	}
	if( ( internal_value->flags & LIBFVALUE_VALUE_FLAG_DATA_MANAGED ) != 0 )
	{
		if( internal_value->data != NULL )
		{
			memory_free(
			 internal_value->data );

			internal_value->data      = NULL;
			internal_value->data_size = 0;
		}
		internal_value->flags &= ~( LIBFVALUE_VALUE_FLAG_DATA_MANAGED );
	}
	if( internal_value->value_entries == NULL )
	{
		number_of_value_entries = 1;
	}
	else if( libfvalue_array_get_number_of_entries(
	          internal_value->value_entries,
	          &number_of_value_entries,
	          error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of entries in the value entries array.",
		 function );

		goto on_error;
	}
	if( number_of_value_entries <= 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid number of value entries value out of bounds.",
		 function );

		goto on_error;
	}
	data_size = internal_value->data_size;

	internal_value->data = (uint8_t *) memory_allocate(
	                                    sizeof( uint8_t ) * data_size );

	if( internal_value->data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create data.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_value->data,
	     0,
	     internal_value->data_size ) == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_COPY_FAILED,
		 "%s: unable to clear data.",
		 function );

		goto on_error;
	}
	internal_value->data_size = data_size;

	internal_value->flags |= LIBFVALUE_VALUE_FLAG_DATA_MANAGED;

	return( 1 );

on_error:
	if( internal_value->data != NULL )
	{
		memory_free(
		 internal_value->data );

		internal_value->data = NULL;
	}
	return( -1 );
}

/* Determines if the value has data
 * Returns 1 if the value has data, 0 if not or -1 on error
 */
int libfvalue_value_has_data(
     libfvalue_value_t *value,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	static char *function                      = "libfvalue_value_has_data";

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( internal_value->data != NULL )
	{
		return( 1 );
	}
	return( 0 );
}

/* Retrieves the data
 * Returns 1 if successful or -1 on error
 */
int libfvalue_value_get_data(
     libfvalue_value_t *value,
     uint8_t **data,
     size_t *data_size,
     uint8_t *byte_order,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	static char *function                      = "libfvalue_value_get_data";

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid data.",
		 function );

		return( -1 );
	}
	if( data_size == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid data size.",
		 function );

		return( -1 );
	}
	if( byte_order == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid byte order.",
		 function );

		return( -1 );
	}
	*data       = internal_value->data;
	*data_size  = internal_value->data_size;
	*byte_order = internal_value->byte_order;

	return( 1 );
}

/* Retrieves the data size
 * Returns 1 if successful or -1 on error
 */
int libfvalue_value_get_data_size(
     libfvalue_value_t *value,
     size_t *data_size,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	static char *function                      = "libfvalue_value_get_data_size";

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( data_size == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid data size.",
		 function );

		return( -1 );
	}
	*data_size  = internal_value->data_size;

	return( 1 );
}

/* Sets the data
 * Returns 1 if successful or -1 on error
 */
int libfvalue_value_set_data(
     libfvalue_value_t *value,
     const uint8_t *data,
     size_t data_size,
     uint8_t byte_order,
     uint8_t flags,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	static char *function                      = "libfvalue_value_set_data";

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( ( byte_order != LIBFVALUE_ENDIAN_NATIVE )
	 && ( byte_order != LIBFVALUE_ENDIAN_BIG )
	 && ( byte_order != LIBFVALUE_ENDIAN_LITTLE ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported byte order: 0x%02" PRIx8 ".",
		 function,
		 byte_order );

		return( -1 );
	}
	if( ( flags & ~( LIBFVALUE_VALUE_DATA_FLAG_MANAGED | LIBFVALUE_VALUE_DATA_FLAG_CLONE_BY_REFERENCE ) ) != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported flags: 0x%02" PRIx8 ".",
		 function,
		 flags );

		return( -1 );
	}
	if( ( internal_value->flags & LIBFVALUE_VALUE_FLAG_DATA_MANAGED ) != 0 )
	{
		if( internal_value->data != NULL )
		{
			memory_free(
			 internal_value->data );

			internal_value->data      = NULL;
			internal_value->data_size = 0;
		}
		internal_value->flags &= ~( LIBFVALUE_VALUE_FLAG_DATA_MANAGED );
	}
	/* Make sure empty values have data that refers to NULL
	 */
	if( ( data == NULL )
	 || ( data_size == 0 ) )
	{
		internal_value->data = NULL;
	}
	else if( ( flags & LIBFVALUE_VALUE_DATA_FLAG_CLONE_BY_REFERENCE ) != 0 )
	{
		internal_value->data = (uint8_t *) data;

		if( ( flags & LIBFVALUE_VALUE_DATA_FLAG_MANAGED ) != 0 )
		{
			internal_value->flags |= LIBFVALUE_VALUE_FLAG_DATA_MANAGED;
		}
	}
	else
	{
		internal_value->data = (uint8_t *) memory_allocate(
		                                    sizeof( uint8_t ) * data_size );

		if( internal_value->data == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create data.",
			 function );

			goto on_error;
		}
		if( memory_copy(
		     internal_value->data,
		     data,
		     data_size ) == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_COPY_FAILED,
			 "%s: unable to copy data.",
			 function );

			goto on_error;
		}
		internal_value->flags |= LIBFVALUE_VALUE_FLAG_DATA_MANAGED;
	}
	internal_value->data_size  = data_size;
	internal_value->byte_order = byte_order;

	return( 1 );

on_error:
	if( internal_value->data != NULL )
	{
		memory_free(
		 internal_value->data );

		internal_value->data = NULL;
	}
	return( -1 );
}

/* Sets the data of a variable sized string
 * This function will look for a NUL-character as the indicator of
 * the end of the string upto data_size
 * Returns 1 if successfull, 0 if value type not supported or -1 on error
 */
int libfvalue_value_set_data_string(
     libfvalue_value_t *value,
     const uint8_t *data,
     size_t data_size,
     uint8_t byte_order,
     uint8_t flags,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	static char *function                      = "libfvalue_value_set_data_string";
	size_t data_index                          = 0;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM )
	 && ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_UTF8 )
	 && ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_UTF16 )
	 && ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_UTF32 ) )
	{
		return( 0 );
	}
	if( data != NULL )
	{
		if( ( internal_value->type == LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM )
		 || ( internal_value->type == LIBFVALUE_VALUE_TYPE_STRING_UTF8 ) )
		{
			while( data_index < data_size )
			{
				if( data[ data_index ] == 0 )
				{
					data_index += 1;

					break;
				}
				data_index += 1;
			}
		}
		else if( internal_value->type == LIBFVALUE_VALUE_TYPE_STRING_UTF16 )
		{
			while( ( data_index + 1 ) < data_size )
			{
				if( ( data[ data_index ] == 0 )
				 && ( data[ data_index + 1 ] == 0 ) )
				{
					data_index += 2;

					break;
				}
				data_index += 2;
			}
		}
		else if( internal_value->type == LIBFVALUE_VALUE_TYPE_STRING_UTF32 )
		{
			while( ( data_index + 3 ) < data_size )
			{
				if( ( data[ data_index ] == 0 )
				 && ( data[ data_index + 1 ] == 0 )
				 && ( data[ data_index + 2 ] == 0 )
				 && ( data[ data_index + 3 ] == 0 ) )
				{
					data_index += 4;

					break;
				}
				data_index += 4;
			}
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libnotify_verbose != 0 )
		{
			libnotify_printf(
			 "%s: string data:\n",
			 function );
			libnotify_print_data(
			 data,
			 data_index );
		}
#endif
	}
	if( libfvalue_value_set_data(
	     value,
	     data,
	     data_index,
	     byte_order,
	     flags,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set data.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Sets the data of an array of variable sized strings
 * This function will look for a NUL-character as the indicator of
 * the end of the string upto data_size
 * Returns 1 if successfull, 0 if value type not supported or -1 on error
 */
int libfvalue_value_set_data_strings_array(
     libfvalue_value_t *value,
     const uint8_t *data,
     size_t data_size,
     uint8_t byte_order,
     uint8_t flags,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	static char *function                      = "libfvalue_value_set_data_strings_array";
	size_t data_index                          = 0;
	size_t last_data_index                     = 0;
	size_t value_entry_index                   = 0;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM )
	 && ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_UTF8 )
	 && ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_UTF16 )
	 && ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_UTF32 ) )
	{
		return( 0 );
	}
	if( libfvalue_value_set_data(
	     value,
	     data,
	     data_size,
	     byte_order,
	     flags,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set data.",
		 function );

		return( -1 );
	}
	if( data != NULL )
	{
		while( data_index < data_size )
		{
			if( ( internal_value->type == LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM )
			 || ( internal_value->type == LIBFVALUE_VALUE_TYPE_STRING_UTF8 ) )
			{
				while( data_index < data_size )
				{
					if( data[ data_index ] == 0 )
					{
						data_index += 1;

						break;
					}
					data_index += 1;
				}
			}
			else if( internal_value->type == LIBFVALUE_VALUE_TYPE_STRING_UTF16 )
			{
				while( ( data_index + 1 ) < data_size )
				{
					if( ( data[ data_index ] == 0 )
					 && ( data[ data_index + 1 ] == 0 ) )
					{
						data_index += 2;

						break;
					}
					data_index += 2;
				}
			}
			else if( internal_value->type == LIBFVALUE_VALUE_TYPE_STRING_UTF32 )
			{
				while( ( data_index + 3 ) < data_size )
				{
					if( ( data[ data_index ] == 0 )
					 && ( data[ data_index + 1 ] == 0 )
					 && ( data[ data_index + 2 ] == 0 )
					 && ( data[ data_index + 3 ] == 0 ) )
					{
						data_index += 4;

						break;
					}
					data_index += 4;
				}
			}
#if defined( HAVE_DEBUG_OUTPUT )
			if( libnotify_verbose != 0 )
			{
				libnotify_printf(
				 "%s: strings array value entry: %d data:\n",
				 function,
				 value_entry_index );
				libnotify_print_data(
				 data,
				 data_index );
			}
#endif
			if( libfvalue_value_append_value_entry(
			     value,
			     last_data_index,
			     data_index - last_data_index,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_APPEND_FAILED,
				 "%s: unable to append entry: %d in values entries array.",
				 function,
				 value_entry_index );

				return( -1 );
			}
			last_data_index = data_index;
	
			value_entry_index++;
		}
	}
	return( 1 );
}

/* Copies the data
 * Returns 1 if successful or -1 on error
 */
int libfvalue_value_copy_data(
     libfvalue_value_t *value,
     uint8_t *data,
     size_t data_size,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	static char *function                      = "libfvalue_value_copy_data";

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( internal_value->data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid value - missing data.",
		 function );

		return( -1 );
	}
	if( data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid data.",
		 function );

		return( -1 );
	}
	if( ( data_size == 0 )
	 || ( data_size > (size_t) SSIZE_MAX ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid data size value out of bounds.",
		 function );

		return( -1 );
	}
	if( data_size < internal_value->data_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: data too small.",
		 function );

		return( -1 );
	}
	if( memory_copy(
	     data,
	     internal_value->data,
	     internal_value->data_size ) == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_COPY_FAILED,
		 "%s: unable to copy data.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the entry data
 * Returns 1 if successful or -1 on error
 */
int libfvalue_value_get_entry_data(
     libfvalue_value_t *value,
     int value_entry_index,
     uint8_t **entry_data,
     size_t *entry_data_size,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	libfvalue_value_entry_t *value_entry       = NULL;
	static char *function                      = "libfvalue_value_get_entry_data";

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( entry_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid entry data.",
		 function );

		return( -1 );
	}
	if( entry_data_size == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid data size.",
		 function );

		return( -1 );
	}
	if( internal_value->value_entries == NULL )
	{
		if( value_entry_index != 0 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: invalid value index value out of bounds.",
			 function );

			return( -1 );
		}
		*entry_data      = internal_value->data;
		*entry_data_size = internal_value->data_size;
	}
	else
	{
		if( internal_value->data == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid value - missing data.",
			 function );

			return( -1 );
		}
		if( internal_value->data_size > (size_t) SSIZE_MAX )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
			 "%s: invalid data size value exceeds maximum.",
			 function );

			return( -1 );
		}
		if( libfvalue_array_get_entry_by_index(
		     internal_value->value_entries,
		     value_entry_index,
		     (intptr_t **) &value_entry,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve entry: %d from values entries array.",
			 function,
			 value_entry_index );

			return( -1 );
		}
		if( value_entry == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing value entry.",
			 function );

			return( -1 );
		}
		if( value_entry->offset > internal_value->data_size )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: value entry offset: %d out of bounds.",
			 function,
			 value_entry_index );

			return( -1 );
		}
		if( ( value_entry->offset + value_entry->size ) > internal_value->data_size )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: value entry size: %d out of bounds.",
			 function,
			 value_entry_index );

			return( -1 );
		}
		if( value_entry->size != 0 )
		{
			*entry_data = &( internal_value->data[ value_entry->offset ] );
		}
		else
		{
			*entry_data = NULL;
		}
		*entry_data_size = value_entry->size;
	}
	return( 1 );
}

/* Copies the entry data
 * Returns 1 if successful or -1 on error
 */
int libfvalue_value_copy_entry_data(
     libfvalue_value_t *value,
     int value_entry_index,
     uint8_t *entry_data,
     size_t entry_data_size,
     liberror_error_t **error )
{
	uint8_t *value_entry_data    = NULL;
	static char *function        = "libfvalue_value_copy_entry_data";
	size_t value_entry_data_size = 0;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	if( entry_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid entry data.",
		 function );

		return( -1 );
	}
	if( ( entry_data_size == 0 )
	 || ( entry_data_size > (size_t) SSIZE_MAX ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid entry data size value out of bounds.",
		 function );

		return( -1 );
	}
	if( libfvalue_value_get_entry_data(
	     value,
	     value_entry_index,
	     &value_entry_data,
	     &value_entry_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry data: %d.",
		 function,
		 value_entry_index );

		return( -1 );
	}
	if( value_entry_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing value entry data.",
		 function );

		return( -1 );
	}
	if( entry_data_size < value_entry_data_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: entry data too small.",
		 function );

		return( -1 );
	}
	if( memory_copy(
	     entry_data,
	     value_entry_data,
	     value_entry_data_size ) == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_COPY_FAILED,
		 "%s: unable to copy entry data.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the metadata
 * Returns 1 if successful or -1 on error
 */
int libfvalue_value_get_metadata(
     libfvalue_value_t *value,
     uint8_t **metadata,
     size_t *metadata_size,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	static char *function                      = "libfvalue_value_get_metadata";

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( metadata == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid metadata.",
		 function );

		return( -1 );
	}
	if( metadata_size == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid metadata size.",
		 function );

		return( -1 );
	}
	*metadata       = internal_value->metadata;
	*metadata_size  = internal_value->metadata_size;

	return( 1 );
}

/* Sets the metadata
 * Returns 1 if successful or -1 on error
 */
int libfvalue_value_set_metadata(
     libfvalue_value_t *value,
     const uint8_t *metadata,
     size_t metadata_size,
     uint8_t flags,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	static char *function                      = "libfvalue_value_set_metadata";

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( ( flags & ~( LIBFVALUE_VALUE_METADATA_FLAG_MANAGED | LIBFVALUE_VALUE_METADATA_FLAG_CLONE_BY_REFERENCE ) ) != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported flags: 0x%02" PRIx8 ".",
		 function,
		 flags );

		return( -1 );
	}
	if( ( internal_value->flags & LIBFVALUE_VALUE_FLAG_METADATA_MANAGED ) != 0 )
	{
		if( internal_value->metadata != NULL )
		{
			memory_free(
			 internal_value->metadata );

			internal_value->metadata      = NULL;
			internal_value->metadata_size = 0;
		}
		internal_value->flags &= ~( LIBFVALUE_VALUE_FLAG_METADATA_MANAGED );
	}
	/* Make sure empty values have data that refers to NULL
	 */
	if( ( metadata == NULL )
	 || ( metadata_size == 0 ) )
	{
		internal_value->metadata = NULL;
	}
	else if( ( flags & LIBFVALUE_VALUE_METADATA_FLAG_CLONE_BY_REFERENCE ) != 0 )
	{
		internal_value->metadata = (uint8_t *) metadata;

		if( ( flags & LIBFVALUE_VALUE_METADATA_FLAG_MANAGED ) != 0 )
		{
			internal_value->flags |= LIBFVALUE_VALUE_FLAG_METADATA_MANAGED;
		}
	}
	else
	{
		internal_value->metadata = (uint8_t *) memory_allocate(
		                                        sizeof( uint8_t ) * metadata_size );

		if( internal_value->metadata == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create metadata.",
			 function );

			goto on_error;
		}
		if( memory_copy(
		     internal_value->metadata,
		     metadata,
		     metadata_size ) == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_COPY_FAILED,
			 "%s: unable to copy metadata.",
			 function );

			goto on_error;
		}
		internal_value->flags |= LIBFVALUE_VALUE_FLAG_METADATA_MANAGED;
	}
	internal_value->metadata_size = metadata_size;

	return( 1 );

on_error:
	if( internal_value->metadata != NULL )
	{
		memory_free(
		 internal_value->metadata );

		internal_value->metadata = NULL;
	}
	return( -1 );
}

/* Retrieves the codepage
 * Returns 1 if successful or -1 on error
 */
int libfvalue_value_get_codepage(
     libfvalue_value_t *value,
     int *codepage,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	static char *function                      = "libfvalue_value_get_codepage";

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( codepage == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid codepage.",
		 function );

		return( -1 );
	}
	*codepage = internal_value->codepage;

	return( 1 );
}

/* Sets the codepage
 * Returns 1 if successful or -1 on error
 */
int libfvalue_value_set_codepage(
     libfvalue_value_t *value,
     int codepage,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	static char *function                      = "libfvalue_value_set_codepage";

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( ( codepage != LIBFVALUE_CODEPAGE_ASCII )
	 && ( codepage != LIBFVALUE_CODEPAGE_ISO_8859_1 )
	 && ( codepage != LIBFVALUE_CODEPAGE_ISO_8859_2 )
	 && ( codepage != LIBFVALUE_CODEPAGE_ISO_8859_3 )
	 && ( codepage != LIBFVALUE_CODEPAGE_ISO_8859_4 )
	 && ( codepage != LIBFVALUE_CODEPAGE_ISO_8859_5 )
	 && ( codepage != LIBFVALUE_CODEPAGE_ISO_8859_6 )
	 && ( codepage != LIBFVALUE_CODEPAGE_ISO_8859_7 )
	 && ( codepage != LIBFVALUE_CODEPAGE_ISO_8859_8 )
	 && ( codepage != LIBFVALUE_CODEPAGE_ISO_8859_9 )
	 && ( codepage != LIBFVALUE_CODEPAGE_ISO_8859_10 )
	 && ( codepage != LIBFVALUE_CODEPAGE_ISO_8859_11 )
	 && ( codepage != LIBFVALUE_CODEPAGE_ISO_8859_13 )
	 && ( codepage != LIBFVALUE_CODEPAGE_ISO_8859_14 )
	 && ( codepage != LIBFVALUE_CODEPAGE_ISO_8859_15 )
	 && ( codepage != LIBFVALUE_CODEPAGE_ISO_8859_16 )
	 && ( codepage != LIBFVALUE_CODEPAGE_KOI8_R )
	 && ( codepage != LIBFVALUE_CODEPAGE_KOI8_U )
	 && ( codepage != LIBFVALUE_CODEPAGE_UNICODE )
	 && ( codepage != LIBFVALUE_CODEPAGE_WINDOWS_874 )
	 && ( codepage != LIBFVALUE_CODEPAGE_WINDOWS_932 )
	 && ( codepage != LIBFVALUE_CODEPAGE_WINDOWS_936 )
	 && ( codepage != LIBFVALUE_CODEPAGE_WINDOWS_1250 )
	 && ( codepage != LIBFVALUE_CODEPAGE_WINDOWS_1251 )
	 && ( codepage != LIBFVALUE_CODEPAGE_WINDOWS_1252 )
	 && ( codepage != LIBFVALUE_CODEPAGE_WINDOWS_1253 )
	 && ( codepage != LIBFVALUE_CODEPAGE_WINDOWS_1254 )
	 && ( codepage != LIBFVALUE_CODEPAGE_WINDOWS_1256 )
	 && ( codepage != LIBFVALUE_CODEPAGE_WINDOWS_1257 )
	 && ( codepage != LIBFVALUE_CODEPAGE_WINDOWS_1258 ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported codepage.",
		 function );

		return( -1 );
	}
	internal_value->codepage = codepage;

	return( 1 );
}

/* Value entry functions
 */

/* Resizes the values entries
 * Returns 1 if successful or -1 on error
 */
int libfvalue_value_resize_value_entries(
     libfvalue_value_t *value,
     int number_of_value_entries,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	static char *function                      = "libfvalue_value_resize_value_entries";

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( number_of_value_entries < 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: number of value entries out of bounds.",
		 function );

		return( -1 );
	}
	if( internal_value->value_entries == NULL )
	{
		if( libfvalue_array_initialize(
		     &( internal_value->value_entries ),
		     number_of_value_entries,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create value entries array.",
			 function );

			return( -1 );
		}
	}
	else
	{
		if( libfvalue_array_resize(
		     internal_value->value_entries,
		     number_of_value_entries,
		     (int (*)(intptr_t **, liberror_error_t **)) &libfvalue_value_entry_free,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_RESIZE_FAILED,
			 "%s: unable to resize value entries array.",
			 function );

			return( -1 );
		}
	}
	return( 1 );
}

/* Retrieves the number of values entries
 * Returns 1 if successful or -1 on error
 */
int libfvalue_value_get_number_of_value_entries(
     libfvalue_value_t *value,
     int *number_of_value_entries,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	static char *function                      = "libfvalue_value_get_number_of_value_entries";

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( internal_value->value_entries == NULL )
	{
		if( number_of_value_entries == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
			 "%s: invalid number of value entries.",
			 function );

			return( -1 );
		}
		*number_of_value_entries = 1;
	}
	else if( libfvalue_array_get_number_of_entries(
	          internal_value->value_entries,
	          number_of_value_entries,
	          error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_RESIZE_FAILED,
		 "%s: unable to retrieve number of value entries.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Sets a specific value entry
 * Returns 1 if successful or -1 on error
 */
int libfvalue_value_set_value_entry(
     libfvalue_value_t *value,
     int value_entry_index,
     size_t value_entry_offset,
     size_t value_entry_size,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	libfvalue_value_entry_t *value_entry       = NULL;
	static char *function                      = "libfvalue_value_set_value_entry";

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( value_entry_offset > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid entry entry offset value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( value_entry_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid entry entry size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( value_entry_offset > internal_value->data_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: value entry offset out of bounds.",
		 function );

		return( -1 );
	}
	if( ( value_entry_offset + value_entry_size ) > internal_value->data_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: value entry size out of bounds.",
		 function );

		return( -1 );
	}
	if( libfvalue_array_get_entry_by_index(
	     internal_value->value_entries,
	     value_entry_index,
	     (intptr_t **) &value_entry,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from values entries array.",
		 function,
		 value_entry_index );

		return( -1 );
	}
	if( value_entry == NULL )
	{
		if( libfvalue_value_entry_initialize(
		     &value_entry,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create value entry.",
			 function );

			return( -1 );
		}
		if( libfvalue_array_set_entry_by_index(
		     internal_value->value_entries,
		     value_entry_index,
		     (intptr_t *) value_entry,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set entry: %d in values entries array.",
			 function,
			 value_entry_index );

			return( -1 );
		}
	}
	value_entry->offset = value_entry_offset;
	value_entry->size   = value_entry_size;

	return( 1 );
}

/* Appends a value entry
 * Returns 1 if successful or -1 on error
 */
int libfvalue_value_append_value_entry(
     libfvalue_value_t *value,
     size_t value_entry_offset,
     size_t value_entry_size,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	libfvalue_value_entry_t *value_entry       = NULL;
	static char *function                      = "libfvalue_value_append_value_entry";
	int value_entry_index                      = 0;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( value_entry_offset > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid entry entry offset value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( value_entry_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid entry entry size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( value_entry_offset > internal_value->data_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: value entry offset out of bounds.",
		 function );

		return( -1 );
	}
	if( ( value_entry_offset + value_entry_size ) > internal_value->data_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: value entry size out of bounds.",
		 function );

		return( -1 );
	}
	if( internal_value->value_entries == NULL )
	{
		if( libfvalue_array_initialize(
		     &( internal_value->value_entries ),
		     0,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create value entries array.",
			 function );

			return( -1 );
		}
	}
	if( libfvalue_value_entry_initialize(
	     &value_entry,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create value entry.",
		 function );

		return( -1 );
	}
	value_entry->offset = value_entry_offset;
	value_entry->size   = value_entry_size;

	if( libfvalue_array_append_entry(
	     internal_value->value_entries,
	     &value_entry_index,
	     (intptr_t *) value_entry,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to append entry: %d to values entries array.",
		 function,
		 value_entry_index );

		return( -1 );
	}
	return( 1 );
}

/* Marshalling functions
 */

#ifdef TODO
/* Reads the value data from a file stream
 * Returns the number of bytes read or -1 on error
 */
ssize_t libfvalue_value_read_from_file_stream(
         libfvalue_value_t *value,
         FILE *file_stream,
         liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	static char *function                      = "libfvalue_value_read_from_file_stream";

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( file_stream == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file stream.",
		 function );

		return( -1 );
	}
/* TODO implement function */
	return( -1 );
}
#endif

/* Writes the value data to a file stream
 * Returns the number of bytes written or -1 on error
 */
ssize_t libfvalue_value_write_to_file_stream(
         libfvalue_value_t *value,
         FILE *file_stream,
         liberror_error_t **error )
{
	libcstring_system_character_t *value_string = NULL;
	libfvalue_internal_value_t *internal_value  = NULL;
	static char *function                       = "libfvalue_value_write_to_file_stream";
	const char *type_string                     = NULL;
	size_t value_string_size                    = 0;
	ssize_t write_count                         = 0;
	int print_count                             = 0;
	int result                                  = 0;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( ( internal_value->identifier == NULL )
	 || ( internal_value->identifier[ 0 ] == 0 ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid value - missing identifier.",
		 function );

		return( -1 );
	}
	if( file_stream == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file stream.",
		 function );

		return( -1 );
	}
/* TODO add support for
 * date and time types
 * floating point types
 */
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_BOOLEAN:
			type_string = "boolean";

			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_8BIT:
			type_string = "int8";

			break;

		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_8BIT:
			type_string = "uint8";

			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_16BIT:
			type_string = "int16";

			break;

		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_16BIT:
			type_string = "uint16";

			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_32BIT:
			type_string = "int32";

			break;

		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_32BIT:
			type_string = "uint32";

			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_64BIT:
			type_string = "int64";

			break;

		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_64BIT:
			type_string = "uint64";

			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF8:
			type_string = "utf8";

			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF16:
			type_string = "utf16";

			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF32:
			type_string = "utf32";

			break;

		default:
			return( 0 );
	}
	print_count = fprintf(
	               file_stream,
	               "<%s type=\"%s\">",
	               (char *) internal_value->identifier,
	               type_string );

	if( print_count < 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_WRITE_FAILED,
		 "%s: unable to write to file stream.",
		 function );

		return( -1 );
	}
	write_count += print_count;

/* TODO add support for
 * date and time types
 * floating point types
 * utf16 and utf32
 */
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_BOOLEAN:
			if( *( internal_value->data ) != 0 )
			{
				print_count = fprintf(
				               file_stream,
				               "true" );
			}
			else
			{
				print_count = fprintf(
				               file_stream,
				               "false" );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_8BIT:
			print_count = fprintf(
			               file_stream,
			               "%" PRIi8 "",
			               (int8_t) *( internal_value->data ) );
			break;

		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_8BIT:
			print_count = fprintf(
			               file_stream,
			               "%" PRIu8 "",
			               (uint8_t) *( internal_value->data ) );
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_16BIT:
			print_count = fprintf(
			               file_stream,
			               "%" PRIi16 "",
			               (int16_t) *( internal_value->data ) );
			break;

		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_16BIT:
			print_count = fprintf(
			               file_stream,
			               "%" PRIu16 "",
			               (uint16_t) *( internal_value->data ) );
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_32BIT:
			print_count = fprintf(
			               file_stream,
			               "%" PRIi32 "",
			               (int32_t) *( internal_value->data ) );
			break;

		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_32BIT:
			print_count = fprintf(
			               file_stream,
			               "%" PRIu32 "",
			               (uint32_t) *( internal_value->data ) );
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_64BIT:
			print_count = fprintf(
			               file_stream,
			               "%" PRIi64 "",
			               (int64_t) *( internal_value->data ) );
			break;

		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_64BIT:
			print_count = fprintf(
			               file_stream,
			               "%" PRIu64 "",
			               (uint64_t) *( internal_value->data ) );
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF8:
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
			result = libuna_utf16_string_size_from_utf8_stream(
				  internal_value->data,
				  internal_value->data_size,
				  &value_string_size,
				  error );
#else
			result = libuna_utf8_string_size_from_utf8_stream(
				  internal_value->data,
				  internal_value->data_size,
				  &value_string_size,
				  error );
#endif
			if( result != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine string size of UTF-8 stream.",
				 function );

				return( -1 );
			}
			value_string = libcstring_system_string_allocate(
			                value_string_size );

			if( value_string == NULL )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_MEMORY,
				 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
				 "%s: unable to create value string.",
				 function );

				return( -1 );
			}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
			result = libuna_utf16_string_copy_from_utf8_stream(
				  (uint16_t *) value_string,
				  value_string_size,
				  internal_value->data,
				  internal_value->data_size,
				  error );
#else
			result = libuna_utf8_string_copy_from_utf8_stream(
				  (uint8_t *) value_string,
				  value_string_size,
				  internal_value->data,
				  internal_value->data_size,
				  error );
#endif
			if( result != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy UTF-8 stream to string.",
				 function );

				memory_free(
				 value_string );

				return( -1 );
			}
			print_count = fprintf(
			               file_stream,
			               "%" PRIs_LIBCSTRING_SYSTEM "",
			               value_string );

			memory_free(
			 value_string );

			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF16:
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
			result = libuna_utf16_string_size_from_utf16_stream(
				  internal_value->data,
				  internal_value->data_size,
				  internal_value->byte_order,
				  &value_string_size,
				  error );

#else
			result = libuna_utf8_string_size_from_utf16_stream(
				  internal_value->data,
				  internal_value->data_size,
				  internal_value->byte_order,
				  &value_string_size,
				  error );
#endif
			value_string = libcstring_system_string_allocate(
			                value_string_size );

			if( value_string == NULL )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_MEMORY,
				 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
				 "%s: unable to create value string.",
				 function );

				return( -1 );
			}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
			result = libuna_utf16_string_copy_from_utf16_stream(
				  (uint16_t *) value_string,
				  value_string_size,
				  internal_value->data,
				  internal_value->data_size,
				  internal_value->byte_order,
				  error );
#else
			result = libuna_utf8_string_copy_from_utf16_stream(
				  (uint8_t *) value_string,
				  value_string_size,
				  internal_value->data,
				  internal_value->data_size,
				  internal_value->byte_order,
				  error );
#endif
			if( result != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy UTF-16 stream to string.",
				 function );

				memory_free(
				 value_string );

				return( -1 );
			}
			print_count = fprintf(
			               file_stream,
			               "%" PRIs_LIBCSTRING_SYSTEM "",
			               value_string );

			memory_free(
			 value_string );

			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF32:
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
			result = libuna_utf16_string_size_from_utf32_stream(
				  internal_value->data,
				  internal_value->data_size,
				  internal_value->byte_order,
				  &value_string_size,
				  error );

#else
			result = libuna_utf8_string_size_from_utf32_stream(
				  internal_value->data,
				  internal_value->data_size,
				  internal_value->byte_order,
				  &value_string_size,
				  error );
#endif
			value_string = libcstring_system_string_allocate(
			                value_string_size );

			if( value_string == NULL )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_MEMORY,
				 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
				 "%s: unable to create value string.",
				 function );

				return( -1 );
			}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
			result = libuna_utf16_string_copy_from_utf32_stream(
				  (uint16_t *) value_string,
				  value_string_size,
				  internal_value->data,
				  internal_value->data_size,
				  internal_value->byte_order,
				  error );
#else
			result = libuna_utf8_string_copy_from_utf32_stream(
				  (uint8_t *) value_string,
				  value_string_size,
				  internal_value->data,
				  internal_value->data_size,
				  internal_value->byte_order,
				  error );
#endif
			if( result != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy UTF-32 stream to string.",
				 function );

				memory_free(
				 value_string );

				return( -1 );
			}
			print_count = fprintf(
			               file_stream,
			               "%" PRIs_LIBCSTRING_SYSTEM "",
			               value_string );

			memory_free(
			 value_string );

			break;

		default:
			break;
	}
	if( print_count < 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_WRITE_FAILED,
		 "%s: unable to write to file stream.",
		 function );

		return( -1 );
	}
	write_count += print_count;

	print_count = fprintf(
	               file_stream,
	               "</%s>",
	               (char *) internal_value->identifier );

	if( print_count < 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_WRITE_FAILED,
		 "%s: unable to write to file stream.",
		 function );

		return( -1 );
	}
	write_count += print_count;

	return( write_count );
}

/* Boolean value functions
 */

/* Copies the value data from a boolean value
 * Returns 1 if successful, 0 if value type not supported or -1 on error
 */
int libfvalue_value_copy_from_boolean(
     libfvalue_value_t *value,
     int value_entry_index,
     uint8_t value_boolean,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	uint8_t *entry_data                        = NULL;
	static char *function                      = "libfvalue_value_copy_from_boolean";
	size_t entry_data_size                     = 0;
	uint64_t value_64bit                       = 0;
	uint32_t value_32bit                       = 0;
	uint16_t value_16bit                       = 0;
	int result                                 = 1;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( internal_value->data == NULL )
	{
		if( libfvalue_value_initialize_data(
		     internal_value,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create value data.",
			 function );

			return( -1 );
		}
	}
	if( libfvalue_value_get_entry_data(
	     value,
	     value_entry_index,
	     &entry_data,
	     &entry_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry data: %d.",
		 function,
		 value_entry_index );

		return( -1 );
	}
	if( entry_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing entry data.",
		 function );

		return( -1 );
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_BOOLEAN:
		case LIBFVALUE_VALUE_TYPE_INTEGER_8BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_8BIT:
			if( entry_data_size != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( value_boolean != 0 )
			{
				*entry_data = 1;
			}
			else
			{
				*entry_data = 0;
			}
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_16BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_16BIT:
			if( entry_data_size != 2 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( value_boolean != 0 )
			{
				value_16bit = 1;
			}
			else
			{
				value_16bit = 0;
			}
			if( internal_value->byte_order == LIBFVALUE_ENDIAN_NATIVE )
			{
				*( (uint16_t *) entry_data ) = value_16bit;
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_BIG )
			{
				byte_stream_copy_from_uint16_big_endian(
				 entry_data,
				 value_16bit );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_LITTLE )
			{
				byte_stream_copy_from_uint16_little_endian(
				 entry_data,
				 value_16bit );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_32BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_32BIT:
			if( entry_data_size != 4 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( value_boolean != 0 )
			{
				value_32bit = 1;
			}
			else
			{
				value_32bit = 0;
			}
			if( internal_value->byte_order == LIBFVALUE_ENDIAN_NATIVE )
			{
				*( (uint32_t *) entry_data ) = value_32bit;
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_BIG )
			{
				byte_stream_copy_from_uint32_big_endian(
				 entry_data,
				 value_32bit );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_LITTLE )
			{
				byte_stream_copy_from_uint32_little_endian(
				 entry_data,
				 value_32bit );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_64BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_64BIT:
			if( entry_data_size != 8 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( value_boolean != 0 )
			{
				value_64bit = 1;
			}
			else
			{
				value_64bit = 0;
			}
			if( internal_value->byte_order == LIBFVALUE_ENDIAN_NATIVE )
			{
				*( (uint64_t *) entry_data ) = value_64bit;
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_BIG )
			{
				byte_stream_copy_from_uint64_big_endian(
				 entry_data,
				 value_64bit );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_LITTLE )
			{
				byte_stream_copy_from_uint64_little_endian(
				 entry_data,
				 value_64bit );
			}
			break;

		default:
			result = 0;

			break;
	}
	return( result );
}

/* Copies the value data to a boolean value
 * Returns 1 if successful, 0 if value type not supported or -1 on error
 */
int libfvalue_value_copy_to_boolean(
     libfvalue_value_t *value,
     int value_entry_index,
     uint8_t *value_boolean,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	uint8_t *entry_data                        = NULL;
	static char *function                      = "libfvalue_value_copy_to_boolean";
	size_t entry_data_size                     = 0;
	int result                                 = 1;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( value_boolean == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value boolean.",
		 function );

		return( -1 );
	}
	if( libfvalue_value_get_entry_data(
	     value,
	     value_entry_index,
	     &entry_data,
	     &entry_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry data: %d.",
		 function,
		 value_entry_index );

		return( -1 );
	}
	if( entry_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing entry data.",
		 function );

		return( -1 );
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_BOOLEAN:
		case LIBFVALUE_VALUE_TYPE_INTEGER_8BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_8BIT:
			if( entry_data_size != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( *entry_data != 0 )
			{
				*value_boolean = 1;
			}
			else
			{
				*value_boolean = 0;
			}
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_16BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_16BIT:
			if( entry_data_size != 2 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( *( (uint16_t *) entry_data ) != 0 )
			{
				*value_boolean = 1;
			}
			else
			{
				*value_boolean = 0;
			}
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_32BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_32BIT:
			if( entry_data_size != 4 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( *( (uint32_t *) entry_data ) != 0 )
			{
				*value_boolean = 1;
			}
			else
			{
				*value_boolean = 0;
			}
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_64BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_64BIT:
			if( entry_data_size != 8 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( *( (uint64_t *) entry_data ) != 0 )
			{
				*value_boolean = 1;
			}
			else
			{
				*value_boolean = 0;
			}
			break;

		default:
			result = 0;

			break;
	}
	return( result );
}

/* Integer value functions
 */

/* Copies the value data from an 8-bit value
 * Returns 1 if successful, 0 if value type not supported or -1 on error
 */
int libfvalue_value_copy_from_8bit(
     libfvalue_value_t *value,
     int value_entry_index,
     uint8_t value_8bit,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	uint8_t *entry_data                        = NULL;
	static char *function                      = "libfvalue_value_copy_from_8bit";
	size_t entry_data_size                     = 0;
	uint64_t value_64bit                       = 0;
	uint32_t value_32bit                       = 0;
	uint16_t value_16bit                       = 0;
	int result                                 = 1;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( internal_value->data == NULL )
	{
		if( libfvalue_value_initialize_data(
		     internal_value,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create value data.",
			 function );

			return( -1 );
		}
	}
	if( libfvalue_value_get_entry_data(
	     value,
	     value_entry_index,
	     &entry_data,
	     &entry_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry data: %d.",
		 function,
		 value_entry_index );

		return( -1 );
	}
	if( entry_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing entry data.",
		 function );

		return( -1 );
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_BOOLEAN:
			if( entry_data_size != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( value_8bit != 0 )
			{
				*entry_data = 1;
			}
			else
			{
				*entry_data = 0;
			}
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_8BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_8BIT:
			if( entry_data_size != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			*entry_data = value_8bit;

			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_16BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_16BIT:
			if( entry_data_size != 2 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( internal_value->type == LIBFVALUE_VALUE_TYPE_INTEGER_16BIT )
			{
				if( ( value_8bit & 0x80 ) != 0 )
				{
					value_16bit = (uint16_t) 1 << 15;
					value_8bit &= 0x7f;
				}
			}
			value_16bit |= value_8bit;

			if( internal_value->byte_order == LIBFVALUE_ENDIAN_NATIVE )
			{
				*( (uint16_t *) entry_data ) = value_16bit;
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_BIG )
			{
				byte_stream_copy_from_uint16_big_endian(
				 entry_data,
				 value_16bit );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_LITTLE )
			{
				byte_stream_copy_from_uint16_little_endian(
				 entry_data,
				 value_16bit );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_32BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_32BIT:
			if( entry_data_size != 4 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( internal_value->type == LIBFVALUE_VALUE_TYPE_INTEGER_32BIT )
			{
				if( ( value_8bit & 0x80 ) != 0 )
				{
					value_32bit = (uint32_t) 1 << 31;
					value_8bit &= 0x7f;
				}
			}
			value_32bit |= value_8bit;

			if( internal_value->byte_order == LIBFVALUE_ENDIAN_NATIVE )
			{
				*( (uint32_t *) entry_data ) = value_32bit;
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_BIG )
			{
				byte_stream_copy_from_uint32_big_endian(
				 entry_data,
				 value_32bit );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_LITTLE )
			{
				byte_stream_copy_from_uint32_little_endian(
				 entry_data,
				 value_32bit );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_64BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_64BIT:
			if( entry_data_size != 8 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( internal_value->type == LIBFVALUE_VALUE_TYPE_INTEGER_64BIT )
			{
				if( ( value_8bit & 0x80 ) != 0 )
				{
					value_64bit = (uint64_t) 1 << 63;
					value_8bit &= 0x7f;
				}
			}
			value_64bit |= value_8bit;

			if( internal_value->byte_order == LIBFVALUE_ENDIAN_NATIVE )
			{
				*( (uint64_t *) entry_data ) = value_64bit;
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_BIG )
			{
				byte_stream_copy_from_uint64_big_endian(
				 entry_data,
				 value_64bit );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_LITTLE )
			{
				byte_stream_copy_from_uint64_little_endian(
				 entry_data,
				 value_64bit );
			}
			break;

		default:
			result = 0;

			break;
	}
	return( result );
}

/* Copies the value data to an 8-bit value
 * Returns 1 if successful, 0 if value type not supported or -1 on error
 */
int libfvalue_value_copy_to_8bit(
     libfvalue_value_t *value,
     int value_entry_index,
     uint8_t *value_8bit,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	uint8_t *entry_data                        = NULL;
	static char *function                      = "libfvalue_value_copy_to_8bit";
	size_t entry_data_size                     = 0;
	int result                                 = 1;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( value_8bit == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value 8-bit.",
		 function );

		return( -1 );
	}
	if( libfvalue_value_get_entry_data(
	     value,
	     value_entry_index,
	     &entry_data,
	     &entry_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry data: %d.",
		 function,
		 value_entry_index );

		return( -1 );
	}
	if( entry_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing entry data.",
		 function );

		return( -1 );
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_BOOLEAN:
		case LIBFVALUE_VALUE_TYPE_INTEGER_8BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_8BIT:
			if( entry_data_size != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			*value_8bit = (uint8_t) *entry_data;

			break;

		default:
			result = 0;

			break;
	}
	return( result );
}

/* Copies the value data from a 16-bit value
 * Returns 1 if successful, 0 if value type not supported or -1 on error
 */
int libfvalue_value_copy_from_16bit(
     libfvalue_value_t *value,
     int value_entry_index,
     uint16_t value_16bit,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	uint8_t *entry_data                        = NULL;
	static char *function                      = "libfvalue_value_copy_from_16bit";
	size_t entry_data_size                     = 0;
	uint64_t value_64bit                       = 0;
	uint32_t value_32bit                       = 0;
	int result                                 = 1;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( internal_value->data == NULL )
	{
		if( libfvalue_value_initialize_data(
		     internal_value,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create value data.",
			 function );

			return( -1 );
		}
	}
	if( libfvalue_value_get_entry_data(
	     value,
	     value_entry_index,
	     &entry_data,
	     &entry_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry data: %d.",
		 function,
		 value_entry_index );

		return( -1 );
	}
	if( entry_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing entry data.",
		 function );

		return( -1 );
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_BOOLEAN:
			if( entry_data_size != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( value_16bit != 0 )
			{
				*entry_data = 1;
			}
			else
			{
				*entry_data = 0;
			}
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_16BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_16BIT:
			if( entry_data_size != 2 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( internal_value->byte_order == LIBFVALUE_ENDIAN_NATIVE )
			{
				*( (uint16_t *) entry_data ) = value_16bit;
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_BIG )
			{
				byte_stream_copy_from_uint16_big_endian(
				 entry_data,
				 value_16bit );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_LITTLE )
			{
				byte_stream_copy_from_uint16_little_endian(
				 entry_data,
				 value_16bit );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_32BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_32BIT:
			if( entry_data_size != 4 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( internal_value->type == LIBFVALUE_VALUE_TYPE_INTEGER_16BIT )
			{
				value_32bit = (int32_t) value_16bit;
			}
			else
			{
				value_32bit = (uint32_t) value_16bit;
			}
			if( internal_value->byte_order == LIBFVALUE_ENDIAN_NATIVE )
			{
				*( (uint32_t *) entry_data ) = value_32bit;
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_BIG )
			{
				byte_stream_copy_from_uint32_big_endian(
				 entry_data,
				 value_32bit );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_LITTLE )
			{
				byte_stream_copy_from_uint32_little_endian(
				 entry_data,
				 value_32bit );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_64BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_64BIT:
			if( entry_data_size != 8 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( internal_value->type == LIBFVALUE_VALUE_TYPE_INTEGER_64BIT )
			{
				value_64bit = (int64_t) value_16bit;
			}
			else
			{
				value_64bit = (uint64_t) value_16bit;
			}
			if( internal_value->byte_order == LIBFVALUE_ENDIAN_NATIVE )
			{
				*( (uint64_t *) entry_data ) = value_64bit;
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_BIG )
			{
				byte_stream_copy_from_uint64_big_endian(
				 entry_data,
				 value_64bit );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_LITTLE )
			{
				byte_stream_copy_from_uint64_little_endian(
				 entry_data,
				 value_64bit );
			}
			break;

		default:
			result = 0;

			break;
	}
	return( result );
}

/* Copies the value data to a 16-bit value
 * Returns 1 if successful, 0 if value type not supported or -1 on error
 */
int libfvalue_value_copy_to_16bit(
     libfvalue_value_t *value,
     int value_entry_index,
     uint16_t *value_16bit,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	uint8_t *entry_data                        = NULL;
	static char *function                      = "libfvalue_value_copy_to_16bit";
	size_t entry_data_size                     = 0;
	int result                                 = 1;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( value_16bit == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value 16-bit.",
		 function );

		return( -1 );
	}
	if( libfvalue_value_get_entry_data(
	     value,
	     value_entry_index,
	     &entry_data,
	     &entry_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry data: %d.",
		 function,
		 value_entry_index );

		return( -1 );
	}
	if( entry_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing entry data.",
		 function );

		return( -1 );
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_BOOLEAN:
		case LIBFVALUE_VALUE_TYPE_INTEGER_8BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_8BIT:
			if( entry_data_size != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			*value_16bit = (uint16_t) *entry_data;

			if( internal_value->type == LIBFVALUE_VALUE_TYPE_INTEGER_8BIT )
			{
				if( ( *value_16bit & 0x80 ) != 0 )
				{
					*value_16bit = ( (uint16_t) 1 << 15 ) & ( *value_16bit & 0x7f );
				}
			}
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_16BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_16BIT:
			if( entry_data_size != 2 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( internal_value->byte_order == LIBFVALUE_ENDIAN_NATIVE )
			{
				*value_16bit = *( (uint16_t *) entry_data );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_BIG )
			{
				byte_stream_copy_to_uint16_big_endian(
				 entry_data,
				 *value_16bit );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_LITTLE )
			{
				byte_stream_copy_to_uint16_little_endian(
				 entry_data,
				 *value_16bit );
			}
			break;

		default:
			result = 0;

			break;
	}
	return( result );
}

/* Copies the value data from a 32-bit value
 * Returns 1 if successful, 0 if value type not supported or -1 on error
 */
int libfvalue_value_copy_from_32bit(
     libfvalue_value_t *value,
     int value_entry_index,
     uint32_t value_32bit,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	uint8_t *entry_data                        = NULL;
	static char *function                      = "libfvalue_value_copy_from_32bit";
	size_t entry_data_size                     = 0;
	uint64_t value_64bit                       = 0;
	int result                                 = 1;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( internal_value->data == NULL )
	{
		if( libfvalue_value_initialize_data(
		     internal_value,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create value data.",
			 function );

			return( -1 );
		}
	}
	if( libfvalue_value_get_entry_data(
	     value,
	     value_entry_index,
	     &entry_data,
	     &entry_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry data: %d.",
		 function,
		 value_entry_index );

		return( -1 );
	}
	if( entry_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing entry data.",
		 function );

		return( -1 );
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_BOOLEAN:
			if( entry_data_size != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( value_32bit != 0 )
			{
				*entry_data = 1;
			}
			else
			{
				*entry_data = 0;
			}
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_32BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_32BIT:
			if( entry_data_size != 4 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( internal_value->byte_order == LIBFVALUE_ENDIAN_NATIVE )
			{
				*( (uint32_t *) entry_data ) = value_32bit;
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_BIG )
			{
				byte_stream_copy_from_uint32_big_endian(
				 entry_data,
				 value_32bit );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_LITTLE )
			{
				byte_stream_copy_from_uint32_little_endian(
				 entry_data,
				 value_32bit );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_64BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_64BIT:
			if( entry_data_size != 8 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( internal_value->type == LIBFVALUE_VALUE_TYPE_INTEGER_64BIT )
			{
				value_64bit = (int64_t) value_32bit;
			}
			else
			{
				value_64bit = (uint64_t) value_32bit;
			}
			if( internal_value->byte_order == LIBFVALUE_ENDIAN_NATIVE )
			{
				*( (uint64_t *) entry_data ) = value_64bit;
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_BIG )
			{
				byte_stream_copy_from_uint64_big_endian(
				 entry_data,
				 value_64bit );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_LITTLE )
			{
				byte_stream_copy_from_uint64_little_endian(
				 entry_data,
				 value_64bit );
			}
			break;

		default:
			result = 0;

			break;
	}
	return( result );
}

/* Copies the value data to a 32-bit value
 * Returns 1 if successful, 0 if value type not supported or -1 on error
 */
int libfvalue_value_copy_to_32bit(
     libfvalue_value_t *value,
     int value_entry_index,
     uint32_t *value_32bit,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	uint8_t *entry_data                        = NULL;
	static char *function                      = "libfvalue_value_copy_to_32bit";
	size_t entry_data_size                     = 0;
	int result                                 = 1;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( value_32bit == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value 32-bit.",
		 function );

		return( -1 );
	}
	if( libfvalue_value_get_entry_data(
	     value,
	     value_entry_index,
	     &entry_data,
	     &entry_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry data: %d.",
		 function,
		 value_entry_index );

		return( -1 );
	}
	if( entry_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing entry data.",
		 function );

		return( -1 );
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_BOOLEAN:
		case LIBFVALUE_VALUE_TYPE_INTEGER_8BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_8BIT:
			if( entry_data_size != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			*value_32bit = (uint32_t) *entry_data;

			if( internal_value->type == LIBFVALUE_VALUE_TYPE_INTEGER_8BIT )
			{
				if( ( *value_32bit & 0x80 ) != 0 )
				{
					*value_32bit = ( (uint32_t) 1 << 31 ) & ( *value_32bit & 0x7f );
				}
			}
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_16BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_16BIT:
			if( entry_data_size != 2 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( internal_value->byte_order == LIBFVALUE_ENDIAN_NATIVE )
			{
				*value_32bit = (uint32_t) *( (uint16_t *) entry_data );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_BIG )
			{
				byte_stream_copy_to_uint16_big_endian(
				 entry_data,
				 *value_32bit );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_LITTLE )
			{
				byte_stream_copy_to_uint16_little_endian(
				 entry_data,
				 *value_32bit );
			}
			if( internal_value->type == LIBFVALUE_VALUE_TYPE_INTEGER_16BIT )
			{
				if( ( *value_32bit & 0x8000 ) != 0 )
				{
					*value_32bit = ( (uint32_t) 1 << 31 ) & ( *value_32bit & 0x7fff );
				}
			}
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_32BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_32BIT:
			if( entry_data_size != 4 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( internal_value->byte_order == LIBFVALUE_ENDIAN_NATIVE )
			{
				*value_32bit = *( (uint32_t *) entry_data );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_BIG )
			{
				byte_stream_copy_to_uint32_big_endian(
				 entry_data,
				 *value_32bit );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_LITTLE )
			{
				byte_stream_copy_to_uint32_little_endian(
				 entry_data,
				 *value_32bit );
			}
			break;

		default:
			result = 0;

			break;
	}
	return( result );
}

/* Copies the value data from a 64-bit value
 * Returns 1 if successful, 0 if value type not supported or -1 on error
 */
int libfvalue_value_copy_from_64bit(
     libfvalue_value_t *value,
     int value_entry_index,
     uint64_t value_64bit,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	uint8_t *entry_data                        = NULL;
	static char *function                      = "libfvalue_value_copy_from_64bit";
	size_t entry_data_index                    = 0;
	size_t entry_data_size                     = 0;
	int result                                 = 1;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( internal_value->data == NULL )
	{
		if( libfvalue_value_initialize_data(
		     internal_value,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create value data.",
			 function );

			return( -1 );
		}
	}
	if( libfvalue_value_get_entry_data(
	     value,
	     value_entry_index,
	     &entry_data,
	     &entry_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry data: %d.",
		 function,
		 value_entry_index );

		return( -1 );
	}
	if( entry_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing entry data.",
		 function );

		return( -1 );
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_BOOLEAN:
			if( entry_data_size != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( value_64bit != 0 )
			{
				*entry_data = 1;
			}
			else
			{
				*entry_data = 0;
			}
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_64BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_64BIT:
			if( entry_data_size != 8 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( internal_value->byte_order == LIBFVALUE_ENDIAN_NATIVE )
			{
				*( (uint64_t *) entry_data ) = value_64bit;
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_BIG )
			{
				byte_stream_copy_from_uint64_big_endian(
				 entry_data,
				 value_64bit );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_LITTLE )
			{
				byte_stream_copy_from_uint64_little_endian(
				 entry_data,
				 value_64bit );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF8:
			if( internal_value->format == LIBFVALUE_VALUE_FORMAT_DECIMAL )
			{
				if( libfvalue_utf8_string_decimal_copy_from_64bit(
				     entry_data,
				     entry_data_size,
				     &entry_data_index,
				     value_64bit,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
					 "%s: unable to copy UTF-8 string of decimal value from a 64-bit value.",
					 function );

					return( -1 );
				}
			}
			else if( internal_value->format == LIBFVALUE_VALUE_FORMAT_HEXADECIMAL )
			{
				if( libfvalue_utf8_string_hexadecimal_copy_from_64bit(
				     entry_data,
				     entry_data_size,
				     value_64bit,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
					 "%s: unable to copy UTF-8 string of hexadecimal value from a 64-bit value.",
					 function );

					return( -1 );
				}
			}
			else
			{
				result = 0;
			}
			break;

		default:
			result = 0;

			break;
	}
	return( result );
}

/* Copies the value data to a 64-bit value
 * Returns 1 if successful, 0 if value type not supported or -1 on error
 */
int libfvalue_value_copy_to_64bit(
     libfvalue_value_t *value,
     int value_entry_index,
     uint64_t *value_64bit,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	uint8_t *entry_data                        = NULL;
	static char *function                      = "libfvalue_value_copy_to_64bit";
	size_t entry_data_size                     = 0;
	int result                                 = 1;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( value_64bit == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value 64-bit.",
		 function );

		return( -1 );
	}
	if( libfvalue_value_get_entry_data(
	     value,
	     value_entry_index,
	     &entry_data,
	     &entry_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry data: %d.",
		 function,
		 value_entry_index );

		return( -1 );
	}
	if( entry_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing entry data.",
		 function );

		return( -1 );
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_BOOLEAN:
		case LIBFVALUE_VALUE_TYPE_INTEGER_8BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_8BIT:
			if( entry_data_size != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			*value_64bit = (uint8_t) *entry_data;

			if( internal_value->type == LIBFVALUE_VALUE_TYPE_INTEGER_8BIT )
			{
				if( ( *value_64bit & 0x80 ) != 0 )
				{
					*value_64bit = ( (uint64_t) 1 << 63 ) | ( *value_64bit & 0x7f );
				}
			}
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_16BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_16BIT:
			if( entry_data_size != 2 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( internal_value->byte_order == LIBFVALUE_ENDIAN_NATIVE )
			{
				*value_64bit = (uint16_t) *entry_data;
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_BIG )
			{
				byte_stream_copy_to_uint16_big_endian(
				 entry_data,
				 *value_64bit );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_LITTLE )
			{
				byte_stream_copy_to_uint16_little_endian(
				 entry_data,
				 *value_64bit );
			}
			if( internal_value->type == LIBFVALUE_VALUE_TYPE_INTEGER_16BIT )
			{
				if( ( *value_64bit & 0x8000 ) != 0 )
				{
					*value_64bit = ( (uint64_t) 1 << 63 ) | ( *value_64bit & 0x7fff );
				}
			}
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_32BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_32BIT:
			if( entry_data_size != 4 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( internal_value->byte_order == LIBFVALUE_ENDIAN_NATIVE )
			{
				*value_64bit = (uint32_t) *entry_data;
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_BIG )
			{
				byte_stream_copy_to_uint32_big_endian(
				 entry_data,
				 *value_64bit );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_LITTLE )
			{
				byte_stream_copy_to_uint32_little_endian(
				 entry_data,
				 *value_64bit );
			}
			if( internal_value->type == LIBFVALUE_VALUE_TYPE_INTEGER_32BIT )
			{
				if( ( *value_64bit & 0x80000000UL ) != 0 )
				{
					*value_64bit = ( (uint64_t) 1 << 63 ) | ( *value_64bit & 0x7fffffffUL );
				}
			}
			break;

		case LIBFVALUE_VALUE_TYPE_INTEGER_64BIT:
		case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_64BIT:
			if( entry_data_size != 8 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( internal_value->byte_order == LIBFVALUE_ENDIAN_NATIVE )
			{
				*value_64bit = (uint64_t) *entry_data;
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_BIG )
			{
				byte_stream_copy_to_uint64_big_endian(
				 entry_data,
				 *value_64bit );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_LITTLE )
			{
				byte_stream_copy_to_uint64_little_endian(
				 entry_data,
				 *value_64bit );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_FILETIME:
			if( entry_data_size != 8 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
/* TODO add other endian support make sure filetime alignment is in order */
			if( internal_value->byte_order == LIBFVALUE_ENDIAN_LITTLE )
			{
				byte_stream_copy_to_uint64_little_endian(
				 entry_data,
				 *value_64bit );
			}
			else
			{
				result = 0;
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF8:
			if( internal_value->format == LIBFVALUE_VALUE_FORMAT_DECIMAL )
			{
				if( libfvalue_utf8_string_decimal_copy_to_64bit(
				     entry_data,
				     entry_data_size,
				     value_64bit,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
					 "%s: unable to copy UTF-8 string of decimal value to a 64-bit value.",
					 function );

					return( -1 );
				}
			}
			else if( internal_value->format == LIBFVALUE_VALUE_FORMAT_HEXADECIMAL )
			{
				if( libfvalue_utf8_string_hexadecimal_copy_to_64bit(
				     entry_data,
				     entry_data_size,
				     value_64bit,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
					 "%s: unable to copy UTF-8 string of hexadecimal value to a 64-bit value.",
					 function );

					return( -1 );
				}
			}
			else
			{
				result = 0;
			}
			break;

		default:
			result = 0;

			break;
	}
	return( result );
}

/* Floating point value functions
 */

/* Copies the value data from a float value
 * Returns 1 if successful, 0 if value type not supported or -1 on error
 */
int libfvalue_value_copy_from_float(
     libfvalue_value_t *value,
     int value_entry_index,
     float value_float,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	uint8_t *entry_data                        = NULL;
	static char *function                      = "libfvalue_value_copy_from_float";
	byte_stream_float32_t value_float32        = { 0 };
	size_t entry_data_size                     = 0;
	int result                                 = 1;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( internal_value->data == NULL )
	{
		if( libfvalue_value_initialize_data(
		     internal_value,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create value data.",
			 function );

			return( -1 );
		}
	}
	if( libfvalue_value_get_entry_data(
	     value,
	     value_entry_index,
	     &entry_data,
	     &entry_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry data: %d.",
		 function,
		 value_entry_index );

		return( -1 );
	}
	if( entry_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing entry data.",
		 function );

		return( -1 );
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_FLOATING_POINT_32BIT:
			if( entry_data_size != 4 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			value_float32.floating_point = value_float;

			if( internal_value->byte_order == LIBFVALUE_ENDIAN_NATIVE )
			{
				*( (uint32_t *) entry_data ) = value_float32.integer;
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_BIG )
			{
				byte_stream_copy_from_uint32_big_endian(
				 entry_data,
				 value_float32.integer );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_LITTLE )
			{
				byte_stream_copy_from_uint32_little_endian(
				 entry_data,
				 value_float32.integer );
			}
			break;

		default:
			result = 0;

			break;
	}
	return( result );
}

/* Copies the value data to a float value
 * Returns 1 if successful, 0 if value type not supported or -1 on error
 */
int libfvalue_value_copy_to_float(
     libfvalue_value_t *value,
     int value_entry_index,
     float *value_float,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	uint8_t *entry_data                        = NULL;
	static char *function                      = "libfvalue_value_copy_to_float";
	byte_stream_float32_t value_float32        = { 0 };
	size_t entry_data_size                     = 0;
	int result                                 = 1;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( value_float == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value float.",
		 function );

		return( -1 );
	}
	if( libfvalue_value_get_entry_data(
	     value,
	     value_entry_index,
	     &entry_data,
	     &entry_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry data: %d.",
		 function,
		 value_entry_index );

		return( -1 );
	}
	if( entry_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing entry data.",
		 function );

		return( -1 );
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_FLOATING_POINT_32BIT:
			if( entry_data_size != 4 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( internal_value->byte_order == LIBFVALUE_ENDIAN_NATIVE )
			{
				value_float32.integer = *( (uint32_t *) entry_data );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_BIG )
			{
				byte_stream_copy_to_uint32_big_endian(
				 entry_data,
				 value_float32.integer );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_LITTLE )
			{
				byte_stream_copy_to_uint32_little_endian(
				 entry_data,
				 value_float32.integer );
			}
			*value_float = value_float32.floating_point;

			break;

		default:
			result = 0;

			break;
	}
	return( result );
}

/* Copies the value data from a double value
 * Returns 1 if successful, 0 if value type not supported or -1 on error
 */
int libfvalue_value_copy_from_double(
     libfvalue_value_t *value,
     int value_entry_index,
     double value_double,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	uint8_t *entry_data                        = NULL;
	static char *function                      = "libfvalue_value_copy_from_double";
	byte_stream_float64_t value_float64        = { 0 };
	size_t entry_data_size                     = 0;
	int result                                 = 1;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( internal_value->data == NULL )
	{
		if( libfvalue_value_initialize_data(
		     internal_value,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create value data.",
			 function );

			return( -1 );
		}
	}
	if( libfvalue_value_get_entry_data(
	     value,
	     value_entry_index,
	     &entry_data,
	     &entry_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry data: %d.",
		 function,
		 value_entry_index );

		return( -1 );
	}
	if( entry_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing entry data.",
		 function );

		return( -1 );
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_FLOATING_POINT_64BIT:
			if( entry_data_size != 8 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			value_float64.floating_point = value_double;

			if( internal_value->byte_order == LIBFVALUE_ENDIAN_NATIVE )
			{
				*( (uint64_t *) entry_data ) = value_float64.integer;
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_BIG )
			{
				byte_stream_copy_from_uint64_big_endian(
				 entry_data,
				 value_float64.integer );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_LITTLE )
			{
				byte_stream_copy_from_uint64_little_endian(
				 entry_data,
				 value_float64.integer );
			}
			break;

		default:
			result = 0;

			break;
	}
	return( result );
}

/* Copies the value data to a double value
 * Returns 1 if successful, 0 if value type not supported or -1 on error
 */
int libfvalue_value_copy_to_double(
     libfvalue_value_t *value,
     int value_entry_index,
     double *value_double,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	uint8_t *entry_data                        = NULL;
	static char *function                      = "libfvalue_value_copy_to_double";
	byte_stream_float32_t value_float32        = { 0 };
	byte_stream_float64_t value_float64        = { 0 };
	size_t entry_data_size                     = 0;
	int result                                 = 1;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( value_double == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value double.",
		 function );

		return( -1 );
	}
	if( libfvalue_value_get_entry_data(
	     value,
	     value_entry_index,
	     &entry_data,
	     &entry_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry data: %d.",
		 function,
		 value_entry_index );

		return( -1 );
	}
	if( entry_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing entry data.",
		 function );

		return( -1 );
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_FLOATING_POINT_32BIT:
			if( entry_data_size != 4 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( internal_value->byte_order == LIBFVALUE_ENDIAN_NATIVE )
			{
				value_float32.integer = *( (uint32_t *) entry_data );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_BIG )
			{
				byte_stream_copy_to_uint32_big_endian(
				 entry_data,
				 value_float32.integer );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_LITTLE )
			{
				byte_stream_copy_to_uint32_little_endian(
				 entry_data,
				 value_float32.integer );
			}
			*value_double = (double) value_float32.floating_point;

			break;

		case LIBFVALUE_VALUE_TYPE_FLOATING_POINT_64BIT:
			if( entry_data_size != 8 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: entry data size out of bounds.",
				 function );

				return( -1 );
			}
			if( internal_value->byte_order == LIBFVALUE_ENDIAN_NATIVE )
			{
				value_float64.integer = *( (uint64_t *) entry_data );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_BIG )
			{
				byte_stream_copy_to_uint64_big_endian(
				 entry_data,
				 value_float64.integer );
			}
			else if( internal_value->byte_order == LIBFVALUE_ENDIAN_LITTLE )
			{
				byte_stream_copy_to_uint64_little_endian(
				 entry_data,
				 value_float64.integer );
			}
			*value_double = value_float64.floating_point;

			break;

		default:
			result = 0;

			break;
	}
	return( result );
}

/* String value functions
 */

/* Copies the value data from an UTF-8 encoded string
 * Returns 1 if successful, 0 if value type not supported or -1 on error
 */
int libfvalue_value_copy_from_utf8_string(
     libfvalue_value_t *value,
     int value_entry_index,
     const uint8_t *utf8_string,
     size_t utf8_string_size,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	static char *function                      = "libfvalue_value_copy_from_utf8_string";
	size_t value_data_size                     = 0;
	int result                                 = 1;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

/* TODO multi value support */
	if( value_entry_index != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid value index value out of bounds.",
		 function );

		return( -1 );
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM:
			if( libuna_byte_stream_size_from_utf8(
			     utf8_string,
			     utf8_string_size,
			     internal_value->codepage,
			     &value_data_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine byte stream size of UTF-8 string.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF8:
			if( libuna_utf8_stream_size_from_utf8(
			     utf8_string,
			     utf8_string_size,
			     &value_data_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine UTF-8 stream size of UTF-8 string.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF16:
			if( libuna_utf16_stream_size_from_utf8(
			     utf8_string,
			     utf8_string_size,
			     &value_data_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine UTF-16 stream size of UTF-8 string.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF32:
			if( libuna_utf32_stream_size_from_utf8(
			     utf8_string,
			     utf8_string_size,
			     &value_data_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine UTF-32 stream size of UTF-8 string.",
				 function );

				return( -1 );
			}
			break;

		default:
			result = 0;

			break;
	}
	if( result == 0 )
	{
		return( 0 );
	}
	if( internal_value->data != NULL )
	{
		if( ( internal_value->flags & LIBFVALUE_VALUE_FLAG_DATA_MANAGED ) != 0 )
		{
			memory_free(
			 internal_value->data );

			internal_value->flags &= ~( LIBFVALUE_VALUE_FLAG_DATA_MANAGED );
		}
		internal_value->data      = NULL;
		internal_value->data_size = 0;
	}
	if( internal_value->data == NULL )
	{
		internal_value->data_size = value_data_size;

		internal_value->data = (uint8_t *) memory_allocate(
		                                    sizeof( uint8_t ) * internal_value->data_size );

		if( internal_value->data == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create data.",
			 function );

			return( -1 );
		}
		internal_value->flags |= LIBFVALUE_VALUE_FLAG_DATA_MANAGED;
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM:
			if( libuna_byte_stream_copy_from_utf8(
			     internal_value->data,
			     internal_value->data_size,
			     internal_value->codepage,
			     utf8_string,
			     utf8_string_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy byte stream from UTF-8 string.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF8:
			if( libuna_utf8_stream_copy_from_utf8(
			     internal_value->data,
			     internal_value->data_size,
			     utf8_string,
			     utf8_string_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy UTF-8 stream from UTF-8 string.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF16:
			if( libuna_utf16_stream_copy_from_utf8(
			     internal_value->data,
			     internal_value->data_size,
			     internal_value->byte_order,
			     utf8_string,
			     utf8_string_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy UTF-16 stream from UTF-8 string.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF32:
			if( libuna_utf32_stream_copy_from_utf8(
			     internal_value->data,
			     internal_value->data_size,
			     internal_value->byte_order,
			     utf8_string,
			     utf8_string_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy UTF-32 stream from UTF-8 string.",
				 function );

				return( -1 );
			}
			break;

		default:
			break;
	}
	return( 1 );
}

/* Retrieves the size of an UTF-8 encoded string of the value data
 * Returns 1 if successful, 0 if value type not supported or -1 on error
 */
int libfvalue_value_get_utf8_string_size(
     libfvalue_value_t *value,
     int value_entry_index,
     size_t *utf8_string_size,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	uint8_t *entry_data                        = NULL;
	static char *function                      = "libfvalue_value_get_utf8_string_size";
	size_t entry_data_size                     = 0;
	int result                                 = 1;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM )
	 && ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_UTF8 )
	 && ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_UTF16 )
	 && ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_UTF32 ) )
	{
		return( 0 );
	}
	if( libfvalue_value_get_entry_data(
	     value,
	     value_entry_index,
	     &entry_data,
	     &entry_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry data: %d.",
		 function,
		 value_entry_index );

		return( -1 );
	}
	if( entry_data == NULL )
	{
		if( utf8_string_size == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
			 "%s: invalid UTF-8 string size.",
			 function );

			return( -1 );
		}
		*utf8_string_size = 0;

		return( 1 );
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM:
			/* Codepage 1200 represents Unicode
			 * If the codepage is 1200 determine if the string is encoded in UTF-8 or UTF-16 little-endian
			 */
			if( internal_value->codepage == LIBFVALUE_CODEPAGE_UNICODE )
			{
				result = 0;

				if( ( entry_data_size % 2 ) == 0 )
				{
					result = libuna_utf8_string_size_from_utf16_stream(
						  entry_data,
						  entry_data_size,
						  LIBUNA_ENDIAN_LITTLE,
						  utf8_string_size,
						  error );

					if( result != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_GET_FAILED,
						 "%s: unable to determine UTF-8 string size of UTF-16 stream for codepage 1200.",
						 function );

#if defined( HAVE_DEBUG_OUTPUT )
						if( ( error != NULL )
						 && ( *error != NULL ) )
						{
							libnotify_print_error_backtrace(
							 *error );
						}
#endif
						liberror_error_free(
						 error );
					}
				}
				if( result != 1 )
				{
					result = libuna_utf8_string_size_from_utf8_stream(
					          entry_data,
					          entry_data_size,
					          utf8_string_size,
					          error );

					if( result != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_GET_FAILED,
						 "%s: unable to determine UTF-8 string size of UTF-8 stream for codepage 1200.",
						 function );

						return( -1 );
					}
				}
			}
			else
			{
				if( libuna_utf8_string_size_from_byte_stream(
				     entry_data,
				     entry_data_size,
				     internal_value->codepage,
				     utf8_string_size,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to determine UTF-8 string size of byte stream.",
					 function );

					return( -1 );
				}
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF8:
			if( libuna_utf8_string_size_from_utf8_stream(
			     entry_data,
			     entry_data_size,
			     utf8_string_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine UTF-8 string size of UTF-8 stream.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF16:
			if( libuna_utf8_string_size_from_utf16_stream(
			     entry_data,
			     entry_data_size,
			     internal_value->byte_order,
			     utf8_string_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine UTF-8 string size of UTF-16 stream.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF32:
			if( libuna_utf8_string_size_from_utf32_stream(
			     entry_data,
			     entry_data_size,
			     internal_value->byte_order,
			     utf8_string_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine UTF-8 string size of UTF-32 stream.",
				 function );

				return( -1 );
			}
			break;

		default:
			result = 0;

			break;
	}
	return( result );
}

/* Copies the value data to an UTF-8 encoded string
 * Returns 1 if successful, 0 if value type not supported or -1 on error
 */
int libfvalue_value_copy_to_utf8_string(
     libfvalue_value_t *value,
     int value_entry_index,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	uint8_t *entry_data                        = NULL;
	static char *function                      = "libfvalue_value_copy_to_utf8_string";
	size_t entry_data_size                     = 0;
	int result                                 = 1;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM )
	 && ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_UTF8 )
	 && ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_UTF16 )
	 && ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_UTF32 ) )
	{
		return( 0 );
	}
	if( libfvalue_value_get_entry_data(
	     value,
	     value_entry_index,
	     &entry_data,
	     &entry_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry data: %d.",
		 function,
		 value_entry_index );

		return( -1 );
	}
	if( entry_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing entry data.",
		 function );

		return( -1 );
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM:
			/* Codepage 1200 represents Unicode
		 	 * If the codepage is 1200 determine if the string is encoded in UTF-8 or UTF-16 little-endian
			 */
			if( internal_value->codepage == LIBFVALUE_CODEPAGE_UNICODE )
			{
				result = 0;

				if( ( entry_data_size % 2 ) == 0 )
				{
					result = libuna_utf8_string_copy_from_utf16_stream(
						  utf8_string,
						  utf8_string_size,
						  entry_data,
						  entry_data_size,
						  LIBUNA_ENDIAN_LITTLE,
						  error );

					if( result != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
						 "%s: unable to copy UTF-16 stream to UTF-8 string for codepage 1200.",
						 function );

#if defined( HAVE_DEBUG_OUTPUT )
						if( ( error != NULL )
						 && ( *error != NULL ) )
						{
							libnotify_print_error_backtrace(
							 *error );
						}
#endif
						liberror_error_free(
						 error );
					}
				}
				if( result != 1 )
				{
					result = libuna_utf8_string_copy_from_utf8_stream(
					          utf8_string,
					          utf8_string_size,
					          entry_data,
					          entry_data_size,
					          error );

					if( result != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
						 "%s: unable to copy UTF-8 stream to UTF-8 string for codepage 1200.",
						 function );

						return( -1 );
					}
				}
			}
			else
			{
				if( libuna_utf8_string_copy_from_byte_stream(
				     utf8_string,
				     utf8_string_size,
				     entry_data,
				     entry_data_size,
				     internal_value->codepage,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
					 "%s: unable to copy byte stream to UTF-8 string.",
					 function );

					return( -1 );
				}
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF8:
			if( libuna_utf8_string_copy_from_utf8_stream(
			     utf8_string,
			     utf8_string_size,
			     entry_data,
			     entry_data_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy UTF-8 stream to UTF-8 string.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF16:
			if( libuna_utf8_string_copy_from_utf16_stream(
			     utf8_string,
			     utf8_string_size,
			     entry_data,
			     entry_data_size,
			     internal_value->byte_order,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy UTF-16 stream to UTF-8 string.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF32:
			if( libuna_utf8_string_copy_from_utf32_stream(
			     utf8_string,
			     utf8_string_size,
			     entry_data,
			     entry_data_size,
			     internal_value->byte_order,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy UTF-32 stream to UTF-8 string.",
				 function );

				return( -1 );
			}
			break;

		default:
			result = 0;

			break;
	}
	return( result );
}

/* Copies the value data from an UTF-16 encoded string
 * Returns 1 if successful, 0 if value type not supported or -1 on error
 */
int libfvalue_value_copy_from_utf16_string(
     libfvalue_value_t *value,
     int value_entry_index,
     const uint16_t *utf16_string,
     size_t utf16_string_size,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	static char *function                      = "libfvalue_value_copy_from_utf16_string";
	size_t value_data_size                     = 0;
	int result                                 = 1;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

/* TODO multi value support */
	if( value_entry_index != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid value index value out of bounds.",
		 function );

		return( -1 );
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM:
			if( libuna_byte_stream_size_from_utf16(
			     utf16_string,
			     utf16_string_size,
			     internal_value->codepage,
			     &value_data_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine byte stream size of UTF-16 string.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF8:
			if( libuna_utf8_stream_size_from_utf16(
			     utf16_string,
			     utf16_string_size,
			     &value_data_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine UTF-8 stream size of UTF-16 string.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF16:
			if( libuna_utf16_stream_size_from_utf16(
			     utf16_string,
			     utf16_string_size,
			     &value_data_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine UTF-16 stream size of UTF-16 string.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF32:
			if( libuna_utf32_stream_size_from_utf16(
			     utf16_string,
			     utf16_string_size,
			     &value_data_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine UTF-32 stream size of UTF-16 string.",
				 function );

				return( -1 );
			}
			break;

		default:
			result = 0;

			break;
	}
	if( result == 0 )
	{
		return( 0 );
	}
	if( internal_value->data != NULL )
	{
		if( ( internal_value->flags & LIBFVALUE_VALUE_FLAG_DATA_MANAGED ) != 0 )
		{
			memory_free(
			 internal_value->data );

			internal_value->flags &= ~( LIBFVALUE_VALUE_FLAG_DATA_MANAGED );
		}
		internal_value->data      = NULL;
		internal_value->data_size = 0;
	}
	if( internal_value->data == NULL )
	{
		internal_value->data_size = value_data_size;

		internal_value->data = (uint8_t *) memory_allocate(
		                                    sizeof( uint16_t ) * internal_value->data_size );

		if( internal_value->data == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create data.",
			 function );

			return( -1 );
		}
		internal_value->flags |= LIBFVALUE_VALUE_FLAG_DATA_MANAGED;
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM:
			if( libuna_byte_stream_copy_from_utf16(
			     internal_value->data,
			     internal_value->data_size,
			     internal_value->codepage,
			     utf16_string,
			     utf16_string_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy byte stream from UTF-16 string.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF8:
			if( libuna_utf8_stream_copy_from_utf16(
			     internal_value->data,
			     internal_value->data_size,
			     utf16_string,
			     utf16_string_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy UTF-8 stream from UTF-16 string.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF16:
			if( libuna_utf16_stream_copy_from_utf16(
			     internal_value->data,
			     internal_value->data_size,
			     internal_value->byte_order,
			     utf16_string,
			     utf16_string_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy UTF-16 stream from UTF-16 string.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF32:
			if( libuna_utf32_stream_copy_from_utf16(
			     internal_value->data,
			     internal_value->data_size,
			     internal_value->byte_order,
			     utf16_string,
			     utf16_string_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy UTF-32 stream from UTF-16 string.",
				 function );

				return( -1 );
			}
			break;

		default:
			break;
	}
	return( 1 );
}

/* Retrieves the size of an UTF-16 encoded string of the value data
 * Returns 1 if successful, 0 if value type not supported or -1 on error
 */
int libfvalue_value_get_utf16_string_size(
     libfvalue_value_t *value,
     int value_entry_index,
     size_t *utf16_string_size,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	uint8_t *entry_data                        = NULL;
	static char *function                      = "libfvalue_value_get_utf16_string_size";
	size_t entry_data_size                     = 0;
	int result                                 = 1;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM )
	 && ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_UTF8 )
	 && ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_UTF16 )
	 && ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_UTF32 ) )
	{
		return( 0 );
	}
	if( libfvalue_value_get_entry_data(
	     value,
	     value_entry_index,
	     &entry_data,
	     &entry_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry data: %d.",
		 function,
		 value_entry_index );

		return( -1 );
	}
	if( entry_data == NULL )
	{
		if( utf16_string_size == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
			 "%s: invalid UTF-16 string size.",
			 function );

			return( -1 );
		}
		*utf16_string_size = 0;

		return( 1 );
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM:
			/* Codepage 1200 represents Unicode
		 	 * If the codepage is 1200 determine if the string is encoded in UTF-8 or UTF-16 little-endian
			 */
			if( internal_value->codepage == LIBFVALUE_CODEPAGE_UNICODE )
			{
				result = 0;

				if( ( entry_data_size % 2 ) == 0 )
				{
					result = libuna_utf16_string_size_from_utf16_stream(
						  entry_data,
						  entry_data_size,
						  LIBUNA_ENDIAN_LITTLE,
						  utf16_string_size,
						  error );

					if( result != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_GET_FAILED,
						 "%s: unable to determine UTF-16 string size of UTF-16 stream for codepage 1200.",
						 function );

#if defined( HAVE_DEBUG_OUTPUT )
						if( ( error != NULL )
						 && ( *error != NULL ) )
						{
							libnotify_print_error_backtrace(
							 *error );
						}
#endif
						liberror_error_free(
						 error );
					}
				}
				if( result != 1 )
				{
					result = libuna_utf16_string_size_from_utf8_stream(
					          entry_data,
					          entry_data_size,
					          utf16_string_size,
					          error );

					if( result != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_GET_FAILED,
						 "%s: unable to determine UTF-16 string size of UTF-8 stream for codepage 1200.",
						 function );

						return( -1 );
					}
				}
			}
			else
			{
				if( libuna_utf16_string_size_from_byte_stream(
				     entry_data,
				     entry_data_size,
				     internal_value->codepage,
				     utf16_string_size,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to determine UTF-16 string size of byte stream.",
					 function );

					return( -1 );
				}
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF8:
			if( libuna_utf16_string_size_from_utf8_stream(
			     entry_data,
			     entry_data_size,
			     utf16_string_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine UTF-16 string size of UTF-8 stream.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF16:
			if( libuna_utf16_string_size_from_utf16_stream(
			     entry_data,
			     entry_data_size,
			     internal_value->byte_order,
			     utf16_string_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine UTF-16 string size of UTF-16 stream.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF32:
			if( libuna_utf16_string_size_from_utf32_stream(
			     entry_data,
			     entry_data_size,
			     internal_value->byte_order,
			     utf16_string_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine UTF-16 string size of UTF-32 stream.",
				 function );

				return( -1 );
			}
			break;

		default:
			result = 0;

			break;
	}
	return( result );
}

/* Copies the value data to an UTF-16 encoded string
 * Returns 1 if successful, 0 if value type not supported or -1 on error
 */
int libfvalue_value_copy_to_utf16_string(
     libfvalue_value_t *value,
     int value_entry_index,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	uint8_t *entry_data                        = NULL;
	static char *function                      = "libfvalue_value_copy_to_utf16_string";
	size_t entry_data_size                     = 0;
	int result                                 = 1;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM )
	 && ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_UTF8 )
	 && ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_UTF16 )
	 && ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_UTF32 ) )
	{
		return( 0 );
	}
	if( libfvalue_value_get_entry_data(
	     value,
	     value_entry_index,
	     &entry_data,
	     &entry_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry data: %d.",
		 function,
		 value_entry_index );

		return( -1 );
	}
	if( entry_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing entry data.",
		 function );

		return( -1 );
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM:
			/* Codepage 1200 represents Unicode
			 * If the codepage is 1200 determine if the string is encoded in UTF-8 or UTF-16 little-endian
			 */
			if( internal_value->codepage == LIBFVALUE_CODEPAGE_UNICODE )
			{
				result = 0;

				if( ( entry_data_size % 2 ) == 0 )
				{
					result = libuna_utf16_string_copy_from_utf16_stream(
						  utf16_string,
						  utf16_string_size,
						  entry_data,
						  entry_data_size,
						  LIBUNA_ENDIAN_LITTLE,
						  error );

					if( result != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
						 "%s: unable to copy UTF-16 stream to UTF-16 string for codepage 1200.",
						 function );

#if defined( HAVE_DEBUG_OUTPUT )
						if( ( error != NULL )
						 && ( *error != NULL ) )
						{
							libnotify_print_error_backtrace(
							 *error );
						}
#endif
						liberror_error_free(
						 error );
					}
				}
				if( result != 1 )
				{
					result = libuna_utf16_string_copy_from_utf8_stream(
					          utf16_string,
					          utf16_string_size,
					          entry_data,
					          entry_data_size,
					          error );

					if( result != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
						 "%s: unable to copy UTF-8 stream to UTF-16 string for codepage 1200.",
						 function );

						return( -1 );
					}
				}
			}
			else
			{
				if( libuna_utf16_string_copy_from_byte_stream(
				     utf16_string,
				     utf16_string_size,
				     entry_data,
				     entry_data_size,
				     internal_value->codepage,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
					 "%s: unable to copy byte stream to UTF-16 string.",
					 function );

					return( -1 );
				}
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF8:
			if( libuna_utf16_string_copy_from_utf8_stream(
			     utf16_string,
			     utf16_string_size,
			     entry_data,
			     entry_data_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy UTF-8 stream to UTF-16 string.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF16:
			if( libuna_utf16_string_copy_from_utf16_stream(
			     utf16_string,
			     utf16_string_size,
			     entry_data,
			     entry_data_size,
			     internal_value->byte_order,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy UTF-16 stream to UTF-16 string.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF32:
			if( libuna_utf16_string_copy_from_utf32_stream(
			     utf16_string,
			     utf16_string_size,
			     entry_data,
			     entry_data_size,
			     internal_value->byte_order,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy UTF-32 stream to UTF-16 string.",
				 function );

				return( -1 );
			}
			break;

		default:
			result = 0;

			break;
	}
	return( result );
}

/* Copies the value data from an UTF-32 encoded string
 * Returns 1 if successful, 0 if value type not supported or -1 on error
 */
int libfvalue_value_copy_from_utf32_string(
     libfvalue_value_t *value,
     int value_entry_index,
     const uint32_t *utf32_string,
     size_t utf32_string_size,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	static char *function                      = "libfvalue_value_copy_from_utf32_string";
	size_t value_data_size                     = 0;
	int result                                 = 1;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

/* TODO multi value support */
	if( value_entry_index != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid value index value out of bounds.",
		 function );

		return( -1 );
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM:
			if( libuna_byte_stream_size_from_utf32(
			     utf32_string,
			     utf32_string_size,
			     internal_value->codepage,
			     &value_data_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine byte stream size of UTF-32 string.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF8:
			if( libuna_utf8_stream_size_from_utf32(
			     utf32_string,
			     utf32_string_size,
			     &value_data_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine UTF-8 stream size of UTF-32 string.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF16:
			if( libuna_utf16_stream_size_from_utf32(
			     utf32_string,
			     utf32_string_size,
			     &value_data_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine UTF-16 stream size of UTF-32 string.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF32:
			if( libuna_utf32_stream_size_from_utf32(
			     utf32_string,
			     utf32_string_size,
			     &value_data_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine UTF-32 stream size of UTF-32 string.",
				 function );

				return( -1 );
			}
			break;

		default:
			result = 0;

			break;
	}
	if( result == 0 )
	{
		return( 0 );
	}
	if( internal_value->data != NULL )
	{
		if( ( internal_value->flags & LIBFVALUE_VALUE_FLAG_DATA_MANAGED ) != 0 )
		{
			memory_free(
			 internal_value->data );

			internal_value->flags &= ~( LIBFVALUE_VALUE_FLAG_DATA_MANAGED );
		}
		internal_value->data      = NULL;
		internal_value->data_size = 0;
	}
	if( internal_value->data == NULL )
	{
		internal_value->data_size = value_data_size;

		internal_value->data = (uint8_t *) memory_allocate(
		                                    sizeof( uint32_t ) * internal_value->data_size );

		if( internal_value->data == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create data.",
			 function );

			return( -1 );
		}
		internal_value->flags |= LIBFVALUE_VALUE_FLAG_DATA_MANAGED;
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM:
			if( libuna_byte_stream_copy_from_utf32(
			     internal_value->data,
			     internal_value->data_size,
			     internal_value->codepage,
			     utf32_string,
			     utf32_string_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy byte stream from UTF-32 string.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF8:
			if( libuna_utf8_stream_copy_from_utf32(
			     internal_value->data,
			     internal_value->data_size,
			     utf32_string,
			     utf32_string_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy UTF-8 stream from UTF-32 string.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF16:
			if( libuna_utf16_stream_copy_from_utf32(
			     internal_value->data,
			     internal_value->data_size,
			     internal_value->byte_order,
			     utf32_string,
			     utf32_string_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy UTF-16 stream from UTF-32 string.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF32:
			if( libuna_utf32_stream_copy_from_utf32(
			     internal_value->data,
			     internal_value->data_size,
			     internal_value->byte_order,
			     utf32_string,
			     utf32_string_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy UTF-32 stream from UTF-32 string.",
				 function );

				return( -1 );
			}
			break;

		default:
			break;
	}
	return( 1 );
}

/* Retrieves the size of an UTF-32 encoded string of the value data
 * Returns 1 if successful, 0 if value type not supported or -1 on error
 */
int libfvalue_value_get_utf32_string_size(
     libfvalue_value_t *value,
     int value_entry_index,
     size_t *utf32_string_size,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	uint8_t *entry_data                        = NULL;
	static char *function                      = "libfvalue_value_get_utf32_string_size";
	size_t entry_data_size                     = 0;
	int result                                 = 1;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM )
	 && ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_UTF8 )
	 && ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_UTF16 )
	 && ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_UTF32 ) )
	{
		return( 0 );
	}
	if( libfvalue_value_get_entry_data(
	     value,
	     value_entry_index,
	     &entry_data,
	     &entry_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry data: %d.",
		 function,
		 value_entry_index );

		return( -1 );
	}
	if( entry_data == NULL )
	{
		if( utf32_string_size == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
			 "%s: invalid UTF-32 string size.",
			 function );

			return( -1 );
		}
		*utf32_string_size = 0;

		return( 1 );
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM:
			/* Codepage 1200 represents Unicode
		 	 * If the codepage is 1200 determine if the string is encoded in UTF-8 or UTF-16 little-endian
			 */
			if( internal_value->codepage == LIBFVALUE_CODEPAGE_UNICODE )
			{
				result = 0;

				if( ( entry_data_size % 2 ) == 0 )
				{
					result = libuna_utf32_string_size_from_utf16_stream(
						  entry_data,
						  entry_data_size,
						  LIBUNA_ENDIAN_LITTLE,
						  utf32_string_size,
						  error );

					if( result != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_GET_FAILED,
						 "%s: unable to determine UTF-32 string size of UTF-16 stream for codepage 1200.",
						 function );

#if defined( HAVE_DEBUG_OUTPUT )
						if( ( error != NULL )
						 && ( *error != NULL ) )
						{
							libnotify_print_error_backtrace(
							 *error );
						}
#endif
						liberror_error_free(
						 error );
					}
				}
				if( result != 1 )
				{
					result = libuna_utf32_string_size_from_utf8_stream(
					          entry_data,
					          entry_data_size,
					          utf32_string_size,
					          error );

					if( result != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_GET_FAILED,
						 "%s: unable to determine UTF-32 string size of UTF-8 stream for codepage 1200.",
						 function );

						return( -1 );
					}
				}
			}
			else
			{
				if( libuna_utf32_string_size_from_byte_stream(
				     entry_data,
				     entry_data_size,
				     internal_value->codepage,
				     utf32_string_size,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to determine UTF-32 string size of byte stream.",
					 function );

					return( -1 );
				}
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF8:
			if( libuna_utf32_string_size_from_utf8_stream(
			     entry_data,
			     entry_data_size,
			     utf32_string_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine UTF-32 string size of UTF-8 stream.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF16:
			if( libuna_utf32_string_size_from_utf16_stream(
			     entry_data,
			     entry_data_size,
			     internal_value->byte_order,
			     utf32_string_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine UTF-32 string size of UTF-16 stream.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF32:
			if( libuna_utf32_string_size_from_utf32_stream(
			     entry_data,
			     entry_data_size,
			     internal_value->byte_order,
			     utf32_string_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine UTF-32 string size of UTF-32 stream.",
				 function );

				return( -1 );
			}
			break;

		default:
			result = 0;

			break;
	}
	return( result );
}

/* Copies the value data to an UTF-32 encoded string
 * Returns 1 if successful, 0 if value type not supported or -1 on error
 */
int libfvalue_value_copy_to_utf32_string(
     libfvalue_value_t *value,
     int value_entry_index,
     uint32_t *utf32_string,
     size_t utf32_string_size,
     liberror_error_t **error )
{
	libfvalue_internal_value_t *internal_value = NULL;
	uint8_t *entry_data                        = NULL;
	static char *function                      = "libfvalue_value_copy_to_utf32_string";
	size_t entry_data_size                     = 0;
	int result                                 = 1;

	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	internal_value = (libfvalue_internal_value_t *) value;

	if( ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM )
	 && ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_UTF8 )
	 && ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_UTF16 )
	 && ( internal_value->type != LIBFVALUE_VALUE_TYPE_STRING_UTF32 ) )
	{
		return( 0 );
	}
	if( libfvalue_value_get_entry_data(
	     value,
	     value_entry_index,
	     &entry_data,
	     &entry_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry data: %d.",
		 function,
		 value_entry_index );

		return( -1 );
	}
	if( entry_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing entry data.",
		 function );

		return( -1 );
	}
	switch( internal_value->type )
	{
		case LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM:
			/* Codepage 1200 represents Unicode
		 	 * If the codepage is 1200 determine if the string is encoded in UTF-8 or UTF-16 little-endian
			 */
			if( internal_value->codepage == LIBFVALUE_CODEPAGE_UNICODE )
			{
				result = 0;

				if( ( entry_data_size % 2 ) == 0 )
				{
					result = libuna_utf32_string_copy_from_utf16_stream(
						  utf32_string,
						  utf32_string_size,
						  entry_data,
						  entry_data_size,
						  LIBUNA_ENDIAN_LITTLE,
						  error );

					if( result != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
						 "%s: unable to copy UTF-16 stream to UTF-32 string for codepage 1200.",
						 function );

#if defined( HAVE_DEBUG_OUTPUT )
						if( ( error != NULL )
						 && ( *error != NULL ) )
						{
							libnotify_print_error_backtrace(
							 *error );
						}
#endif
						liberror_error_free(
						 error );
					}
				}
				if( result != 1 )
				{
					result = libuna_utf32_string_copy_from_utf8_stream(
					          utf32_string,
					          utf32_string_size,
					          entry_data,
					          entry_data_size,
					          error );

					if( result != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
						 "%s: unable to copy UTF-8 stream to UTF-32 string for codepage 1200.",
						 function );

						return( -1 );
					}
				}
			}
			else
			{
				if( libuna_utf32_string_copy_from_byte_stream(
				     utf32_string,
				     utf32_string_size,
				     entry_data,
				     entry_data_size,
				     internal_value->codepage,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
					 "%s: unable to copy byte stream to UTF-32 string.",
					 function );

					return( -1 );
				}
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF8:
			if( libuna_utf32_string_copy_from_utf8_stream(
			     utf32_string,
			     utf32_string_size,
			     entry_data,
			     entry_data_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy UTF-8 stream to UTF-32 string.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF16:
			if( libuna_utf32_string_copy_from_utf16_stream(
			     utf32_string,
			     utf32_string_size,
			     entry_data,
			     entry_data_size,
			     internal_value->byte_order,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy UTF-16 stream to UTF-32 string.",
				 function );

				return( -1 );
			}
			break;

		case LIBFVALUE_VALUE_TYPE_STRING_UTF32:
			if( libuna_utf32_string_copy_from_utf32_stream(
			     utf32_string,
			     utf32_string_size,
			     entry_data,
			     entry_data_size,
			     internal_value->byte_order,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy UTF-32 stream to UTF-32 string.",
				 function );

				return( -1 );
			}
			break;

		default:
			result = 0;

			break;
	}
	return( result );
}

