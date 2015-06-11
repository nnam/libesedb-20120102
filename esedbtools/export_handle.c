/* 
 * Export handle
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
#include <file_stream.h>
#include <memory.h>
#include <types.h>

#include <libcstring.h>
#include <liberror.h>

#include <libsystem.h>

#include "esedbinput.h"
#include "esedbtools_libesedb.h"
#include "esedbtools_libfdatetime.h"
#include "export.h"
#include "export_exchange.h"
#include "export_handle.h"
#include "windows_search.h"
#include "windows_security.h"

#define EXPORT_HANDLE_NOTIFY_STREAM	stdout

/* Initializes the export handle
 * Returns 1 if successful or -1 on error
 */
int export_handle_initialize(
     export_handle_t **export_handle,
     liberror_error_t **error )
{
	static char *function = "export_handle_initialize";

	if( export_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid export handle.",
		 function );

		return( -1 );
	}
	if( *export_handle != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid export handle value already set.",
		 function );

		return( -1 );
	}
	*export_handle = memory_allocate_structure(
	                  export_handle_t );

	if( *export_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create export handle.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *export_handle,
	     0,
	     sizeof( export_handle_t ) ) == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear export handle.",
		 function );

		goto on_error;
	}
	if( libesedb_file_initialize(
	     &( ( *export_handle )->input_file ),
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to initialize input file.",
		 function );

		goto on_error;
	}
	( *export_handle )->export_mode    = EXPORT_MODE_TABLES;
	( *export_handle )->ascii_codepage = LIBESEDB_CODEPAGE_WINDOWS_1252;
	( *export_handle )->notify_stream  = EXPORT_HANDLE_NOTIFY_STREAM;

	return( 1 );

on_error:
	if( *export_handle != NULL )
	{
		memory_free(
		 *export_handle );

		*export_handle = NULL;
	}
	return( -1 );
}

/* Frees the export handle and its elements
 * Returns 1 if successful or -1 on error
 */
int export_handle_free(
     export_handle_t **export_handle,
     liberror_error_t **error )
{
	static char *function = "export_handle_free";
	int result            = 1;

	if( export_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid export handle.",
		 function );

		return( -1 );
	}
	if( *export_handle != NULL )
	{
		if( libesedb_file_free(
		     &( ( *export_handle )->input_file ),
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free input file.",
			 function );

			result = -1;
		}
		if( ( *export_handle )->target_path != NULL )
		{
			memory_free(
			 ( *export_handle )->target_path );
		}
		if( ( *export_handle )->items_export_path != NULL )
		{
			memory_free(
			 ( *export_handle )->items_export_path );
		}
		memory_free(
		 *export_handle );

		*export_handle = NULL;
	}
	return( result );
}

/* Signals the export handle to abort
 * Returns 1 if successful or -1 on error
 */
int export_handle_signal_abort(
     export_handle_t *export_handle,
     liberror_error_t **error )
{
	static char *function = "export_handle_signal_abort";

	if( export_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid export handle.",
		 function );

		return( -1 );
	}
	export_handle->abort = 1;

	return( 1 );
}

/* Sets the export mode
 * Returns 1 if successful, 0 if unsupported values or -1 on error
 */
int export_handle_set_export_mode(
     export_handle_t *export_handle,
     const libcstring_system_character_t *string,
     liberror_error_t **error )
{
	static char *function = "export_handle_set_export_mode";
	size_t string_length  = 0;
	int result            = 0;

	if( export_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid export handle.",
		 function );

		return( -1 );
	}
	if( string == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid string.",
		 function );

		return( -1 );
	}
	string_length = libcstring_system_string_length(
	                 string );

	if( string_length == 3 )
	{
		if( libcstring_system_string_compare(
		     string,
		     _LIBCSTRING_SYSTEM_STRING( "all" ),
		     3 ) == 0 )
		{
			export_handle->export_mode = EXPORT_MODE_ALL;

			result = 1;
		}
	}
	else if( string_length == 6 )
	{
		if( libcstring_system_string_compare(
		     string,
		     _LIBCSTRING_SYSTEM_STRING( "tables" ),
		     6 ) == 0 )
		{
			export_handle->export_mode = EXPORT_MODE_TABLES;

			result = 1;
		}
	}
	return( result );
}

/* Sets the ascii codepage
 * Returns 1 if successful or -1 on error
 */
int export_handle_set_ascii_codepage(
     export_handle_t *export_handle,
     const libcstring_system_character_t *string,
     liberror_error_t **error )
{
	static char *function = "export_handle_set_ascii_codepage";
	int result            = 0;

	if( export_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid export handle.",
		 function );

		return( -1 );
	}
	result = esedbinput_determine_ascii_codepage(
	          string,
	          &( export_handle->ascii_codepage ),
	          error );

	if( result == -1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to determine ASCII codepage.",
		 function );

		return( -1 );
	}
	return( result );
}

/* Sets the target path
 * Returns 1 if successful or -1 on error
 */
int export_handle_set_target_path(
     export_handle_t *export_handle,
     const libcstring_system_character_t *target_path,
     liberror_error_t **error )
{
	static char *function                                      = "export_handle_set_target_path";
	size_t target_path_length                                  = 0;

#if defined( WINAPI )
	libcstring_system_character_t *extended_length_target_path = NULL;
        size_t extended_length_target_path_size                    = 0;
	int result                                                 = 0;
#endif

	if( export_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid export handle.",
		 function );

		return( -1 );
	}
	if( target_path == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid target path.",
		 function );

		return( -1 );
	}
	if( export_handle->target_path != NULL )
	{
		memory_free(
		 export_handle->target_path );

		export_handle->target_path      = NULL;
		export_handle->target_path_size = 0;
	}
	target_path_length = libcstring_system_string_length(
	                      target_path );

#if defined( WINAPI )
	result = libsystem_path_create_windows_extended(
	          target_path,
                  target_path_length,
                  &extended_length_target_path,
                  &extended_length_target_path_size,
                  error );

        if( result == -1 )
        {
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create extended-length target path.",
		 function );

		goto on_error;
        }
        else if( result != 0 )
        {
                target_path        = extended_length_target_path;
                target_path_length = extended_length_target_path_size - 1;
        }
#endif
	if( target_path_length > 0 )
	{
		export_handle->target_path = libcstring_system_string_allocate(
		                              target_path_length + 1 );

		if( export_handle->target_path == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create target path.",
			 function );

			goto on_error;
		}
		if( libcstring_system_string_copy(
		     export_handle->target_path,
		     target_path,
		     target_path_length ) == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
			 "%s: unable to copy target path.",
			 function );

			goto on_error;
		}
		( export_handle->target_path )[ target_path_length ] = 0;

		export_handle->target_path_size = target_path_length + 1;
	}
#if defined( WINAPI )
	memory_free(
	 extended_length_target_path );
#endif
	return( 1 );

on_error:
	if( export_handle->target_path != NULL )
	{
		memory_free(
		 export_handle->target_path );

		export_handle->target_path      = NULL;
		export_handle->target_path_size = 0;
	}
#if defined( WINAPI )
	if( extended_length_target_path != NULL )
	{
		memory_free(
		 extended_length_target_path );
	}
#endif
	return( -1 );
}

/* Sets an export path consisting of a base path and a suffix
 * Returns 1 if successful or -1 on error
 */
int export_handle_set_export_path(
     export_handle_t *export_handle,
     const libcstring_system_character_t *base_path,
     size_t base_path_length,
     const libcstring_system_character_t *suffix,
     size_t suffix_length,
     libcstring_system_character_t **export_path,
     size_t *export_path_size,
     liberror_error_t **error )
{
	static char *function = "export_handle_set_export_path";

	if( export_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid export handle.",
		 function );

		return( -1 );
	}
	if( base_path == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid base path.",
		 function );

		return( -1 );
	}
	if( base_path_length > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid base path length value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( suffix == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid suffix.",
		 function );

		return( -1 );
	}
	if( suffix_length > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid suffix length value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( export_path == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid export path.",
		 function );

		return( -1 );
	}
	if( export_path_size == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid export path size.",
		 function );

		return( -1 );
	}
	if( *export_path != NULL )
	{
		memory_free(
		 *export_path );

		*export_path      = NULL;
		*export_path_size = 0;
	}
	*export_path_size = base_path_length + suffix_length + 1;

	*export_path = libcstring_system_string_allocate(
	                *export_path_size );

	if( *export_path == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create export path.",
		 function );

		goto on_error;
	}
	if( libcstring_system_string_copy(
	     *export_path,
	     base_path,
	     base_path_length ) == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy base path to item export path.",
		 function );

		goto on_error;
	}
	if( libcstring_system_string_copy(
	     &( ( *export_path )[ base_path_length ] ),
	     suffix,
	     suffix_length ) == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy suffix to item export path.",
		 function );

		goto on_error;
	}
	( *export_path )[ *export_path_size - 1 ] = 0;

	return( 1 );

on_error:
	if( *export_path != NULL )
	{
		memory_free(
		 *export_path );

		*export_path      = NULL;
		*export_path_size = 0;
	}
	return( -1 );
}

/* Creates the items export path
 * Returns 1 if successful, 0 if already exists or -1 on error
 */
int export_handle_create_items_export_path(
     export_handle_t *export_handle,
     liberror_error_t **error )
{
	static char *function = "export_handle_create_items_export_path";
	int result            = 0;

	if( export_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid export handle.",
		 function );

		return( -1 );
	}
	if( export_handle_set_export_path(
	     export_handle,
	     export_handle->target_path,
	     export_handle->target_path_size - 1,
	     _LIBCSTRING_SYSTEM_STRING( ".export" ),
	     7,
	     &( export_handle->items_export_path ),
	     &( export_handle->items_export_path_size ),
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set items export path.",
		 function );

		return( -1 );
	}
	result = libsystem_file_exists(
		  export_handle->items_export_path,
		  error );

	if( result == -1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_GENERIC,
		 "%s: unable to determine if %" PRIs_LIBCSTRING_SYSTEM " exists.",
		 function,
		 export_handle->items_export_path );

		return( -1 );
	}
	else if( result == 1 )
	{
		return( 0 );
	}
	return( 1 );
}

/* Opens the export handle
 * Returns 1 if successful or -1 on error
 */
int export_handle_open(
     export_handle_t *export_handle,
     const libcstring_system_character_t *filename,
     liberror_error_t **error )
{
	static char *function = "export_handle_open";

	if( export_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid export handle.",
		 function );

		return( -1 );
	}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	if( libesedb_file_open_wide(
	     export_handle->input_file,
	     filename,
	     LIBESEDB_OPEN_READ,
	     error ) != 1 )
#else
	if( libesedb_file_open(
	     export_handle->input_file,
	     filename,
	     LIBESEDB_OPEN_READ,
	     error ) != 1 )
#endif
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_OPEN_FAILED,
		 "%s: unable to open input file.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Closes the export handle
 * Returns the 0 if succesful or -1 on error
 */
int export_handle_close(
     export_handle_t *export_handle,
     liberror_error_t **error )
{
	static char *function = "export_handle_close";

	if( export_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid export handle.",
		 function );

		return( -1 );
	}
	if( libesedb_file_close(
	     export_handle->input_file,
	     error ) != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_CLOSE_FAILED,
		 "%s: unable to close input file.",
		 function );

		return( -1 );
	}
	return( 0 );
}

/* Creates an item filename
 * Returns 1 if successful or -1 on error
 */
int export_handle_create_item_filename(
     export_handle_t *export_handle,
     int item_index,
     const libcstring_system_character_t *item_name,
     size_t item_name_length,
     libcstring_system_character_t **item_filename,
     size_t *item_filename_size,
     liberror_error_t **error )
{
	static char *function = "export_handle_create_item_filename";
	int number_of_digits  = 0;
	int remainder         = 0;

	if( export_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid export handle.",
		 function );

		return( -1 );
	}
	if( item_index < 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid item index value out of bounds.",
		 function );

		return( -1 );
	}
	if( item_name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid item name.",
		 function );

		return( -1 );
	}
	if( item_filename == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid item filename.",
		 function );

		return( -1 );
	}
	if( item_filename_size == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid item filename size.",
		 function );

		return( -1 );
	}
	remainder = item_index;

	do
	{
		number_of_digits++;

		remainder /= 10;
	}
	while( remainder > 0 );

	*item_filename_size = item_name_length + number_of_digits + 2;

	*item_filename = libcstring_system_string_allocate(
	                  *item_filename_size );

	if( *item_filename == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create item filename string.",
		 function );

		goto on_error;
	}
	if( libcstring_system_string_copy(
	     *item_filename,
	     item_name,
	     item_name_length ) == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy item name.",
		 function );

		goto on_error;
	}
	( *item_filename )[ item_name_length++ ] = (libcstring_system_character_t) '.';

	if( libsystem_string_decimal_copy_from_64_bit(
	     *item_filename,
	     *item_filename_size,
	     &item_name_length,
	     (uint64_t) item_index,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy item index.",
		 function );

		goto on_error;
	}
	( *item_filename )[ item_name_length ] = 0;
	     
	return( 1 );

on_error:
	if( *item_filename != NULL )
	{
		memory_free(
		 *item_filename );

		*item_filename = NULL;
	}
	*item_filename_size = 0;

	return( -1 );
}

/* Creates a text item file
 * Returns 1 if successful, 0 if the file already exists or -1 on error
 */
int export_handle_create_text_item_file(
     export_handle_t *export_handle,
     const libcstring_system_character_t *item_filename,
     size_t item_filename_length,
     const libcstring_system_character_t *export_path,
     size_t export_path_length,
     FILE **item_file_stream,
     liberror_error_t **error )
{
	libcstring_system_character_t *item_filename_path = NULL;
	static char *function                             = "export_handle_create_text_item_file";
	size_t item_filename_path_size                    = 0;
	int result                                        = 0;

	if( export_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid export handle.",
		 function );

		return( -1 );
	}
	if( item_file_stream == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid item file stream.",
		 function );

		return( -1 );
	}
	if( libsystem_path_create(
	     item_filename,
	     item_filename_length,
	     export_path,
	     export_path_length,
	     &item_filename_path,
	     &item_filename_path_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create item filename path.",
		 function );

		goto on_error;
	}
	result = libsystem_file_exists(
	          item_filename_path,
	          error );

	if( result == -1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_GENERIC,
		 "%s: unable to determine if %" PRIs_LIBCSTRING_SYSTEM " exists.",
		 function,
		 item_filename_path );

		goto on_error;
	}
	else if( result != 0 )
	{
		memory_free(
		 item_filename_path );

		return( 0 );
	}
	*item_file_stream = libsystem_file_stream_open(
	                     item_filename_path,
	                     _LIBCSTRING_SYSTEM_STRING( "w" ) );

	if( *item_file_stream == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_OPEN_FAILED,
		 "%s: unable to open: %" PRIs_LIBCSTRING_SYSTEM ".",
		 function,
		 item_filename_path );

		goto on_error;
	}
	memory_free(
	 item_filename_path );

	return( 1 );

on_error:
	if( item_filename_path != NULL )
	{
		memory_free(
		 item_filename_path );
	}
	return( -1 );
}

/* Exports the table
 * Returns 1 if successful or -1 on error
 */
int export_handle_export_table(
     export_handle_t *export_handle,
     libesedb_table_t *table,
     int table_index,
     const libcstring_system_character_t *table_name,
     size_t table_name_length,
     const libcstring_system_character_t *export_path,
     size_t export_path_length,
     log_handle_t *log_handle,
     liberror_error_t **error )
{
	libcstring_system_character_t *item_filename = NULL;
	libcstring_system_character_t *value_string  = NULL;
	libesedb_column_t *column                    = NULL;
	libesedb_record_t *record                    = NULL;
	FILE *table_file_stream                      = NULL;
	static char *function                        = "export_handle_export_table";
	size_t item_filename_size                    = 0;
	size_t value_string_size                     = 0;
	int column_iterator                          = 0;
	int known_table                              = 0;
	int number_of_columns                        = 0;
	int number_of_records                        = 0;
	int record_iterator                          = 0;
	int result                                   = 0;

	if( table == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid table.",
		 function );

		return( -1 );
	}
	if( table_name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid table name.",
		 function );

		return( -1 );
	}
	if( export_handle_create_item_filename(
	     export_handle,
	     table_index,
	     table_name,
	     table_name_length,
	     &item_filename,
	     &item_filename_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create item filename.",
		 function );

		goto on_error;
	}
	result = export_handle_create_text_item_file(
	          export_handle,
	          item_filename,
	          item_filename_size - 1,
	          export_path,
	          export_path_length,
	          &table_file_stream,
	          error );

	if( result == -1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create table file.",
		 function );

		goto on_error;
	}
	else if( result == 0 )
	{
		log_handle_printf(
		 log_handle,
		 "Skipping table: %" PRIs_LIBCSTRING_SYSTEM " it already exists.\n",
		 item_filename );

		memory_free(
		 item_filename );

		return( 1 );
	}
	memory_free(
	 item_filename );

	item_filename = NULL;

	/* Write the column names to the table file
	 */
	if( libesedb_table_get_number_of_columns(
	     table,
	     &number_of_columns,
	     0,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of columns.",
		 function );

		goto on_error;
	}
	for( column_iterator = 0;
	     column_iterator < number_of_columns;
	     column_iterator++ )
	{
		if( libesedb_table_get_column(
		     table,
		     column_iterator,
		     &column,
		     0,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve column: %d.",
			 function,
			 column_iterator );

			goto on_error;
		}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
		result = libesedb_column_get_utf16_name_size(
		          column,
		          &value_string_size,
		          error );
#else
		result = libesedb_column_get_utf8_name_size(
		          column,
		          &value_string_size,
		          error );
#endif
		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve the size of the column name.",
			 function );

			goto on_error;
		}
		if( value_string_size == 0 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing column name.",
			 function );

			goto on_error;
		}
		value_string = libcstring_system_string_allocate(
		                value_string_size );

		if( value_string == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create column name string.",
			 function );

			goto on_error;
		}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
		result = libesedb_column_get_utf16_name(
		          column,
		          (uint16_t *) value_string,
		          value_string_size,
		          error );
#else
		result = libesedb_column_get_utf8_name(
		          column,
		          (uint8_t *) value_string,
		          value_string_size,
		          error );
#endif
		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve the column name.",
			 function );

			goto on_error;
		}
		fprintf(
		 table_file_stream,
		 "%" PRIs_LIBCSTRING_SYSTEM "",
		 value_string );

		memory_free(
		 value_string );

		value_string = NULL;

		if( libesedb_column_free(
		     &column,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free column.",
			 function );

			goto on_error;
		}
		if( column_iterator == ( number_of_columns - 1 ) )
		{
			fprintf(
			 table_file_stream,
			 "\n" );
		}
		else
		{
			fprintf(
			 table_file_stream,
			 "\t" );
		}
	}
	/* Write the record (row) values to the table file
	 */
	if( libesedb_table_get_number_of_records(
	     table,
	     &number_of_records,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of records.",
		 function );

		goto on_error;
	}
	for( record_iterator = 0;
	     record_iterator < number_of_records;
	     record_iterator++ )
	{
		if( libesedb_table_get_record(
		     table,
		     record_iterator,
		     &record,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve record: %d.",
			 function,
			 record_iterator );

			goto on_error;
		}
		known_table = 0;

		if( table_name_length == 3 )
		{
			if( libcstring_system_string_compare(
			     table_name,
			     _LIBCSTRING_SYSTEM_STRING( "Msg" ),
			     3 ) == 0 )
			{
				known_table = 1;

				result = export_exchange_record_msg(
				          record,
				          table_file_stream,
				          log_handle,
				          error );
			}
		}
		else if( table_name_length == 6 )
		{
			if( libcstring_system_string_compare(
			     table_name,
			     _LIBCSTRING_SYSTEM_STRING( "Global" ),
			     6 ) == 0 )
			{
				known_table = 1;

				result = export_exchange_record_global(
				          record,
				          table_file_stream,
				          log_handle,
				          error );
			}
		}
		else if( table_name_length == 7 )
		{
			if( libcstring_system_string_compare(
			     table_name,
			     _LIBCSTRING_SYSTEM_STRING( "Folders" ),
			     7 ) == 0 )
			{
				known_table = 1;

				result = export_exchange_record_folders(
				          record,
				          table_file_stream,
				          log_handle,
				          error );
			}
			else if( libcstring_system_string_compare(
			          table_name,
			          _LIBCSTRING_SYSTEM_STRING( "Mailbox" ),
			          7 ) == 0 )
			{
				known_table = 1;

				result = export_exchange_record_mailbox(
				          record,
				          table_file_stream,
				          log_handle,
				          error );
			}
		}
		else if( table_name_length == 11 )
		{
			if( libcstring_system_string_compare(
			     table_name,
			     _LIBCSTRING_SYSTEM_STRING( "PerUserRead" ),
			     11 ) == 0 )
			{
				known_table = 1;

				result = export_exchange_record_per_user_read(
				          record,
				          table_file_stream,
				          log_handle,
				          error );
			}
		}
		else if( table_name_length == 12 )
		{
			if( libcstring_system_string_compare(
			     table_name,
			     _LIBCSTRING_SYSTEM_STRING( "SmTblSection" ),
			     12 ) == 0 )
			{
				known_table = 1;

				result = windows_security_export_record_smtblsection(
				          record,
				          table_file_stream,
				          log_handle,
				          error );
			}
			else if( libcstring_system_string_compare(
			          table_name,
			          _LIBCSTRING_SYSTEM_STRING( "SmTblVersion" ),
			          12 ) == 0 )
			{
				known_table = 1;

				result = windows_security_export_record_smtblversion(
				          record,
				          table_file_stream,
				          log_handle,
				          error );
			}
		}
		else if( table_name_length == 14 )
		{
			if( libcstring_system_string_compare(
			     table_name,
			     _LIBCSTRING_SYSTEM_STRING( "SystemIndex_0A" ),
			     14 ) == 0 )
			{
				known_table = 1;

				result = windows_search_export_record_systemindex_0a(
				          record,
				          export_handle->ascii_codepage,
				          table_file_stream,
				          log_handle,
				          error );
			}
		}
		else if( table_name_length == 16 )
		{
			if( libcstring_system_string_compare(
			     table_name,
			     _LIBCSTRING_SYSTEM_STRING( "SystemIndex_Gthr" ),
			     16 ) == 0 )
			{
				known_table = 1;

				result = windows_search_export_record_systemindex_gthr(
				          record,
				          table_file_stream,
				          log_handle,
				          error );
			}
		}
		if( known_table == 0 )
		{
			result = export_handle_export_record(
			          record,
			          table_file_stream,
			          log_handle,
			          error );
		}
		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GENERIC,
			 "%s: unable to export record.",
			 function );

			goto on_error;
		}
		if( libesedb_record_free(
		     &record,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free record.",
			 function );

			goto on_error;
		}
		if( export_handle->abort != 0 )
		{
			break;
		}
	}
	if( libsystem_file_stream_close(
	     table_file_stream ) != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_CLOSE_FAILED,
		 "%s: unable to close table file.",
		 function );

		goto on_error;
	}
	table_file_stream = NULL;

	if( export_handle->abort == 0 )
	{
		if( export_handle->export_mode != EXPORT_MODE_TABLES )
		{
			if( export_handle_export_indexes(
			     export_handle,
			     table,
			     table_name,
			     table_name_length,
			     export_path,
			     export_path_length,
			     log_handle,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GENERIC,
				 "%s: unable to export indexes.",
				 function );

				goto on_error;
			}
		}
	}
	return( 1 );

on_error:
	if( record != NULL )
	{
		libesedb_record_free(
		 &record,
		 NULL );
	}
	if( value_string != NULL )
	{
		memory_free(
		 value_string );
	}
	if( column != NULL )
	{
		libesedb_column_free(
		 &column,
		 NULL );
	}
	if( table_file_stream != NULL )
	{
		libsystem_file_stream_close(
		 table_file_stream );
	}
	if( item_filename != NULL )
	{
		memory_free(
		 item_filename );
	}
	return( -1 );
}

/* Exports the indexes of a specific table
 * Returns 1 if successful or -1 on error
 */
int export_handle_export_indexes(
     export_handle_t *export_handle,
     libesedb_table_t *table,
     const libcstring_system_character_t *table_name,
     size_t table_name_length,
     const libcstring_system_character_t *export_path,
     size_t export_path_length,
     log_handle_t *log_handle,
     liberror_error_t **error )
{
	libcstring_system_character_t *index_directory_name = NULL;
	libcstring_system_character_t *index_name           = NULL;
	libesedb_index_t *index                             = NULL;
	static char *function                               = "export_handle_export_indexes";
	size_t index_directory_name_size                    = 0;
	size_t index_name_size                              = 0;
	int index_iterator                                  = 0;
	int number_of_indexes                               = 0;
	int result                                          = 0;

	if( table == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid table.",
		 function );

		return( -1 );
	}
	if( libsystem_path_create(
	     table_name,
	     table_name_length,
	     export_path,
	     export_path_length,
	     &index_directory_name,
	     &index_directory_name_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create index directory.",
		 function );

		goto on_error;
	}
	result = libsystem_file_exists(
	          index_directory_name,
	          error );

	if( result == -1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_GENERIC,
		 "%s: unable to determine if %" PRIs_LIBCSTRING_SYSTEM " exists.",
		 function,
		 index_directory_name );

		goto on_error;
	}
	else if( result == 1 )
	{
		log_handle_printf(
		 log_handle,
		 "Skipping indexes they already exists.\n" );

		memory_free(
		 index_directory_name );

		return( 1 );
	}
	if( libsystem_directory_make(
	     index_directory_name,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_WRITE_FAILED,
		 "%s: unable to make directory: %" PRIs_LIBCSTRING_SYSTEM ".",
		 function,
		 index_directory_name );

		goto on_error;
	}
	log_handle_printf(
	 log_handle,
	 "Created directory: %" PRIs_LIBCSTRING_SYSTEM ".\n",
	 index_directory_name );

	memory_free(
	 index_directory_name );

	index_directory_name = NULL;

	if( libesedb_table_get_number_of_indexes(
	     table,
	     &number_of_indexes,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of indexes.",
		 function );

		goto on_error;
	}
	/* Ignore index 1 because it is a build-in index that points to the table
	 */
	for( index_iterator = 1;
	     index_iterator < number_of_indexes;
	     index_iterator++ )
	{
		if( libesedb_table_get_index(
		     table,
		     index_iterator,
		     &index,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve index: %d.",
			 function,
			 index_iterator + 1 );

			goto on_error;
		}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
		result = libesedb_index_get_utf16_name_size(
			  index,
			  &index_name_size,
			  error );
#else
		result = libesedb_index_get_utf8_name_size(
			  index,
			  &index_name_size,
			  error );
#endif
		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve the size of the index name.",
			 function );

			goto on_error;
		}
		if( index_name_size == 0 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing index name.",
			 function );

			goto on_error;
		}
		index_name = libcstring_system_string_allocate(
		              index_name_size );

		if( index_name == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create index name string.",
			 function );

			goto on_error;
		}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
		result = libesedb_index_get_utf16_name(
			  index,
			  (uint16_t *) index_name,
			  index_name_size,
			  error );
#else
		result = libesedb_index_get_utf8_name(
			  index,
			  (uint8_t *) index_name,
			  index_name_size,
			  error );
#endif
		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve the index name.",
			 function );

			goto on_error;
		}
		fprintf(
		 export_handle->notify_stream,
		 "Exporting index %d (%" PRIs_LIBCSTRING_SYSTEM ")",
		 index_iterator + 1,
		 index_name );

		/* TODO want to change this to an export index name or refactor */
		if( index_name == NULL )
		{
			fprintf(
			 export_handle->notify_stream,
			 " out of %d",
			 number_of_indexes );
		}
		fprintf(
		 export_handle->notify_stream,
		 ".\n" );

		if( libsystem_path_sanitize_filename(
		     index_name,
		     &index_name_size,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable sanitize index name.",
			 function );

			goto on_error;
		}
		if( export_handle_export_index(
		     export_handle,
		     index,
		     index_iterator,
		     index_name,
		     index_name_size - 1,
		     export_path,
		     export_path_length,
		     log_handle,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GENERIC,
			 "%s: unable to export index: %d.",
			 function,
			 index_iterator );

			goto on_error;
		}
		memory_free(
		 index_name );

		index_name = NULL;

		if( libesedb_index_free(
		     &index,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free index.",
			 function );

			goto on_error;
		}
	}
	return( 1 );

on_error:
	if( index_name != NULL )
	{
		memory_free(
		 index_name );
	}
	if( index != NULL )
	{
		libesedb_index_free(
		 &index,
		 NULL );
	}
	if( index_directory_name != NULL )
	{
		memory_free(
		 index_directory_name );
	}
	return( -1 );
}

/* Exports the index
 * Returns 1 if successful or -1 on error
 */
int export_handle_export_index(
     export_handle_t *export_handle,
     libesedb_index_t *index,
     int index_iterator,
     const libcstring_system_character_t *index_name,
     size_t index_name_length,
     const libcstring_system_character_t *export_path,
     size_t export_path_length,
     log_handle_t *log_handle,
     liberror_error_t **error )
{
	libcstring_system_character_t *item_filename = NULL;
	libesedb_record_t *record                    = NULL;
	FILE *index_file_stream                      = NULL;
	static char *function                        = "export_handle_export_index";
	int known_index                              = 0;
	size_t item_filename_size                    = 0;
	int number_of_records                        = 0;
	int record_iterator                          = 0;
	int result                                   = 0;

	if( index == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid index.",
		 function );

		return( -1 );
	}
	if( index_name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid index name.",
		 function );

		return( -1 );
	}
	if( export_handle_create_item_filename(
	     export_handle,
	     index_iterator,
	     index_name,
	     index_name_length,
	     &item_filename,
	     &item_filename_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create item filename.",
		 function );

		goto on_error;
	}
	result = export_handle_create_text_item_file(
	          export_handle,
	          item_filename,
	          item_filename_size - 1,
	          export_path,
	          export_path_length,
	          &index_file_stream,
	          error );

	if( result == -1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create index file.",
		 function );

		goto on_error;
	}
	else if( result == 0 )
	{
		log_handle_printf(
		 log_handle,
		 "Skipping index: %" PRIs_LIBCSTRING_SYSTEM " it already exists.\n",
		 item_filename );

		memory_free(
		 item_filename );

		return( 1 );
	}
	memory_free(
	 item_filename );

	item_filename = NULL;

#ifdef TODO
	/* Write the column names to the index file
	 */
	if( libesedb_index_get_number_of_columns(
	     index,
	     &number_of_columns,
	     0,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of columns.",
		 function );

		goto on_error;
	}
	for( column_iterator = 0;
	     column_iterator < number_of_columns;
	     column_iterator++ )
	{
		if( libesedb_index_get_column(
		     index,
		     column_iterator,
		     &column,
		     0,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve column: %d.",
			 function,
			 column_iterator );

			goto on_error;
		}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
		result = libesedb_column_get_utf16_name_size(
		          column,
		          &value_string_size,
		          error );
#else
		result = libesedb_column_get_utf8_name_size(
		          column,
		          &value_string_size,
		          error );
#endif
		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve the size of the column name.",
			 function );

			goto on_error;
		}
		if( value_string_size == 0 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing column name.",
			 function );

			goto on_error;
		}
		value_string = libcstring_system_string_allocate(
		                value_string_size );

		if( value_string == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create column name string.",
			 function );

			goto on_error;
		}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
		result = libesedb_column_get_utf16_name(
		          column,
		          (uint16_t *) value_string,
		          value_string_size,
		          error );
#else
		result = libesedb_column_get_utf8_name(
		          column,
		          (uint8_t *) value_string,
		          value_string_size,
		          error );
#endif
		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve the column name.",
			 function );

			goto on_error;
		}
		fprintf(
		 index_file_stream,
		 "%" PRIs_LIBCSTRING_SYSTEM "",
		 value_string );

		memory_free(
		 value_string );

		value_string = NULL;

		if( libesedb_column_free(
		     &column,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free column.",
			 function );

			goto on_error;
		}
		if( column_iterator == ( number_of_columns - 1 ) )
		{
			fprintf(
			 index_file_stream,
			 "\n" );
		}
		else
		{
			fprintf(
			 index_file_stream,
			 "\t" );
		}
	}
#endif
	/* Write the record (row) values to the index file
	 */
	if( libesedb_index_get_number_of_records(
	     index,
	     &number_of_records,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of records.",
		 function );

		goto on_error;
	}
	for( record_iterator = 0;
	     record_iterator < number_of_records;
	     record_iterator++ )
	{
		if( libesedb_index_get_record(
		     index,
		     record_iterator,
		     &record,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve record: %d.",
			 function,
			 record_iterator );

			goto on_error;
		}
		known_index = 0;

		if( known_index == 0 )
		{
			result = export_handle_export_record(
			          record,
			          index_file_stream,
			          log_handle,
			          error );
		}
		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GENERIC,
			 "%s: unable to export record.",
			 function );

			goto on_error;
		}
		if( libesedb_record_free(
		     &record,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free record.",
			 function );

			goto on_error;
		}
	}
	if( libsystem_file_stream_close(
	     index_file_stream ) != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_CLOSE_FAILED,
		 "%s: unable to close index file.",
		 function );

		goto on_error;
	}
	index_file_stream = NULL;

	return( 1 );

on_error:
	if( record != NULL )
	{
		libesedb_record_free(
		 &record,
		 NULL );
	}
#ifdef TODO
	if( value_string != NULL )
	{
		memory_free(
		 value_string );
	}
	if( column != NULL )
	{
		libesedb_column_free(
		 &column,
		 NULL );
	}
#endif
	if( index_file_stream != NULL )
	{
		libsystem_file_stream_close(
		 index_file_stream );
	}
	if( item_filename != NULL )
	{
		memory_free(
		 item_filename );
	}
	return( -1 );
}

/* Exports the values in a record
 * Returns 1 if successful or -1 on error
 */
int export_handle_export_record(
     libesedb_record_t *record,
     FILE *record_file_stream,
     log_handle_t *log_handle,
     liberror_error_t **error )
{
	static char *function = "export_handle_export_record";
	int number_of_values  = 0;
	int value_iterator    = 0;

	if( record == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid record.",
		 function );

		return( -1 );
	}
	if( record_file_stream == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid record file stream.",
		 function );

		return( -1 );
	}
	if( libesedb_record_get_number_of_values(
	     record,
	     &number_of_values,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of values.",
		 function );

		return( -1 );
	}
	for( value_iterator = 0;
	     value_iterator < number_of_values;
	     value_iterator++ )
	{
		if( export_handle_export_record_value(
		     record,
		     value_iterator,
		     record_file_stream,
		     log_handle,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GENERIC,
			 "%s: unable to export record value: %d.",
			 function,
			 value_iterator );

			return( -1 );
		}
		if( value_iterator == ( number_of_values - 1 ) )
		{
			fprintf(
			 record_file_stream,
			 "\n" );
		}
		else
		{
			fprintf(
			 record_file_stream,
			 "\t" );
		}
	}
	return( 1 );
}

/* Exports a record value
 * Returns 1 if successful or -1 on error
 */
int export_handle_export_record_value(
     libesedb_record_t *record,
     int record_value_entry,
     FILE *record_file_stream,
     log_handle_t *log_handle,
     liberror_error_t **error )
{
	libcstring_system_character_t filetime_string[ 32 ];

	libcstring_system_character_t *value_string = NULL;
        libesedb_long_value_t *long_value           = NULL;
        libesedb_multi_value_t *multi_value         = NULL;
	libfdatetime_filetime_t *filetime           = NULL;
	uint8_t *binary_data                        = NULL;
	uint8_t *value_data                         = NULL;
	static char *function                       = "export_handle_export_record_value";
	size_t binary_data_size                     = 0;
	size_t value_data_size                      = 0;
	size_t value_string_size                    = 0;
	double value_double                         = 0.0;
	float value_float                           = 0.0;
	uint64_t value_64bit                        = 0;
	uint32_t column_identifier                  = 0;
	uint32_t column_type                        = 0;
	uint32_t value_32bit                        = 0;
	uint16_t value_16bit                        = 0;
	uint8_t value_8bit                          = 0;
	uint8_t value_flags                         = 0;
	int long_value_segment_iterator             = 0;
	int multi_value_iterator                    = 0;
	int number_of_long_value_segments           = 0;
	int number_of_multi_values                  = 0;
	int result                                  = 0;

	if( record == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid record.",
		 function );

		return( -1 );
	}
	if( record_file_stream == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid record file stream.",
		 function );

		return( -1 );
	}
	if( libesedb_record_get_column_identifier(
	     record,
	     record_value_entry,
	     &column_identifier,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve column identifier of value: %d.",
		 function,
		 record_value_entry );

		return( -1 );
	}
	if( libesedb_record_get_column_type(
	     record,
	     record_value_entry,
	     &column_type,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve column type of value: %d.",
		 function,
		 record_value_entry );

		return( -1 );
	}
	if( libesedb_record_get_value(
	     record,
	     record_value_entry,
	     &value_data,
	     &value_data_size,
	     &value_flags,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value: %d.",
		 function,
		 record_value_entry );

		return( -1 );
	}
	if( ( value_flags & ~( LIBESEDB_VALUE_FLAG_VARIABLE_SIZE ) ) == 0 )
	{
		switch( column_type )
		{
			case LIBESEDB_COLUMN_TYPE_BOOLEAN:
				result = libesedb_record_get_value_boolean(
					  record,
					  record_value_entry,
					  &value_8bit,
					  error );

				if( result == -1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve boolean value: %d.",
					 function,
					 record_value_entry );

					return( -1 );
				}
				else if( result != 0 )
				{
					if( value_8bit == 0 )
					{
						fprintf(
						 record_file_stream,
						 "false" );
					}
					else
					{
						fprintf(
						 record_file_stream,
						 "true" );
					}
				}
				break;

			case LIBESEDB_COLUMN_TYPE_INTEGER_8BIT_UNSIGNED:
				result = libesedb_record_get_value_8bit(
					  record,
					  record_value_entry,
					  &value_8bit,
					  error );

				if( result == -1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve 8-bit value: %d.",
					 function,
					 record_value_entry );

					return( -1 );
				}
				else if( result != 0 )
				{
					fprintf(
					 record_file_stream,
					 "%" PRIu8 "",
					 value_8bit );
				}
				break;

			case LIBESEDB_COLUMN_TYPE_INTEGER_16BIT_SIGNED:
			case LIBESEDB_COLUMN_TYPE_INTEGER_16BIT_UNSIGNED:
				result = libesedb_record_get_value_16bit(
					  record,
					  record_value_entry,
					  &value_16bit,
					  error );

				if( result == -1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve 16-bit value: %d.",
					 function,
					 record_value_entry );

					return( -1 );
				}
				else if( result != 0 )
				{
					if( column_type == LIBESEDB_COLUMN_TYPE_INTEGER_16BIT_SIGNED )
					{
						fprintf(
						 record_file_stream,
						 "%" PRIi16 "",
						 (int16_t) value_16bit );
					}
					else
					{
						fprintf(
						 record_file_stream,
						 "%" PRIu16 "",
						 value_16bit );
					}
				}
				break;

			case LIBESEDB_COLUMN_TYPE_INTEGER_32BIT_SIGNED:
			case LIBESEDB_COLUMN_TYPE_INTEGER_32BIT_UNSIGNED:
				result = libesedb_record_get_value_32bit(
					  record,
					  record_value_entry,
					  &value_32bit,
					  error );

				if( result == -1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve 32-bit value: %d.",
					 function,
					 record_value_entry );

					return( -1 );
				}
				else if( result != 0 )
				{
					if( column_type == LIBESEDB_COLUMN_TYPE_INTEGER_32BIT_SIGNED )
					{
						fprintf(
						 record_file_stream,
						 "%" PRIi32 "",
						 (int32_t) value_32bit );
					}
					else
					{
						fprintf(
						 record_file_stream,
						 "%" PRIu32 "",
						 value_32bit );
					}
				}
				break;

			case LIBESEDB_COLUMN_TYPE_CURRENCY:
			case LIBESEDB_COLUMN_TYPE_INTEGER_64BIT_SIGNED:
				result = libesedb_record_get_value_64bit(
					  record,
					  record_value_entry,
					  &value_64bit,
					  error );

				if( result == -1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve 64-bit value: %d.",
					 function,
					 record_value_entry );

					return( -1 );
				}
				else if( result != 0 )
				{
					if( column_type == LIBESEDB_COLUMN_TYPE_INTEGER_64BIT_SIGNED )
					{ 
						fprintf(
						 record_file_stream,
						 "%" PRIi64 "",
						 (int64_t) value_64bit );
					}
					else
					{
						fprintf(
						 record_file_stream,
						 "%" PRIu64 "",
						 value_64bit );
					}
				}
				break;

			case LIBESEDB_COLUMN_TYPE_DATE_TIME:
				result = libesedb_record_get_value_filetime(
					  record,
					  record_value_entry,
					  &value_64bit,
					  error );

				if( result == -1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve filetime value: %d.",
					 function,
					 record_value_entry );

					return( -1 );
				}
				else if( result != 0 )
				{
					if( libfdatetime_filetime_initialize(
					     &filetime,
					     error ) != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
						 "%s: unable to create filetime.",
						 function );

						return( -1 );
					}
					if( libfdatetime_filetime_copy_from_64bit(
					     filetime,
					     value_64bit,
					     error ) != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
						 "%s: unable to copy filetime from 64-bit value.",
						 function );

						libfdatetime_filetime_free(
						 &filetime,
						 NULL );

						return( -1 );
					}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
					result = libfdatetime_filetime_copy_to_utf16_string(
					          filetime,
					          (uint16_t *) filetime_string,
					          32,
					          LIBFDATETIME_STRING_FORMAT_FLAG_DATE_TIME_MICRO_SECONDS,
					          LIBFDATETIME_DATE_TIME_FORMAT_CTIME,
					          error );
#else
					result = libfdatetime_filetime_copy_to_utf8_string(
					          filetime,
					          (uint8_t *) filetime_string,
					          32,
					          LIBFDATETIME_STRING_FORMAT_FLAG_DATE_TIME_MICRO_SECONDS,
					          LIBFDATETIME_DATE_TIME_FORMAT_CTIME,
					          error );
#endif
					if( result != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
						 "%s: unable to copy filetime to string.",
						 function );

						libfdatetime_filetime_free(
						 &filetime,
						 NULL );

						return( -1 );
					}
					if( libfdatetime_filetime_free(
					     &filetime,
					     error ) != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
						 "%s: unable to free filetime.",
						 function );

						return( -1 );
					}
					fprintf(
					 record_file_stream,
					 "%" PRIs_LIBCSTRING_SYSTEM "",
					 filetime_string );
				}
				break;

			case LIBESEDB_COLUMN_TYPE_FLOAT_32BIT:
				result = libesedb_record_get_value_floating_point_32bit(
				          record,
				          record_value_entry,
				          &value_float,
				          error );

				if( result == -1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve single precision floating point value: %d.",
					 function,
					 record_value_entry );

					return( -1 );
				}
				else if( result != 0 )
				{
					fprintf(
					 record_file_stream,
					 "%f",
					 value_float );
				}
				break;

			case LIBESEDB_COLUMN_TYPE_DOUBLE_64BIT:
				result = libesedb_record_get_value_floating_point_64bit(
				          record,
				          record_value_entry,
				          &value_double,
				          error );

				if( result == -1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve double precision floating point value: %d.",
					 function,
					 record_value_entry );

					return( -1 );
				}
				else if( result != 0 )
				{
					fprintf(
					 record_file_stream,
					 "%f",
					 value_double );
				}
				break;

			case LIBESEDB_COLUMN_TYPE_TEXT:
			case LIBESEDB_COLUMN_TYPE_LARGE_TEXT:
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
				result = libesedb_record_get_value_utf16_string_size(
					  record,
					  record_value_entry,
					  &value_string_size,
					  error );
#else
				result = libesedb_record_get_value_utf8_string_size(
					  record,
					  record_value_entry,
					  &value_string_size,
					  error );
#endif

				if( result == -1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve size of value string: %d (%" PRIu32 ").",
					 function,
					 record_value_entry,
					 column_identifier );

					return( -1 );
				}
				if( result != 0 )
				{
					if( value_string_size == 0 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
						 "%s: missing value string.",
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
					result = libesedb_record_get_value_utf16_string(
					          record,
					          record_value_entry,
					          (uint16_t *) value_string,
					          value_string_size,
					          error );
#else
					result = libesedb_record_get_value_utf8_string(
					          record,
					          record_value_entry,
					          (uint8_t *) value_string,
					          value_string_size,
					          error );
#endif
					if( result != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_GET_FAILED,
						 "%s: unable to retrieve value string: %d.",
						 function,
						 record_value_entry );

						memory_free(
						 value_string );

						return( -1 );
					}
					export_text(
					 value_string,
					 value_string_size,
					 record_file_stream );

					memory_free(
					 value_string );
				}
				break;

			default:
				export_binary_data(
				 value_data,
				 value_data_size,
				 record_file_stream );

				break;
		}
	}
	else if( ( ( value_flags & LIBESEDB_VALUE_FLAG_COMPRESSED ) != 0 )
	      && ( ( value_flags & LIBESEDB_VALUE_FLAG_MULTI_VALUE ) == 0 ) )
	{
		switch( column_type )
		{
			case LIBESEDB_COLUMN_TYPE_LARGE_TEXT:
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
				result = libesedb_record_get_value_utf16_string_size(
					  record,
					  record_value_entry,
					  &value_string_size,
					  error );
#else
				result = libesedb_record_get_value_utf8_string_size(
					  record,
					  record_value_entry,
					  &value_string_size,
					  error );
#endif

				if( result == -1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve size of value string: %d (%" PRIu32 ").",
					 function,
					 record_value_entry,
					 column_identifier );

					return( -1 );
				}
				if( result != 0 )
				{
					if( value_string_size == 0 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
						 "%s: missing value string.",
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
					result = libesedb_record_get_value_utf16_string(
					          record,
					          record_value_entry,
					          (uint16_t *) value_string,
					          value_string_size,
					          error );
#else
					result = libesedb_record_get_value_utf8_string(
					          record,
					          record_value_entry,
					          (uint8_t *) value_string,
					          value_string_size,
					          error );
#endif
					if( result != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_GET_FAILED,
						 "%s: unable to retrieve value string: %d.",
						 function,
						 record_value_entry );

						memory_free(
						 value_string );

						return( -1 );
					}
					export_text(
					 value_string,
					 value_string_size,
					 record_file_stream );

					memory_free(
					 value_string );
				}
				break;

			case LIBESEDB_COLUMN_TYPE_LARGE_BINARY_DATA:
				result = libesedb_record_get_value_binary_data_size(
					  record,
					  record_value_entry,
					  &binary_data_size,
					  error );

				if( result == -1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve size of binary data: %d (%" PRIu32 ").",
					 function,
					 record_value_entry,
					 column_identifier );

					return( -1 );
				}
				if( result != 0 )
				{
					if( binary_data_size == 0 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
						 "%s: missing binary data.",
						 function );

						return( -1 );
					}
					binary_data = (uint8_t *) memory_allocate(
					                           sizeof( uint8_t ) * binary_data_size  );

					if( binary_data == NULL )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_MEMORY,
						 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
						 "%s: unable to create binary data.",
						 function );

						return( -1 );
					}
					result = libesedb_record_get_value_binary_data(
					          record,
					          record_value_entry,
					          binary_data,
					          binary_data_size,
					          error );

					if( result != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_GET_FAILED,
						 "%s: unable to retrieve binary data: %d.",
						 function,
						 record_value_entry );

						memory_free(
						 binary_data );

						return( -1 );
					}
					export_binary_data(
					 binary_data,
					 binary_data_size,
					 record_file_stream );

					memory_free(
					 binary_data );
				}
				break;

			default:
				export_binary_data(
				 value_data,
				 value_data_size,
				 record_file_stream );

				break;
		}
	}
	else if( ( ( value_flags & LIBESEDB_VALUE_FLAG_LONG_VALUE ) != 0 )
	      && ( ( value_flags & LIBESEDB_VALUE_FLAG_MULTI_VALUE ) == 0 ) )
	{
		result = libesedb_record_get_long_value(
		          record,
		          record_value_entry,
		          &long_value,
		          error );

		if( result != 1 )
		{
			log_handle_printf(
			 log_handle,
			 "Unable to retrieve long value of record entry: %d.\n",
			 record_value_entry );

			if( libsystem_notify_verbose != 0 )
			{
				libsystem_notify_printf(
				 "%s: unable to retrieve long value of record entry: %d.",
				 function,
				 record_value_entry );
			}
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve long value of record entry: %d.",
			 function,
			 record_value_entry );

#if defined( HAVE_DEBUG_OUTPUT )
			if( ( error != NULL )
			 && ( *error != NULL ) )
			{
				libsystem_notify_print_error_backtrace(
				 *error );
			}
#endif
			liberror_error_free(
			 error );
		}
		else
		{
			if( libesedb_long_value_get_number_of_segments(
			     long_value,
			     &number_of_long_value_segments,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve number of long value segments.",
				 function );

				libesedb_long_value_free(
				 &long_value,
				 NULL );

				return( -1 );
			}
			for( long_value_segment_iterator = 0;
			     long_value_segment_iterator < number_of_long_value_segments;
			     long_value_segment_iterator++ )
			{
				if( libesedb_long_value_get_segment_data(
				     long_value,
				     long_value_segment_iterator,
				     &value_data,
				     &value_data_size,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve long value segment: %d of record entry: %d.",
					 function,
					 long_value_segment_iterator,
					 record_value_entry );

					libesedb_long_value_free(
					 &long_value,
					 NULL );

					return( -1 );
				}
				if( value_data != NULL )
				{
#if defined( HAVE_DEBUG_OUTPUT ) && defined( LONG_VALUE_TEST )
libsystem_notify_printf(
 "LONG VALUE DATA: %d out of %d\n",
 long_value_segment_iterator + 1,
 number_of_long_value_segments );
libsystem_notify_print_data(
 value_data,
 value_data_size );
#endif
				}
			}
			if( libesedb_long_value_free(
			     &long_value,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free long value.",
				 function );

				return( -1 );
			}

			if( libesedb_long_value_free(
			     &long_value,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free long value.",
				 function );

				return( -1 );
			}
		}
	}
	/* TODO handle 0x10 flags */
	else if( ( ( value_flags & LIBESEDB_VALUE_FLAG_MULTI_VALUE ) != 0 )
	      && ( ( value_flags & LIBESEDB_VALUE_FLAG_LONG_VALUE ) == 0 )
	      && ( ( value_flags & 0x10 ) == 0 ) )
	{
		/* TODO what about non string multi values ?
		 */
		if( libesedb_record_get_multi_value(
		     record,
		     record_value_entry,
		     &multi_value,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve multi value of record entry: %d.",
			 function,
			 record_value_entry );

			return( -1 );
		}
		if( libesedb_multi_value_get_number_of_values(
		     multi_value,
		     &number_of_multi_values,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve number of multi values.",
			 function );

			libesedb_multi_value_free(
			 &multi_value,
			 NULL );

			return( -1 );
		}
		for( multi_value_iterator = 0;
	 	     multi_value_iterator < number_of_multi_values;
		     multi_value_iterator++ )
		{
			if( libesedb_multi_value_get_value(
			     multi_value,
			     multi_value_iterator,
			     &column_type,
			     &value_data,
			     &value_data_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve multi value: %d of record entry: %d.",
				 function,
				 multi_value_iterator,
				 record_value_entry );

				return( -1 );
			}
			if( value_data != NULL )
			{
				if( ( column_type == LIBESEDB_COLUMN_TYPE_TEXT )
				 || ( column_type == LIBESEDB_COLUMN_TYPE_LARGE_TEXT ) )
				{
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
					result = libesedb_multi_value_get_value_utf16_string_size(
						  multi_value,
						  multi_value_iterator,
						  &value_string_size,
						  error );
#else
					result = libesedb_multi_value_get_value_utf8_string_size(
						  multi_value,
						  multi_value_iterator,
						  &value_string_size,
						  error );
#endif

					if( result == -1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_GET_FAILED,
						 "%s: unable to retrieve size of string of multi value: %d of record entry: %d (%" PRIu32 ").",
						 function,
						 multi_value_iterator,
						 record_value_entry,
						 column_identifier );

						libesedb_multi_value_free(
						 &multi_value,
						 NULL );

						return( -1 );
					}
					else if( result != 0 )
					{
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

							libesedb_multi_value_free(
							 &multi_value,
							 NULL );

							return( -1 );
						}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
						result = libesedb_multi_value_get_value_utf16_string(
						          multi_value,
						          multi_value_iterator,
						          (uint16_t *) value_string,
						          value_string_size,
						          error );
#else
						result = libesedb_multi_value_get_value_utf8_string(
						          multi_value,
						          multi_value_iterator,
						          (uint8_t *) value_string,
						          value_string_size,
						          error );
#endif
						if( result != 1 )
						{
							liberror_error_set(
							 error,
							 LIBERROR_ERROR_DOMAIN_RUNTIME,
							 LIBERROR_RUNTIME_ERROR_GET_FAILED,
							 "%s: unable to retrieve string of multi value: %d of record entry: %d.",
							 function,
							 multi_value_iterator,
							 record_value_entry );

							memory_free(
							 value_string );
							libesedb_multi_value_free(
							 &multi_value,
							 NULL );

							return( -1 );
						}
						export_text(
						 value_string,
						 value_string_size,
						 record_file_stream );

						memory_free(
						 value_string );
					}
					if( multi_value_iterator < ( number_of_multi_values - 1 ) )
					{
						fprintf(
						 record_file_stream,
						 "; " );
					}
				}
				else
				{
					export_binary_data(
					 value_data,
					 value_data_size,
					 record_file_stream );
				}
			}
		}
		if( libesedb_multi_value_free(
		     &multi_value,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free multi value: %d.",
			 function,
			 multi_value_iterator );

			return( -1 );
		}
	}
	else
	{
		export_binary_data(
		 value_data,
		 value_data_size,
		 record_file_stream );
	}
	return( 1 );
}

/* Exports the items in the file
 * Returns the 1 if succesful, 0 if no items are available or -1 on error
 */
int export_handle_export_file(
     export_handle_t *export_handle,
     const libcstring_system_character_t *export_table_name,
     size_t export_table_name_length,
     log_handle_t *log_handle,
     liberror_error_t **error )
{
	libcstring_system_character_t *table_name = NULL;
	libesedb_table_t *table                   = NULL;
	static char *function                     = "export_handle_export_file";
	size_t table_name_size                    = 0;
	int number_of_tables                      = 0;
	int result                                = 0;
	int table_index                           = 0;

	if( export_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid export handle.",
		 function );

		return( -1 );
	}
	if( libesedb_file_get_number_of_tables(
	     export_handle->input_file,
	     &number_of_tables,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of tables.",
		 function );

		goto on_error;
	}
	if( number_of_tables == 0 )
	{
		return( 0 );
	}
	if( libsystem_directory_make(
	     export_handle->items_export_path,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_WRITE_FAILED,
		 "%s: unable to make directory: %" PRIs_LIBCSTRING_SYSTEM ".",
		 function,
		 export_handle->items_export_path );

		goto on_error;
	}
	log_handle_printf(
	 log_handle,
	 "Created directory: %" PRIs_LIBCSTRING_SYSTEM ".\n",
	 export_handle->items_export_path );

	for( table_index = 0;
	     table_index < number_of_tables;
	     table_index++ )
	{
		if( libesedb_file_get_table(
		     export_handle->input_file,
		     table_index,
		     &table,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve table: %d.",
			 function,
			 table_index );

			goto on_error;
		}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
		result = libesedb_table_get_utf16_name_size(
		          table,
		          &table_name_size,
		          error );
#else
		result = libesedb_table_get_utf8_name_size(
		          table,
		          &table_name_size,
		          error );
#endif
		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve the size of the table name.",
			 function );

			goto on_error;
		}
		if( table_name_size == 0 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing table name.",
			 function );

			goto on_error;
		}
		table_name = libcstring_system_string_allocate(
		              table_name_size );

		if( table_name == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create table name string.",
			 function );

			goto on_error;
		}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
		result = libesedb_table_get_utf16_name(
		          table,
		          (uint16_t *) table_name,
		          table_name_size,
		          error );
#else
		result = libesedb_table_get_utf8_name(
		          table,
		          (uint8_t *) table_name,
		          table_name_size,
		          error );
#endif
		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve the table name.",
			 function );

			goto on_error;
		}
		if( ( export_table_name == NULL )
		 || ( ( table_name_size == ( export_table_name_length + 1 ) )
		   && ( libcstring_system_string_compare(
		         table_name,
		         export_table_name,
		         export_table_name_length ) == 0 ) ) )
		{
			fprintf(
			 export_handle->notify_stream,
			 "Exporting table %d (%" PRIs_LIBCSTRING_SYSTEM ")",
			 table_index + 1,
			 table_name );

			if( export_table_name == NULL )
			{
				fprintf(
				 export_handle->notify_stream,
				 " out of %d",
				 number_of_tables );
			}
			fprintf(
			 export_handle->notify_stream,
			 ".\n" );

			if( libsystem_path_sanitize_filename(
			     table_name,
			     &table_name_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable sanitize table name.",
				 function );

				goto on_error;
			}
			if( export_handle_export_table(
			     export_handle,
			     table,
			     table_index,
			     table_name,
			     table_name_size - 1,
			     export_handle->items_export_path,
			     export_handle->items_export_path_size - 1,
			     log_handle,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GENERIC,
				 "%s: unable to export table: %d.",
				 function,
				 table_index );

				goto on_error;
			}
		}
		memory_free(
		 table_name );

		table_name = NULL;

		if( libesedb_table_free(
		     &table,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free table: %d.",
			 function,
			 table_index );

			goto on_error;
		}
	}
	return( 1 );

on_error:
	if( table_name != NULL )
	{
		memory_free(
		 table_name );
	}
	if( table != NULL )
	{
		libesedb_table_free(
		 &table,
		 NULL );
	}
	return( -1 );
}

