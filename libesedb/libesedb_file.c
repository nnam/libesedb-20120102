/*
 * File functions
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

#include <libcstring.h>
#include <liberror.h>
#include <libnotify.h>

#include "libesedb_catalog.h"
#include "libesedb_database.h"
#include "libesedb_debug.h"
#include "libesedb_definitions.h"
#include "libesedb_i18n.h"
#include "libesedb_io_handle.h"
#include "libesedb_file.h"
#include "libesedb_libbfio.h"
#include "libesedb_libfcache.h"
#include "libesedb_libfdata.h"
#include "libesedb_page.h"
#include "libesedb_table.h"
#include "libesedb_table_definition.h"

/* Initializes a file
 * Make sure the value file is pointing to is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libesedb_file_initialize(
     libesedb_file_t **file,
     liberror_error_t **error )
{
	libesedb_internal_file_t *internal_file = NULL;
	static char *function                   = "libesedb_file_initialize";

	if( file == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	if( *file != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid file value already set.",
		 function );

		return( -1 );
	}
	internal_file = memory_allocate_structure(
	                 libesedb_internal_file_t );

	if( internal_file == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create file.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_file,
	     0,
	     sizeof( libesedb_internal_file_t ) ) == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear file.",
		 function );

		memory_free(
		 internal_file );

		return( -1 );
	}
	if( libesedb_io_handle_initialize(
	     &( internal_file->io_handle ),
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create IO handle.",
		 function );

		goto on_error;
	}
	if( libesedb_i18n_initialize(
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to initalize internationalization (i18n).",
		 function );

		goto on_error;
	}
	*file = (libesedb_file_t *) internal_file;

	return( 1 );

on_error:
	if( internal_file != NULL )
	{
		if( internal_file->io_handle != NULL )
		{
			libesedb_io_handle_free(
			 &( internal_file->io_handle ),
			 NULL );
		}
		memory_free(
		 internal_file );
	}
	return( -1 );
}

/* Frees a file
 * Returns 1 if successful or -1 on error
 */
int libesedb_file_free(
     libesedb_file_t **file,
     liberror_error_t **error )
{
	libesedb_internal_file_t *internal_file = NULL;
	static char *function                   = "libesedb_file_free";
	int result                              = 1;

	if( file == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	if( *file != NULL )
	{
		internal_file = (libesedb_internal_file_t *) *file;

		if( internal_file->file_io_handle != NULL )
		{
			if( libesedb_file_close(
			     *file,
			     error ) != 0 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_IO,
				 LIBERROR_IO_ERROR_CLOSE_FAILED,
				 "%s: unable to close file.",
				 function );

				result = -1;
			}
		}
		*file = NULL;

		if( libesedb_io_handle_free(
		     &( internal_file->io_handle ),
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
		memory_free(
		 internal_file );
	}
	return( result );
}

/* Signals a file to abort its current activity
 * Returns 1 if successful or -1 on error
 */
int libesedb_file_signal_abort(
     libesedb_file_t *file,
     liberror_error_t **error )
{
	static char *function = "libesedb_file_signal_abort";

	if( file == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	( (libesedb_internal_file_t *) file )->abort = 1;

	return( 1 );
}

/* Opens a file
 * Returns 1 if successful or -1 on error
 */
int libesedb_file_open(
     libesedb_file_t *file,
     const char *filename,
     int access_flags,
     liberror_error_t **error )
{
	libbfio_handle_t *file_io_handle        = NULL;
	libesedb_internal_file_t *internal_file = NULL;
	static char *function                   = "libesedb_file_open";

	if( file == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	if( filename == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid filename.",
		 function );

		return( -1 );
	}
	if( ( ( access_flags & LIBESEDB_ACCESS_FLAG_READ ) == 0 )
	 && ( ( access_flags & LIBESEDB_ACCESS_FLAG_WRITE ) == 0 ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported access flags.",
		 function );

		return( -1 );
	}
	if( ( access_flags & LIBESEDB_ACCESS_FLAG_WRITE ) != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: write access currently not supported.",
		 function );

		return( -1 );
	}
	internal_file = (libesedb_internal_file_t *) file;

	if( libbfio_file_initialize(
	     &file_io_handle,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create file IO handle.",
		 function );

		goto on_error;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libbfio_handle_set_track_offsets_read(
	     file_io_handle,
	     1,
	     error ) != 1 )
	{
                liberror_error_set(
                 error,
                 LIBERROR_ERROR_DOMAIN_RUNTIME,
                 LIBERROR_RUNTIME_ERROR_SET_FAILED,
                 "%s: unable to set track offsets read in file IO handle.",
                 function );

		goto on_error;
	}
#endif
	if( libbfio_file_set_name(
	     file_io_handle,
	     filename,
	     libcstring_narrow_string_length(
	      filename ) + 1,
	     error ) != 1 )
	{
                liberror_error_set(
                 error,
                 LIBERROR_ERROR_DOMAIN_RUNTIME,
                 LIBERROR_RUNTIME_ERROR_SET_FAILED,
                 "%s: unable to set filename in file IO handle.",
                 function );

		goto on_error;
	}
	if( libesedb_file_open_file_io_handle(
	     file,
	     file_io_handle,
	     access_flags,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_OPEN_FAILED,
		 "%s: unable to open file: %s.",
		 function,
		 filename );

		goto on_error;
	}
	internal_file->file_io_handle_created_in_library = 1;

	return( 1 );

on_error:
	if( file_io_handle != NULL )
	{
		libbfio_handle_free(
		 &file_io_handle,
		 NULL );
	}
	return( -1 );
}

#if defined( HAVE_WIDE_CHARACTER_TYPE )

/* Opens a file
 * Returns 1 if successful or -1 on error
 */
int libesedb_file_open_wide(
     libesedb_file_t *file,
     const wchar_t *filename,
     int access_flags,
     liberror_error_t **error )
{
	libbfio_handle_t *file_io_handle        = NULL;
	libesedb_internal_file_t *internal_file = NULL;
	static char *function                   = "libesedb_file_open_wide";

	if( file == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	if( filename == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid filename.",
		 function );

		return( -1 );
	}
	if( ( ( access_flags & LIBESEDB_ACCESS_FLAG_READ ) == 0 )
	 && ( ( access_flags & LIBESEDB_ACCESS_FLAG_WRITE ) == 0 ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported access flags.",
		 function );

		return( -1 );
	}
	if( ( access_flags & LIBESEDB_ACCESS_FLAG_WRITE ) != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: write access currently not supported.",
		 function );

		return( -1 );
	}
	internal_file = (libesedb_internal_file_t *) file;

	if( libbfio_file_initialize(
	     &file_io_handle,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create file IO handle.",
		 function );

		goto on_error;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libbfio_handle_set_track_offsets_read(
	     file_io_handle,
	     1,
	     error ) != 1 )
	{
                liberror_error_set(
                 error,
                 LIBERROR_ERROR_DOMAIN_RUNTIME,
                 LIBERROR_RUNTIME_ERROR_SET_FAILED,
                 "%s: unable to set track offsets read in file IO handle.",
                 function );

		goto on_error;
	}
#endif
	if( libbfio_file_set_name_wide(
	     file_io_handle,
	     filename,
	     libcstring_wide_string_length(
	      filename ) + 1,
	     error ) != 1 )
	{
                liberror_error_set(
                 error,
                 LIBERROR_ERROR_DOMAIN_RUNTIME,
                 LIBERROR_RUNTIME_ERROR_SET_FAILED,
                 "%s: unable to set filename in file IO handle.",
                 function );

		goto on_error;
	}
	if( libesedb_file_open_file_io_handle(
	     file,
	     file_io_handle,
	     access_flags,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_OPEN_FAILED,
		 "%s: unable to open file: %ls.",
		 function,
		 filename );

		goto on_error;
	}
	internal_file->file_io_handle_created_in_library = 1;

	return( 1 );

on_error:
	if( file_io_handle != NULL )
	{
		libbfio_handle_free(
		 &file_io_handle,
		 NULL );
	}
	return( -1 );
}

#endif

/* Opens a file using a Basic File IO (bfio) handle
 * Returns 1 if successful or -1 on error
 */
int libesedb_file_open_file_io_handle(
     libesedb_file_t *file,
     libbfio_handle_t *file_io_handle,
     int access_flags,
     liberror_error_t **error )
{
	libesedb_internal_file_t *internal_file = NULL;
	static char *function                   = "libesedb_file_open_file_io_handle";
	int bfio_access_flags                   = 0;
	int file_io_handle_is_open              = 0;

	if( file == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	internal_file = (libesedb_internal_file_t *) file;

	if( internal_file->file_io_handle != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid internal file - file IO handle already set.",
		 function );

		return( -1 );
	}
	if( file_io_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file IO handle.",
		 function );

		return( -1 );
	}
	if( ( ( access_flags & LIBESEDB_ACCESS_FLAG_READ ) == 0 )
	 && ( ( access_flags & LIBESEDB_ACCESS_FLAG_WRITE ) == 0 ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported access flags.",
		 function );

		return( -1 );
	}
	if( ( access_flags & LIBESEDB_ACCESS_FLAG_WRITE ) != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: write access currently not supported.",
		 function );

		return( -1 );
	}
	if( ( access_flags & LIBESEDB_ACCESS_FLAG_READ ) != 0 )
	{
		bfio_access_flags = LIBBFIO_ACCESS_FLAG_READ;
	}
	internal_file->file_io_handle = file_io_handle;

	file_io_handle_is_open = libbfio_handle_is_open(
	                          internal_file->file_io_handle,
	                          error );

	if( file_io_handle_is_open == -1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_OPEN_FAILED,
		 "%s: unable to open file.",
		 function );

		return( -1 );
	}
	else if( file_io_handle_is_open == 0 )
	{
		if( libbfio_handle_open(
		     internal_file->file_io_handle,
		     bfio_access_flags,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_OPEN_FAILED,
			 "%s: unable to open file IO handle.",
			 function );

			return( -1 );
		}
	}
	if( libesedb_file_open_read(
	     internal_file,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read from file handle.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Closes a file
 * Returns 0 if successful or -1 on error
 */
int libesedb_file_close(
     libesedb_file_t *file,
     liberror_error_t **error )
{
	libesedb_internal_file_t *internal_file = NULL;
	static char *function                   = "libesedb_file_close";
	int result                              = 0;

	if( file == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	internal_file = (libesedb_internal_file_t *) file;

	if( internal_file->file_io_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid file - missing file IO handle.",
		 function );

		return( -1 );
	}
	if( internal_file->file_io_handle_created_in_library != 0 )
	{
#if defined( HAVE_DEBUG_OUTPUT )
		if( libnotify_verbose != 0 )
		{
			if( libesedb_debug_print_read_offsets(
			     internal_file->file_io_handle,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_PRINT_FAILED,
				 "%s: unable to print the read offsets.",
				 function );

				result = -1;
			}
		}
#endif
		if( libbfio_handle_close(
		     internal_file->file_io_handle,
		     error ) != 0 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_CLOSE_FAILED,
			 "%s: unable to close file IO handle.",
			 function );

			result = -1;
		}
		if( libbfio_handle_free(
		     &( internal_file->file_io_handle ),
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free file IO handle.",
			 function );

			result = -1;
		}
	}
	internal_file->file_io_handle                    = NULL;
	internal_file->file_io_handle_created_in_library = 0;

	if( libfdata_vector_free(
	     &( internal_file->pages_vector ),
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to free pages vector.",
		 function );

		result = -1;
	}
	if( libfcache_cache_free(
	     &( internal_file->pages_cache ),
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to free pages cache.",
		 function );

		result = -1;
	}
	if( libesedb_database_free(
	     &( internal_file->database ),
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to free database.",
		 function );

		result = -1;
	}
	if( libesedb_catalog_free(
	     &( internal_file->catalog ),
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to free catalog.",
		 function );

		result = -1;
	}
	return( result );
}

/* Opens a file for reading
 * Returns 1 if successful or -1 on error
 */
int libesedb_file_open_read(
     libesedb_internal_file_t *internal_file,
     liberror_error_t **error )
{
	static char *function = "libesedb_file_open_read";
	off64_t file_offset   = 0;
	size64_t file_size    = 0;
	int result            = 0;

	if( internal_file == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid internal file.",
		 function );

		return( -1 );
	}
	if( internal_file->io_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid internal file - missing IO handle.",
		 function );

		return( -1 );
	}
	if( internal_file->pages_vector != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid internal file - pages vector already set.",
		 function );

		return( -1 );
	}
	if( internal_file->pages_cache != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid internal file - pages cache already set.",
		 function );

		return( -1 );
	}
	if( internal_file->database != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid internal file - database already set.",
		 function );

		return( -1 );
	}
	if( internal_file->catalog != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid internal file - catalog already set.",
		 function );

		return( -1 );
	}
	if( libbfio_handle_get_size(
	     internal_file->file_io_handle,
	     &file_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve file size.",
		 function );

		goto on_error;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libnotify_verbose != 0 )
	{
		libnotify_printf(
		 "Reading (database) file header:\n" );
	}
#endif
	if( libesedb_io_handle_read_file_header(
	     internal_file->io_handle,
	     internal_file->file_io_handle,
	     0,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read (database) file header.",
		 function );

		goto on_error;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libnotify_verbose != 0 )
	{
		libnotify_printf(
		 "Reading backup (database) file header:\n" );
	}
#endif
	if( internal_file->io_handle->page_size != 0 )
	{
		result = libesedb_io_handle_read_file_header(
		          internal_file->io_handle,
		          internal_file->file_io_handle,
		          internal_file->io_handle->page_size,
		          error );
	}
	if( result != 1 )
	{
		file_offset = 0x0800;

		while( file_offset <= 0x8000 )
		{
#if defined( HAVE_DEBUG_OUTPUT )
			if( ( libnotify_verbose != 0 )
			 && ( error != NULL )
			 && ( *error != NULL ) )
			{
				libnotify_print_error_backtrace(
				 *error );
			}
#endif
			liberror_error_free(
			 error );

			result = libesedb_io_handle_read_file_header(
				  internal_file->io_handle,
				  internal_file->file_io_handle,
				  file_offset,
				  error );

			if( result == 1 )
			{
				break;
			}
			file_offset <<= 1;
		}
	}
	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read backup (database) file header.",
		 function );

		goto on_error;
	}
	if( internal_file->io_handle->format_version != 0x620 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported format version: 0x%04" PRIx32 ".",
		 function,
		 internal_file->io_handle->format_version );

		goto on_error;
	}
	if( internal_file->io_handle->page_size == 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid page size.",
		 function );

		goto on_error;
	}
	if( internal_file->io_handle->format_version == 0x620 )
	{
		if( internal_file->io_handle->format_revision < 0x11 )
		{
			if( ( internal_file->io_handle->page_size != 0x1000 )
			 && ( internal_file->io_handle->page_size != 0x2000 ) )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
				 "%s: unsupported page size: %" PRIu32 " (0x%04" PRIx32 ") for format version: 0x%" PRIx32 " revision: 0x%" PRIx32 ".",
				 function,
				 internal_file->io_handle->page_size,
				 internal_file->io_handle->page_size,
				 internal_file->io_handle->format_version,
				 internal_file->io_handle->format_revision );

				goto on_error;
			}
		}
		else
		{
			if( ( internal_file->io_handle->page_size != 0x0800 )
			 && ( internal_file->io_handle->page_size != 0x1000 )
			 && ( internal_file->io_handle->page_size != 0x2000 )
			 && ( internal_file->io_handle->page_size != 0x4000 )
			 && ( internal_file->io_handle->page_size != 0x8000 ) )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
				 "%s: unsupported page size: %" PRIu32 " (0x%04" PRIx32 ") for format version: 0x%" PRIx32 " revision: 0x%" PRIx32 ".",
				 function,
				 internal_file->io_handle->page_size,
				 internal_file->io_handle->page_size,
				 internal_file->io_handle->format_version,
				 internal_file->io_handle->format_revision );

				goto on_error;
			}
		}
	}
	if( libesedb_io_handle_set_pages_data_range(
	     internal_file->io_handle,
	     file_size,
	     error ) != 1 )
	{
                liberror_error_set(
                 error,
                 LIBERROR_ERROR_DOMAIN_RUNTIME,
                 LIBERROR_RUNTIME_ERROR_SET_FAILED,
                 "%s: unable to set pages data range in IO handle.",
                 function );

		goto on_error;
	}
	/* TODO clone function ? */
	if( libfdata_vector_initialize(
	     &( internal_file->pages_vector ),
	     (size64_t) internal_file->io_handle->page_size,
	     (intptr_t *) internal_file->io_handle,
	     NULL,
	     NULL,
	     &libesedb_io_handle_read_page,
	     LIBFDATA_FLAG_IO_HANDLE_NON_MANAGED,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create pages vector.",
		 function );

		goto on_error;
	}
	if( libfdata_vector_append_segment(
	     internal_file->pages_vector,
	     internal_file->io_handle->pages_data_offset,
	     internal_file->io_handle->pages_data_size,
	     0,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to append segment to pages vector.",
		 function );

		goto on_error;
	}
	if( libfcache_cache_initialize(
	     &( internal_file->pages_cache ),
	     LIBESEDB_MAXIMUM_CACHE_ENTRIES_PAGES,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create pages cache.",
		 function );

		goto on_error;
	}
	if( internal_file->io_handle->file_type == LIBESEDB_FILE_TYPE_DATABASE )
	{
#if defined( HAVE_DEBUG_OUTPUT )
		if( libnotify_verbose != 0 )
		{
			libnotify_printf(
			 "Reading the database:\n" );
		}
#endif
		if( libesedb_database_initialize(
		     &( internal_file->database ),
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create database.",
			 function );

			goto on_error;
		}
		if( libesedb_database_read(
		     internal_file->database,
		     internal_file->file_io_handle,
		     internal_file->io_handle,
		     internal_file->pages_vector,
		     internal_file->pages_cache,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read database.",
			 function );

			goto on_error;
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libnotify_verbose != 0 )
		{
			libnotify_printf(
			 "Reading the catalog:\n" );
		}
#endif
		if( libesedb_catalog_initialize(
		     &( internal_file->catalog ),
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create catalog.",
			 function );

			goto on_error;
		}
		if( libesedb_catalog_read(
		     internal_file->catalog,
		     internal_file->file_io_handle,
		     internal_file->io_handle,
		     internal_file->pages_vector,
		     internal_file->pages_cache,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read catalog.",
			 function );

			goto on_error;
		}
		/* TODO what about the backup of the catalog */
	}
	return( 1 );

on_error:
	if( internal_file->pages_vector != NULL )
	{
		libfdata_vector_free(
		 &( internal_file->pages_vector ),
		 NULL );
	}
	if( internal_file->pages_cache != NULL )
	{
		libfcache_cache_free(
		 &( internal_file->pages_cache ),
		 NULL );
	}
	if( internal_file->database != NULL )
	{
		libesedb_database_free(
		 &( internal_file->database ),
		 NULL );
	}
	if( internal_file->catalog != NULL )
	{
		libesedb_catalog_free(
		 &( internal_file->catalog ),
		 NULL );
	}
	return( -1 );
}

/* Retrieves the file type
 * Returns 1 if successful or -1 on error
 */
int libesedb_file_get_type(
     libesedb_file_t *file,
     uint32_t *type,
     liberror_error_t **error )
{
	libesedb_internal_file_t *internal_file = NULL;
	static char *function                   = "libesedb_file_get_type";

	if( file == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	internal_file = (libesedb_internal_file_t *) file;

	if( internal_file->io_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid internal file - missing IO handle.",
		 function );

		return( -1 );
	}
	if( type == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid type.",
		 function );

		return( -1 );
	}
	*type = internal_file->io_handle->file_type;

	return( 1 );
}

/* Retrieves the file (current) version
 * Returns 1 if successful or -1 on error
 */
int libesedb_file_get_format_version(
     libesedb_file_t *file,
     uint32_t *format_version,
     uint32_t *format_revision,
     liberror_error_t **error )
{
	libesedb_internal_file_t *internal_file = NULL;
	static char *function                   = "libesedb_file_get_format_version";

	if( file == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	internal_file = (libesedb_internal_file_t *) file;

	if( internal_file->io_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid internal file - missing IO handle.",
		 function );

		return( -1 );
	}
	if( format_version == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid format version.",
		 function );

		return( -1 );
	}
	if( format_revision == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid format revision.",
		 function );

		return( -1 );
	}
	*format_version  = internal_file->io_handle->format_version;
	*format_revision = internal_file->io_handle->format_revision;

	return( 1 );
}

/* Retrieves the file creation format version
 * Returns 1 if successful or -1 on error
 */
int libesedb_file_get_creation_format_version(
     libesedb_file_t *file,
     uint32_t *format_version,
     uint32_t *format_revision,
     liberror_error_t **error )
{
	libesedb_internal_file_t *internal_file = NULL;
	static char *function                   = "libesedb_file_get_creation_format_version";

	if( file == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	internal_file = (libesedb_internal_file_t *) file;

	if( internal_file->io_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid internal file - missing IO handle.",
		 function );

		return( -1 );
	}
	if( format_version == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid format version.",
		 function );

		return( -1 );
	}
	if( format_revision == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid format revision.",
		 function );

		return( -1 );
	}
	*format_version  = internal_file->io_handle->creation_format_version;
	*format_revision = internal_file->io_handle->creation_format_revision;

	return( 1 );
}

/* Retrieves the file page size
 * Returns 1 if successful or -1 on error
 */
int libesedb_file_get_page_size(
     libesedb_file_t *file,
     uint32_t *page_size,
     liberror_error_t **error )
{
	libesedb_internal_file_t *internal_file = NULL;
	static char *function                   = "libesedb_file_get_page_size";

	if( file == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	internal_file = (libesedb_internal_file_t *) file;

	if( internal_file->io_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid internal file - missing IO handle.",
		 function );

		return( -1 );
	}
	if( page_size == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid page size.",
		 function );

		return( -1 );
	}
	*page_size = internal_file->io_handle->page_size;

	return( 1 );
}

/* Retrieves the number of tables
 * Returns 1 if successful or -1 on error
 */
int libesedb_file_get_number_of_tables(
     libesedb_file_t *file,
     int *number_of_tables,
     liberror_error_t **error )
{
	libesedb_internal_file_t *internal_file = NULL;
	static char *function                   = "libesedb_file_get_number_of_tables";

	if( file == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	internal_file = (libesedb_internal_file_t *) file;

	if( internal_file->catalog == NULL )
	{
		if( number_of_tables == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
			 "%s: invalid number of tables.",
			 function );

			return( -1 );
		}
		*number_of_tables = 0;
	}
	else
	{
		if( libesedb_catalog_get_number_of_table_definitions(
		     internal_file->catalog,
		     number_of_tables,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve number of tables.",
			 function );

			return( -1 );
		}
	}
	return( 1 );
}

/* Retrieves a specific table
 * Returns 1 if successful or -1 on error
 */
int libesedb_file_get_table(
     libesedb_file_t *file,
     int table_entry,
     libesedb_table_t **table,
     liberror_error_t **error )
{
	libesedb_internal_file_t *internal_file                = NULL;
	libesedb_table_definition_t *table_definition          = NULL;
	libesedb_table_definition_t *template_table_definition = NULL;
	static char *function                                  = "libesedb_file_get_table";

	if( file == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	internal_file = (libesedb_internal_file_t *) file;

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
	if( *table != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid table value already set.",
		 function );

		return( -1 );
	}
	if( libesedb_catalog_get_table_definition_by_index(
	     internal_file->catalog,
	     table_entry,
	     &table_definition,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve table definition: %d.",
		 function,
		 table_entry );

		return( -1 );
	}
	if( table_definition == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing table definition.",
		 function );

		return( -1 );
	}
	if( table_definition->table_catalog_definition == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid table definition - missing table catalog definition.",
		 function );

		return( -1 );
	}
	if( table_definition->table_catalog_definition->template_name != NULL )
	{
		if( libesedb_catalog_get_table_definition_by_utf8_name(
		     internal_file->catalog,
		     table_definition->table_catalog_definition->template_name,
		     table_definition->table_catalog_definition->template_name_size,
		     &template_table_definition,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve template table definition.",
			 function );

			return( -1 );
		}
	}
	if( libesedb_table_initialize(
	     table,
	     internal_file->file_io_handle,
	     internal_file->io_handle,
	     table_definition,
	     template_table_definition,
	     LIBESEDB_ITEM_FLAGS_DEFAULT,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create table.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the table for the UTF-8 encoded name
 * Returns 1 if successful, 0 if no table could be found or -1 on error
 */
int libesedb_file_get_table_by_utf8_name(
     libesedb_file_t *file,
     const uint8_t *utf8_string,
     size_t utf8_string_length,
     libesedb_table_t **table,
     liberror_error_t **error )
{
	libesedb_internal_file_t *internal_file                = NULL;
	libesedb_table_definition_t *table_definition          = NULL;
	libesedb_table_definition_t *template_table_definition = NULL;
	static char *function                                  = "libesedb_file_get_table_by_utf8_name";
	int result                                             = 0;

	if( file == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	internal_file = (libesedb_internal_file_t *) file;

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
	if( *table != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid table value already set.",
		 function );

		return( -1 );
	}
	result = libesedb_catalog_get_table_definition_by_utf8_name(
	          internal_file->catalog,
	          utf8_string,
	          utf8_string_length,
	          &table_definition,
	          error );

	if( result == -1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve table definition.",
		 function );

		return( -1 );
	}
	else if( result != 0 )
	{
		if( table_definition == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing table definition.",
			 function );

			return( -1 );
		}
		if( table_definition->table_catalog_definition == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid table definition - missing table catalog definition.",
			 function );

			return( -1 );
		}
		if( table_definition->table_catalog_definition->template_name != NULL )
		{
			if( libesedb_catalog_get_table_definition_by_name(
			     internal_file->catalog,
			     table_definition->table_catalog_definition->template_name,
			     table_definition->table_catalog_definition->template_name_size,
			     &template_table_definition,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve template table definition.",
				 function );

				return( -1 );
			}
		}
		if( libesedb_table_initialize(
		     table,
		     internal_file->file_io_handle,
		     internal_file->io_handle,
		     table_definition,
		     template_table_definition,
		     LIBESEDB_ITEM_FLAGS_DEFAULT,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create table.",
			 function );

			return( -1 );
		}
	}
	return( result );
}

/* Retrieves the table for the UTF-16 encoded name
 * Returns 1 if successful, 0 if no table could be found or -1 on error
 */
int libesedb_file_get_table_by_utf16_name(
     libesedb_file_t *file,
     const uint16_t *utf16_string,
     size_t utf16_string_length,
     libesedb_table_t **table,
     liberror_error_t **error )
{
	libesedb_internal_file_t *internal_file                = NULL;
	libesedb_table_definition_t *table_definition          = NULL;
	libesedb_table_definition_t *template_table_definition = NULL;
	static char *function                                  = "libesedb_file_get_table_by_utf16_name";
	int result                                             = 0;

	if( file == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file.",
		 function );

		return( -1 );
	}
	internal_file = (libesedb_internal_file_t *) file;

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
	if( *table != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid table value already set.",
		 function );

		return( -1 );
	}
	result = libesedb_catalog_get_table_definition_by_utf16_name(
	          internal_file->catalog,
	          utf16_string,
	          utf16_string_length,
	          &table_definition,
	          error );

	if( result == -1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve table definition.",
		 function );

		return( -1 );
	}
	else if( result != 0 )
	{
		if( table_definition == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing table definition.",
			 function );

			return( -1 );
		}
		if( table_definition->table_catalog_definition == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid table definition - missing table catalog definition.",
			 function );

			return( -1 );
		}
		if( table_definition->table_catalog_definition->template_name != NULL )
		{
			if( libesedb_catalog_get_table_definition_by_name(
			     internal_file->catalog,
			     table_definition->table_catalog_definition->template_name,
			     table_definition->table_catalog_definition->template_name_size,
			     &template_table_definition,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve template table definition.",
				 function );

				return( -1 );
			}
		}
		if( libesedb_table_initialize(
		     table,
		     internal_file->file_io_handle,
		     internal_file->io_handle,
		     table_definition,
		     template_table_definition,
		     LIBESEDB_ITEM_FLAGS_DEFAULT,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create table.",
			 function );

			return( -1 );
		}
	}
	return( result );
}

