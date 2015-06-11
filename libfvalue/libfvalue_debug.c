/*
 * Debug functions
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
#include <memory.h>
#include <types.h>

#include <libcstring.h>
#include <liberror.h>
#include <libnotify.h>

#include "libfvalue_debug.h"
#include "libfvalue_definitions.h"
#include "libfvalue_value.h"

#if defined( HAVE_LIBFDATETIME_H ) || defined( HAVE_LOCAL_LIBFDATETIME )
#include "libfvalue_libfdatetime.h"
#endif

#if defined( HAVE_LIBFGUID_H ) || defined( HAVE_LOCAL_LIBFGUID )
#include "libfvalue_libfguid.h"
#endif

#if defined( HAVE_DEBUG_OUTPUT )

/* Prints the value
 * Returns 1 if successful or -1 on error
 */
int libfvalue_debug_print_value(
     libfvalue_value_t *value,
     uint8_t flags,
     liberror_error_t **error )
{
	libcstring_system_character_t *value_string = NULL;
	uint8_t *value_data                         = NULL;
	static char *function                       = "libfvalue_debug_print_value";
	size_t value_data_size                      = 0;
	size_t value_string_size                    = 0;
	double value_double                         = 0;
	float value_float                           = 0;
	uint64_t value_64bit                        = 0;
	uint32_t value_32bit                        = 0;
	uint16_t value_16bit                        = 0;
	uint8_t value_8bit                          = 0;
	uint8_t value_boolean                       = 0;
	uint8_t value_byte_order                    = 0;
	uint8_t value_type                          = 0;
	int number_of_value_entries                 = 0;
	int result                                  = 0;

#if defined( HAVE_LIBFDATETIME ) || defined( HAVE_LOCAL_LIBFDATETIME )
	libcstring_system_character_t date_time_string[ 32 ];

	libfdatetime_filetime_t *filetime           = NULL;
#endif

#if defined( HAVE_LIBFGUID ) || defined( HAVE_LOCAL_LIBFGUID )
	libcstring_system_character_t guid_string[ LIBFGUID_IDENTIFIER_STRING_SIZE ];

	libfguid_identifier_t *guid                 = NULL;
#endif

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
	if( ( flags & ~( LIBFVALUE_DEBUG_PRINT_FLAG_WITH_TYPE_DESCRIPTION ) ) != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported flags: 0x%02" PRIx8 ".",
		 function,
		 flags );

		return( -1 );
	}
	result = libfvalue_value_has_data(
	          value,
	          error );

	if( result == -1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to determine if value has data.",
		 function );

		return( -1 );
	}
	else if( result == 0 )
	{
		return( 1 );
	}
	if( libfvalue_value_get_type(
	     value,
	     &value_type,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value type.",
		 function );

		return( -1 );
	}
	if( libfvalue_value_get_number_of_value_entries(
	     value,
	     &number_of_value_entries,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of value entries.",
		 function );

		return( -1 );
	}
	result = 0;

	if( number_of_value_entries == 1 )
	{
		switch( value_type )
		{
			case LIBFVALUE_VALUE_TYPE_UNDEFINED:
			case LIBFVALUE_VALUE_TYPE_BINARY_DATA:
			case LIBFVALUE_VALUE_TYPE_ENUMERATION:
				break;

			case LIBFVALUE_VALUE_TYPE_BOOLEAN:
				result = libfvalue_value_copy_to_boolean(
				          value,
				          0,
				          &value_boolean,
				          error );

				if( result == -1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
					 "%s: unable to copy value to boolean value.",
					 function );

					if( error != NULL )
					{
						libnotify_print_error_backtrace(
						 *error );
					}
					liberror_error_free(
					 error );

					result = 0;
				}
				else if( result != 0 )
				{
					if( ( flags & LIBFVALUE_DEBUG_PRINT_FLAG_WITH_TYPE_DESCRIPTION ) == 0 )
					{
						libnotify_printf(
						 "%s",
						 ( value_boolean != 0 ) ? "true" : "false" );
					}
					else
					{
						libnotify_printf(
						 "Boolean\t: %s\n\n",
						 ( value_boolean != 0 ) ? "true" : "false" );
					}
				}
				break;

			case LIBFVALUE_VALUE_TYPE_INTEGER_8BIT:
			case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_8BIT:
				result = libfvalue_value_copy_to_8bit(
				          value,
				          0,
				          &value_8bit,
				          error );

				if( result == -1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
					 "%s: unable to copy value to 8-bit value.",
					 function );

					if( error != NULL )
					{
						libnotify_print_error_backtrace(
						 *error );
					}
					liberror_error_free(
					 error );

					result = 0;
				}
				else if( result != 0 )
				{
					if( ( flags & LIBFVALUE_DEBUG_PRINT_FLAG_WITH_TYPE_DESCRIPTION ) == 0 )
					{
						if( value_type == LIBFVALUE_VALUE_TYPE_INTEGER_8BIT )
						{
							libnotify_printf(
							 "%" PRIi8 "",
							 (int8_t) value_8bit );
						}
						else
						{
							libnotify_printf(
							 "%" PRIu8 "",
							 value_8bit );
						}
					}
					else
					{
						if( value_type == LIBFVALUE_VALUE_TYPE_INTEGER_8BIT )
						{
							libnotify_printf(
							 "Integer 8-bit signed\t: %" PRIi8 "\n\n",
							 (int8_t) value_8bit );
						}
						else
						{
							libnotify_printf(
							 "Integer 8-bit unsigned\t: %" PRIu8 "\n\n",
							 value_8bit );
						}
					}
				}
				break;

			case LIBFVALUE_VALUE_TYPE_INTEGER_16BIT:
			case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_16BIT:
				result = libfvalue_value_copy_to_16bit(
				          value,
				          0,
				          &value_16bit,
				          error );

				if( result == -1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
					 "%s: unable to copy value to 16-bit value.",
					 function );

					if( error != NULL )
					{
						libnotify_print_error_backtrace(
						 *error );
					}
					liberror_error_free(
					 error );

					result = 0;
				}
				else if( result != 0 )
				{
					if( ( flags & LIBFVALUE_DEBUG_PRINT_FLAG_WITH_TYPE_DESCRIPTION ) == 0 )
					{
						if( value_type == LIBFVALUE_VALUE_TYPE_INTEGER_16BIT )
						{
							libnotify_printf(
							 "%" PRIi16 "",
							 (int16_t) value_16bit );
						}
						else
						{
							libnotify_printf(
							 "%" PRIu16 "",
							 value_16bit );
						}
					}
					else
					{
						if( value_type == LIBFVALUE_VALUE_TYPE_INTEGER_16BIT )
						{
							libnotify_printf(
							 "Integer 16-bit signed\t: %" PRIi16 "\n\n",
							 (int16_t) value_16bit );
						}
						else
						{
							libnotify_printf(
							 "Integer 16-bit unsigned\t: %" PRIu16 "\n\n",
							 value_16bit );
						}
					}
				}
				break;

			case LIBFVALUE_VALUE_TYPE_INTEGER_32BIT:
			case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_32BIT:
				result = libfvalue_value_copy_to_32bit(
				          value,
				          0,
				          &value_32bit,
				          error );

				if( result == -1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
					 "%s: unable to copy value to 32-bit value.",
					 function );

					if( error != NULL )
					{
						libnotify_print_error_backtrace(
						 *error );
					}
					liberror_error_free(
					 error );

					result = 0;
				}
				else if( result != 0 )
				{
					if( ( flags & LIBFVALUE_DEBUG_PRINT_FLAG_WITH_TYPE_DESCRIPTION ) == 0 )
					{
						if( value_type == LIBFVALUE_VALUE_TYPE_INTEGER_32BIT )
						{
							libnotify_printf(
							 "%" PRIi32 "",
							 (int32_t) value_32bit );
						}
						else
						{
							libnotify_printf(
							 "%" PRIu32 "",
							 value_32bit );
						}
					}
					else
					{
						if( value_type == LIBFVALUE_VALUE_TYPE_INTEGER_32BIT )
						{
							libnotify_printf(
							 "Integer 32-bit signed\t: %" PRIi32 "\n\n",
							 (int32_t) value_32bit );
						}
						else
						{
							libnotify_printf(
							 "Integer 32-bit unsigned\t: %" PRIu32 "\n\n",
							 value_32bit );
						}
					}
				}
				break;

			case LIBFVALUE_VALUE_TYPE_INTEGER_64BIT:
			case LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_64BIT:
				result = libfvalue_value_copy_to_64bit(
				          value,
				          0,
				          &value_64bit,
				          error );

				if( result == -1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
					 "%s: unable to copy value to 64-bit value.",
					 function );

					if( error != NULL )
					{
						libnotify_print_error_backtrace(
						 *error );
					}
					liberror_error_free(
					 error );

					result = 0;
				}
				else if( result != 0 )
				{
					if( ( flags & LIBFVALUE_DEBUG_PRINT_FLAG_WITH_TYPE_DESCRIPTION ) == 0 )
					{
						if( value_type == LIBFVALUE_VALUE_TYPE_INTEGER_64BIT )
						{
							libnotify_printf(
							 "%" PRIi64 "",
							 (int64_t) value_64bit );
						}
						else
						{
							libnotify_printf(
							 "%" PRIu64 "",
							 value_64bit );
						}
					}
					else
					{
						if( value_type == LIBFVALUE_VALUE_TYPE_INTEGER_64BIT )
						{
							libnotify_printf(
							 "Integer 64-bit signed\t: %" PRIi64 "\n\n",
							 (int64_t) value_64bit );
						}
						else
						{
							libnotify_printf(
							 "Integer 64-bit unsigned\t: %" PRIu64 "\n\n",
							 value_64bit );
						}
					}
				}
				break;

			case LIBFVALUE_VALUE_TYPE_FLOATING_POINT_32BIT:
				result = libfvalue_value_copy_to_float(
				          value,
				          0,
				          &value_float,
				          error );

				if( result == -1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
					 "%s: unable to copy value to single precision floating point value.",
					 function );

					if( error != NULL )
					{
						libnotify_print_error_backtrace(
						 *error );
					}
					liberror_error_free(
					 error );

					result = 0;
				}
				else if( result != 0 )
				{
					if( ( flags & LIBFVALUE_DEBUG_PRINT_FLAG_WITH_TYPE_DESCRIPTION ) == 0 )
					{
						libnotify_printf(
						 "%f",
						 value_float );
					}
					else
					{
						libnotify_printf(
						 "Floating point single precision\t: %f\n\n",
						 value_float );
					}
				}
				break;

			case LIBFVALUE_VALUE_TYPE_FLOATING_POINT_64BIT:
				result = libfvalue_value_copy_to_double(
				          value,
				          0,
				          &value_double,
				          error );

				if( result == -1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
					 "%s: unable to copy value to double precision floating point value.",
					 function );

					if( error != NULL )
					{
						libnotify_print_error_backtrace(
						 *error );
					}
					liberror_error_free(
					 error );

					result = 0;
				}
				else if( result != 0 )
				{
					if( ( flags & LIBFVALUE_DEBUG_PRINT_FLAG_WITH_TYPE_DESCRIPTION ) == 0 )
					{
						libnotify_printf(
						 "%f",
						 value_double );
					}
					else
					{
						libnotify_printf(
						 "Floating point double precision\t: %f\n\n",
						 value_double );
					}
				}
				break;

			case LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM:
			case LIBFVALUE_VALUE_TYPE_STRING_UTF8:
			case LIBFVALUE_VALUE_TYPE_STRING_UTF16:
			case LIBFVALUE_VALUE_TYPE_STRING_UTF32:
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
				result = libfvalue_value_get_utf16_string_size(
				          value,
				          0,
				          &value_string_size,
				          error );
#else
				result = libfvalue_value_get_utf8_string_size(
				          value,
				          0,
				          &value_string_size,
				          error );
#endif
				if( result == -1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to determine value string size.",
					 function );

					if( error != NULL )
					{
						libnotify_print_error_backtrace(
						 *error );
					}
					liberror_error_free(
					 error );

					result = 0;
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

						return( -1 );
					}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
					result = libfvalue_value_copy_to_utf16_string(
						  value,
						  0,
						  (uint16_t *) value_string,
						  value_string_size,
						  error );
#else
					result = libfvalue_value_copy_to_utf8_string(
						  value,
						  0,
						  (uint8_t *) value_string,
						  value_string_size,
						  error );
#endif
					if( result == -1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
						 "%s: unable to copy value to string value.",
						 function );

						if( error != NULL )
						{
							libnotify_print_error_backtrace(
							 *error );
						}
						liberror_error_free(
						 error );

						result = 0;
					}
					else if( result != 0 )
					{
						if( ( flags & LIBFVALUE_DEBUG_PRINT_FLAG_WITH_TYPE_DESCRIPTION ) == 0 )
						{
							libnotify_printf(
							 "%" PRIs_LIBCSTRING_SYSTEM "",
							 value_string );
						}
						else if( value_type == LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM )
						{
							libnotify_printf(
							 "Byte stream string\t: %" PRIs_LIBCSTRING_SYSTEM "\n\n",
							 value_string );
						}
						else if( value_type == LIBFVALUE_VALUE_TYPE_STRING_UTF8 )
						{
							libnotify_printf(
							 "UTF-8 string\t: %" PRIs_LIBCSTRING_SYSTEM "\n\n",
							 value_string );
						}
						else if( value_type == LIBFVALUE_VALUE_TYPE_STRING_UTF16 )
						{
							libnotify_printf(
							 "UTF-16 string\t: %" PRIs_LIBCSTRING_SYSTEM "\n\n",
							 value_string );
						}
						else if( value_type == LIBFVALUE_VALUE_TYPE_STRING_UTF32 )
						{
							libnotify_printf(
							 "UTF-32 string\t: %" PRIs_LIBCSTRING_SYSTEM "\n\n",
							 value_string );
						}
					}
					memory_free(
					 value_string );
				}
				break;

#if defined( HAVE_LIBFDATETIME ) || defined( HAVE_LOCAL_LIBFDATETIME )
			case LIBFVALUE_VALUE_TYPE_FATDATE:
			case LIBFVALUE_VALUE_TYPE_FATDATETIME:
			case LIBFVALUE_VALUE_TYPE_FATTIME:
			case LIBFVALUE_VALUE_TYPE_NSF_TIMEDATE:
			case LIBFVALUE_VALUE_TYPE_HFSTIME:
			case LIBFVALUE_VALUE_TYPE_POSIX_TIME:
/* TODO */
				break;

			case LIBFVALUE_VALUE_TYPE_FILETIME:
				if( libfvalue_value_get_data(
				     value,
				     &value_data,
				     &value_data_size,
				     &value_byte_order,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve value data.",
					 function );

					return( -1 );
				}
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
				if( value_byte_order == LIBFVALUE_ENDIAN_NATIVE )
				{
					result = libfdatetime_filetime_copy_from_64bit(
					          filetime,
					          *( (uint64_t *) value_data ),
					          error );

					if( result != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
						 "%s: unable to copy filetime from 64-bit value.",
						 function );

						if( error != NULL )
						{
							libnotify_print_error_backtrace(
							 *error );
						}
						liberror_error_free(
						 error );

						result = 0;
					}
				}
				else
				{
					if( libfdatetime_filetime_copy_from_byte_stream(
					     filetime,
					     value_data,
					     value_data_size,
					     value_byte_order,
					     error ) != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
						 "%s: unable to copy filetime from byte stream.",
						 function );

						if( error != NULL )
						{
							libnotify_print_error_backtrace(
							 *error );
						}
						liberror_error_free(
						 error );

						result = 0;
					}
				}
				if( result != 0 )
				{
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
					result = libfdatetime_filetime_copy_to_utf16_string(
						  filetime,
						  (uint16_t *) date_time_string,
						  32,
						  LIBFDATETIME_STRING_FORMAT_FLAG_DATE_TIME_MICRO_SECONDS,
						  LIBFDATETIME_DATE_TIME_FORMAT_CTIME,
						  error );
#else
					result = libfdatetime_filetime_copy_to_utf8_string(
						  filetime,
						  (uint8_t *) date_time_string,
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

						if( error != NULL )
						{
							libnotify_print_error_backtrace(
							 *error );
						}
						liberror_error_free(
						 error );

						result = 0;
					}
					else
					{
						if( ( flags & LIBFVALUE_DEBUG_PRINT_FLAG_WITH_TYPE_DESCRIPTION ) == 0 )
						{
							libnotify_printf(
							 "%" PRIs_LIBCSTRING_SYSTEM "",
							 date_time_string );
						}
						else
						{
							libnotify_printf(
							 "Filetime\t: %" PRIs_LIBCSTRING_SYSTEM "\n",
							 date_time_string );
						}
					}
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
				break;
#endif /* #if defined( HAVE_LIBFDATETIME ) || defined( HAVE_LOCAL_LIBFDATETIME ) */

#if defined( HAVE_LIBFGUID ) || defined( HAVE_LOCAL_LIBFGUID )
			case LIBFVALUE_VALUE_TYPE_GUID:
				if( libfvalue_value_get_data(
				     value,
				     &value_data,
				     &value_data_size,
				     &value_byte_order,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve value data.",
					 function );

					return( -1 );
				}
				if( libfguid_identifier_initialize(
				     &guid,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
					 "%s: unable to create GUID.",
					 function );

					return( -1 );
				}
				result = libfguid_identifier_copy_from_byte_stream(
				          guid,
				          value_data,
				          value_data_size,
				          value_byte_order,
				          error );

				if( result == -1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
					 "%s: unable to copy GUID from byte stream.",
					 function );

					if( error != NULL )
					{
						libnotify_print_error_backtrace(
						 *error );
					}
					liberror_error_free(
					 error );

					result = 0;
				}
				if( result != 0 )
				{
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
					result = libfguid_identifier_copy_to_utf16_string(
					          guid,
					          (uint16_t *) guid_string,
					          LIBFGUID_IDENTIFIER_STRING_SIZE,
					          error );
#else
					result = libfguid_identifier_copy_to_utf8_string(
					          guid,
					          (uint8_t *) guid_string,
					          LIBFGUID_IDENTIFIER_STRING_SIZE,
					          error );
#endif
					if( result != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
						 "%s: unable to copy GUID to string.",
						 function );

						if( error != NULL )
						{
							libnotify_print_error_backtrace(
							 *error );
						}
						liberror_error_free(
						 error );

						result = 0;
					}
					else
					{
						if( ( flags & LIBFVALUE_DEBUG_PRINT_FLAG_WITH_TYPE_DESCRIPTION ) == 0 )
						{
							libnotify_printf(
							 "%" PRIs_LIBCSTRING_SYSTEM "",
							 guid_string );
						}
						else
						{
							libnotify_printf(
							 "GUID\t: %" PRIs_LIBCSTRING_SYSTEM "\n\n",
							 guid_string );
						}
					}
				}
				if( libfguid_identifier_free(
				     &guid,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
					 "%s: unable to free GUID.",
					 function );

					return( -1 );
				}
				break;
#endif /* #if defined( HAVE_LIBFGUID ) || defined( HAVE_LOCAL_LIBFGUID ) */

			default:
				break;
		}
	}
	if( result == 0 )
	{
		if( libfvalue_value_get_data(
		     value,
		     &value_data,
		     &value_data_size,
		     &value_byte_order,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve value data.",
			 function );

			return( -1 );
		}
		if( ( flags & LIBFVALUE_DEBUG_PRINT_FLAG_WITH_TYPE_DESCRIPTION ) != 0 )
		{
			libnotify_print_data(
			 value_data,
			 value_data_size );
		}
	}
	return( 1 );
}

#endif

