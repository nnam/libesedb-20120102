/*
 * String functions
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

#include <libcstring.h>
#include <liberror.h>

#include "libfvalue_split_string.h"
#include "libfvalue_string.h"
#include "libfvalue_types.h"

/* Copies an UTF-8 encoded string of a decimal value from a 8-bit value
 * Returns 1 if successful or -1 on error
 */
int libfvalue_utf8_string_decimal_copy_from_8bit(
     uint8_t *utf8_string,
     size_t utf8_string_size,
     size_t *utf8_string_index,
     uint8_t value_8bit,
     liberror_error_t **error )
{
	static char *function        = "libfvalue_utf8_string_decimal_copy_from_8bit";
	uint8_t divider              = 0;
	uint8_t number_of_characters = 0;

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
	if( utf8_string_index == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-8 string index.",
		 function );

		return( -1 );
	}
	if( *utf8_string_index >= utf8_string_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid UTF-8 string index value out of bounds.",
		 function );

		return( -1 );
	}
	divider              = 1;
	number_of_characters = 1;

	while( ( value_8bit / divider ) >= 10 )
	{
		divider *= 10;

		number_of_characters += 1;
	}
	if( ( *utf8_string_index + number_of_characters ) >= utf8_string_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-8 string size too small.",
		 function );

		return( -1 );
	}
	while( divider > 1 )
	{
		utf8_string[ *utf8_string_index ] = (uint8_t) '0' + (uint8_t) ( value_8bit / divider );

		*utf8_string_index += 1;

		value_8bit %= divider;

		divider /= 10;
	}
	utf8_string[ *utf8_string_index ] = (uint8_t) '0' + (uint8_t) ( value_8bit / divider );

	*utf8_string_index += 1;

	return( 1 );
}

/* Copies an UTF-8 encoded string of a decimal value to a 8-bit value
 * Returns 1 if successful or -1 on error
 */
int libfvalue_utf8_string_decimal_copy_to_8bit(
     const uint8_t *utf8_string,
     size_t utf8_string_size,
     uint8_t *value_8bit,
     liberror_error_t **error )
{
	static char *function        = "libfvalue_utf8_string_decimal_copy_to_8bit";
	size_t string_index          = 0;
	uint8_t character_value      = 0;
	uint8_t maximum_string_index = 3;
	int8_t sign                  = 1;

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
	*value_8bit = 0;

	if( utf8_string[ string_index ] == (uint8_t) '-' )
	{
		string_index++;
		maximum_string_index++;

		sign = -1;
	}
	else if( utf8_string[ string_index ] == (uint8_t) '+' )
	{
		string_index++;
		maximum_string_index++;
	}
	while( string_index < utf8_string_size )
	{
		if( utf8_string[ string_index ] == 0 )
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
		*value_8bit *= 10;

		if( ( utf8_string[ string_index ] >= (uint8_t) '0' )
		 && ( utf8_string[ string_index ] <= (uint8_t) '9' ) )
		{
			character_value = (uint8_t) ( utf8_string[ string_index ] - (uint8_t) '0' );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported character value: %c at index: %d.",
			 function,
			 (char) utf8_string[ string_index ],
			 string_index );

			return( -1 );
		}
		*value_8bit += character_value;

		string_index++;
	}
	if( sign == -1 )
	{
		*value_8bit *= (uint8_t) -1;
	}
	return( 1 );
}

/* Copies an UTF-8 encoded string of a decimal value from a 16-bit value
 * Returns 1 if successful or -1 on error
 */
int libfvalue_utf8_string_decimal_copy_from_16bit(
     uint8_t *utf8_string,
     size_t utf8_string_size,
     size_t *utf8_string_index,
     uint16_t value_16bit,
     liberror_error_t **error )
{
	static char *function        = "libfvalue_utf8_string_decimal_copy_from_16bit";
	uint16_t divider             = 0;
	uint8_t number_of_characters = 0;

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
	if( utf8_string_index == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-8 string index.",
		 function );

		return( -1 );
	}
	if( *utf8_string_index >= utf8_string_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid UTF-8 string index value out of bounds.",
		 function );

		return( -1 );
	}
	divider              = 1;
	number_of_characters = 1;

	while( ( value_16bit / divider ) >= 10 )
	{
		divider *= 10;

		number_of_characters += 1;
	}
	if( ( *utf8_string_index + number_of_characters ) >= utf8_string_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-8 string size too small.",
		 function );

		return( -1 );
	}
	while( divider > 1 )
	{
		utf8_string[ *utf8_string_index ] = (uint8_t) '0' + (uint8_t) ( value_16bit / divider );

		*utf8_string_index += 1;

		value_16bit %= divider;

		divider /= 10;
	}
	utf8_string[ *utf8_string_index ] = (uint8_t) '0' + (uint8_t) ( value_16bit / divider );

	*utf8_string_index += 1;

	return( 1 );
}

/* Copies an UTF-8 encoded string of a decimal value to a 16-bit value
 * Returns 1 if successful or -1 on error
 */
int libfvalue_utf8_string_decimal_copy_to_16bit(
     const uint8_t *utf8_string,
     size_t utf8_string_size,
     uint16_t *value_16bit,
     liberror_error_t **error )
{
	static char *function        = "libfvalue_utf8_string_decimal_copy_to_16bit";
	size_t string_index          = 0;
	uint8_t character_value      = 0;
	uint8_t maximum_string_index = 5;
	int8_t sign                  = 1;

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
	*value_16bit = 0;

	if( utf8_string[ string_index ] == (uint8_t) '-' )
	{
		string_index++;
		maximum_string_index++;

		sign = -1;
	}
	else if( utf8_string[ string_index ] == (uint8_t) '+' )
	{
		string_index++;
		maximum_string_index++;
	}
	while( string_index < utf8_string_size )
	{
		if( utf8_string[ string_index ] == 0 )
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
		*value_16bit *= 10;

		if( ( utf8_string[ string_index ] >= (uint8_t) '0' )
		 && ( utf8_string[ string_index ] <= (uint8_t) '9' ) )
		{
			character_value = (uint8_t) ( utf8_string[ string_index ] - (uint8_t) '0' );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported character value: %c at index: %d.",
			 function,
			 (char) utf8_string[ string_index ],
			 string_index );

			return( -1 );
		}
		*value_16bit += character_value;

		string_index++;
	}
	if( sign == -1 )
	{
		*value_16bit *= (uint16_t) -1;
	}
	return( 1 );
}

/* Copies an UTF-8 encoded string of a decimal value from a 32-bit value
 * Returns 1 if successful or -1 on error
 */
int libfvalue_utf8_string_decimal_copy_from_32bit(
     uint8_t *utf8_string,
     size_t utf8_string_size,
     size_t *utf8_string_index,
     uint32_t value_32bit,
     liberror_error_t **error )
{
	static char *function        = "libfvalue_utf8_string_decimal_copy_from_32bit";
	uint32_t divider             = 0;
	uint8_t number_of_characters = 0;

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
	if( utf8_string_index == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-8 string index.",
		 function );

		return( -1 );
	}
	if( *utf8_string_index >= utf8_string_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid UTF-8 string index value out of bounds.",
		 function );

		return( -1 );
	}
	divider              = 1;
	number_of_characters = 1;

	while( ( value_32bit / divider ) >= 10 )
	{
		divider *= 10;

		number_of_characters += 1;
	}
	if( ( *utf8_string_index + number_of_characters ) >= utf8_string_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-8 string size too small.",
		 function );

		return( -1 );
	}
	while( divider > 1 )
	{
		utf8_string[ *utf8_string_index ] = (uint8_t) '0' + (uint8_t) ( value_32bit / divider );

		*utf8_string_index += 1;

		value_32bit %= divider;

		divider /= 10;
	}
	utf8_string[ *utf8_string_index ] = (uint8_t) '0' + (uint8_t) ( value_32bit / divider );

	*utf8_string_index += 1;

	return( 1 );
}

/* Copies an UTF-8 encoded string of a decimal value to a 32-bit value
 * Returns 1 if successful or -1 on error
 */
int libfvalue_utf8_string_decimal_copy_to_32bit(
     const uint8_t *utf8_string,
     size_t utf8_string_size,
     uint32_t *value_32bit,
     liberror_error_t **error )
{
	static char *function        = "libfvalue_utf8_string_decimal_copy_to_32bit";
	size_t string_index          = 0;
	uint8_t character_value      = 0;
	uint8_t maximum_string_index = 10;
	int8_t sign                  = 1;

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
	*value_32bit = 0;

	if( utf8_string[ string_index ] == (uint8_t) '-' )
	{
		string_index++;
		maximum_string_index++;

		sign = -1;
	}
	else if( utf8_string[ string_index ] == (uint8_t) '+' )
	{
		string_index++;
		maximum_string_index++;
	}
	while( string_index < utf8_string_size )
	{
		if( utf8_string[ string_index ] == 0 )
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
		*value_32bit *= 10;

		if( ( utf8_string[ string_index ] >= (uint8_t) '0' )
		 && ( utf8_string[ string_index ] <= (uint8_t) '9' ) )
		{
			character_value = (uint8_t) ( utf8_string[ string_index ] - (uint8_t) '0' );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported character value: %c at index: %d.",
			 function,
			 (char) utf8_string[ string_index ],
			 string_index );

			return( -1 );
		}
		*value_32bit += character_value;

		string_index++;
	}
	if( sign == -1 )
	{
		*value_32bit *= (uint32_t) -1;
	}
	return( 1 );
}

/* Copies an UTF-8 encoded string of a decimal value from a 64-bit value
 * Returns 1 if successful or -1 on error
 */
int libfvalue_utf8_string_decimal_copy_from_64bit(
     uint8_t *utf8_string,
     size_t utf8_string_size,
     size_t *utf8_string_index,
     uint64_t value_64bit,
     liberror_error_t **error )
{
	static char *function        = "libfvalue_utf8_string_decimal_copy_from_64bit";
	uint64_t divider             = 0;
	uint8_t number_of_characters = 0;

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
	if( utf8_string_index == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-8 string index.",
		 function );

		return( -1 );
	}
	if( *utf8_string_index >= utf8_string_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid UTF-8 string index value out of bounds.",
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
	if( ( *utf8_string_index + number_of_characters ) >= utf8_string_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-8 string size too small.",
		 function );

		return( -1 );
	}
	while( divider > 1 )
	{
		utf8_string[ *utf8_string_index ] = (uint8_t) '0' + (uint8_t) ( value_64bit / divider );

		*utf8_string_index += 1;

		value_64bit %= divider;

		divider /= 10;
	}
	utf8_string[ *utf8_string_index ] = (uint8_t) '0' + (uint8_t) ( value_64bit / divider );

	*utf8_string_index += 1;

	return( 1 );
}

/* Copies an UTF-8 encoded string of a decimal value to a 64-bit value
 * Returns 1 if successful or -1 on error
 */
int libfvalue_utf8_string_decimal_copy_to_64bit(
     const uint8_t *utf8_string,
     size_t utf8_string_size,
     uint64_t *value_64bit,
     liberror_error_t **error )
{
	static char *function        = "libfvalue_utf8_string_decimal_copy_to_64bit";
	size_t string_index          = 0;
	uint8_t character_value      = 0;
	uint8_t maximum_string_index = 20;
	int8_t sign                  = 1;

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

	if( utf8_string[ string_index ] == (uint8_t) '-' )
	{
		string_index++;
		maximum_string_index++;

		sign = -1;
	}
	else if( utf8_string[ string_index ] == (uint8_t) '+' )
	{
		string_index++;
		maximum_string_index++;
	}
	while( string_index < utf8_string_size )
	{
		if( utf8_string[ string_index ] == 0 )
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

		if( ( utf8_string[ string_index ] >= (uint8_t) '0' )
		 && ( utf8_string[ string_index ] <= (uint8_t) '9' ) )
		{
			character_value = (uint8_t) ( utf8_string[ string_index ] - (uint8_t) '0' );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported character value: %c at index: %d.",
			 function,
			 (char) utf8_string[ string_index ],
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

/* Copies an UTF-8 encoded string of a hexadecimal value from a 64-bit value
 * Returns 1 if successful or -1 on error
 */
int libfvalue_utf8_string_hexadecimal_copy_from_64bit(
     uint8_t *utf8_string,
     size_t utf8_string_size,
     uint64_t value_64bit,
     liberror_error_t **error )
{
	static char *function = "libfvalue_utf8_string_hexadecimal_copy_from_64bit";
	size_t string_index   = 0;
	uint8_t byte_value    = 0;
	int8_t byte_shift     = 0;

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
	/* The smallest string is 0, with an end of string character
	 */
	if( utf8_string_size < 2 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-8 string is too small.",
		 function );

		return( -1 );
	}
	utf8_string[ string_index++ ] = (uint8_t) '0';

	if( value_64bit != 0 )
	{
		/* The smallest string is 0x#, with an end of string character
		 */
		if( utf8_string_size < 4 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: UTF-8 string is too small.",
			 function );

			return( -1 );
		}
		utf8_string[ string_index++ ] = (uint8_t) 'x';

		byte_shift = 60;

		do
		{
			byte_value = ( value_64bit >> byte_shift ) & 0x0f;

			if( ( byte_shift > 0 )
			 && ( byte_value == 0 ) )
			{
				continue;
			}
			if( string_index >= utf8_string_size )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-8 string is too small.",
				 function );

				return( -1 );
			}
			if( byte_value <= 9 )
			{
				utf8_string[ string_index++ ] = (uint8_t) '0' + byte_value;
			}
			else
			{
				utf8_string[ string_index++ ] = (uint8_t) 'a' + byte_value - 10;
			}
			byte_shift -= 4;
		}
		while( byte_shift >= 0 );

		if( string_index >= utf8_string_size )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: UTF-8 string is too small.",
			 function );

			return( -1 );
		}
	}
	utf8_string[ string_index++ ] = 0;

	return( 1 );
}

/* Copies an UTF-8 encoded string of a hexadecimal value to a 64-bit value
 * Returns 1 if successful or -1 on error
 */
int libfvalue_utf8_string_hexadecimal_copy_to_64bit(
     const uint8_t *utf8_string,
     size_t utf8_string_size,
     uint64_t *value_64bit,
     liberror_error_t **error )
{
	static char *function = "libfvalue_utf8_string_hexadecimal_copy_to_64bit";
	size_t string_index   = 0;
	uint8_t byte_value    = 0;

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
	if( ( utf8_string_size > 2 )
	 && ( utf8_string[ 0 ] == (uint8_t) '0' )
	 && ( utf8_string[ 1 ] == (uint8_t) 'x' ) )
	{
		string_index = 2;
	}
	*value_64bit = 0;

	while( string_index < utf8_string_size )
	{
		if( utf8_string[ string_index ] == 0 )
		{
			break;
		}
		if( string_index > (size_t) 20 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_LARGE,
			 "%s: string too large.",
			 function );

			return( -1 );
		}
		*value_64bit <<= 4;

		if( ( utf8_string[ string_index ] >= (uint8_t) '0' )
		 && ( utf8_string[ string_index ] <= (uint8_t) '9' ) )
		{
			byte_value = (uint8_t) ( utf8_string[ string_index ] - (uint8_t) '0' );
		}
		else if( ( utf8_string[ string_index ] >= (uint8_t) 'A' )
		      && ( utf8_string[ string_index ] <= (uint8_t) 'F' ) )
		{
			byte_value = (uint8_t) ( utf8_string[ string_index ] - (uint8_t) 'A' + 10 );
		}
		else if( ( utf8_string[ string_index ] >= (uint8_t) 'a' )
		      && ( utf8_string[ string_index ] <= (uint8_t) 'f' ) )
		{
			byte_value = (uint8_t) ( utf8_string[ string_index ] - (uint8_t) 'a' + 10 );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported character value: %c at index: %d.",
			 function,
			 (char) utf8_string[ string_index ],
			 string_index );

			return( -1 );
		}
		*value_64bit += byte_value;

		string_index++;
	}
	return( 1 );
}

/* Splits an UTF-8 string
 * Returns 1 if successful or -1 on error
 */
int libfvalue_utf8_string_split(
     const uint8_t *utf8_string,
     size_t utf8_string_size,
     uint8_t delimiter,
     libfvalue_split_utf8_string_t **split_string,
     liberror_error_t **error )
{
	uint8_t *segment_start = NULL;
	uint8_t *segment_end   = NULL;
	uint8_t *string_end    = NULL;
	static char *function  = "libfvalue_utf8_string_split";
	size_t string_size     = 0;
	ssize_t segment_length = 0;
	int number_of_segments = 0;
	int segment_index      = 0;

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
	if( ( utf8_string_size == 0 )
	 || ( utf8_string[ 0 ] == 0 ) )
	{
		return( 1 );
	}
	/* Determine the number of segments
	 */
	segment_start = (uint8_t *) utf8_string;
	string_end    = (uint8_t *) &( utf8_string[ utf8_string_size - 1 ] );

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
		else if( segment_end != utf8_string )
		{
			segment_start = segment_end + 1;
		}
	}
	while( segment_end != NULL );

	number_of_segments = segment_index;

	if( libfvalue_split_utf8_string_initialize(
	     split_string,
	     utf8_string,
	     utf8_string_size,
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
	if( libfvalue_split_utf8_string_get_string(
	     *split_string,
	     &segment_start,
	     &string_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve split UTF-8 string.",
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

			if( libfvalue_split_utf8_string_set_segment_by_index(
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
				 "%s: unable to set split UTF-8 string segment: %d.",
				 function,
				 segment_index );

				goto on_error;
			}
		}
		if( segment_end == NULL )
		{
			break;
		}
		if( segment_end == string_end )
		{
			segment_start++;
		}
		if( segment_end != string_end )
		{
			segment_start = segment_end + 1;
		}
	}
	return( 1 );

on_error:
	if( *split_string != NULL )
	{
		libfvalue_split_utf8_string_free(
		 split_string,
		 NULL );
	}
	return( -1 );
}

/* Copies an UTF-16 encoded string of a decimal value from a 8-bit value
 * Returns 1 if successful or -1 on error
 */
int libfvalue_utf16_string_decimal_copy_from_8bit(
     uint16_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     uint8_t value_8bit,
     liberror_error_t **error )
{
	static char *function        = "libfvalue_utf16_string_decimal_copy_from_8bit";
	uint8_t divider              = 0;
	uint8_t number_of_characters = 0;

	if( utf16_string == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 string.",
		 function );

		return( -1 );
	}
	if( utf16_string_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-16 string size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( utf16_string_index == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 string index.",
		 function );

		return( -1 );
	}
	if( *utf16_string_index >= utf16_string_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid UTF-16 string index value out of bounds.",
		 function );

		return( -1 );
	}
	divider              = 1;
	number_of_characters = 1;

	while( ( value_8bit / divider ) >= 10 )
	{
		divider *= 10;

		number_of_characters += 1;
	}
	if( ( *utf16_string_index + number_of_characters ) >= utf16_string_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-16 string size too small.",
		 function );

		return( -1 );
	}
	while( divider > 1 )
	{
		utf16_string[ *utf16_string_index ] = (uint16_t) '0' + (uint16_t) ( value_8bit / divider );

		*utf16_string_index += 1;

		value_8bit %= divider;

		divider /= 10;
	}
	utf16_string[ *utf16_string_index ] = (uint16_t) '0' + (uint16_t) ( value_8bit / divider );

	*utf16_string_index += 1;

	return( 1 );
}

/* Copies an UTF-16 encoded string of a decimal value to a 8-bit value
 * Returns 1 if successful or -1 on error
 */
int libfvalue_utf16_string_decimal_copy_to_8bit(
     const uint16_t *utf16_string,
     size_t utf16_string_size,
     uint8_t *value_8bit,
     liberror_error_t **error )
{
	static char *function        = "libfvalue_utf16_string_decimal_copy_to_8bit";
	size_t string_index          = 0;
	uint8_t character_value      = 0;
	uint8_t maximum_string_index = 3;
	int8_t sign                  = 1;

	if( utf16_string == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 string.",
		 function );

		return( -1 );
	}
	if( utf16_string_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-16 string size value exceeds maximum.",
		 function );

		return( -1 );
	}
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
	*value_8bit = 0;

	if( utf16_string[ string_index ] == (uint8_t) '-' )
	{
		string_index++;
		maximum_string_index++;

		sign = -1;
	}
	else if( utf16_string[ string_index ] == (uint8_t) '+' )
	{
		string_index++;
		maximum_string_index++;
	}
	while( string_index < utf16_string_size )
	{
		if( utf16_string[ string_index ] == 0 )
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
		*value_8bit *= 10;

		if( ( utf16_string[ string_index ] >= (uint8_t) '0' )
		 && ( utf16_string[ string_index ] <= (uint8_t) '9' ) )
		{
			character_value = (uint8_t) ( utf16_string[ string_index ] - (uint8_t) '0' );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported character value: %lc at index: %d.",
			 function,
			 (wchar_t) utf16_string[ string_index ],
			 string_index );

			return( -1 );
		}
		*value_8bit += character_value;

		string_index++;
	}
	if( sign == -1 )
	{
		*value_8bit *= (uint8_t) -1;
	}
	return( 1 );
}

/* Copies an UTF-16 encoded string of a decimal value from a 16-bit value
 * Returns 1 if successful or -1 on error
 */
int libfvalue_utf16_string_decimal_copy_from_16bit(
     uint16_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     uint16_t value_16bit,
     liberror_error_t **error )
{
	static char *function        = "libfvalue_utf16_string_decimal_copy_from_16bit";
	uint16_t divider             = 0;
	uint8_t number_of_characters = 0;

	if( utf16_string == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 string.",
		 function );

		return( -1 );
	}
	if( utf16_string_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-16 string size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( utf16_string_index == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 string index.",
		 function );

		return( -1 );
	}
	if( *utf16_string_index >= utf16_string_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid UTF-16 string index value out of bounds.",
		 function );

		return( -1 );
	}
	divider              = 1;
	number_of_characters = 1;

	while( ( value_16bit / divider ) >= 10 )
	{
		divider *= 10;

		number_of_characters += 1;
	}
	if( ( *utf16_string_index + number_of_characters ) >= utf16_string_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-16 string size too small.",
		 function );

		return( -1 );
	}
	while( divider > 1 )
	{
		utf16_string[ *utf16_string_index ] = (uint16_t) '0' + (uint16_t) ( value_16bit / divider );

		*utf16_string_index += 1;

		value_16bit %= divider;

		divider /= 10;
	}
	utf16_string[ *utf16_string_index ] = (uint16_t) '0' + (uint16_t) ( value_16bit / divider );

	*utf16_string_index += 1;

	return( 1 );
}

/* Copies an UTF-16 encoded string of a decimal value to a 16-bit value
 * Returns 1 if successful or -1 on error
 */
int libfvalue_utf16_string_decimal_copy_to_16bit(
     const uint16_t *utf16_string,
     size_t utf16_string_size,
     uint16_t *value_16bit,
     liberror_error_t **error )
{
	static char *function        = "libfvalue_utf16_string_decimal_copy_to_16bit";
	size_t string_index          = 0;
	uint8_t character_value      = 0;
	uint8_t maximum_string_index = 5;
	int8_t sign                  = 1;

	if( utf16_string == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 string.",
		 function );

		return( -1 );
	}
	if( utf16_string_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-16 string size value exceeds maximum.",
		 function );

		return( -1 );
	}
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
	*value_16bit = 0;

	if( utf16_string[ string_index ] == (uint8_t) '-' )
	{
		string_index++;
		maximum_string_index++;

		sign = -1;
	}
	else if( utf16_string[ string_index ] == (uint8_t) '+' )
	{
		string_index++;
		maximum_string_index++;
	}
	while( string_index < utf16_string_size )
	{
		if( utf16_string[ string_index ] == 0 )
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
		*value_16bit *= 10;

		if( ( utf16_string[ string_index ] >= (uint8_t) '0' )
		 && ( utf16_string[ string_index ] <= (uint8_t) '9' ) )
		{
			character_value = (uint8_t) ( utf16_string[ string_index ] - (uint8_t) '0' );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported character value: %lc at index: %d.",
			 function,
			 (wchar_t) utf16_string[ string_index ],
			 string_index );

			return( -1 );
		}
		*value_16bit += character_value;

		string_index++;
	}
	if( sign == -1 )
	{
		*value_16bit *= (uint16_t) -1;
	}
	return( 1 );
}

/* Copies an UTF-16 encoded string of a decimal value from a 32-bit value
 * Returns 1 if successful or -1 on error
 */
int libfvalue_utf16_string_decimal_copy_from_32bit(
     uint16_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     uint32_t value_32bit,
     liberror_error_t **error )
{
	static char *function        = "libfvalue_utf16_string_decimal_copy_from_32bit";
	uint32_t divider             = 0;
	uint8_t number_of_characters = 0;

	if( utf16_string == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 string.",
		 function );

		return( -1 );
	}
	if( utf16_string_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-16 string size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( utf16_string_index == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 string index.",
		 function );

		return( -1 );
	}
	if( *utf16_string_index >= utf16_string_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid UTF-16 string index value out of bounds.",
		 function );

		return( -1 );
	}
	divider              = 1;
	number_of_characters = 1;

	while( ( value_32bit / divider ) >= 10 )
	{
		divider *= 10;

		number_of_characters += 1;
	}
	if( ( *utf16_string_index + number_of_characters ) >= utf16_string_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-16 string size too small.",
		 function );

		return( -1 );
	}
	while( divider > 1 )
	{
		utf16_string[ *utf16_string_index ] = (uint16_t) '0' + (uint16_t) ( value_32bit / divider );

		*utf16_string_index += 1;

		value_32bit %= divider;

		divider /= 10;
	}
	utf16_string[ *utf16_string_index ] = (uint16_t) '0' + (uint16_t) ( value_32bit / divider );

	*utf16_string_index += 1;

	return( 1 );
}

/* Copies an UTF-16 encoded string of a decimal value to a 32-bit value
 * Returns 1 if successful or -1 on error
 */
int libfvalue_utf16_string_decimal_copy_to_32bit(
     const uint16_t *utf16_string,
     size_t utf16_string_size,
     uint32_t *value_32bit,
     liberror_error_t **error )
{
	static char *function        = "libfvalue_utf16_string_decimal_copy_to_32bit";
	size_t string_index          = 0;
	uint8_t character_value      = 0;
	uint8_t maximum_string_index = 10;
	int8_t sign                  = 1;

	if( utf16_string == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 string.",
		 function );

		return( -1 );
	}
	if( utf16_string_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-16 string size value exceeds maximum.",
		 function );

		return( -1 );
	}
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
	*value_32bit = 0;

	if( utf16_string[ string_index ] == (uint8_t) '-' )
	{
		string_index++;
		maximum_string_index++;

		sign = -1;
	}
	else if( utf16_string[ string_index ] == (uint8_t) '+' )
	{
		string_index++;
		maximum_string_index++;
	}
	while( string_index < utf16_string_size )
	{
		if( utf16_string[ string_index ] == 0 )
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
		*value_32bit *= 10;

		if( ( utf16_string[ string_index ] >= (uint8_t) '0' )
		 && ( utf16_string[ string_index ] <= (uint8_t) '9' ) )
		{
			character_value = (uint8_t) ( utf16_string[ string_index ] - (uint8_t) '0' );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported character value: %lc at index: %d.",
			 function,
			 (wchar_t) utf16_string[ string_index ],
			 string_index );

			return( -1 );
		}
		*value_32bit += character_value;

		string_index++;
	}
	if( sign == -1 )
	{
		*value_32bit *= (uint32_t) -1;
	}
	return( 1 );
}

/* Copies an UTF-16 encoded string of a decimal value from a 64-bit value
 * Returns 1 if successful or -1 on error
 */
int libfvalue_utf16_string_decimal_copy_from_64bit(
     uint16_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     uint64_t value_64bit,
     liberror_error_t **error )
{
	static char *function        = "libfvalue_utf16_string_decimal_copy_from_64bit";
	uint64_t divider             = 0;
	uint8_t number_of_characters = 0;

	if( utf16_string == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 string.",
		 function );

		return( -1 );
	}
	if( utf16_string_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-16 string size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( utf16_string_index == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 string index.",
		 function );

		return( -1 );
	}
	if( *utf16_string_index >= utf16_string_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid UTF-16 string index value out of bounds.",
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
	if( ( *utf16_string_index + number_of_characters ) >= utf16_string_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-16 string size too small.",
		 function );

		return( -1 );
	}
	while( divider > 1 )
	{
		utf16_string[ *utf16_string_index ] = (uint16_t) '0' + (uint16_t) ( value_64bit / divider );

		*utf16_string_index += 1;

		value_64bit %= divider;

		divider /= 10;
	}
	utf16_string[ *utf16_string_index ] = (uint16_t) '0' + (uint16_t) ( value_64bit / divider );

	*utf16_string_index += 1;

	return( 1 );
}

/* Copies an UTF-16 encoded string of a decimal value to a 64-bit value
 * Returns 1 if successful or -1 on error
 */
int libfvalue_utf16_string_decimal_copy_to_64bit(
     const uint16_t *utf16_string,
     size_t utf16_string_size,
     uint64_t *value_64bit,
     liberror_error_t **error )
{
	static char *function        = "libfvalue_utf16_string_decimal_copy_to_64bit";
	size_t string_index          = 0;
	uint8_t character_value      = 0;
	uint8_t maximum_string_index = 20;
	int8_t sign                  = 1;

	if( utf16_string == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 string.",
		 function );

		return( -1 );
	}
	if( utf16_string_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-16 string size value exceeds maximum.",
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

	if( utf16_string[ string_index ] == (uint8_t) '-' )
	{
		string_index++;
		maximum_string_index++;

		sign = -1;
	}
	else if( utf16_string[ string_index ] == (uint8_t) '+' )
	{
		string_index++;
		maximum_string_index++;
	}
	while( string_index < utf16_string_size )
	{
		if( utf16_string[ string_index ] == 0 )
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

		if( ( utf16_string[ string_index ] >= (uint8_t) '0' )
		 && ( utf16_string[ string_index ] <= (uint8_t) '9' ) )
		{
			character_value = (uint8_t) ( utf16_string[ string_index ] - (uint8_t) '0' );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported character value: %lc at index: %d.",
			 function,
			 (wchar_t) utf16_string[ string_index ],
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

/* Copies an UTF-16 encoded string of a hexadecimal value from a 64-bit value
 * Returns 1 if successful or -1 on error
 */
int libfvalue_utf16_string_hexadecimal_copy_from_64bit(
     uint16_t *utf16_string,
     size_t utf16_string_size,
     uint64_t value_64bit,
     liberror_error_t **error )
{
	static char *function = "libfvalue_utf16_string_hexadecimal_copy_from_64bit";
	size_t string_index   = 0;
	uint8_t byte_value    = 0;
	int8_t byte_shift     = 0;

	if( utf16_string == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 string.",
		 function );

		return( -1 );
	}
	if( utf16_string_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-16 string size value exceeds maximum.",
		 function );

		return( -1 );
	}
	/* The smallest string is 0, with an end of string character
	 */
	if( utf16_string_size < 2 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-16 string is too small.",
		 function );

		return( -1 );
	}
	utf16_string[ string_index++ ] = (uint16_t) '0';

	if( value_64bit != 0 )
	{
		/* The smallest string is 0x#, with an end of string character
		 */
		if( utf16_string_size < 4 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: UTF-16 string is too small.",
			 function );

			return( -1 );
		}
		utf16_string[ string_index++ ] = (uint16_t) 'x';

		byte_shift = 60;

		do
		{
			byte_value = ( value_64bit >> byte_shift ) & 0x0f;

			if( ( byte_shift > 0 )
			 && ( byte_value == 0 ) )
			{
				continue;
			}
			if( string_index >= utf16_string_size )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-16 string is too small.",
				 function );

				return( -1 );
			}
			if( byte_value <= 9 )
			{
				utf16_string[ string_index++ ] = (uint16_t) '0' + byte_value;
			}
			else
			{
				utf16_string[ string_index++ ] = (uint16_t) 'a' + byte_value - 10;
			}
			byte_shift -= 4;
		}
		while( byte_shift >= 0 );

		if( string_index >= utf16_string_size )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: UTF-16 string is too small.",
			 function );

			return( -1 );
		}
	}
	utf16_string[ string_index++ ] = 0;

	return( 1 );
}

/* Copies an UTF-16 encoded string of a hexadecimal value to a 64-bit value
 * Returns 1 if successful or -1 on error
 */
int libfvalue_utf16_string_hexadecimal_copy_to_64bit(
     const uint16_t *utf16_string,
     size_t utf16_string_size,
     uint64_t *value_64bit,
     liberror_error_t **error )
{
	static char *function = "libfvalue_utf16_string_hexadecimal_copy_to_64bit";
	size_t string_index   = 0;
	uint8_t byte_value    = 0;

	if( utf16_string == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 string.",
		 function );

		return( -1 );
	}
	if( utf16_string_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-16 string size value exceeds maximum.",
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
	if( ( utf16_string_size > 2 )
	 && ( utf16_string[ 0 ] == (uint16_t) '0' )
	 && ( utf16_string[ 1 ] == (uint16_t) 'x' ) )
	{
		string_index = 2;
	}
	*value_64bit = 0;

	while( string_index < utf16_string_size )
	{
		if( utf16_string[ string_index ] == 0 )
		{
			break;
		}
		if( string_index > (size_t) 20 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_LARGE,
			 "%s: string too large.",
			 function );

			return( -1 );
		}
		*value_64bit <<= 4;

		if( ( utf16_string[ string_index ] >= (uint16_t) '0' )
		 && ( utf16_string[ string_index ] <= (uint16_t) '9' ) )
		{
			byte_value = (uint8_t) ( utf16_string[ string_index ] - (uint16_t) '0' );
		}
		else if( ( utf16_string[ string_index ] >= (uint16_t) 'A' )
		      && ( utf16_string[ string_index ] <= (uint16_t) 'F' ) )
		{
			byte_value = (uint8_t) ( utf16_string[ string_index ] - (uint16_t) 'A' + 10 );
		}
		else if( ( utf16_string[ string_index ] >= (uint16_t) 'a' )
		      && ( utf16_string[ string_index ] <= (uint16_t) 'f' ) )
		{
			byte_value = (uint8_t) ( utf16_string[ string_index ] - (uint16_t) 'a' + 10 );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported character value: %c at index: %d.",
			 function,
			 (char) utf16_string[ string_index ],
			 string_index );

			return( -1 );
		}
		*value_64bit += byte_value;

		string_index++;
	}
	return( 1 );
}

/* Splits an UTF-16 string
 * Returns 1 if successful or -1 on error
 */
int libfvalue_utf16_string_split(
     const uint16_t *utf16_string,
     size_t utf16_string_size,
     uint16_t delimiter,
     libfvalue_split_utf16_string_t **split_string,
     liberror_error_t **error )
{
	uint16_t *segment_start = NULL;
	uint16_t *segment_end   = NULL;
	uint16_t *string_end    = NULL;
	static char *function   = "libfvalue_utf16_string_split";
	size_t string_size      = 0;
	ssize_t segment_length  = 0;
	int number_of_segments  = 0;
	int segment_index       = 0;

	if( utf16_string == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 string.",
		 function );

		return( -1 );
	}
	if( utf16_string_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-16 string size value exceeds maximum.",
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
	if( ( utf16_string_size == 0 )
	 || ( utf16_string[ 0 ] == 0 ) )
	{
		return( 1 );
	}
	/* Determine the number of segments
	 */
	segment_start = (uint16_t *) utf16_string;
	string_end    = (uint16_t *) &( utf16_string[ utf16_string_size - 1 ] );

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
		else if( segment_end != utf16_string )
		{
			segment_start = segment_end + 1;
		}
	}
	while( segment_end != NULL );

	number_of_segments = segment_index;

	if( libfvalue_split_utf16_string_initialize(
	     split_string,
	     utf16_string,
	     utf16_string_size,
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
	if( libfvalue_split_utf16_string_get_string(
	     *split_string,
	     &segment_start,
	     &string_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve split UTF-16 string.",
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

			if( libfvalue_split_utf16_string_set_segment_by_index(
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
				 "%s: unable to set split UTF-16 string segment: %d.",
				 function,
				 segment_index );

				goto on_error;
			}
		}
		if( segment_end == NULL )
		{
			break;
		}
		if( segment_end == string_end )
		{
			segment_start++;
		}
		if( segment_end != string_end )
		{
			segment_start = segment_end + 1;
		}
	}
	return( 1 );

on_error:
	if( *split_string != NULL )
	{
		libfvalue_split_utf16_string_free(
		 split_string,
		 NULL );
	}
	return( -1 );
}

