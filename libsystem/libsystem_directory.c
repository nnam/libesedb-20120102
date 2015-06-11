/*
 * Directory functions
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

#include <common.h>
#include <types.h>

#include <libcstring.h>
#include <liberror.h>

#if defined( HAVE_ERRNO_H ) || defined( WINAPI )
#include <errno.h>
#endif

#if defined( HAVE_SYS_STAT_H )
#include <sys/stat.h>
#endif

#if defined( WINAPI ) && !defined( __CYGWIN__ )
#include <direct.h>
#endif

#if defined( HAVE_UNISTD_H )
#include <unistd.h>
#endif

#include "libsystem_directory.h"
#include "libsystem_error_string.h"

#if defined( HAVE_CHDIR ) || defined( WINAPI )

/* Changes the directory
 * Returns 1 if successful or -1 on error
 */
int libsystem_directory_change(
     const libcstring_system_character_t *directory_name,
     liberror_error_t **error )
{
	libcstring_system_character_t error_string[ 128 ];

	static char *function = "libsystem_directory_change";

#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	DWORD error_code      = 0;
#endif

	if( directory_name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid directory name.",
		 function );

		return( -1 );
	}
#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
/* TODO add pre Windows XP support */
	if( SetCurrentDirectory(
	     directory_name ) == 0 )
	{
		error_code = GetLastError();

		if( libsystem_error_string_copy_from_error_number(
		     error_string,
		     128,
		     error_code,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to change directory with error: %" PRIs_LIBCSTRING_SYSTEM "",
			 function,
			 error_string );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to change directory.",
			 function );
		}
		return( -1 );
	}
#elif defined( WINAPI )
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	if( _wchdir(
	     directory_name ) != 0 )
#else
	if( _chdir(
	     directory_name ) != 0 )
#endif
	{
		if( libsystem_error_string_copy_from_error_number(
		     error_string,
		     128,
		     errno,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to change directory with error: %" PRIs_LIBCSTRING_SYSTEM "",
			 function,
			 error_string );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to change directory.",
			 function );
		}
		return( -1 );
	}

#else
/* Sanity check
 */
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
#error Missing wide character chdir function
#endif

	if( chdir(
	     directory_name ) != 0 )
	{
		if( libsystem_error_string_copy_from_error_number(
		     error_string,
		     128,
		     errno,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to change directory with error: %" PRIs_LIBCSTRING_SYSTEM "",
			 function,
			 error_string );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to change directory.",
			 function );
		}
		return( -1 );
	}

#endif
	return( 1 );
}

#endif /* defined( HAVE_CHDIR ) || defined( WINAPI ) */

#if defined( HAVE_MKDIR ) || defined( WINAPI )

/* Makes the directory
 * Returns 1 if successful or -1 on error
 */
int libsystem_directory_make(
     const libcstring_system_character_t *directory_name,
     liberror_error_t **error )
{
	libcstring_system_character_t error_string[ 128 ];

	static char *function = "libsystem_directory_make";

#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	DWORD error_code      = 0;
#endif

	if( directory_name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid directory name.",
		 function );

		return( -1 );
	}
#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
/* TODO add pre Windows XP support */
	if( CreateDirectory(
	     directory_name,
	     NULL ) == 0 )
	{
		error_code = GetLastError();

		if( libsystem_error_string_copy_from_error_number(
		     error_string,
		     128,
		     error_code,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to make directory with error: %" PRIs_LIBCSTRING_SYSTEM "",
			 function,
			 error_string );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to make directory.",
			 function );
		}
		return( -1 );
	}
#elif defined( WINAPI )
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	if( _wmkdir(
	     directory_name ) != 0 )
#else
	if( _mkdir(
	     directory_name ) != 0 )
#endif
	{
		if( libsystem_error_string_copy_from_error_number(
		     error_string,
		     128,
		     errno,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to make directory with error: %" PRIs_LIBCSTRING_SYSTEM "",
			 function,
			 error_string );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to make directory.",
			 function );
		}
		return( -1 );
	}

#else
/* Sanity check
 */
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
#error Missing wide character mkdir function
#endif
	if( mkdir(
	     directory_name,
	     0755 ) != 0 )
	{
		if( libsystem_error_string_copy_from_error_number(
		     error_string,
		     128,
		     errno,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to make directory with error: %" PRIs_LIBCSTRING_SYSTEM "",
			 function,
			 error_string );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to make directory.",
			 function );
		}
		return( -1 );
	}

#endif
	return( 1 );
}

#endif /* defined( HAVE_MKDIR ) || defined( WINAPI ) */

