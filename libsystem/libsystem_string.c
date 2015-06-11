/*
 * Character string functions
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
#include <memory.h>
#include <types.h>

#include <libcstring.h>
#include <liberror.h>

#if defined( HAVE_ERRNO_H ) || defined( WINAPI )
#include <errno.h>
#endif

#if defined( HAVE_LOCALE_H ) || defined( WINAPI )
#include <locale.h>
#endif

#if defined( HAVE_LANGINFO_H )
#include <langinfo.h>
#endif

#include "libsystem_libuna.h"
#include "libsystem_split_string.h"
#include "libsystem_string.h"

/* Copies a string of a decimal value from a 64-bit value
 * Returns 1 if successful or -1 on error
 */
int libsystem_string_decimal_copy_from_64_bit(
     libcstring_system_character_t *string,
     size_t string_size,
     size_t *string_index,
     uint64_t value_64bit,
     liberror_error_t **error )
{
	static char *function        = "libsystem_string_decimal_copy_from_64_bit";
	uint64_t divider             = 0;
	uint8_t number_of_characters = 0;

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
	if( string_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid string size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( string_index == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid string index.",
		 function );

		return( -1 );
	}
	if( *string_index >= string_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid string index value out of bounds.",
		 function );

		return( -1 );
	}
	divider              = 1;
	number_of_characters = 1;

	while( ( value_64bit / divider ) >= 10 )
	{
		divider *= 10;

		number_of_characters += 1;
	}
	if( ( *string_index + number_of_characters ) >= string_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: string size too small.",
		 function );

		return( -1 );
	}
	while( divider > 1 )
	{
		string[ *string_index ] = (uint8_t) '0' + (uint8_t) ( value_64bit / divider );

		*string_index += 1;

		value_64bit %= divider;

		divider /= 10;
	}
	string[ *string_index ] = (uint8_t) '0' + (uint8_t) ( value_64bit / divider );

	*string_index += 1;

	return( 1 );
}

/* Copies a string of a decimal value to a 16-bit value
 * Returns 1 if successful or -1 on error
 */
int libsystem_string_decimal_copy_to_64_bit(
     const libcstring_system_character_t *string,
     size_t string_size,
     uint64_t *value_64bit,
     liberror_error_t **error )
{
	static char *function                         = "libsystem_string_decimal_copy_to_64_bit";
	size_t string_index                           = 0;
	libcstring_system_character_t character_value = 0;
	uint8_t maximum_string_index                  = 20;
	int8_t sign                                   = 1;

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
	if( string_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid string size value exceeds maximum.",
		 function );

		return( -1 );
	}
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
	*value_64bit = 0;

	if( string[ string_index ] == (libcstring_system_character_t) '-' )
	{
		string_index++;
		maximum_string_index++;

		sign = -1;
	}
	else if( string[ string_index ] == (libcstring_system_character_t) '+' )
	{
		string_index++;
		maximum_string_index++;
	}
	while( string_index < string_size )
	{
		if( string[ string_index ] == 0 )
		{
			break;
		}
		if( string_index > (size_t) maximum_string_index )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_LARGE,
			 "%s: string too large.",
			 function );

			return( -1 );
		}
		*value_64bit *= 10;

		if( ( string[ string_index ] >= (libcstring_system_character_t) '0' )
		 && ( string[ string_index ] <= (libcstring_system_character_t) '9' ) )
		{
			character_value = (libcstring_system_character_t) ( string[ string_index ] - (libcstring_system_character_t) '0' );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported character value: %" PRIc_LIBCSTRING_SYSTEM " at index: %d.",
			 function,
			 string[ string_index ],
			 string_index );

			return( -1 );
		}
		*value_64bit += character_value;

		string_index++;
	}
	if( sign == -1 )
	{
		*value_64bit *= (uint64_t) -1;
	}
	return( 1 );
}

/* Determines the system string size from the UTF-8 string
 * Returns 1 if successful or -1 on error
 */
int libsystem_string_size_from_utf8_string(
     const uint8_t *utf8_string,
     size_t utf8_string_size,
     size_t *string_size,
     liberror_error_t **error )
{
	static char *function = "libsystem_string_size_from_utf8_string";

	if( utf8_string == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-8 string.",
		 function );

		return( -1 );
	}
	if( utf8_string_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-8 string size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( string_size == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid string size.",
		 function );

		return( -1 );
	}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
#if SIZEOF_WCHAR_T == 4
	if( libuna_utf32_string_size_from_utf8(
	     (libuna_utf8_character_t *) utf8_string,
	     utf8_string_size,
	     string_size,
	     error ) != 1 )
#elif SIZEOF_WCHAR_T == 2
	if( libuna_utf16_string_size_from_utf8(
	     (libuna_utf8_character_t *) utf8_string,
	     utf8_string_size,
	     string_size,
	     error ) != 1 )
#endif
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to determine string size.",
		 function );

		return( -1 );
	}
#else
	if( libcstring_narrow_system_string_codepage == 0 )
	{
		*string_size = 1 + libcstring_narrow_string_length(
		                    (char *) utf8_string );
	}
	else if( libuna_byte_stream_size_from_utf8(
	          (libuna_utf8_character_t *) utf8_string,
	          utf8_string_size,
	          libcstring_narrow_system_string_codepage,
	          string_size,
	          error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to determine string size.",
		 function );

		return( -1 );
	}
#endif
	return( 1 );
}

/* Copies the system string size from the UTF-8 string
 * Returns 1 if successful or -1 on error
 */
int libsystem_string_copy_from_utf8_string(
     libcstring_system_character_t *string,
     size_t string_size,
     const uint8_t *utf8_string,
     size_t utf8_string_size,
     liberror_error_t **error )
{
	static char *function = "libsystem_string_copy_from_utf8_string";

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
	if( string_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid string size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( utf8_string == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-8 string.",
		 function );

		return( -1 );
	}
	if( utf8_string_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-8 string size value exceeds maximum.",
		 function );

		return( -1 );
	}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
#if SIZEOF_WCHAR_T == 4
	if( libuna_utf32_string_copy_from_utf8(
	     (libuna_utf32_character_t *) string,
	     string_size,
	     (libuna_utf8_character_t *) utf8_string,
	     utf8_string_size,
	     error ) != 1 )
#elif SIZEOF_WCHAR_T == 2
	if( libuna_utf16_string_copy_from_utf8(
	     (libuna_utf16_character_t *) string,
	     string_size,
	     (libuna_utf8_character_t *) utf8_string,
	     utf8_string_size,
	     error ) != 1 )
#endif
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to set string.",
		 function );

		return( -1 );
	}
#else
	if( libcstring_narrow_system_string_codepage == 0 )
	{
		if( string_size < utf8_string_size )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: string too small.",
			 function );

			return( -1 );
		}
		if( memory_copy(
		     string,
		     utf8_string,
		     utf8_string_size ) == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_COPY_FAILED,
			 "%s: unable to set string.",
			 function );

			return( -1 );
		}
	}
	else if( libuna_byte_stream_copy_from_utf8(
	          (uint8_t *) string,
	          string_size,
	          libcstring_narrow_system_string_codepage,
	          (libuna_utf8_character_t *) utf8_string,
	          utf8_string_size,
	          error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to set string.",
		 function );

		return( -1 );
	}
#endif
	return( 1 );
}

/* Determines the UTF-8 string size from the system string
 * Returns 1 if successful or -1 on error
 */
int libsystem_string_size_to_utf8_string(
     const libcstring_system_character_t *string,
     size_t string_size,
     size_t *utf8_string_size,
     liberror_error_t **error )
{
	static char *function = "libsystem_string_size_to_utf8_string";

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
	if( string_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid string size value exceeds maximum.",
		 function );

		return( -1 );
	}
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
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
#if SIZEOF_WCHAR_T == 4
	if( libuna_utf8_string_size_from_utf32(
	     (libuna_utf32_character_t *) string,
	     string_size,
	     utf8_string_size,
	     error ) != 1 )
#elif SIZEOF_WCHAR_T == 2
	if( libuna_utf8_string_size_from_utf16(
	     (libuna_utf16_character_t *) string,
	     string_size,
	     utf8_string_size,
	     error ) != 1 )
#endif
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to determine UTF-8 string size.",
		 function );

		return( -1 );
	}
#else
	if( libcstring_narrow_system_string_codepage == 0 )
	{
		*utf8_string_size = 1 + libcstring_system_string_length(
		                         string );
	}
	else if( libuna_utf8_string_size_from_byte_stream(
	          (uint8_t *) string,
	          string_size,
	          libcstring_narrow_system_string_codepage,
	          utf8_string_size,
	          error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to determine UTF-8 string size.",
		 function );

		return( -1 );
	}
#endif
	return( 1 );
}

/* Copies the UTF-8 string size from the system string
 * Returns 1 if successful or -1 on error
 */
int libsystem_string_copy_to_utf8_string(
     const libcstring_system_character_t *string,
     size_t string_size,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     liberror_error_t **error )
{
	static char *function = "libsystem_string_copy_to_utf8_string";

	if( utf8_string == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-8 string.",
		 function );

		return( -1 );
	}
	if( utf8_string_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-8 string size value exceeds maximum.",
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
	if( string_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid string size value exceeds maximum.",
		 function );

		return( -1 );
	}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
#if SIZEOF_WCHAR_T == 4
	if( libuna_utf8_string_copy_from_utf32(
	     (libuna_utf8_character_t *) utf8_string,
	     utf8_string_size,
	     (libuna_utf32_character_t *) string,
	     string_size,
	     error ) != 1 )
#elif SIZEOF_WCHAR_T == 2
	if( libuna_utf8_string_copy_from_utf16(
	     (libuna_utf8_character_t *) utf8_string,
	     utf8_string_size,
	     (libuna_utf16_character_t *) string,
	     string_size,
	     error ) != 1 )
#endif
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to set UTF-8 string.",
		 function );

		return( -1 );
	}
#else
	if( libcstring_narrow_system_string_codepage == 0 )
	{
		if( utf8_string_size < string_size )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: UTF-8 string too small.",
			 function );

			return( -1 );
		}
		if( memory_copy(
		     utf8_string,
		     string,
		     string_size ) == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_COPY_FAILED,
			 "%s: unable to set string.",
			 function );

			return( -1 );
		}
	}
	else if( libuna_utf8_string_copy_from_byte_stream(
	          (libuna_utf8_character_t *) utf8_string,
	          utf8_string_size,
	          (uint8_t *) string,
	          string_size,
	          libcstring_narrow_system_string_codepage,
	          error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to set UTF-8 string.",
		 function );

		return( -1 );
	}
#endif
	return( 1 );
}

/* Splits a string
 * Returns 1 if successful or -1 on error
 */
int libsystem_string_split(
     const libcstring_system_character_t *string,
     size_t string_size,
     libcstring_system_character_t delimiter,
     libsystem_split_string_t **split_string,
     liberror_error_t **error )
{
	libcstring_system_character_t *segment_start = NULL;
	libcstring_system_character_t *segment_end   = NULL;
	libcstring_system_character_t *string_end    = NULL;
	static char *function                        = "libsystem_string_split";
	ssize_t segment_length                       = 0;
	int number_of_segments                       = 0;
	int segment_index                            = 0;

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
	if( string_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid string size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( split_string == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid split string.",
		 function );

		return( -1 );
	}
	if( *split_string != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid split string already set.",
		 function );

		return( -1 );
	}
	/* An empty string has no segments
	 */
	if( ( string_size == 0 )
	 || ( string[ 0 ] == 0 ) )
	{
		return( 1 );
	}
	/* Determine the number of segments
	 */
	segment_start = (libcstring_system_character_t *) string;
	string_end    = (libcstring_system_character_t *) &( string[ string_size - 1 ] );

	do
	{
		segment_end = segment_start;

		while( segment_end <= string_end )
		{
			if( ( segment_end == string_end )
			 || ( *segment_end == 0 ) )
			{
				segment_end = NULL;

				break;
			}
			else if( *segment_end == delimiter )
			{
				break;
			}
			segment_end++;
		}
		if( segment_end > string_end )
		{
			break;
		}
		segment_index++;

		if( segment_end == NULL )
		{
			break;
		}
		if( segment_end == segment_start )
		{
			segment_start++;
		}
		else if( segment_end != string )
		{
			segment_start = segment_end + 1;
		}
	}
	while( segment_end != NULL );

	number_of_segments = segment_index;

	if( libsystem_split_string_initialize(
	     split_string,
	     string,
	     string_size,
	     number_of_segments,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to intialize split string.",
		 function );

		goto on_error;
	}
	if( *split_string == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing split string.",
		 function );

		goto on_error;
	}
	/* Do not bother splitting empty strings
	 */
	if( number_of_segments == 0 )
	{
		return( 1 );
	}
	/* Determine the segments
	 * empty segments are stored as strings only containing the end of character
	 */
	if( libsystem_split_string_get_string(
	     *split_string,
	     &segment_start,
	     &string_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve split string.",
		 function );

		goto on_error;
	}
	if( segment_start == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing segment start.",
		 function );

		goto on_error;
	}
	if( string_size < 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid string size value out of bounds.",
		 function );

		goto on_error;
	}
	segment_end = segment_start;
	string_end  = &( segment_start[ string_size - 1 ] );

	for( segment_index = 0;
	     segment_index < number_of_segments;
	     segment_index++ )
	{
		segment_end = segment_start;

		while( segment_end <= string_end )
		{
			if( ( segment_end == string_end )
			 || ( *segment_end == 0 ) )
			{
				segment_end = NULL;

				break;
			}
			else if( *segment_end == delimiter )
			{
				break;
			}
			segment_end++;
		}
		if( segment_end == NULL )
		{
			segment_length = (ssize_t) ( string_end - segment_start );
		}
		else
		{
			segment_length = (ssize_t) ( segment_end - segment_start );
		}
		if( segment_length >= 0 )
		{
			segment_start[ segment_length ] = 0;

			if( libsystem_split_string_set_segment_by_index(
			     *split_string,
			     segment_index,
			     segment_start,
			     segment_length + 1,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set split string segment: %d.",
				 function,
				 segment_index );

				goto on_error;
			}
		}
		if( segment_end == NULL )
		{
			break;
		}
		if( segment_end == ( *split_string )->string )
		{
			segment_start++;
		}
		if( segment_end != ( *split_string )->string )
		{
			segment_start = segment_end + 1;
		}
	}
	return( 1 );

on_error:
	if( *split_string != NULL )
	{
		libsystem_split_string_free(
		 split_string,
		 NULL );
	}
	return( -1 );
}

