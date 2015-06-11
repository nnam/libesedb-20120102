/*
 * Locale functions
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
#include <types.h>

#if defined( __BORLANDC__ ) && __BORLANDC__ <= 0x0520
#include <locale.h>
#elif defined( HAVE_LOCALE_H )
#include <locale.h>
#endif

#if defined( HAVE_LANGINFO_H )
#include <langinfo.h>
#endif

#if defined( HAVE_STDLIB_H ) || defined( WINAPI )
#include <stdlib.h>
#endif

#include "libcstring_definitions.h"
#include "libcstring_codepage.h"
#include "libcstring_locale.h"
#include "libcstring_narrow_string.h"
#include "libcstring_system_string.h"

#if defined( WINAPI ) && ( WINVER < 0x0500 )

/* Cross Windows safe version of GetLocaleInfoA
 * Returns the number of bytes read if successful or 0 on error
 */
int libcstring_GetLocaleInfoA(
     LCID locale,
     LCTYPE lctype,
     LPSTR buffer,
     int size )
{
	FARPROC function       = NULL;
	HMODULE library_handle = NULL;
	int result             = 0;

	library_handle = LoadLibrary(
	                  _LIBCSTRING_SYSTEM_STRING( "kernel32.dll" ) );

	if( library_handle == NULL )
	{
		return( 0 );
	}
	function = GetProcAddress(
		    library_handle,
		    (LPCSTR) "GetLocaleInfoA" );

	if( function != NULL )
	{
		result = function(
			  locale,
			  lctype,
			  buffer,
			  size );
	}
	/* This call should be after using the function
	 * in most cases kernel32.dll will still be available after free
	 */
	if( FreeLibrary(
	     library_handle ) != TRUE )
	{
		result = 0;
	}
	return( result );
}

#endif

/* Retrieves the codepage for the locale character set
 * Returns the codepage or 0 if the character set is UTF-8
 * The codepage will default to LIBCSTRING_CODEPAGE_ASCII the codepage cannot be determined
 */
int libcstring_locale_get_codepage(
     void )
{
	char *locale          = NULL;
	char *charset         = NULL;
	size_t charset_length = 0;
	size_t locale_length  = 0;
	int codepage          = 0;

#if defined( HAVE_LANGINFO_CODESET )
	charset = nl_langinfo(
	           CODESET );
#endif
	if( ( charset != NULL )
	 && ( charset[ 0 ] != 0 ) )
	{
		charset_length = libcstring_narrow_string_length(
		                  charset );
	}
	else
	{
#if defined( HAVE_SETLOCALE ) || ( defined( __BORLANDC__ ) && __BORLANDC__ <= 0x0520 )
		locale = setlocale(
			  LC_CTYPE,
			  "" );

		/* Check if the locale returned is not "C"
		 */
		if( ( locale != NULL )
		 && ( locale[ 0 ] != 0 ) )
		{
			if( ( locale[ 0 ] == 'C' )
			 && ( locale[ 1 ] != 0 ) )
			{
				locale = NULL;
			}
		}
#endif
#if defined( HAVE_GETENV ) || defined( WINAPI )
		/* Check if the locale can be determined from the environment variables
		 */
		if( ( locale == NULL )
		 || ( locale[ 0 ] == 0 ) )
		{
			locale = getenv(
				  "LC_ALL" );
		}
		if( ( locale == NULL )
		 || ( locale[ 0 ] == 0 ) )
		{
			locale = getenv(
				  "LC_TYPE" );
		}
		if( ( locale == NULL )
		 || ( locale[ 0 ] == 0 ) )
		{
			locale = getenv(
				  "LANG" );
		}
#endif
		if( ( locale == NULL )
		 || ( locale[ 0 ] == 0 ) )
		{
			return( LIBCSTRING_CODEPAGE_ASCII );
		}
		locale_length = libcstring_narrow_string_length(
				 locale );

		charset = libcstring_narrow_string_search_character(
			   locale,
			   '.',
			   locale_length + 1 );

		if( charset == NULL )
		{
			return( LIBCSTRING_CODEPAGE_ASCII );
		}
		charset++;

		charset_length = locale_length - (size_t) ( charset - locale );
	}
	/* Determine codepage
	 */
	codepage = LIBCSTRING_CODEPAGE_ASCII;

	if( codepage == LIBCSTRING_CODEPAGE_ASCII )
	{
		if( charset_length == 5 )
		{
			if( libcstring_narrow_string_compare(
			     "UTF-8",
			     charset,
			     5 ) == 0 )
			{
				codepage = 0;
			}
		}
	}
	if( codepage == LIBCSTRING_CODEPAGE_ASCII )
	{
		if( charset_length >= 3 )
		{
			if( libcstring_narrow_string_compare(
			     "874",
			     charset,
			     3 ) == 0 )
			{
				codepage = LIBCSTRING_CODEPAGE_WINDOWS_874;
			}
			else if( libcstring_narrow_string_compare(
			          "932",
			          charset,
			          3 ) == 0 )
			{
				codepage = LIBCSTRING_CODEPAGE_WINDOWS_932;
			}
			else if( libcstring_narrow_string_compare(
			          "936",
			          charset,
			          3 ) == 0 )
			{
				codepage = LIBCSTRING_CODEPAGE_WINDOWS_936;
			}
		}
	}
	if( codepage == LIBCSTRING_CODEPAGE_ASCII )
	{
		if( charset_length >= 4 )
		{
			if( libcstring_narrow_string_compare(
			     "1250",
			     charset,
			     4 ) == 0 )
			{
				codepage = LIBCSTRING_CODEPAGE_WINDOWS_1250;
			}
			else if( libcstring_narrow_string_compare(
				  "1251",
				  charset,
				  4 ) == 0 )
			{
				codepage = LIBCSTRING_CODEPAGE_WINDOWS_1251;
			}
			else if( libcstring_narrow_string_compare(
				  "1252",
				  charset,
				  4 ) == 0 )
			{
				codepage = LIBCSTRING_CODEPAGE_WINDOWS_1252;
			}
			else if( libcstring_narrow_string_compare(
				  "1253",
				  charset,
				  4 ) == 0 )
			{
				codepage = LIBCSTRING_CODEPAGE_WINDOWS_1253;
			}
			else if( libcstring_narrow_string_compare(
				  "1254",
				  charset,
				  4 ) == 0 )
			{
				codepage = LIBCSTRING_CODEPAGE_WINDOWS_1254;
			}
			else if( libcstring_narrow_string_compare(
				  "1255",
				  charset,
				  4 ) == 0 )
			{
				codepage = LIBCSTRING_CODEPAGE_WINDOWS_1255;
			}
			else if( libcstring_narrow_string_compare(
				  "1256",
				  charset,
				  4 ) == 0 )
			{
				codepage = LIBCSTRING_CODEPAGE_WINDOWS_1256;
			}
			else if( libcstring_narrow_string_compare(
				  "1257",
				  charset,
				  4 ) == 0 )
			{
				codepage = LIBCSTRING_CODEPAGE_WINDOWS_1257;
			}
			else if( libcstring_narrow_string_compare(
				  "1258",
				  charset,
				  4 ) == 0 )
			{
				codepage = LIBCSTRING_CODEPAGE_WINDOWS_1258;
			}
			else if( libcstring_narrow_string_compare(
				  "utf8",
				  charset,
				  4 ) == 0 )
			{
				codepage = 0;
			}
		}
	}
	return( codepage );
}

/* Retrieves the decimal point
 * Returns the decimal point string or -1 on error
 */
int libcstring_locale_get_decimal_point(
     void )
{
	int decimal_point         = -1;
#if defined( WINAPI )
	DWORD locale_data         = 0;
#else
	struct lconv *locale_data = NULL;
#endif

#if defined( WINAPI )
#if WINVER >= 0x0600
	if( GetLocaleInfoEx(
	     LOCALE_NAME_USER_DEFAULT,
	     LOCALE_SDECIMAL,
	     (LPWSTR) &locale_data,
	     sizeof( DWORD ) / sizeof( wchar_t ) ) == 0 )
	{
		return( -1 );
	}
#elif WINVER >= 0x0500
	if( GetLocaleInfoA(
	     LOCALE_USER_DEFAULT,
	     LOCALE_SDECIMAL,
	     (LPSTR) &locale_data,
	     sizeof( DWORD ) / sizeof( char ) ) == 0 )
	{
		return( -1 );
	}
#else
	if( libcstring_GetLocaleInfoA(
	     LOCALE_USER_DEFAULT,
	     LOCALE_SDECIMAL,
	     (LPSTR) &locale_data,
	     sizeof( DWORD ) / sizeof( char ) ) == 0 )
	{
		return( -1 );
	}
#endif
	decimal_point = (int) locale_data;
#else
	locale_data = localeconv();

	if( locale_data == NULL )
	{
		return( -1 );
	}
	if( locale_data->decimal_point == NULL )
	{
		return( -1 );
	}
	decimal_point = ( locale_data->decimal_point )[ 0 ];
#endif
	return( decimal_point );
}

