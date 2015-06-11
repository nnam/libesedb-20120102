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

#if defined( HAVE_ERRNO_H ) || defined( WINAPI )
#include <errno.h>
#endif

#if defined( HAVE_SYS_STAT_H ) || defined( WINAPI )
#include <sys/stat.h>
#endif

#if defined( HAVE_FCNTL_H ) || defined( WINAPI )
#include <fcntl.h>
#endif

#if defined( HAVE_UNISTD_H )
#include <unistd.h>
#endif

#if defined( HAVE_GLIB_H )
#include <glib.h>
#include <glib/gstdio.h>
#endif

#if defined( WINAPI ) && defined( USE_CRT_FUNCTIONS )
#include <io.h>
#include <share.h>
#endif

#include "libbfio_definitions.h"
#include "libbfio_error_string.h"
#include "libbfio_file.h"
#include "libbfio_handle.h"
#include "libbfio_libuna.h"
#include "libbfio_path.h"
#include "libbfio_types.h"

/* Initializes the file IO handle
 * Returns 1 if successful or -1 on error
 */
int libbfio_file_io_handle_initialize(
     libbfio_file_io_handle_t **file_io_handle,
     liberror_error_t **error )
{
	static char *function = "libbfio_file_io_handle_initialize";

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
	if( *file_io_handle != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid file IO handle value already set.",
		 function );

		return( -1 );
	}
	*file_io_handle = memory_allocate_structure(
	                   libbfio_file_io_handle_t );

	if( *file_io_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create file IO handle.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *file_io_handle,
	     0,
	     sizeof( libbfio_file_io_handle_t ) ) == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear file IO handle.",
		 function );

		goto on_error;
	}
#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	( *file_io_handle )->file_handle     = INVALID_HANDLE_VALUE;
#else
	( *file_io_handle )->file_descriptor = -1;
#endif

	return( 1 );

on_error:
	if( *file_io_handle != NULL )
	{
		memory_free(
		 *file_io_handle );

		*file_io_handle = NULL;
	}
	return( -1 );
}

/* Initializes the file handle
 * Returns 1 if successful or -1 on error
 */
int libbfio_file_initialize(
     libbfio_handle_t **handle,
     liberror_error_t **error )
{
	libbfio_file_io_handle_t *file_io_handle = NULL;
	static char *function                    = "libbfio_file_initialize";

	if( handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid handle.",
		 function );

		return( -1 );
	}
	if( *handle != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid handle value already set.",
		 function );

		return( -1 );
	}
	if( libbfio_file_io_handle_initialize(
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
	if( libbfio_handle_initialize(
	     handle,
	     (intptr_t *) file_io_handle,
	     (int (*)(intptr_t **, liberror_error_t **)) libbfio_file_io_handle_free,
	     (int (*)(intptr_t **, intptr_t *, liberror_error_t **)) libbfio_file_io_handle_clone,
	     (int (*)(intptr_t *, int, liberror_error_t **)) libbfio_file_open,
	     (int (*)(intptr_t *, liberror_error_t **)) libbfio_file_close,
	     (ssize_t (*)(intptr_t *, uint8_t *, size_t, liberror_error_t **)) libbfio_file_read,
	     (ssize_t (*)(intptr_t *, const uint8_t *, size_t, liberror_error_t **)) libbfio_file_write,
	     (off64_t (*)(intptr_t *, off64_t, int, liberror_error_t **)) libbfio_file_seek_offset,
	     (int (*)(intptr_t *, liberror_error_t **)) libbfio_file_exists,
	     (int (*)(intptr_t *, liberror_error_t **)) libbfio_file_is_open,
	     (int (*)(intptr_t *, size64_t *, liberror_error_t **)) libbfio_file_get_size,
	     LIBBFIO_FLAG_IO_HANDLE_MANAGED | LIBBFIO_FLAG_IO_HANDLE_CLONE_BY_FUNCTION,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create handle.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( file_io_handle != NULL )
	{
		libbfio_file_io_handle_free(
		 &file_io_handle,
		 NULL );
	}
	return( -1 );
}

/* Frees the file IO handle and its attributes
 * Returns 1 if succesful or -1 on error
 */
int libbfio_file_io_handle_free(
     libbfio_file_io_handle_t **file_io_handle,
     liberror_error_t **error )
{
	static char *function = "libbfio_file_io_handle_free";

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
	if( *file_io_handle != NULL )
	{
		if( ( *file_io_handle )->name != NULL )
		{
			memory_free(
			 ( *file_io_handle )->name );
		}
		memory_free(
		 *file_io_handle );

		*file_io_handle = NULL;
	}
	return( 1 );
}

/* Clones (duplicates) the file IO handle and its attributes
 * Returns 1 if succesful or -1 on error
 */
int libbfio_file_io_handle_clone(
     libbfio_file_io_handle_t **destination_file_io_handle,
     libbfio_file_io_handle_t *source_file_io_handle,
     liberror_error_t **error )
{
	static char *function = "libbfio_file_io_handle_clone";

	if( destination_file_io_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid destination file IO handle.",
		 function );

		return( -1 );
	}
	if( *destination_file_io_handle != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: destination file IO handle already set.",
		 function );

		return( -1 );
	}
	if( source_file_io_handle == NULL )
	{
		*destination_file_io_handle = NULL;

		return( 1 );
	}
	if( libbfio_file_io_handle_initialize(
	     destination_file_io_handle,
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
	if( *destination_file_io_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing destination file IO handle.",
		 function );

		goto on_error;
	}
	if( source_file_io_handle->name_size > 0 )
	{
		( *destination_file_io_handle )->name = libcstring_system_string_allocate(
		                                         source_file_io_handle->name_size );

		if( ( *destination_file_io_handle )->name == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create name.",
			 function );

			goto on_error;
		}
		if( source_file_io_handle->name_size > 1 )
		{
			if( libcstring_system_string_copy(
			     ( *destination_file_io_handle )->name,
			     source_file_io_handle->name,
			     source_file_io_handle->name_size ) == NULL )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_MEMORY,
				 LIBERROR_MEMORY_ERROR_COPY_FAILED,
				 "%s: unable to copy name.",
				 function );

				goto on_error;
			}
		}
		( *destination_file_io_handle )->name[ source_file_io_handle->name_size - 1 ] = 0;

		( *destination_file_io_handle )->name_size = source_file_io_handle->name_size;
	}
	return( 1 );

on_error:
	if( *destination_file_io_handle != NULL )
	{
		libbfio_file_io_handle_free(
		 destination_file_io_handle,
		 NULL );
	}
	return( -1 );
}

/* Retrieves the name size of the file handle
 * The name size includes the end of string character
 * Returns 1 if succesful or -1 on error
 */
int libbfio_file_get_name_size(
     libbfio_handle_t *handle,
     size_t *name_size,
     liberror_error_t **error )
{
	libbfio_internal_handle_t *internal_handle = NULL;
	static char *function                      = "libbfio_file_get_name_size";

	if( handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid handle.",
		 function );

		return( -1 );
	}
	internal_handle = (libbfio_internal_handle_t *) handle;

	if( libbfio_file_io_handle_get_name_size(
	     (libbfio_file_io_handle_t *) internal_handle->io_handle,
	     name_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve name size.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the name of the file handle
 * The name size should include the end of string character
 * Returns 1 if succesful or -1 on error
 */
int libbfio_file_get_name(
     libbfio_handle_t *handle,
     char *name,
     size_t name_size,
     liberror_error_t **error )
{
	libbfio_internal_handle_t *internal_handle = NULL;
	static char *function                      = "libbfio_file_get_name";

	if( handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid handle.",
		 function );

		return( -1 );
	}
	internal_handle = (libbfio_internal_handle_t *) handle;

	if( libbfio_file_io_handle_get_name(
	     (libbfio_file_io_handle_t *) internal_handle->io_handle,
	     name,
	     name_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve name.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Sets the name for the file handle
 * Returns 1 if succesful or -1 on error
 */
int libbfio_file_set_name(
     libbfio_handle_t *handle,
     const char *name,
     size_t name_length,
     liberror_error_t **error )
{
	libbfio_internal_handle_t *internal_handle = NULL;
	char *full_name                            = NULL;
	static char *function                      = "libbfio_file_set_name";
	size_t full_name_size                      = 0;

	if( handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid handle.",
		 function );

		return( -1 );
	}
	internal_handle = (libbfio_internal_handle_t *) handle;

	if( libbfio_path_get_full_path(
	     name,
	     name_length,
	     &full_name,
	     &full_name_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to determine full path.",
		 function );

		goto on_error;
	}
	if( libbfio_file_io_handle_set_name(
	     (libbfio_file_io_handle_t *) internal_handle->io_handle,
	     full_name,
	     full_name_size - 1,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set name.",
		 function );

		goto on_error;
	}
	if( ( full_name != NULL )
	 && ( full_name != name ) )
	{
		memory_free(
		 full_name );
	}
	return( 1 );

on_error:
	if( ( full_name != NULL )
	 && ( full_name != name ) )
	{
		memory_free(
		 full_name );
	}
	return( -1 );
}

/* Retrieves the name size of the file IO handle
 * The name size includes the end of string character
 * Returns 1 if succesful or -1 on error
 */
int libbfio_file_io_handle_get_name_size(
     libbfio_file_io_handle_t *file_io_handle,
     size_t *name_size,
     liberror_error_t **error )
{
	static char *function = "libbfio_file_io_handle_get_name_size";

#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	int result            = 0;
#endif

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
	if( file_io_handle->name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid file IO handle - missing name.",
		 function );

		return( -1 );
	}
	if( name_size == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid name size.",
		 function );

		return( -1 );
	}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	if( libcstring_narrow_system_string_codepage == 0 )
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf8_string_size_from_utf32(
		          (libuna_utf32_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          name_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf8_string_size_from_utf16(
		          (libuna_utf16_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          name_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	else
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_byte_stream_size_from_utf32(
		          (libuna_utf32_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          libcstring_narrow_system_string_codepage,
		          name_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_byte_stream_size_from_utf16(
		          (libuna_utf16_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          libcstring_narrow_system_string_codepage,
		          name_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to determine name size.",
		 function );

		return( -1 );
	}
#else
	*name_size = file_io_handle->name_size;
#endif /* defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER ) */

	return( 1 );
}

/* Retrieves the name of the file IO handle
 * The name size should include the end of string character
 * Returns 1 if succesful or -1 on error
 */
int libbfio_file_io_handle_get_name(
     libbfio_file_io_handle_t *file_io_handle,
     char *name,
     size_t name_size,
     liberror_error_t **error )
{
	static char *function   = "libbfio_file_io_handle_get_name";
	size_t narrow_name_size = 0;

#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	int result              = 0;
#endif

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
	if( file_io_handle->name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid file IO handle - missing name.",
		 function );

		return( -1 );
	}
	if( name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid name.",
		 function );

		return( -1 );
	}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	if( libcstring_narrow_system_string_codepage == 0 )
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf8_string_size_from_utf32(
		          (libuna_utf32_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          &narrow_name_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf8_string_size_from_utf16(
		          (libuna_utf16_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          &narrow_name_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	else
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_byte_stream_size_from_utf32(
		          (libuna_utf32_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          libcstring_narrow_system_string_codepage,
		          &narrow_name_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_byte_stream_size_from_utf16(
		          (libuna_utf16_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          libcstring_narrow_system_string_codepage,
		          &narrow_name_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to determine name size.",
		 function );

		return( -1 );
	}
#else
	narrow_name_size = file_io_handle->name_size;
#endif /* defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER ) */

	if( name_size < narrow_name_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: name too small.",
		 function );

		return( -1 );
	}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	if( libcstring_narrow_system_string_codepage == 0 )
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf8_string_copy_from_utf32(
		          (libuna_utf8_character_t *) name,
		          name_size,
		          (libuna_utf32_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf8_string_copy_from_utf16(
		          (libuna_utf8_character_t *) name,
		          name_size,
		          (libuna_utf16_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	else
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_byte_stream_copy_from_utf32(
		          (uint8_t *) name,
		          name_size,
		          libcstring_narrow_system_string_codepage,
		          (libuna_utf32_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_byte_stream_copy_from_utf16(
		          (uint8_t *) name,
		          name_size,
		          libcstring_narrow_system_string_codepage,
		          (libuna_utf16_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to set name.",
		 function );

		return( -1 );
	}
#else
	if( file_io_handle->name_size > 0 )
	{
		if( file_io_handle->name_size > 1 )
		{
			if( libcstring_system_string_copy(
			     name,
			     file_io_handle->name,
			     file_io_handle->name_size ) == NULL )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_MEMORY,
				 LIBERROR_MEMORY_ERROR_COPY_FAILED,
				 "%s: unable to set name.",
				 function );

				return( -1 );
			}
		}
		name[ file_io_handle->name_size - 1 ] = 0;
	}
#endif /* defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER ) */

	return( 1 );
}

/* Sets the name for the file IO handle
 * Returns 1 if succesful or -1 on error
 */
int libbfio_file_io_handle_set_name(
     libbfio_file_io_handle_t *file_io_handle,
     const char *name,
     size_t name_length,
     liberror_error_t **error )
{
	static char *function = "libbfio_file_io_handle_set_name";

#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	int result            = 0;
#endif

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
	if( name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid name.",
		 function );

		return( -1 );
	}
	if( name_length == 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_ZERO_OR_LESS,
		 "%s: invalid name length is zero.",
		 function );

		return( -1 );
	}
	if( name_length >= (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid name length value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( file_io_handle->name != NULL )
	{
#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
		if( file_io_handle->file_handle != INVALID_HANDLE_VALUE )
#else
		if( file_io_handle->file_descriptor != -1 )
#endif
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
			 "%s: name already set: %" PRIs_LIBCSTRING_SYSTEM ".",
			 function,
			 file_io_handle->name );

			return( -1 );
		}
		memory_free(
		  file_io_handle->name );

		 file_io_handle->name      = NULL;
		 file_io_handle->name_size = 0;
	}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	if( libcstring_narrow_system_string_codepage == 0 )
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf32_string_size_from_utf8(
		          (libuna_utf8_character_t *) name,
		          name_length + 1,
		          &( file_io_handle->name_size ),
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf16_string_size_from_utf8(
		          (libuna_utf8_character_t *) name,
		          name_length + 1,
		          &( file_io_handle->name_size ),
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	else
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf32_string_size_from_byte_stream(
		          (uint8_t *) name,
		          name_length + 1,
		          libcstring_narrow_system_string_codepage,
		          &( file_io_handle->name_size ),
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf16_string_size_from_byte_stream(
		          (uint8_t *) name,
		          name_length + 1,
		          libcstring_narrow_system_string_codepage,
		          &( file_io_handle->name_size ),
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to determine name size.",
		 function );

		return( -1 );
	}
#else
	file_io_handle->name_size = name_length + 1;
#endif /* defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER ) */

	file_io_handle->name = libcstring_system_string_allocate(
	                        file_io_handle->name_size );

	if( file_io_handle->name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create name.",
		 function );

		return( -1 );
	}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	if( libcstring_narrow_system_string_codepage == 0 )
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf32_string_copy_from_utf8(
		          (libuna_utf32_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          (libuna_utf8_character_t *) name,
		          name_length + 1,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf16_string_copy_from_utf8(
		          (libuna_utf16_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          (libuna_utf8_character_t *) name,
		          name_length + 1,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	else
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf32_string_copy_from_byte_stream(
		          (libuna_utf32_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          (uint8_t *) name,
		          name_length + 1,
		          libcstring_narrow_system_string_codepage,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf16_string_copy_from_byte_stream(
		          (libuna_utf16_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          (uint8_t *) name,
		          name_length + 1,
		          libcstring_narrow_system_string_codepage,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to set name.",
		 function );

		goto on_error;
	}
#else
	if( name_length > 1 )
	{
		if( libcstring_system_string_copy(
		     file_io_handle->name,
		     name,
		     name_length ) == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_COPY_FAILED,
			 "%s: unable to set name.",
			 function );

			goto on_error;
		}
	}
	file_io_handle->name[ name_length ] = 0;

#endif /* defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER ) */
	return( 1 );

on_error:
	if( file_io_handle->name != NULL )
	{
		memory_free(
		 file_io_handle->name );

		file_io_handle->name      = NULL;
		file_io_handle->name_size = 0;
	}
	return( -1 );
}

#if defined( HAVE_WIDE_CHARACTER_TYPE )

/* Retrieves the name size of the file handle
 * The name size includes the end of string character
 * Returns 1 if succesful or -1 on error
 */
int libbfio_file_get_name_size_wide(
     libbfio_handle_t *handle,
     size_t *name_size,
     liberror_error_t **error )
{
	libbfio_internal_handle_t *internal_handle = NULL;
	static char *function                      = "libbfio_file_get_name_size_wide";

	if( handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid handle.",
		 function );

		return( -1 );
	}
	internal_handle = (libbfio_internal_handle_t *) handle;

	if( libbfio_file_io_handle_get_name_size_wide(
	     (libbfio_file_io_handle_t *) internal_handle->io_handle,
	     name_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve name size.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the name of the file handle
 * The name size should include the end of string character
 * Returns 1 if succesful or -1 on error
 */
int libbfio_file_get_name_wide(
     libbfio_handle_t *handle,
     wchar_t *name,
     size_t name_size,
     liberror_error_t **error )
{
	libbfio_internal_handle_t *internal_handle = NULL;
	static char *function                      = "libbfio_file_get_name_wide";

	if( handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid handle.",
		 function );

		return( -1 );
	}
	internal_handle = (libbfio_internal_handle_t *) handle;

	if( libbfio_file_io_handle_get_name_wide(
	     (libbfio_file_io_handle_t *) internal_handle->io_handle,
	     name,
	     name_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve name.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Sets the name for the file handle
 * Returns 1 if succesful or -1 on error
 */
int libbfio_file_set_name_wide(
     libbfio_handle_t *handle,
     const wchar_t *name,
     size_t name_length,
     liberror_error_t **error )
{
	libbfio_internal_handle_t *internal_handle = NULL;
	wchar_t *full_name                         = NULL;
	static char *function                      = "libbfio_file_set_name_wide";
	size_t full_name_size                      = 0;

	if( handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid handle.",
		 function );

		return( -1 );
	}
	internal_handle = (libbfio_internal_handle_t *) handle;

	if( libbfio_path_get_full_path_wide(
	     name,
	     name_length,
	     &full_name,
	     &full_name_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to determine full path.",
		 function );

		goto on_error;
	}
	if( libbfio_file_io_handle_set_name_wide(
	     (libbfio_file_io_handle_t *) internal_handle->io_handle,
	     full_name,
	     full_name_size - 1,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set name.",
		 function );

		goto on_error;
	}
	if( ( full_name != NULL )
	 && ( full_name != name ) )
	{
		memory_free(
		 full_name );
	}
	return( 1 );

on_error:
	if( ( full_name != NULL )
	 && ( full_name != name ) )
	{
		memory_free(
		 full_name );
	}
	return( -1 );
}

/* Retrieves the name size of the file IO handle
 * The name size includes the end of string character
 * Returns 1 if succesful or -1 on error
 */
int libbfio_file_io_handle_get_name_size_wide(
     libbfio_file_io_handle_t *file_io_handle,
     size_t *name_size,
     liberror_error_t **error )
{
	static char *function = "libbfio_file_io_handle_get_name_size_wide";

#if !defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	int result            = 0;
#endif

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
	if( file_io_handle->name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid handle - invalid IO handle - missing name.",
		 function );

		return( -1 );
	}
	if( name_size == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid name size.",
		 function );

		return( -1 );
	}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	*name_size = file_io_handle->name_size;
#else
	if( libcstring_narrow_system_string_codepage == 0 )
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf32_string_size_from_utf8(
		          (libuna_utf8_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          name_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf16_string_size_from_utf8(
		          (libuna_utf8_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          name_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	else
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf32_string_size_from_byte_stream(
		          (uint8_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          libcstring_narrow_system_string_codepage,
		          name_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf16_string_size_from_byte_stream(
		          (uint8_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          libcstring_narrow_system_string_codepage,
		          name_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to determine name size.",
		 function );

		return( -1 );
	}
#endif /* defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER ) */

	return( 1 );
}

/* Retrieves the name of the file IO handle
 * The name size should include the end of string character
 * Returns 1 if succesful or -1 on error
 */
int libbfio_file_io_handle_get_name_wide(
     libbfio_file_io_handle_t *file_io_handle,
     wchar_t *name,
     size_t name_size,
     liberror_error_t **error )
{
	static char *function = "libbfio_file_io_handle_get_name_wide";
	size_t wide_name_size = 0;

#if !defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	int result            = 0;
#endif

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
	if( file_io_handle->name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid handle - invalid IO handle - missing name.",
		 function );

		return( -1 );
	}
	if( name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid name.",
		 function );

		return( -1 );
	}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	wide_name_size = file_io_handle->name_size;
#else
	if( libcstring_narrow_system_string_codepage == 0 )
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf32_string_size_from_utf8(
		          (libuna_utf8_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          &wide_name_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf16_string_size_from_utf8(
		          (libuna_utf8_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          &wide_name_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	else
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf32_string_size_from_byte_stream(
		          (uint8_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          libcstring_narrow_system_string_codepage,
		          &wide_name_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf16_string_size_from_byte_stream(
		          (uint8_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          libcstring_narrow_system_string_codepage,
		          &wide_name_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to determine name size.",
		 function );

		return( -1 );
	}
#endif /* defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER ) */

	if( name_size < wide_name_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: name too small.",
		 function );

		return( -1 );
	}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	if( file_io_handle->name_size > 0 )
	{
		if( file_io_handle->name_size > 1 )
		{
			if( libcstring_system_string_copy(
			     name,
			     file_io_handle->name,
			     file_io_handle->name_size ) == NULL )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_MEMORY,
				 LIBERROR_MEMORY_ERROR_COPY_FAILED,
				 "%s: unable to set name.",
				 function );

				return( -1 );
			}
		}
		name[ file_io_handle->name_size - 1 ] = 0;
	}
#else
	if( libcstring_narrow_system_string_codepage == 0 )
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf32_string_copy_from_utf8(
		          (libuna_utf32_character_t *) name,
		          name_size,
		          (libuna_utf8_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf16_string_copy_from_utf8(
		          (libuna_utf16_character_t *) name,
		          name_size,
		          (libuna_utf8_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	else
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf32_string_copy_from_byte_stream(
		          (libuna_utf32_character_t *) name,
		          name_size,
		          (uint8_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          libcstring_narrow_system_string_codepage,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf16_string_copy_from_byte_stream(
		          (libuna_utf16_character_t *) name,
		          name_size,
		          (uint8_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          libcstring_narrow_system_string_codepage,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to set name.",
		 function );

		return( -1 );
	}
#endif /* defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER ) */

	return( 1 );
}

/* Sets the name for the file IO handle
 * Returns 1 if succesful or -1 on error
 */
int libbfio_file_io_handle_set_name_wide(
     libbfio_file_io_handle_t *file_io_handle,
     const wchar_t *name,
     size_t name_length,
     liberror_error_t **error )
{
	static char *function = "libbfio_file_io_handle_set_name_wide";

#if !defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	int result            = 0;
#endif

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
	if( name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid name.",
		 function );

		return( -1 );
	}
	if( name_length == 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_ZERO_OR_LESS,
		 "%s: invalid name length is zero.",
		 function );

		return( -1 );
	}
	if( name_length >= (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid name length value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( file_io_handle->name != NULL )
	{
#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
		if( file_io_handle->file_handle != INVALID_HANDLE_VALUE )
#else
		if( file_io_handle->file_descriptor != -1 )
#endif
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
			 "%s: name already set: %" PRIs_LIBCSTRING_SYSTEM ".",
			 function,
			 file_io_handle->name );

			return( -1 );
		}
		memory_free(
		  file_io_handle->name );

		 file_io_handle->name      = NULL;
		 file_io_handle->name_size = 0;
	}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	file_io_handle->name_size = name_length + 1;
#else
	if( libcstring_narrow_system_string_codepage == 0 )
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf8_string_size_from_utf32(
		          (libuna_utf32_character_t *) name,
		          name_length + 1,
		          &( file_io_handle->name_size ),
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf8_string_size_from_utf16(
		          (libuna_utf16_character_t *) name,
		          name_length + 1,
		          &( file_io_handle->name_size ),
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	else
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_byte_stream_size_from_utf32(
		          (libuna_utf32_character_t *) name,
		          name_length + 1,
		          libcstring_narrow_system_string_codepage,
		          &( file_io_handle->name_size ),
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_byte_stream_size_from_utf16(
		          (libuna_utf16_character_t *) name,
		          name_length + 1,
		          libcstring_narrow_system_string_codepage,
		          &( file_io_handle->name_size ),
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to determine name size.",
		 function );

		return( -1 );
	}
#endif /* defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER ) */

	file_io_handle->name = libcstring_system_string_allocate(
	                        file_io_handle->name_size );

	if( file_io_handle->name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create name.",
		 function );

		return( -1 );
	}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	if( name_length > 1 )
	{
		if( libcstring_system_string_copy(
		     file_io_handle->name,
		     name,
		     name_length ) == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_COPY_FAILED,
			 "%s: unable to set name.",
			 function );

			goto on_error;
		}
	}
	file_io_handle->name[ name_length ] = 0;
#else
	if( libcstring_narrow_system_string_codepage == 0 )
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf8_string_copy_from_utf32(
		          (libuna_utf8_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          (libuna_utf32_character_t *) name,
		          name_length + 1,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf8_string_copy_from_utf16(
		          (libuna_utf8_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          (libuna_utf16_character_t *) name,
		          name_length + 1,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	else
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_byte_stream_copy_from_utf32(
		          (uint8_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          libcstring_narrow_system_string_codepage,
		          (libuna_utf32_character_t *) name,
		          name_length + 1,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_byte_stream_copy_from_utf16(
		          (uint8_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          libcstring_narrow_system_string_codepage,
		          (libuna_utf16_character_t *) name,
		          name_length + 1,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to set name.",
		 function );

		goto on_error;
	}
#endif /* defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER ) */

	return( 1 );

on_error:
	if( file_io_handle->name != NULL )
	{
		memory_free(
		 file_io_handle->name );

		file_io_handle->name      = NULL;
		file_io_handle->name_size = 0;
	}
	return( -1 );
}

#endif /* defined( HAVE_WIDE_CHARACTER_TYPE ) */

/* Opens the file handle
 * Returns 1 if successful or -1 on error
 */
int libbfio_file_open(
     libbfio_file_io_handle_t *file_io_handle,
     int access_flags,
     liberror_error_t **error )
{
	libcstring_system_character_t error_string[ LIBBFIO_ERROR_STRING_SIZE ];

	static char *function         = "libbfio_file_open";

#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	DWORD error_code              = 0;
	DWORD file_io_access_flags    = 0;
	DWORD file_io_creation_flags  = 0;
	DWORD file_io_shared_flags    = 0;
#else
#if defined( WINAPI )
	int file_io_shared_flags      = 0;
	int file_io_persmission_flags = 0;
#endif
	int file_io_flags             = 0;
#endif
#if !defined( WINAPI ) && defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	char *narrow_filename         = NULL;
	size_t narrow_filename_size   = 0;
#endif

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
	if( file_io_handle->name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid file IO handle - missing name.",
		 function );

		return( -1 );
	}
#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	if( ( ( access_flags & LIBBFIO_ACCESS_FLAG_READ ) != 0 )
	 && ( ( access_flags & LIBBFIO_ACCESS_FLAG_WRITE ) != 0 ) )
	{
		file_io_access_flags   = GENERIC_WRITE | GENERIC_READ;
		file_io_creation_flags = OPEN_ALWAYS;
		file_io_shared_flags   = FILE_SHARE_READ;
	}
	else if( ( access_flags & LIBBFIO_ACCESS_FLAG_READ ) != 0 )
	{
		file_io_access_flags   = GENERIC_READ;
		file_io_creation_flags = OPEN_EXISTING;
		file_io_shared_flags   = FILE_SHARE_READ;
	}
	else if( ( access_flags & LIBBFIO_ACCESS_FLAG_WRITE ) != 0 )
	{
		file_io_access_flags   = GENERIC_WRITE;
		file_io_creation_flags = OPEN_ALWAYS;
		file_io_shared_flags   = 0;
	}
	else
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported access flags: 0x%02x.",
		 function,
		 access_flags );

		return( -1 );
	}
	if( ( ( access_flags & LIBBFIO_ACCESS_FLAG_WRITE ) != 0 )
	 && ( ( access_flags & LIBBFIO_ACCESS_FLAG_TRUNCATE ) != 0 ) )
	{
		file_io_creation_flags = CREATE_ALWAYS;
	}
	if( file_io_handle->file_handle != INVALID_HANDLE_VALUE )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid file IO handle - file handle already set.",
		 function );

		return( -1 );
	}
	file_io_handle->file_handle = CreateFile(
				       (LPCTSTR) file_io_handle->name,
				       file_io_access_flags,
				       file_io_shared_flags,
				       NULL,
				       file_io_creation_flags,
				       FILE_ATTRIBUTE_NORMAL,
				       NULL );

	if( file_io_handle->file_handle == INVALID_HANDLE_VALUE )
	{
		error_code = GetLastError();

		switch( error_code )
		{
			case ERROR_ACCESS_DENIED:
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_IO,
				 LIBERROR_IO_ERROR_ACCESS_DENIED,
				 "%s: access denied to file: %" PRIs_LIBCSTRING_SYSTEM ".",
				 function,
				 file_io_handle->name );

				break;

			case ERROR_FILE_NOT_FOUND:
			case ERROR_PATH_NOT_FOUND:
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_IO,
				 LIBERROR_IO_ERROR_INVALID_RESOURCE,
				 "%s: no such file: %" PRIs_LIBCSTRING_SYSTEM ".",
				 function,
				 file_io_handle->name );

				break;

			default:
				if( libbfio_error_string_copy_from_error_number(
				     error_string,
				     LIBBFIO_ERROR_STRING_SIZE,
				     error_code,
				     error ) == 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_IO,
					 LIBERROR_IO_ERROR_OPEN_FAILED,
					 "%s: unable to open file: %" PRIs_LIBCSTRING_SYSTEM " with error: %" PRIs_LIBCSTRING_SYSTEM "",
					 function,
					 file_io_handle->name,
					 error_string );
				}
				else
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_IO,
					 LIBERROR_IO_ERROR_OPEN_FAILED,
					 "%s: unable to open file: %" PRIs_LIBCSTRING_SYSTEM ".",
					 function,
					 file_io_handle->name );
				}
				break;
		}
		return( -1 );
	}
#else
	if( ( ( access_flags & LIBBFIO_ACCESS_FLAG_READ ) != 0 )
	 && ( ( access_flags & LIBBFIO_ACCESS_FLAG_WRITE ) != 0 ) )
	{
#if defined( WINAPI )
		file_io_flags             = _O_RDWR | _O_CREAT;
		file_io_persmission_flags = _S_IREAD | _S_IWRITE;
#else
		file_io_flags = O_RDWR | O_CREAT;
#endif
#if defined( _MSC_VER )
		file_io_shared_flags = _SH_DENYWR;
#endif
	}
	else if( ( access_flags & LIBBFIO_ACCESS_FLAG_READ ) != 0 )
	{
#if defined( WINAPI )
		file_io_flags = _O_RDONLY;
#else
		file_io_flags = O_RDONLY;
#endif
#if defined( _MSC_VER )
		file_io_shared_flags = _SH_DENYWR;
#endif
	}
	else if( ( access_flags & LIBBFIO_ACCESS_FLAG_WRITE ) != 0 )
	{
#if defined( WINAPI )
		file_io_flags             = _O_WRONLY | _O_CREAT;
		file_io_persmission_flags = _S_IREAD | _S_IWRITE;
#else
		file_io_flags = O_WRONLY | O_CREAT;
#endif
#if defined( _MSC_VER )
		file_io_shared_flags = _SH_DENYRW;
#endif
	}
	else
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported access flags.",
		 function );

		return( -1 );
	}
	if( ( ( access_flags & LIBBFIO_ACCESS_FLAG_WRITE ) != 0 )
	 && ( ( access_flags & LIBBFIO_ACCESS_FLAG_TRUNCATE ) != 0 ) )
	{
#if defined( WINAPI )
		file_io_flags |= _O_TRUNC;
#else
		file_io_flags |= O_TRUNC;
#endif
	}
	if( file_io_handle->file_descriptor != -1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid file IO handle - file descriptor already set.",
		 function );

		return( -1 );
	}
#if defined( WINAPI )
#if defined( _MSC_VER )
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	if( _wsopen_s(
	     &( file_io_handle->file_descriptor ),
	     (wchar_t *) file_io_handle->name,
	     file_io_flags | _O_BINARY,
	     file_io_shared_flags,
	     file_io_persmission_flags ) != 0 )
#else
	if( _sopen_s(
	     &( file_io_handle->file_descriptor ),
	     (char *) file_io_handle->name,
	     file_io_flags | _O_BINARY,
	     file_io_shared_flags,
	     file_io_persmission_flags ) != 0 )
#endif /* LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER */
#else
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	file_io_handle->file_descriptor = _wsopen(
					   (wchar_t *) file_io_handle->name,
					   file_io_flags | _O_BINARY,
					   file_io_persmission_flags );
#else
	file_io_handle->file_descriptor = _sopen(
					   (char *) file_io_handle->name,
					   file_io_flags | _O_BINARY,
					   file_io_persmission_flags );
#endif /* LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER */

	if( file_io_handle->file_descriptor == -1 )
#endif /* _MSC_VER */
	{
		switch( errno )
		{
			case EACCES:
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_IO,
				 LIBERROR_IO_ERROR_ACCESS_DENIED,
				 "%s: access denied to file: %" PRIs_LIBCSTRING_SYSTEM ".",
				 function,
				 file_io_handle->name );

				break;

			case ENOENT:
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_IO,
				 LIBERROR_IO_ERROR_INVALID_RESOURCE,
				 "%s: no such file: %" PRIs_LIBCSTRING_SYSTEM ".",
				 function,
				 file_io_handle->name );

				break;

			default:
				if( libbfio_error_string_copy_from_error_number(
				     error_string,
				     LIBBFIO_ERROR_STRING_SIZE,
				     errno,
				     error ) == 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_IO,
					 LIBERROR_IO_ERROR_OPEN_FAILED,
					 "%s: unable to open file: %" PRIs_LIBCSTRING_SYSTEM " with error: %" PRIs_LIBCSTRING_SYSTEM "",
					 function,
					 file_io_handle->name,
					 error_string );
				}
				else
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_IO,
					 LIBERROR_IO_ERROR_OPEN_FAILED,
					 "%s: unable to open file: %" PRIs_LIBCSTRING_SYSTEM ".",
					 function,
					 file_io_handle->name );
				}
				break;
		}
		return( -1 );
	}
#if defined( _MSC_VER )
	if( file_io_handle->file_descriptor == -1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_OPEN_FAILED,
		 "%s: invalid file descriptor: %d returned.",
		 function,
		 file_io_handle->file_descriptor );

		return( -1 );
	}
#endif
#else
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	if( libcstring_narrow_system_string_codepage == 0 )
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf8_string_size_from_utf32(
		          (libuna_utf32_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          &narrow_filename_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf8_string_size_from_utf16(
		          (libuna_utf16_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          &narrow_filename_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	else
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_byte_stream_size_from_utf32(
		          (libuna_utf32_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          libcstring_narrow_system_string_codepage,
		          &narrow_filename_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_byte_stream_size_from_utf16(
		          (libuna_utf16_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          libcstring_narrow_system_string_codepage,
		          &narrow_filename_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to determine narrow character filename size.",
		 function );

		return( -1 );
	}
	narrow_filename = libcstring_narrow_string_allocate(
	                   narrow_filename_size );

	if( narrow_filename == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create narrow character filename.",
		 function );

		return( -1 );
	}
	if( libcstring_narrow_system_string_codepage == 0 )
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf8_string_copy_from_utf32(
		          (libuna_utf8_character_t *) narrow_filename,
		          narrow_filename_size,
		          (libuna_utf32_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf8_string_copy_from_utf16(
		          (libuna_utf8_character_t *) narrow_filename,
		          narrow_filename_size,
		          (libuna_utf16_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	else
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_byte_stream_copy_from_utf32(
		          (uint8_t *) narrow_filename,
		          narrow_filename_size,
		          libcstring_narrow_system_string_codepage,
		          (libuna_utf32_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_byte_stream_copy_from_utf16(
		          (uint8_t *) narrow_filename,
		          narrow_filename_size,
		          libcstring_narrow_system_string_codepage,
		          (libuna_utf16_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to set narrow character filename.",
		 function );

		memory_free(
		 narrow_filename );

		return( -1 );
	}
#if defined( HAVE_GLIB_H )
	file_io_handle->file_descriptor = g_open(
	                                   narrow_filename,
	                                   file_io_flags,
	                                   0644 );
#else
	file_io_handle->file_descriptor = open(
	                                   narrow_filename,
	                                   file_io_flags,
	                                   0644 );
#endif /* HAVE_GLIB_H */

	memory_free(
	 narrow_filename );
#else
#if defined( HAVE_GLIB_H )
	file_io_handle->file_descriptor = g_open(
	                                   file_io_handle->name,
	                                   file_io_flags,
	                                   0644 );
#else
	file_io_handle->file_descriptor = open(
	                                   file_io_handle->name,
	                                   file_io_flags,
	                                   0644 );
#endif /* HAVE_GLIB_H */
#endif /* defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER ) */

	if( file_io_handle->file_descriptor == -1 )
	{
		switch( errno )
		{
			case EACCES:
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_IO,
				 LIBERROR_IO_ERROR_ACCESS_DENIED,
				 "%s: access denied to file: %" PRIs_LIBCSTRING_SYSTEM ".",
				 function,
				 file_io_handle->name );

				break;

			case ENOENT:
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_IO,
				 LIBERROR_IO_ERROR_INVALID_RESOURCE,
				 "%s: no such file: %" PRIs_LIBCSTRING_SYSTEM ".",
				 function,
				 file_io_handle->name );

				break;

			default:
				if( libbfio_error_string_copy_from_error_number(
				     error_string,
				     LIBBFIO_ERROR_STRING_SIZE,
				     errno,
				     error ) == 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_IO,
					 LIBERROR_IO_ERROR_OPEN_FAILED,
					 "%s: unable to open file: %" PRIs_LIBCSTRING_SYSTEM " with error: %" PRIs_LIBCSTRING_SYSTEM "",
					 function,
					 file_io_handle->name,
					 error_string );
				}
				else
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_IO,
					 LIBERROR_IO_ERROR_OPEN_FAILED,
					 "%s: unable to open file: %" PRIs_LIBCSTRING_SYSTEM ".",
					 function,
					 file_io_handle->name );
				}
				break;
		}
		return( -1 );
	}
#endif
#endif
	file_io_handle->access_flags = access_flags;

	return( 1 );
}

/* Closes the file handle
 * Returns 0 if successful or -1 on error
 */
int libbfio_file_close(
     libbfio_file_io_handle_t *file_io_handle,
     liberror_error_t **error )
{
	libcstring_system_character_t error_string[ LIBBFIO_ERROR_STRING_SIZE ];

	static char *function = "libbfio_file_close";

#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	DWORD error_code      = 0;
#endif

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
	if( file_io_handle->name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid file IO handle - missing name.",
		 function );

		return( -1 );
	}
#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	if( file_io_handle->file_handle == INVALID_HANDLE_VALUE )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid file IO handle - invalid file handle.",
		 function );

		return( -1 );
	}
#else
	if( file_io_handle->file_descriptor == -1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid file IO handle - invalid file descriptor.",
		 function );

		return( -1 );
	}
#endif
#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	if( CloseHandle(
	     file_io_handle->file_handle ) == 0 )
	{
		error_code = GetLastError();

		if( libbfio_error_string_copy_from_error_number(
		     error_string,
		     LIBBFIO_ERROR_STRING_SIZE,
		     error_code,
		     error ) == 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_OPEN_FAILED,
			 "%s: unable to close file: %" PRIs_LIBCSTRING_SYSTEM " with error: %" PRIs_LIBCSTRING_SYSTEM "",
			 function,
			 file_io_handle->name,
			 error_string );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_OPEN_FAILED,
			 "%s: unable to close file: %" PRIs_LIBCSTRING_SYSTEM ".",
			 function,
			 file_io_handle->name );
		}
		return( -1 );
	}
	file_io_handle->file_handle = INVALID_HANDLE_VALUE;
#else
#if defined( WINAPI )
	if( _close(
	     file_io_handle->file_descriptor ) != 0 )
#else
	if( close(
	     file_io_handle->file_descriptor ) != 0 )
#endif
	{
		if( libbfio_error_string_copy_from_error_number(
		     error_string,
		     LIBBFIO_ERROR_STRING_SIZE,
		     errno,
		     error ) == 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_OPEN_FAILED,
			 "%s: unable to close file: %" PRIs_LIBCSTRING_SYSTEM " with error: %" PRIs_LIBCSTRING_SYSTEM "",
			 function,
			 file_io_handle->name,
			 error_string );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_OPEN_FAILED,
			 "%s: unable to close file: %" PRIs_LIBCSTRING_SYSTEM ".",
			 function,
			 file_io_handle->name );
		}
		return( -1 );
	}
	file_io_handle->file_descriptor = -1;
#endif
	file_io_handle->access_flags = 0;

	return( 0 );
}

/* Reads a buffer from the file handle
 * Returns the number of bytes read if successful, or -1 on error
 */
ssize_t libbfio_file_read(
         libbfio_file_io_handle_t *file_io_handle,
         uint8_t *buffer,
         size_t size,
         liberror_error_t **error )
{
	libcstring_system_character_t error_string[ LIBBFIO_ERROR_STRING_SIZE ];

	static char *function = "libbfio_file_read";
	ssize_t read_count    = 0;

#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	DWORD error_code      = 0;
#endif

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
	if( file_io_handle->name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid file IO handle - missing name.",
		 function );

		return( -1 );
	}
#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	if( file_io_handle->file_handle == INVALID_HANDLE_VALUE )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid file IO handle - invalid file handle.",
		 function );

		return( -1 );
	}
#else
	if( file_io_handle->file_descriptor == -1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid file IO handle - invalid file descriptor.",
		 function );

		return( -1 );
	}
#endif
	if( buffer == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid buffer.",
		 function );

		return( -1 );
	}
#if UINT32_MAX < SSIZE_MAX
	if( size > (size_t) UINT32_MAX )
#else
	if( size > (size_t) SSIZE_MAX )
#endif
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid size value exceeds maximum.",
		 function );

		return( -1 );
	}
#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	if( ReadFile(
	     file_io_handle->file_handle,
	     buffer,
	     (DWORD) size,
	     (LPDWORD) &read_count,
	     NULL ) == 0 )
	{
		error_code = GetLastError();

		switch( error_code )
		{
			case ERROR_HANDLE_EOF:
				break;

			default:
				if( libbfio_error_string_copy_from_error_number(
				     error_string,
				     LIBBFIO_ERROR_STRING_SIZE,
				     error_code,
				     error ) == 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_IO,
					 LIBERROR_IO_ERROR_READ_FAILED,
					 "%s: unable to read from file: %" PRIs_LIBCSTRING_SYSTEM " with error: %" PRIs_LIBCSTRING_SYSTEM "",
					 function,
					 file_io_handle->name,
					 error_string );
				}
				else
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_IO,
					 LIBERROR_IO_ERROR_READ_FAILED,
					 "%s: unable to read from file: %" PRIs_LIBCSTRING_SYSTEM ".",
					 function,
					 file_io_handle->name );
				}
				return( -1 );
		}
	}
	if( read_count < 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_READ_FAILED,
		 "%s: invalid read count: %" PRIzd " returned.",
		 function,
		 read_count );
	}
#else
#if defined( WINAPI )
	read_count = _read(
	              file_io_handle->file_descriptor,
	              (void *) buffer,
	              (unsigned int) size );
#else
	read_count = read(
	              file_io_handle->file_descriptor,
	              (void *) buffer,
	              size );
#endif

	if( read_count < 0 )
	{
		if( libbfio_error_string_copy_from_error_number(
		     error_string,
		     LIBBFIO_ERROR_STRING_SIZE,
		     errno,
		     error ) == 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read from file: %" PRIs_LIBCSTRING_SYSTEM " with error: %" PRIs_LIBCSTRING_SYSTEM "",
			 function,
			 file_io_handle->name,
			 error_string );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read from file: %" PRIs_LIBCSTRING_SYSTEM ".",
			 function,
			 file_io_handle->name );
		}
		return( -1 );
	}
#endif
	return( read_count );
}

/* Writes a buffer to the file handle
 * Returns the number of bytes written if successful, or -1 on error
 */
ssize_t libbfio_file_write(
         libbfio_file_io_handle_t *file_io_handle,
         const uint8_t *buffer,
         size_t size,
         liberror_error_t **error )
{
	libcstring_system_character_t error_string[ LIBBFIO_ERROR_STRING_SIZE ];

	static char *function = "libbfio_file_write";
	ssize_t write_count   = 0;

#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	DWORD error_code      = 0;
#endif

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
	if( file_io_handle->name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid file IO handle - missing name.",
		 function );

		return( -1 );
	}
#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	if( file_io_handle->file_handle == INVALID_HANDLE_VALUE )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid file IO handle - invalid file handle.",
		 function );

		return( -1 );
	}
#else
	if( file_io_handle->file_descriptor == -1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid file IO handle - invalid file descriptor.",
		 function );

		return( -1 );
	}
#endif
	if( buffer == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid buffer.",
		 function );

		return( -1 );
	}
#if UINT32_MAX < SSIZE_MAX
	if( size > (size_t) UINT32_MAX )
#else
	if( size > (size_t) SSIZE_MAX )
#endif
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid size value exceeds maximum.",
		 function );

		return( -1 );
	}
#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	if( WriteFile(
	     file_io_handle->file_handle,
	     buffer,
	     (DWORD) size,
	     (LPDWORD) &write_count,
	     NULL ) == 0 )
	{
		error_code = GetLastError();

		if( libbfio_error_string_copy_from_error_number(
		     error_string,
		     LIBBFIO_ERROR_STRING_SIZE,
		     error_code,
		     error ) == 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_WRITE_FAILED,
			 "%s: unable to write to file: %" PRIs_LIBCSTRING_SYSTEM " with error: %" PRIs_LIBCSTRING_SYSTEM "",
			 function,
			 file_io_handle->name,
			 error_string );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_WRITE_FAILED,
			 "%s: unable to write to file: %" PRIs_LIBCSTRING_SYSTEM ".",
			 function,
			 file_io_handle->name );
		}
		return( -1 );
	}
	if( write_count < 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_WRITE_FAILED,
		 "%s: invalid write count: %" PRIzd " returned.",
		 function,
		 write_count );
	}
#else
#if defined( WINAPI )
	write_count = _write(
	               file_io_handle->file_descriptor,
	               (void *) buffer,
	               (unsigned int) size );
#else
	write_count = write(
	               file_io_handle->file_descriptor,
	               (void *) buffer,
	               size );
#endif

	if( write_count < 0 )
	{
		switch( errno )
		{
			case ENOSPC:
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_OUTPUT,
				 LIBERROR_OUTPUT_ERROR_INSUFFICIENT_SPACE,
				 "%s: insufficient space to write to file: %" PRIs_LIBCSTRING_SYSTEM ".",
				 function,
				 file_io_handle->name );

				break;

			default:
				if( libbfio_error_string_copy_from_error_number(
				     error_string,
				     LIBBFIO_ERROR_STRING_SIZE,
				     errno,
				     error ) == 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_IO,
					 LIBERROR_IO_ERROR_WRITE_FAILED,
					 "%s: unable to write to file: %" PRIs_LIBCSTRING_SYSTEM " with error: %" PRIs_LIBCSTRING_SYSTEM "",
					 function,
					 file_io_handle->name,
					 error_string );
				}
				else
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_IO,
					 LIBERROR_IO_ERROR_WRITE_FAILED,
					 "%s: unable to write to file: %" PRIs_LIBCSTRING_SYSTEM ".",
					 function,
					 file_io_handle->name );
				}
				break;
		}
		return( -1 );
	}
	else if( write_count == 0 )
	{
		if( errno == ENOSPC )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_OUTPUT,
			 LIBERROR_OUTPUT_ERROR_INSUFFICIENT_SPACE,
			 "%s: insufficient space to write to file: %" PRIs_LIBCSTRING_SYSTEM ".",
			 function,
			 file_io_handle->name );

			return( -1 );
		}
	}
#endif
	return( write_count );
}

#if defined( WINAPI ) && ( WINVER < 0x0500 )

#if !defined( INVALID_SET_FILE_POINTER )
#define INVALID_SET_FILE_POINTER	((LONG) -1)
#endif

/* Cross Windows safe version of SetFilePointerEx
 * Returns TRUE if successful or FALSE on error
 */
BOOL libbfio_SetFilePointerEx(
      HANDLE file_handle,
      LARGE_INTEGER distance_to_move_large_integer,
      LARGE_INTEGER *new_file_pointer_large_integer,
      DWORD move_method )
{
	FARPROC function                 = NULL;
	HMODULE library_handle           = NULL;
	LONG distance_to_move_lower_long = 0;
	LONG distance_to_move_upper_long = 0;
	DWORD error_number               = 0;
	BOOL result                      = FALSE;

	if( file_handle == NULL )
	{
		return( FALSE );
	}
	if( new_file_pointer_large_integer == NULL )
	{
		return( FALSE );
	}
	library_handle = LoadLibrary(
	                  _LIBCSTRING_SYSTEM_STRING( "kernel32.dll" ) );

	if( library_handle == NULL )
	{
		return( FALSE );
	}
	function = GetProcAddress(
		    library_handle,
		    (LPCSTR) "SetFilePointerEx" );

	if( function != NULL )
	{
		result = function(
			  file_handle,
			  distance_to_move_large_integer,
			  new_file_pointer_large_integer,
			  move_method );
	}
	else
	{
#if defined( __BORLANDC__ ) && __BORLANDC__ <= 0x520
		distance_to_move_lower_long = distance_to_move_large_integer.QuadPart & 0xffffffffUL;
		distance_to_move_upper_long = distance_to_move_large_integer.QuadPart >> 32;
#else
		distance_to_move_lower_long = distance_to_move_large_integer.LowPart;
		distance_to_move_upper_long = distance_to_move_large_integer.HighPart;
#endif

		distance_to_move_lower_long = SetFilePointer(
					       file_handle,
					       distance_to_move_lower_long,
					       &distance_to_move_upper_long,
					       move_method );

		error_number = GetLastError();

		if( ( distance_to_move_lower_long == (LONG) INVALID_SET_FILE_POINTER )
		 && ( error_number != NO_ERROR ) )
		{
		}
		else
		{
#if defined( __BORLANDC__ ) && __BORLANDC__ <= 0x520
			new_file_pointer_large_integer->QuadPart   = distance_to_move_upper_long;
			new_file_pointer_large_integer->QuadPart <<= 32;
			new_file_pointer_large_integer->QuadPart  += distance_to_move_lower_long;
#else
			new_file_pointer_large_integer->HighPart = distance_to_move_upper_long;
			new_file_pointer_large_integer->LowPart  = distance_to_move_lower_long;
#endif

			result = TRUE;
		}
	}
	/* This call should be after using the function
	 * in most cases kernel32.dll will still be available after free
	 */
	if( FreeLibrary(
	     library_handle ) != TRUE )
	{
		result = FALSE;
	}
	return( result );
}

#endif

/* Seeks a certain offset within the file handle
 * Returns the offset if the seek is successful or -1 on error
 */
off64_t libbfio_file_seek_offset(
         libbfio_file_io_handle_t *file_io_handle,
         off64_t offset,
         int whence,
         liberror_error_t **error )
{
	libcstring_system_character_t error_string[ LIBBFIO_ERROR_STRING_SIZE ];

	static char *function              = "libbfio_file_seek_offset";

#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	LARGE_INTEGER large_integer_offset = LIBBFIO_LARGE_INTEGER_ZERO;
	DWORD error_code                   = 0;
	DWORD move_method                  = 0;
#endif

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
	if( file_io_handle->name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid file IO handle - missing name.",
		 function );

		return( -1 );
	}
#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	if( file_io_handle->file_handle == INVALID_HANDLE_VALUE )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid file IO handle - invalid file handle.",
		 function );

		return( -1 );
	}
#else
	if( file_io_handle->file_descriptor == -1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid file IO handle - invalid file descriptor.",
		 function );

		return( -1 );
	}
#endif
	if( offset > (off64_t) INT64_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid offset value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( ( whence != SEEK_CUR )
	 && ( whence != SEEK_END )
	 && ( whence != SEEK_SET ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported whence.",
		 function );

		return( -1 );
	}
#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	if( whence == SEEK_SET )
	{
		move_method = FILE_BEGIN;
	}
	else if( whence == SEEK_CUR )
	{
		move_method = FILE_CURRENT;
	}
	else if( whence == SEEK_END )
	{
		move_method = FILE_END;
	}
#if defined( __BORLANDC__ ) && __BORLANDC__ <= 0x520
	large_integer_offset.QuadPart = (LONGLONG) offset;
#else
	large_integer_offset.LowPart  = (DWORD) ( 0x0ffffffffUL & offset );
	large_integer_offset.HighPart = (LONG) ( offset >> 32 );
#endif

#if ( WINVER >= 0x0500 )
	if( SetFilePointerEx(
	     file_io_handle->file_handle,
	     large_integer_offset,
	     &large_integer_offset,
	     move_method ) == 0 )
#else
	if( libbfio_SetFilePointerEx(
	     file_io_handle->file_handle,
	     large_integer_offset,
	     &large_integer_offset,
	     move_method ) == 0 )
#endif
	{
		error_code = GetLastError();

		if( libbfio_error_string_copy_from_error_number(
		     error_string,
		     LIBBFIO_ERROR_STRING_SIZE,
		     error_code,
		     error ) == 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_SEEK_FAILED,
			 "%s: unable to find offset: %" PRIi64 " in file: %" PRIs_LIBCSTRING_SYSTEM " with error: %" PRIs_LIBCSTRING_SYSTEM "",
			 function,
			 offset,
			 file_io_handle->name,
			 error_string );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_SEEK_FAILED,
			 "%s: unable to find offset: %" PRIi64 " in file: %" PRIs_LIBCSTRING_SYSTEM ".",
			 function,
			 offset,
			 file_io_handle->name );
		}
		return( -1 );
	}
#if defined( __BORLANDC__ ) && __BORLANDC__ <= 0x520
	offset = (off64_t) large_integer_offset.QuadPart;
#else
	offset = ( (off64_t) large_integer_offset.HighPart << 32 ) + large_integer_offset.LowPart;
#endif

	if( offset < 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_SEEK_FAILED,
		 "%s: invalid offset: %" PRIi64 " returned.",
		 function,
		 offset );

		return( -1 );
	}
#else
#if defined( WINAPI )
	offset = _lseeki64(
	          file_io_handle->file_descriptor,
	          offset,
	          whence );
#else
	offset = lseek(
	          file_io_handle->file_descriptor,
	          (off_t) offset,
	          whence );
#endif

	if( offset < 0 )
	{
		if( libbfio_error_string_copy_from_error_number(
		     error_string,
		     LIBBFIO_ERROR_STRING_SIZE,
		     errno,
		     error ) == 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_SEEK_FAILED,
			 "%s: unable to find offset: %" PRIi64 " in file: %" PRIs_LIBCSTRING_SYSTEM " with error: %" PRIs_LIBCSTRING_SYSTEM "",
			 function,
			 offset,
			 file_io_handle->name,
			 error_string );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_SEEK_FAILED,
			 "%s: unable to find offset: %" PRIi64 " in file: %" PRIs_LIBCSTRING_SYSTEM ".",
			 function,
			 offset,
			 file_io_handle->name );
		}
		return( -1 );
	}
#endif
	return( offset );
}

#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS ) && ( WINVER >= 0x0500 )

/* Function to determine if a file exists
 * Optimized version for Windows 2000 (WINAPI 5) or later
 * Returns 1 if file exists, 0 if not or -1 on error
 */
int libbfio_file_exists(
     libbfio_file_io_handle_t *file_io_handle,
     liberror_error_t **error )
{
	libcstring_system_character_t error_string[ LIBBFIO_ERROR_STRING_SIZE ];

	static char *function = "libbfio_file_exists";
	int result            = 1;
	DWORD error_code      = 0;
	DWORD file_attributes = 0;

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
	if( file_io_handle->name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file IO handle - missing name.",
		 function );

		return( -1 );
	}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	/* Must use GetFileAttributesW here because filename is a 
	 * wide character string and GetFileAttributes is dependent
	 * on UNICODE directives
	 */
	file_attributes = GetFileAttributesW(
	                   (LPCWSTR) file_io_handle->name );
#else
	/* Must use GetFileAttributesA here because filename is a 
	 * narrow character string and GetFileAttributes is dependent
	 * on UNICODE directives
	 */
	file_attributes = GetFileAttributesA(
	                   (LPCSTR) file_io_handle->name );
#endif
	if( file_attributes == INVALID_FILE_ATTRIBUTES )
	{
		error_code = GetLastError();

		switch( error_code )
		{
			case ERROR_ACCESS_DENIED:
				result = 1;

				break;

			case ERROR_FILE_NOT_FOUND:
			case ERROR_PATH_NOT_FOUND:
				result = 0;

				break;

			default:
				if( libbfio_error_string_copy_from_error_number(
				     error_string,
				     LIBBFIO_ERROR_STRING_SIZE,
				     error_code,
				     error ) == 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_IO,
					 LIBERROR_IO_ERROR_GENERIC,
					 "%s: unable to determine attributes of file: %" PRIs_LIBCSTRING_SYSTEM " with error: %" PRIs_LIBCSTRING_SYSTEM "",
					 function,
					 file_io_handle->name,
					 error_string );
				}
				else
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_IO,
					 LIBERROR_IO_ERROR_GENERIC,
					 "%s: unable to determine attributes of file: %" PRIs_LIBCSTRING_SYSTEM ".",
					 function,
					 file_io_handle->name );
				}
				result = -1;

				break;
		}
	}
	return( result );
}

#elif defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )

/* Function to determine if a file exists
 * Returns 1 if file exists, 0 if not or -1 on error
 */
int libbfio_file_exists(
     libbfio_file_io_handle_t *file_io_handle,
     liberror_error_t **error )
{
	libcstring_system_character_t error_string[ LIBBFIO_ERROR_STRING_SIZE ];

	static char *function = "libbfio_file_exists";
	int result            = 1;
	DWORD error_code      = 0;

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
	if( file_io_handle->name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file IO handle - missing name.",
		 function );

		return( -1 );
	}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	/* Must use CreateFileW here because filename is a 
	 * wide character string and CreateFile is dependent
	 * on UNICODE directives
	 */
	file_io_handle->file_handle = CreateFileW(
	                               (LPCWSTR) file_io_handle->name,
	                               GENERIC_READ,
	                               FILE_SHARE_READ,
	                               NULL,
	                               OPEN_EXISTING,
	                               FILE_ATTRIBUTE_NORMAL,
	                               NULL );
#else
	/* Must use CreateFileA here because filename is a 
	 * narrow character string and CreateFile is dependent
	 * on UNICODE directives
	 */
	file_io_handle->file_handle = CreateFileA(
	                               (LPCSTR) file_io_handle->name,
	                               GENERIC_READ,
	                               FILE_SHARE_READ,
	                               NULL,
	                               OPEN_EXISTING,
	                               FILE_ATTRIBUTE_NORMAL,
	                               NULL );
#endif

	if( file_io_handle->file_handle == INVALID_HANDLE_VALUE )
	{
		error_code = GetLastError();

		switch( error_code )
		{
			case ERROR_ACCESS_DENIED:
				result = 1;

				break;

			case ERROR_FILE_NOT_FOUND:
			case ERROR_PATH_NOT_FOUND:
				result = 0;

				break;

			default:
				if( libbfio_error_string_copy_from_error_number(
				     error_string,
				     LIBBFIO_ERROR_STRING_SIZE,
				     error_code,
				     error ) == 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_IO,
					 LIBERROR_IO_ERROR_OPEN_FAILED,
					 "%s: unable to open file: %" PRIs_LIBCSTRING_SYSTEM " with error: %" PRIs_LIBCSTRING_SYSTEM "",
					 function,
					 file_io_handle->name,
					 error_string );
				}
				else
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_IO,
					 LIBERROR_IO_ERROR_OPEN_FAILED,
					 "%s: unable to open file: %" PRIs_LIBCSTRING_SYSTEM ".",
					 function,
					 file_io_handle->name );
				}
				result = -1;

				break;
		}
	}
	else if( CloseHandle(
	          file_io_handle->file_handle ) == 0 )
	{
		error_code = GetLastError();

		if( libbfio_error_string_copy_from_error_number(
		     error_string,
		     LIBBFIO_ERROR_STRING_SIZE,
		     error_code,
		     error ) == 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_OPEN_FAILED,
			 "%s: unable to close file: %" PRIs_LIBCSTRING_SYSTEM " with error: %" PRIs_LIBCSTRING_SYSTEM "",
			 function,
			 file_io_handle->name,
			 error_string );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_OPEN_FAILED,
			 "%s: unable to close file: %" PRIs_LIBCSTRING_SYSTEM ".",
			 function,
			 file_io_handle->name );
		}
		return( -1 );
	}
	file_io_handle->file_handle = INVALID_HANDLE_VALUE;

	return( result );
}

#elif defined( HAVE_STAT ) && !defined( WINAPI )

/* Function to determine if a file exists
 * Optimized version for platforms with the stat function
 * Returns 1 if file exists, 0 if not or -1 on error
 */
int libbfio_file_exists(
     libbfio_file_io_handle_t *file_io_handle,
     liberror_error_t **error )
{
	struct stat file_statistics;

	libcstring_system_character_t error_string[ LIBBFIO_ERROR_STRING_SIZE ];

	static char *function       = "libbfio_file_exists";
	int result                  = 1;

#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	char *narrow_filename       = NULL;
	size_t narrow_filename_size = 0;
#endif

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
	if( file_io_handle->name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file IO handle - missing name.",
		 function );

		return( -1 );
	}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	/* Convert the filename to a narrow string
	 * if the platform has no wide character open function
	 */
	if( libcstring_narrow_system_string_codepage == 0 )
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf8_string_size_from_utf32(
		          (libuna_utf32_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          &narrow_filename_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf8_string_size_from_utf16(
		          (libuna_utf16_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          &narrow_filename_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	else
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_byte_stream_size_from_utf32(
		          (libuna_utf32_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          libcstring_narrow_system_string_codepage,
		          &narrow_filename_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_byte_stream_size_from_utf16(
		          (libuna_utf16_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          libcstring_narrow_system_string_codepage,
		          &narrow_filename_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to determine narrow character filename size.",
		 function );

		return( -1 );
	}
	narrow_filename = libcstring_narrow_string_allocate(
	                   narrow_filename_size );

	if( narrow_filename == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create narrow character filename.",
		 function );

		return( -1 );
	}
	if( libcstring_narrow_system_string_codepage == 0 )
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf8_string_copy_from_utf32(
		          (libuna_utf8_character_t *) narrow_filename,
		          narrow_filename_size,
		          (libuna_utf32_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf8_string_copy_from_utf16(
		          (libuna_utf8_character_t *) narrow_filename,
		          narrow_filename_size,
		          (libuna_utf16_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	else
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_byte_stream_copy_from_utf32(
		          (uint8_t *) narrow_filename,
		          narrow_filename_size,
		          libcstring_narrow_system_string_codepage,
		          (libuna_utf32_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_byte_stream_copy_from_utf16(
		          (uint8_t *) narrow_filename,
		          narrow_filename_size,
		          libcstring_narrow_system_string_codepage,
		          (libuna_utf16_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to set narrow character filename.",
		 function );

		memory_free(
		 narrow_filename );

		return( -1 );
	}
	result = stat(
	          narrow_filename,
	          &file_statistics );

	memory_free(
	 narrow_filename );
#else
	result = stat(
	          (char *) file_io_handle->name,
	          &file_statistics );
#endif

	if( result != 0 )
	{
		switch( errno )
		{
			case EACCES:
				result = 1;

				break;

			case ENOENT:
				result = 0;

				break;

			default:
				if( libbfio_error_string_copy_from_error_number(
				     error_string,
				     LIBBFIO_ERROR_STRING_SIZE,
				     errno,
				     error ) == 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_IO,
					 LIBERROR_IO_ERROR_GENERIC,
					 "%s: unable to stat file: %" PRIs_LIBCSTRING_SYSTEM " with error: %" PRIs_LIBCSTRING_SYSTEM "",
					 function,
					 file_io_handle->name,
					 error_string );
				}
				else
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_IO,
					 LIBERROR_IO_ERROR_GENERIC,
					 "%s: unable to stat file: %" PRIs_LIBCSTRING_SYSTEM ".",
					 function,
					 file_io_handle->name );
				}
				result = -1;

				break;
		}
	}
	else
	{
		result = 1;
	}
	return( result );
}

#else

/* Function to determine if a file exists
 * Returns 1 if file exists, 0 if not or -1 on error
 */
int libbfio_file_exists(
     libbfio_file_io_handle_t *file_io_handle,
     liberror_error_t **error )
{
	libcstring_system_character_t error_string[ LIBBFIO_ERROR_STRING_SIZE ];

	static char *function       = "libbfio_file_exists";
	int result                  = 1;

#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER ) && !defined( WINAPI )
	char *narrow_filename       = NULL;
	size_t narrow_filename_size = 0;
#endif

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
	if( file_io_handle->name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file IO handle - missing name.",
		 function );

		return( -1 );
	}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
#if defined( _MSC_VER )
	if( _wsopen_s(
	     &( file_io_handle->file_descriptor ),
	     (wchar_t *) file_io_handle->name,
	     _O_RDONLY | _O_BINARY,
	     _SH_DENYWR,
	     0 ) != 0 )
	{
		file_io_handle->file_descriptor = -1;
	}
#elif defined( WINAPI )
	file_io_handle->file_descriptor = _wsopen(
	                                   (wchar_t *) file_io_handle->name,
	                                   _O_RDONLY | _O_BINARY,
	                                   0 );
#else
	/* Convert the filename to a narrow string
	 * if the platform has no wide character open function
	 */
	if( libcstring_narrow_system_string_codepage == 0 )
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf8_string_size_from_utf32(
		          (libuna_utf32_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          &narrow_filename_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf8_string_size_from_utf16(
		          (libuna_utf16_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          &narrow_filename_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	else
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_byte_stream_size_from_utf32(
		          (libuna_utf32_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          libcstring_narrow_system_string_codepage,
		          &narrow_filename_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_byte_stream_size_from_utf16(
		          (libuna_utf16_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          libcstring_narrow_system_string_codepage,
		          &narrow_filename_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to determine narrow character filename size.",
		 function );

		return( -1 );
	}
	narrow_filename = libcstring_narrow_string_allocate(
	                   narrow_filename_size );

	if( narrow_filename == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create narrow character filename.",
		 function );

		return( -1 );
	}
	if( libcstring_narrow_system_string_codepage == 0 )
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf8_string_copy_from_utf32(
		          (libuna_utf8_character_t *) narrow_filename,
		          narrow_filename_size,
		          (libuna_utf32_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf8_string_copy_from_utf16(
		          (libuna_utf8_character_t *) narrow_filename,
		          narrow_filename_size,
		          (libuna_utf16_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	else
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_byte_stream_copy_from_utf32(
		          (uint8_t *) narrow_filename,
		          narrow_filename_size,
		          libcstring_narrow_system_string_codepage,
		          (libuna_utf32_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_byte_stream_copy_from_utf16(
		          (uint8_t *) narrow_filename,
		          narrow_filename_size,
		          libcstring_narrow_system_string_codepage,
		          (libuna_utf16_character_t *) file_io_handle->name,
		          file_io_handle->name_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to set narrow character filename.",
		 function );

		memory_free(
		 narrow_filename );

		return( -1 );
	}
	file_io_handle->file_descriptor = open(
	                                   narrow_filename,
	                                   O_RDONLY,
	                                   0644 );

	memory_free(
	 narrow_filename );
#endif
#else
/* The system string is a narrow character string */
#if defined( _MSC_VER )
	if( _sopen_s(
	     &( file_io_handle->file_descriptor ),
	     (char *) file_io_handle->name,
	     _O_RDONLY | _O_BINARY,
	     _SH_DENYWR,
	     0 ) != 0 )
	{
		file_io_handle->file_descriptor = -1;
	}
#elif defined( WINAPI )
	file_io_handle->file_descriptor = _sopen(
					   (char *) file_io_handle->name,
					   _O_RDONLY | _O_BINARY,
					   0 );
#else
	file_io_handle->file_descriptor = open(
	                                   (char *) file_io_handle->name,
	                                   O_RDONLY,
	                                   0644 );
#endif
#endif
	if( file_io_handle->file_descriptor == -1 )
	{
		switch( errno )
		{
			case EACCES:
				result = 1;

				break;

			case ENOENT:
				result = 0;

				break;

			default:
				if( libbfio_error_string_copy_from_error_number(
				     error_string,
				     LIBBFIO_ERROR_STRING_SIZE,
				     errno,
				     error ) == 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_IO,
					 LIBERROR_IO_ERROR_OPEN_FAILED,
					 "%s: unable to open file: %" PRIs_LIBCSTRING_SYSTEM " with error: %" PRIs_LIBCSTRING_SYSTEM "",
					 function,
					 file_io_handle->name,
					 error_string );
				}
				else
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_IO,
					 LIBERROR_IO_ERROR_OPEN_FAILED,
					 "%s: unable to open file: %" PRIs_LIBCSTRING_SYSTEM ".",
					 function,
					 file_io_handle->name );
				}
				result = -1;

				break;
		}
	}
#if defined( WINAPI )
	else if( _close(
		  file_io_handle->file_descriptor ) != 0 )
#else
	else if( close(
		  file_io_handle->file_descriptor ) != 0 )
#endif
	{
		if( libbfio_error_string_copy_from_error_number(
		     error_string,
		     LIBBFIO_ERROR_STRING_SIZE,
		     errno,
		     error ) == 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_OPEN_FAILED,
			 "%s: unable to close file: %" PRIs_LIBCSTRING_SYSTEM " with error: %" PRIs_LIBCSTRING_SYSTEM "",
			 function,
			 file_io_handle->name,
			 error_string );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_OPEN_FAILED,
			 "%s: unable to close file: %" PRIs_LIBCSTRING_SYSTEM ".",
			 function,
			 file_io_handle->name );
		}
	}
	file_io_handle->file_descriptor = -1;	

	return( result );
}

#endif

/* Check if the file is open
 * Returns 1 if open, 0 if not or -1 on error
 */
int libbfio_file_is_open(
     libbfio_file_io_handle_t *file_io_handle,
     liberror_error_t **error )
{
	static char *function = "libbfio_file_is_open";

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
#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	if( file_io_handle->file_handle == INVALID_HANDLE_VALUE )
	{
		return( 0 );
	}
#else
	if( file_io_handle->file_descriptor == -1 )
	{
		return( 0 );
	}
#endif
	return( 1 );
}

#if defined( WINAPI ) && ( WINVER < 0x0500 )

/* Cross Windows safe version of GetFileSizeEx
 * Returns TRUE if successful or FALSE on error
 */
BOOL libbfio_GetFileSizeEx(
      HANDLE file_handle,
      LARGE_INTEGER *file_size_large_integer )
{
	FARPROC function            = NULL;
	HMODULE library_handle      = NULL;
	DWORD error_number          = 0;
	DWORD file_size_upper_dword = 0;
	DWORD file_size_lower_dword = 0;
	BOOL result                 = FALSE;

	if( file_handle == NULL )
	{
		return( FALSE );
	}
	if( file_size_large_integer == NULL )
	{
		return( FALSE );
	}
	library_handle = LoadLibrary(
	                  _LIBCSTRING_SYSTEM_STRING( "kernel32.dll" ) );

	if( library_handle == NULL )
	{
		return( FALSE );
	}
	function = GetProcAddress(
		    library_handle,
		    (LPCSTR) "GetFileSizeEx" );

	if( function != NULL )
	{
		result = function(
			  file_handle,
			  file_size_large_integer );
	}
	else
	{
		file_size_lower_dword = GetFileSize(
		                         file_handle,
		                         &file_size_upper_dword );

		error_number = GetLastError();

		if( ( file_size_lower_dword == INVALID_FILE_SIZE )
		 && ( error_number != NO_ERROR ) )
		{
		}
		else
		{
#if defined( __BORLANDC__ ) && __BORLANDC__ <= 0x520
			file_size_large_integer->QuadPart   = file_size_upper_dword;
			file_size_large_integer->QuadPart <<= 32;
			file_size_large_integer->QuadPart  += file_size_lower_dword;
#else
			file_size_large_integer->HighPart = file_size_upper_dword;
			file_size_large_integer->LowPart  = file_size_lower_dword;
#endif

			result = TRUE;
		}
	}
	/* This call should be after using the function
	 * in most cases kernel32.dll will still be available after free
	 */
	if( FreeLibrary(
	     library_handle ) != TRUE )
	{
		result = FALSE;
	}
	return( result );
}

#endif

/* Retrieves the file size
 * Returns 1 if successful or -1 on error
 */
int libbfio_file_get_size(
     libbfio_file_io_handle_t *file_io_handle,
     size64_t *size,
     liberror_error_t **error )
{
	static char *function            = "libbfio_file_get_size";

#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	LARGE_INTEGER large_integer_size = LIBBFIO_LARGE_INTEGER_ZERO;
#else
	off64_t offset                   = 0;

#if defined( _MSC_VER )
	struct __stat64 file_stat;
#elif defined( __BORLANDC__ )
	struct stati64 file_stat;
#else
	struct stat file_stat;
#endif
#endif

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
#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	if( file_io_handle->file_handle == INVALID_HANDLE_VALUE )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid file IO handle - invalid file handle.",
		 function );

		return( -1 );
	}
#else
	if( file_io_handle->file_descriptor == -1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid file IO handle - invalid file descriptor.",
		 function );

		return( -1 );
	}
#endif
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
#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
#if ( WINVER >= 0x0500 )
	if( GetFileSizeEx(
	     file_io_handle->file_handle,
	     &large_integer_size ) == 0 )
#else
	if( libbfio_GetFileSizeEx(
	     file_io_handle->file_handle,
	     &large_integer_size ) == 0 )
#endif
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve file size.",
		 function );

		return( -1 );
	}
#if defined( __BORLANDC__ ) && __BORLANDC__ <= 0x520
	*size = (size64_t) large_integer_size.QuadPart;
#else
	*size = ( (size64_t) large_integer_size.HighPart << 32 ) + large_integer_size.LowPart;
#endif
#else
#if defined( _MSC_VER )
	if( _fstat64(
	     file_io_handle->file_descriptor,
	     &file_stat ) != 0 )
#elif defined( __BORLANDC__ )
	if( _fstati64(
	     file_io_handle->file_descriptor,
	     &file_stat ) != 0 )
#else
	if( fstat(
	     file_io_handle->file_descriptor,
	     &file_stat ) != 0 )
#endif
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: file stat failed.",
		 function );

		return( -1 );
	}
	if( S_ISBLK( file_stat.st_mode )
	 || S_ISCHR( file_stat.st_mode ) )
	{
		/* If the file is a device try to seek the end of the file
		 */
		offset = libbfio_file_seek_offset(
		          file_io_handle,
		          0,
		          SEEK_END,
		          error );

		if( offset == -1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_SEEK_FAILED,
			 "%s: unable to find end of file.",
			 function );

			return( -1 );
		}
		*size = (size64_t) offset;

		offset = libbfio_file_seek_offset(
		          file_io_handle,
		          0,
		          SEEK_SET,
		          error );

		if( offset == -1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_SEEK_FAILED,
			 "%s: unable to find start of file.",
			 function );

			return( -1 );
		}
	}
	else
	{
		*size = (size64_t) file_stat.st_size;
	}
#endif
	return( 1 );
}

