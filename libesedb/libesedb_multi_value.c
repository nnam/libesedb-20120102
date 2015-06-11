/*
 * Multi value functions
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

#include "libesedb_catalog_definition.h"
#include "libesedb_compression.h"
#include "libesedb_definitions.h"
#include "libesedb_libfvalue.h"
#include "libesedb_multi_value.h"
#include "libesedb_types.h"

/* Initializes the multi value and its values
 * Returns 1 if successful or -1 on error
 */
int libesedb_multi_value_initialize(
     libesedb_multi_value_t **multi_value,
     libesedb_catalog_definition_t *column_catalog_definition,
     libfvalue_value_t *record_value,
     liberror_error_t **error )
{
	libesedb_internal_multi_value_t *internal_multi_value = NULL;
	static char *function                                 = "libesedb_multi_value_initialize";

	if( multi_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid multi value.",
		 function );

		return( -1 );
	}
	if( *multi_value != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid multi value value already set.",
		 function );

		return( -1 );
	}
	if( column_catalog_definition == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid column catalog definition.",
		 function );

		return( -1 );
	}
	if( record_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid record value.",
		 function );

		return( -1 );
	}
	internal_multi_value = memory_allocate_structure(
	                        libesedb_internal_multi_value_t );

	if( internal_multi_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create internal multi value.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_multi_value,
	     0,
	     sizeof( libesedb_internal_multi_value_t ) ) == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear internal multi value.",
		 function );

		goto on_error;
	}
	internal_multi_value->column_catalog_definition = column_catalog_definition;
	internal_multi_value->record_value              = record_value;

	*multi_value = (libesedb_multi_value_t *) internal_multi_value;

	return( 1 );

on_error:
	if( internal_multi_value != NULL )
	{
		memory_free(
		 internal_multi_value );
	}
	return( -1 );
}

/* Frees a multi value
 * Returns 1 if successful or -1 on error
 */
int libesedb_multi_value_free(
     libesedb_multi_value_t **multi_value,
     liberror_error_t **error )
{
	static char *function = "libesedb_multi_value_free";

	if( multi_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid multi value.",
		 function );

		return( -1 );
	}
	if( *multi_value != NULL )
	{
		/* The column_catalog_definition and record_value references are freed elsewhere
		 */
		memory_free(
		 *multi_value );

		*multi_value = NULL;
	}
	return( 1 );
}

/* Retrieves the number of values of the multi value
 * Returns 1 if successful or -1 on error
 */
int libesedb_multi_value_get_number_of_values(
     libesedb_multi_value_t *multi_value,
     int *number_of_values,
     liberror_error_t **error )
{
	libesedb_internal_multi_value_t *internal_multi_value = NULL;
	static char *function                                 = "libesedb_multi_value_get_number_of_entries";

	if( multi_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid multi value.",
		 function );

		return( -1 );
	}
	internal_multi_value = (libesedb_internal_multi_value_t *) multi_value;

	if( libfvalue_value_get_number_of_value_entries(
	     internal_multi_value->record_value,
	     number_of_values,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of value entries.",
		 function );
	}
	return( 1 );
}

/* Retrieves a specific value of the multi value
 * Returns 1 if successful or -1 on error
 */
int libesedb_multi_value_get_value(
     libesedb_multi_value_t *multi_value,
     int value_index,
     uint32_t *column_type,
     uint8_t **value_data,
     size_t *value_data_size,
     liberror_error_t **error )
{
	libesedb_internal_multi_value_t *internal_multi_value = NULL;
	static char *function                                 = "libesedb_multi_value_get_value";

	if( multi_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid multi value.",
		 function );

		return( -1 );
	}
	internal_multi_value = (libesedb_internal_multi_value_t *) multi_value;

	if( libesedb_catalog_definition_get_column_type(
	     internal_multi_value->column_catalog_definition,
	     column_type,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve catalog definition column type.",
		 function );

		return( -1 );
	}
	if( libfvalue_value_get_entry_data(
	     internal_multi_value->record_value,
	     value_index,
	     value_data,
	     value_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value entry data: %d.",
		 function,
		 value_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the 32-bit value of a specific value of the multi value
 * Returns 1 if successful or -1 on error
 */
int libesedb_multi_value_get_value_32bit(
     libesedb_multi_value_t *multi_value,
     int value_index,
     uint32_t *value_32bit,
     liberror_error_t **error )
{
	libesedb_internal_multi_value_t *internal_multi_value = NULL;
	static char *function                                 = "libesedb_multi_value_get_value_32bit";
	uint32_t column_type                                  = 0;

	if( multi_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid multi value.",
		 function );

		return( -1 );
	}
	internal_multi_value = (libesedb_internal_multi_value_t *) multi_value;

	if( libesedb_catalog_definition_get_column_type(
	     internal_multi_value->column_catalog_definition,
	     &column_type,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve catalog definition column type.",
		 function );

		return( -1 );
	}
	if( ( column_type != LIBESEDB_COLUMN_TYPE_INTEGER_32BIT_SIGNED )
	 && ( column_type != LIBESEDB_COLUMN_TYPE_INTEGER_32BIT_UNSIGNED ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported column type: %" PRIu32 ".",
		 function,
		 column_type );

		return( -1 );
	}
	if( libfvalue_value_copy_to_32bit(
	     internal_multi_value->record_value,
	     value_index,
	     value_32bit,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy value entry: %d to 32-bit value.",
		 function,
		 value_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the 64-bit value of a specific value of the multi value
 * Returns 1 if successful or -1 on error
 */
int libesedb_multi_value_get_value_64bit(
     libesedb_multi_value_t *multi_value,
     int value_index,
     uint64_t *value_64bit,
     liberror_error_t **error )
{
	libesedb_internal_multi_value_t *internal_multi_value = NULL;
	static char *function                                 = "libesedb_multi_value_get_value_64bit";
	uint32_t column_type                                  = 0;

	if( multi_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid multi value.",
		 function );

		return( -1 );
	}
	internal_multi_value = (libesedb_internal_multi_value_t *) multi_value;

	if( libesedb_catalog_definition_get_column_type(
	     internal_multi_value->column_catalog_definition,
	     &column_type,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve catalog definition column type.",
		 function );

		return( -1 );
	}
	if( ( column_type != LIBESEDB_COLUMN_TYPE_CURRENCY )
	 && ( column_type != LIBESEDB_COLUMN_TYPE_INTEGER_64BIT_SIGNED ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported column type: %" PRIu32 ".",
		 function,
		 column_type );

		return( -1 );
	}
	if( libfvalue_value_copy_to_64bit(
	     internal_multi_value->record_value,
	     value_index,
	     value_64bit,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy value entry: %d to 64-bit value.",
		 function,
		 value_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the 64-bit filetime value of a specific value of the multi value
 * Returns 1 if successful or -1 on error
 */
int libesedb_multi_value_get_value_filetime(
     libesedb_multi_value_t *multi_value,
     int value_index,
     uint64_t *value_filetime,
     liberror_error_t **error )
{
	libesedb_internal_multi_value_t *internal_multi_value = NULL;
	static char *function                                 = "libesedb_multi_value_get_value_filetime";
	uint32_t column_type                                  = 0;

	if( multi_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid multi value.",
		 function );

		return( -1 );
	}
	internal_multi_value = (libesedb_internal_multi_value_t *) multi_value;

	if( libesedb_catalog_definition_get_column_type(
	     internal_multi_value->column_catalog_definition,
	     &column_type,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve catalog definition column type.",
		 function );

		return( -1 );
	}
	if( column_type != LIBESEDB_COLUMN_TYPE_DATE_TIME )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported column type: %" PRIu32 ".",
		 function,
		 column_type );

		return( -1 );
	}
	/* Copy the filetime to a 64-bit value
	 */
	if( libfvalue_value_copy_to_64bit(
	     internal_multi_value->record_value,
	     value_index,
	     value_filetime,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy value entry: %d to 64-bit value.",
		 function,
		 value_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the size of the UTF-8 encoded string of a specific value of the multi value
 * The returned size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
int libesedb_multi_value_get_value_utf8_string_size(
     libesedb_multi_value_t *multi_value,
     int value_index,
     size_t *utf8_string_size,
     liberror_error_t **error )
{
	libesedb_internal_multi_value_t *internal_multi_value = NULL;
	uint8_t *entry_data                                   = NULL;
	uint8_t *value_metadata                               = NULL;
	static char *function                                 = "libesedb_multi_value_get_value_utf8_string_size";
	size_t entry_data_size                                = 0;
	size_t value_metadata_size                            = 0;
	uint32_t column_type                                  = 0;
	uint8_t value_flags                                   = 0;
	int result                                            = 0;

	if( multi_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid multi value.",
		 function );

		return( -1 );
	}
	internal_multi_value = (libesedb_internal_multi_value_t *) multi_value;

	if( libesedb_catalog_definition_get_column_type(
	     internal_multi_value->column_catalog_definition,
	     &column_type,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve catalog definition column type.",
		 function );

		return( -1 );
	}
	if( ( column_type != LIBESEDB_COLUMN_TYPE_TEXT )
	 && ( column_type != LIBESEDB_COLUMN_TYPE_LARGE_TEXT ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported column type: %" PRIu32 ".",
		 function,
		 column_type );

		return( -1 );
	}
	/* The metadata contains the value flags
	 */
	if( libfvalue_value_get_metadata(
	     internal_multi_value->record_value,
	     &value_metadata,
	     &value_metadata_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value metadata.",
		 function );

		return( -1 );
	}
	if( value_metadata != NULL )
	{
		value_flags = *value_metadata;
	}
	if( ( ( value_flags & LIBESEDB_VALUE_FLAG_COMPRESSED ) != 0 )
	 && ( value_index == 0 ) )
	{
		if( libfvalue_value_get_entry_data(
		     internal_multi_value->record_value,
		     value_index,
		     &entry_data,
		     &entry_data_size,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve record value entry: %d data.",
			 function,
			 value_index );

			return( -1 );
		}
		result = libesedb_compression_get_utf8_string_size(
			  entry_data,
			  entry_data_size,
			  utf8_string_size,
			  error );
	}
	else
	{
		result = libfvalue_value_get_utf8_string_size(
			  internal_multi_value->record_value,
			  value_index,
			  utf8_string_size,
			  error );
	}
	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable retrieve UTF-8 string size: %d.",
		 function,
		 value_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the UTF-8 encoded string value of a specific value of the multi value
 * The size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libesedb_multi_value_get_value_utf8_string(
     libesedb_multi_value_t *multi_value,
     int value_index,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     liberror_error_t **error )
{
	libesedb_internal_multi_value_t *internal_multi_value = NULL;
	uint8_t *entry_data                                   = NULL;
	uint8_t *value_metadata                               = NULL;
	static char *function                                 = "libesedb_multi_value_get_value_utf8_string";
	size_t entry_data_size                                = 0;
	size_t value_metadata_size                            = 0;
	uint32_t column_type                                  = 0;
	uint8_t value_flags                                   = 0;
	int result                                            = 0;

	if( multi_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid multi value.",
		 function );

		return( -1 );
	}
	internal_multi_value = (libesedb_internal_multi_value_t *) multi_value;

	if( libesedb_catalog_definition_get_column_type(
	     internal_multi_value->column_catalog_definition,
	     &column_type,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve catalog definition column type.",
		 function );

		return( -1 );
	}
	if( ( column_type != LIBESEDB_COLUMN_TYPE_TEXT )
	 && ( column_type != LIBESEDB_COLUMN_TYPE_LARGE_TEXT ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported column type: %" PRIu32 ".",
		 function,
		 column_type );

		return( -1 );
	}
	/* The metadata contains the value flags
	 */
	if( libfvalue_value_get_metadata(
	     internal_multi_value->record_value,
	     &value_metadata,
	     &value_metadata_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value metadata.",
		 function );

		return( -1 );
	}
	if( value_metadata != NULL )
	{
		value_flags = *value_metadata;
	}
	if( ( ( value_flags & LIBESEDB_VALUE_FLAG_COMPRESSED ) != 0 )
	 && ( value_index == 0 ) )
	{
		if( libfvalue_value_get_entry_data(
		     internal_multi_value->record_value,
		     value_index,
		     &entry_data,
		     &entry_data_size,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve record value entry: %d data.",
			 function,
			 value_index );

			return( -1 );
		}
		result = libesedb_compression_copy_to_utf8_string(
			  entry_data,
			  entry_data_size,
			  utf8_string,
			  utf8_string_size,
			  error );
	}
	else
	{
		result = libfvalue_value_copy_to_utf8_string(
			  internal_multi_value->record_value,
			  value_index,
			  utf8_string,
			  utf8_string_size,
			  error );
	}
	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy value entry: %d to UTF-8 string.",
		 function,
		 value_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the size of the UTF-16 encoded string of a specific value of the multi value
 * The returned size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
int libesedb_multi_value_get_value_utf16_string_size(
     libesedb_multi_value_t *multi_value,
     int value_index,
     size_t *utf16_string_size,
     liberror_error_t **error )
{
	libesedb_internal_multi_value_t *internal_multi_value = NULL;
	uint8_t *entry_data                                   = NULL;
	uint8_t *value_metadata                               = NULL;
	static char *function                                 = "libesedb_multi_value_get_value_utf16_string_size";
	size_t entry_data_size                                = 0;
	size_t value_metadata_size                            = 0;
	uint32_t column_type                                  = 0;
	uint8_t value_flags                                   = 0;
	int result                                            = 0;

	if( multi_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid multi value.",
		 function );

		return( -1 );
	}
	internal_multi_value = (libesedb_internal_multi_value_t *) multi_value;

	if( libesedb_catalog_definition_get_column_type(
	     internal_multi_value->column_catalog_definition,
	     &column_type,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve catalog definition column type.",
		 function );

		return( -1 );
	}
	if( ( column_type != LIBESEDB_COLUMN_TYPE_TEXT )
	 && ( column_type != LIBESEDB_COLUMN_TYPE_LARGE_TEXT ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported column type: %" PRIu32 ".",
		 function,
		 column_type );

		return( -1 );
	}
	/* The metadata contains the value flags
	 */
	if( libfvalue_value_get_metadata(
	     internal_multi_value->record_value,
	     &value_metadata,
	     &value_metadata_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value metadata.",
		 function );

		return( -1 );
	}
	if( value_metadata != NULL )
	{
		value_flags = *value_metadata;
	}
	if( ( ( value_flags & LIBESEDB_VALUE_FLAG_COMPRESSED ) != 0 )
	 && ( value_index == 0 ) )
	{
		if( libfvalue_value_get_entry_data(
		     internal_multi_value->record_value,
		     value_index,
		     &entry_data,
		     &entry_data_size,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve record value entry: %d data.",
			 function,
			 value_index );

			return( -1 );
		}
		result = libesedb_compression_get_utf16_string_size(
			  entry_data,
			  entry_data_size,
			  utf16_string_size,
			  error );
	}
	else
	{
		result = libfvalue_value_get_utf16_string_size(
			  internal_multi_value->record_value,
			  value_index,
			  utf16_string_size,
			  error );
	}
	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable retrieve UTF-16 string size: %d.",
		 function,
		 value_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the UTF-16 encoded string value of a specific value of the multi value
 * The size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libesedb_multi_value_get_value_utf16_string(
     libesedb_multi_value_t *multi_value,
     int value_index,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     liberror_error_t **error )
{
	libesedb_internal_multi_value_t *internal_multi_value = NULL;
	uint8_t *entry_data                                   = NULL;
	uint8_t *value_metadata                               = NULL;
	static char *function                                 = "libesedb_multi_value_get_value_utf16_string";
	size_t entry_data_size                                = 0;
	size_t value_metadata_size                            = 0;
	uint32_t column_type                                  = 0;
	uint8_t value_flags                                   = 0;
	int result                                            = 0;

	if( multi_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid multi value.",
		 function );

		return( -1 );
	}
	internal_multi_value = (libesedb_internal_multi_value_t *) multi_value;

	if( libesedb_catalog_definition_get_column_type(
	     internal_multi_value->column_catalog_definition,
	     &column_type,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve catalog definition column type.",
		 function );

		return( -1 );
	}
	if( ( column_type != LIBESEDB_COLUMN_TYPE_TEXT )
	 && ( column_type != LIBESEDB_COLUMN_TYPE_LARGE_TEXT ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported column type: %" PRIu32 ".",
		 function,
		 column_type );

		return( -1 );
	}
	/* The metadata contains the value flags
	 */
	if( libfvalue_value_get_metadata(
	     internal_multi_value->record_value,
	     &value_metadata,
	     &value_metadata_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value metadata.",
		 function );

		return( -1 );
	}
	if( value_metadata != NULL )
	{
		value_flags = *value_metadata;
	}
	if( ( ( value_flags & LIBESEDB_VALUE_FLAG_COMPRESSED ) != 0 )
	 && ( value_index == 0 ) )
	{
		if( libfvalue_value_get_entry_data(
		     internal_multi_value->record_value,
		     value_index,
		     &entry_data,
		     &entry_data_size,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve record value entry: %d data.",
			 function,
			 value_index );

			return( -1 );
		}
		result = libesedb_compression_copy_to_utf16_string(
			  entry_data,
			  entry_data_size,
			  utf16_string,
			  utf16_string_size,
			  error );
	}
	else
	{
		result = libfvalue_value_copy_to_utf16_string(
			  internal_multi_value->record_value,
			  value_index,
			  utf16_string,
			  utf16_string_size,
			  error );
	}
	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy value entry: %d to UTF-16 string.",
		 function,
		 value_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the size of a binary data value of a specific value of the multi value
 * Returns 1 if successful or -1 on error
 */
int libesedb_multi_value_get_value_binary_data_size(
     libesedb_multi_value_t *multi_value,
     int value_index,
     size_t *binary_data_size,
     liberror_error_t **error )
{
	libesedb_internal_multi_value_t *internal_multi_value = NULL;
	uint8_t *value_entry_data                             = NULL;
	static char *function                                 = "libesedb_multi_value_get_value_binary_data_size";
	uint32_t column_type                                  = 0;

	if( multi_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid multi value.",
		 function );

		return( -1 );
	}
	internal_multi_value = (libesedb_internal_multi_value_t *) multi_value;

	if( libesedb_catalog_definition_get_column_type(
	     internal_multi_value->column_catalog_definition,
	     &column_type,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve catalog definition column type.",
		 function );

		return( -1 );
	}
	if( ( column_type != LIBESEDB_COLUMN_TYPE_BINARY_DATA )
	 && ( column_type != LIBESEDB_COLUMN_TYPE_LARGE_BINARY_DATA ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported column type: %" PRIu32 ".",
		 function,
		 column_type );

		return( -1 );
	}
	if( libfvalue_value_get_entry_data(
	     internal_multi_value->record_value,
	     value_index,
	     &value_entry_data,
	     binary_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value entry data: %d.",
		 function,
		 value_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the binary data value of a specific value of the multi value
 * Returns 1 if successful or -1 on error
 */
int libesedb_multi_value_get_value_binary_data(
     libesedb_multi_value_t *multi_value,
     int value_index,
     uint8_t *binary_data,
     size_t binary_data_size,
     liberror_error_t **error )
{
	libesedb_internal_multi_value_t *internal_multi_value = NULL;
	static char *function                                 = "libesedb_multi_value_get_value_binary_data";
	uint32_t column_type                                  = 0;

	if( multi_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid multi value.",
		 function );

		return( -1 );
	}
	internal_multi_value = (libesedb_internal_multi_value_t *) multi_value;

	if( libesedb_catalog_definition_get_column_type(
	     internal_multi_value->column_catalog_definition,
	     &column_type,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve catalog definition column type.",
		 function );

		return( -1 );
	}
	if( ( column_type != LIBESEDB_COLUMN_TYPE_TEXT )
	 && ( column_type != LIBESEDB_COLUMN_TYPE_LARGE_TEXT ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported column type: %" PRIu32 ".",
		 function,
		 column_type );

		return( -1 );
	}
	if( libfvalue_value_copy_entry_data(
	     internal_multi_value->record_value,
	     value_index,
	     binary_data,
	     binary_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable copy value entry data: %d.",
		 function,
		 value_index );

		return( -1 );
	}
	return( 1 );
}

