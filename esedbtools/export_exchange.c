/* 
 * Exchange database export functions
 *
 * Copyright (c) 2010, Joachim Metz <jbmetz@users.sourceforge.net>
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

/* Define HAVE_LOCAL_LIBFWNT for local use of libfwnt
 */
#if defined( HAVE_LOCAL_LIBFWNT )

#include <libfwnt_definitions.h>
#include <libfwnt_security_identifier.h>
#include <libfwnt_types.h>

#elif defined( HAVE_LIBFWNT_H )

/* If libtool DLL support is enabled set LIBFWNT_DLL_IMPORT
 * before including libfwnt.h
 */
#if defined( _WIN32 ) && defined( DLL_IMPORT )
#define LIBFWNT_DLL_IMPORT
#endif

#include <libfwnt.h>

#else
#error Missing libfwnt.h
#endif

/* Define HAVE_LOCAL_LIBFMAPI for local use of libfmapi
 */
#if defined( HAVE_LOCAL_LIBFMAPI )

#include <libfmapi_definitions.h>
#include <libfmapi_types.h>

#if defined( HAVE_DEBUG_OUTPUT )
#include <libfmapi_debug.h>
#endif

#elif defined( HAVE_LIBFMAPI_H )

/* If libtool DLL support is enabled set LIBFMAPI_DLL_IMPORT
 * before including libfmapi.h
 */
#if defined( _WIN32 ) && defined( DLL_IMPORT )
#define LIBFMAPI_DLL_IMPORT
#endif

#include <libfmapi.h>

#else
#error Missing libfmapi.h
#endif

#include <libsystem.h>

#include "esedbtools_libesedb.h"
#include "esedbtools_libfdatetime.h"
#include "esedbtools_libfguid.h"
#include "esedbtools_libuna.h"
#include "export.h"
#include "export_exchange.h"
#include "export_handle.h"

enum EXPORT_EXCHANGE_KNOWN_COLUMN_TYPES
{
	EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_UNDEFINED,
	EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_BINARY_DATA,
	EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_32BIT,
	EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_64BIT,
	EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_FILETIME,
	EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_GUID,
	EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_MAPI_ENTRYID,
	EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_MAPI_MULTI_VALUE,
	EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_SID,
	EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_STRING,
};

/* Exports a binary data table record value
 * Returns 1 if successful or -1 on error
 */
int export_exchange_record_binary_data(
     libesedb_record_t *record,
     int record_value_entry,
     FILE *record_file_stream,
     liberror_error_t **error )
{
	uint8_t *value_data    = NULL;
	static char *function  = "export_exchange_record_binary_data";
	size_t value_data_size = 0;
	uint32_t column_type   = 0;
	uint8_t value_flags    = 0;

#if defined( HAVE_DEBUG_OUTPUT ) && defined( HAVE_LOCAL_LIBFMAPI )
	libcstring_system_character_t column_name[ 256 ];

	int result             = 0;
#endif

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
	if( ( column_type != LIBESEDB_COLUMN_TYPE_BINARY_DATA )
	 && ( column_type != LIBESEDB_COLUMN_TYPE_LARGE_BINARY_DATA ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported column type: %" PRIu32 "",
		 function,
		 column_type );

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
	if( value_data != NULL )
	{
#if defined( HAVE_DEBUG_OUTPUT )
		if( libsystem_notify_verbose != 0 )
		{
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
			result = libesedb_record_get_utf16_column_name(
				  record,
				  record_value_entry,
				  (uint16_t *) column_name,
				  256,
				  error );
#else
			result = libesedb_record_get_utf8_column_name(
				  record,
				  record_value_entry,
				  (uint8_t *) column_name,
				  256,
				  error );
#endif
			if( result != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve column name of value: %d.",
				 function,
				 record_value_entry );

				return( -1 );
			}
			libsystem_notify_printf(
			 "%s: column name: %" PRIs_LIBCSTRING_SYSTEM "\n",
			 function,
			 column_name );

			libsystem_notify_print_data(
			 value_data,
			 value_data_size );
		}
#endif
		export_binary_data(
		 value_data,
		 value_data_size,
		 record_file_stream );
	}
	return( 1 );
}

/* Exports a 32-bit value in a binary data table record value
 * Returns 1 if successful or -1 on error
 */
int export_exchange_record_value_32bit(
     libesedb_record_t *record,
     int record_value_entry,
     uint8_t byte_order,
     FILE *record_file_stream,
     liberror_error_t **error )
{
	uint8_t *value_data    = NULL;
	static char *function  = "export_exchange_record_value_32bit";
	size_t value_data_size = 0;
	uint32_t column_type   = 0;
	uint32_t value_32bit   = 0;
	uint8_t value_flags    = 0;

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
	if( ( byte_order != _BYTE_STREAM_ENDIAN_BIG )
	 && ( byte_order != _BYTE_STREAM_ENDIAN_LITTLE ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported byte order: 0x%02" PRIx8 "",
		 function,
		 byte_order );

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
	if( column_type != LIBESEDB_COLUMN_TYPE_BINARY_DATA )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported column type: %" PRIu32 "",
		 function,
		 column_type );

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
		if( value_data != NULL )
		{
			if( value_data_size != 4 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
				 "%s: unsupported value data size: %" PRIzd "",
				 function,
				 value_data_size );

				return( -1 );
			}
			if( byte_order == _BYTE_STREAM_ENDIAN_BIG )
			{
				byte_stream_copy_to_uint32_big_endian(
				 value_data,
				 value_32bit );
			}
			else
			{
				byte_stream_copy_to_uint32_little_endian(
				 value_data,
				 value_32bit );
			}
			fprintf(
			 record_file_stream,
			 "%" PRIu32 "",
			 value_32bit );
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

/* Exports a 64-bit value in a binary data table record value
 * Returns 1 if successful or -1 on error
 */
int export_exchange_record_value_64bit(
     libesedb_record_t *record,
     int record_value_entry,
     uint8_t byte_order,
     FILE *record_file_stream,
     liberror_error_t **error )
{
	uint8_t *value_data    = NULL;
	static char *function  = "export_exchange_record_value_64bit";
	size_t value_data_size = 0;
	uint64_t value_64bit   = 0;
	uint32_t column_type   = 0;
	uint8_t value_flags    = 0;

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
	if( ( byte_order != _BYTE_STREAM_ENDIAN_BIG )
	 && ( byte_order != _BYTE_STREAM_ENDIAN_LITTLE ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported byte order: 0x%02" PRIx8 "",
		 function,
		 byte_order );

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
	if( ( column_type != LIBESEDB_COLUMN_TYPE_BINARY_DATA )
	 && ( column_type != LIBESEDB_COLUMN_TYPE_CURRENCY ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported column type: %" PRIu32 "",
		 function,
		 column_type );

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
		if( value_data != NULL )
		{
			if( value_data_size != 8 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
				 "%s: unsupported value data size: %" PRIzd "",
				 function,
				 value_data_size );

				return( -1 );
			}
			if( byte_order == _BYTE_STREAM_ENDIAN_BIG )
			{
				byte_stream_copy_to_uint64_big_endian(
				 value_data,
				 value_64bit );
			}
			else
			{
				byte_stream_copy_to_uint64_little_endian(
				 value_data,
				 value_64bit );
			}
			/* TODO for now print as hexadecimal */
			fprintf(
			 record_file_stream,
			 "0x%" PRIx64 "",
			 value_64bit );
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

/* Exports a filetime value in a binary data table record value
 * Returns 1 if successful or -1 on error
 */
int export_exchange_record_value_filetime(
     libesedb_record_t *record,
     int record_value_entry,
     uint8_t byte_order,
     FILE *record_file_stream,
     liberror_error_t **error )
{
	libcstring_system_character_t filetime_string[ 32 ];

	libfdatetime_filetime_t *filetime = NULL;
	uint8_t *value_data               = NULL;
	static char *function             = "export_exchange_record_value_filetime";
	size_t value_data_size            = 0;
	uint32_t column_type              = 0;
	uint8_t value_flags               = 0;
	int result                        = 0;

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
	if( ( column_type != LIBESEDB_COLUMN_TYPE_BINARY_DATA )
	 && ( column_type != LIBESEDB_COLUMN_TYPE_CURRENCY ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported column type: %" PRIu32 "",
		 function,
		 column_type );

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
		if( value_data != NULL )
		{
			if( value_data_size != 8 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
				 "%s: unsupported value data size: %" PRIzd "",
				 function,
				 value_data_size );

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
			if( libfdatetime_filetime_copy_from_byte_stream(
			     filetime,
			     value_data,
			     value_data_size,
			     byte_order,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy byte stream to filetime.",
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

/* Exports a GUID value in a binary data table record value
 * Returns 1 if successful or -1 on error
 */
int export_exchange_record_value_guid(
     libesedb_record_t *record,
     int record_value_entry,
     uint8_t byte_order,
     FILE *record_file_stream,
     liberror_error_t **error )
{
	libcstring_system_character_t guid_string[ LIBFGUID_IDENTIFIER_STRING_SIZE ];

	libfguid_identifier_t *guid = NULL;
	uint8_t *value_data         = NULL;
	static char *function       = "export_exchange_record_value_guid";
	size_t value_data_size      = 0;
	uint32_t column_type        = 0;
	uint8_t value_flags         = 0;
	int result                  = 0;

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
	if( column_type != LIBESEDB_COLUMN_TYPE_BINARY_DATA )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported column type: %" PRIu32 "",
		 function,
		 column_type );

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
		if( value_data != NULL )
		{
			if( value_data_size == 16 )
			{
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
				if( libfguid_identifier_copy_from_byte_stream(
				     guid,
				     value_data,
				     value_data_size,
				     byte_order,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
					 "%s: unable to copy byte stream to GUID.",
					 function );

					libfguid_identifier_free(
					 &guid,
					 NULL );

					return( -1 );
				}
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

					libfguid_identifier_free(
					 &guid,
					 NULL );

					return( -1 );
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
				fprintf(
				 record_file_stream,
				 "%" PRIs_LIBCSTRING_SYSTEM "",
				 guid_string );
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
	else
	{
		export_binary_data(
		 value_data,
		 value_data_size,
		 record_file_stream );
	}
	return( 1 );
}

/* Exports a MAPI ENTRYID value in a binary data table record value
 * Returns 1 if successful or -1 on error
 */
int export_exchange_record_value_mapi_entryid(
     libesedb_record_t *record,
     int record_value_entry,
     FILE *record_file_stream,
     liberror_error_t **error )
{
	uint8_t *value_data    = NULL;
	static char *function  = "export_exchange_record_value_mapi_entryid";
	size_t value_data_size = 0;
	uint32_t column_type   = 0;
	uint8_t value_flags    = 0;

#if defined( HAVE_DEBUG_OUTPUT ) && defined( HAVE_LOCAL_LIBFMAPI )
	libcstring_system_character_t column_name[ 256 ];

	int result             = 0;
#endif

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
	if( ( column_type != LIBESEDB_COLUMN_TYPE_BINARY_DATA )
	 && ( column_type != LIBESEDB_COLUMN_TYPE_LARGE_BINARY_DATA ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported column type: %" PRIu32 "",
		 function,
		 column_type );

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
		if( value_data != NULL )
		{
#if defined( HAVE_DEBUG_OUTPUT ) && defined( HAVE_LOCAL_LIBFMAPI )
			if( libsystem_notify_verbose != 0 )
			{
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
				result = libesedb_record_get_utf16_column_name(
					  record,
					  record_value_entry,
					  (uint16_t *) column_name,
					  256,
					  error );
#else
				result = libesedb_record_get_utf8_column_name(
					  record,
					  record_value_entry,
					  (uint8_t *) column_name,
					  256,
					  error );
#endif
				if( result != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve column name of value: %d.",
					 function,
					 record_value_entry );

					return( -1 );
				}
				libsystem_notify_printf(
				 "%s: column name: %" PRIs_LIBCSTRING_SYSTEM "\n",
				 function,
				 column_name );

				if( libfmapi_debug_print_entry_identifier(
				     value_data,
				     value_data_size,
				     LIBUNA_CODEPAGE_WINDOWS_1252,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_PRINT_FAILED,
					 "%s: unable to print entry identifier value: %d\n",
					 function,
					 record_value_entry );

					return( -1 );
				}
			}
#endif
#if defined( HAVE_DEBUG_OUTPUT )
			fprintf(
			 record_file_stream,
			 "ENTRYID: " );
#endif

			export_binary_data(
			 value_data,
			 value_data_size,
			 record_file_stream );
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

/* Exports a MAPI multi value value in a binary data table record value
 * Returns 1 if successful or -1 on error
 */
int export_exchange_record_value_mapi_multi_value(
     libesedb_record_t *record,
     int record_value_entry,
     FILE *record_file_stream,
     liberror_error_t **error )
{
	libesedb_multi_value_t *multi_value = NULL;
	uint8_t *value_data                 = NULL;
	static char *function               = "export_exchange_record_value_mapi_multi_value";
	size_t value_data_size              = 0;
	uint32_t column_type                = 0;
	uint8_t value_flags                 = 0;
	int multi_value_iterator            = 0;
	int number_of_multi_values          = 0;

#if defined( HAVE_DEBUG_OUTPUT ) && defined( HAVE_LOCAL_LIBFMAPI )
	libcstring_system_character_t column_name[ 256 ];

	int result                          = 0;
#endif

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
	if( ( column_type != LIBESEDB_COLUMN_TYPE_BINARY_DATA )
	 && ( column_type != LIBESEDB_COLUMN_TYPE_LARGE_BINARY_DATA ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported column type: %" PRIu32 "",
		 function,
		 column_type );

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
	/* TODO handle 0x10 flag */
	if( ( ( value_flags & LIBESEDB_VALUE_FLAG_MULTI_VALUE ) != 0 )
	 && ( ( value_flags & 0x10 ) == 0 ) )
	{
#if defined( HAVE_DEBUG_OUTPUT ) && defined( HAVE_LOCAL_LIBFMAPI )
		if( libsystem_notify_verbose != 0 )
		{
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
			result = libesedb_record_get_utf16_column_name(
				  record,
				  record_value_entry,
				  (uint16_t *) column_name,
				  256,
				  error );
#else
			result = libesedb_record_get_utf8_column_name(
				  record,
				  record_value_entry,
				  (uint8_t *) column_name,
				  256,
				  error );
#endif
			if( result != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve column name of value: %d.",
				 function,
				 record_value_entry );

				return( -1 );
			}
			libsystem_notify_printf(
			 "%s: column name: %" PRIs_LIBCSTRING_SYSTEM "\n",
			 function,
			 column_name );
		}
#endif
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

				libesedb_multi_value_free(
				 &multi_value,
				 NULL );

				return( -1 );
			}
			if( value_data != NULL )
			{
				/* TODO print entry index */

				libsystem_notify_print_data(
				 value_data,
				 value_data_size );
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

/* Exports a SID value in a binary data table record value
 * Returns 1 if successful or -1 on error
 */
int export_exchange_record_value_sid(
     libesedb_record_t *record,
     int record_value_entry,
     FILE *record_file_stream,
     liberror_error_t **error )
{
	libcstring_system_character_t sid_string[ 128 ];

	libfwnt_security_identifier_t *sid = NULL;
	uint8_t *value_data                = NULL;
	static char *function              = "export_exchange_record_value_sid";
	size_t sid_string_size             = 0;
	size_t value_data_size             = 0;
	uint32_t column_type               = 0;
	uint8_t value_flags                = 0;
	int result                         = 0;

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
	if( column_type != LIBESEDB_COLUMN_TYPE_BINARY_DATA )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported column type: %" PRIu32 "",
		 function,
		 column_type );

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
		if( value_data != NULL )
		{
			if( libfwnt_security_identifier_initialize(
			     &sid,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
				 "%s: unable to create SID.",
				 function );

				return( -1 );
			}
			if( libfwnt_security_identifier_copy_from_byte_stream(
			     sid,
			     value_data,
			     value_data_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy byte stream to SID.",
				 function );

				libfwnt_security_identifier_free(
				 &sid,
				 NULL );

				return( -1 );
			}
			result = libfwnt_security_identifier_get_string_size(
				  sid,
				  &sid_string_size,
				  error );

			if( result != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve SID string size.",
				 function );

				libfwnt_security_identifier_free(
				 &sid,
				 NULL );

				return( -1 );
			}
			/* It is assumed that the SID string cannot be larger than 127 characters
			 * otherwise using dynamic allocation is more appropriate
			 */
			if( sid_string_size > 128 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: SID string size value exceeds maximum.",
				 function );

				libfwnt_security_identifier_free(
				 &sid,
				 NULL );

				return( -1 );
			}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
			result = libfwnt_security_identifier_copy_to_utf16_string(
			          sid,
			          (uint16_t *) sid_string,
			          128,
			          error );
#else
			result = libfwnt_security_identifier_copy_to_utf8_string(
			          sid,
			          (uint8_t *) sid_string,
			          128,
			          error );
#endif
			if( result != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy SID to string.",
				 function );

				libfwnt_security_identifier_free(
				 &sid,
				 NULL );

				return( -1 );
			}
			if( libfwnt_security_identifier_free(
			     &sid,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free SID.",
				 function );

				return( -1 );
			}
			fprintf(
			 record_file_stream,
			 "%" PRIs_LIBCSTRING_SYSTEM "",
			 sid_string );
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

/* Exports a string in a binary data table record value
 * Returns 1 if successful or -1 on error
 */
int export_exchange_record_value_string(
     libesedb_record_t *record,
     int record_value_entry,
     FILE *record_file_stream,
     liberror_error_t **error )
{
	uint8_t *value_data    = NULL;
	static char *function  = "export_exchange_record_value_string";
	size_t value_data_size = 0;
	uint32_t column_type   = 0;
	uint8_t value_flags    = 0;

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
	if( ( column_type != LIBESEDB_COLUMN_TYPE_BINARY_DATA )
	 && ( column_type != LIBESEDB_COLUMN_TYPE_LARGE_BINARY_DATA ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported column type: %" PRIu32 "",
		 function,
		 column_type );

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
/* TODO UTF-16 string also possible ? */
	export_narrow_text(
	 (char *) value_data,
	 value_data_size,
	 record_file_stream );

	return( 1 );
}

/* Exports the values in a Folders table record
 * Returns 1 if successful or -1 on error
 */
int export_exchange_record_folders(
     libesedb_record_t *record,
     FILE *record_file_stream,
     log_handle_t *log_handle,
     liberror_error_t **error )
{
	libcstring_system_character_t column_name[ 256 ];

	static char *function   = "export_exchange_record_folders";
	size_t column_name_size = 0;
	uint32_t column_type    = 0;
	uint8_t byte_order      = _BYTE_STREAM_ENDIAN_LITTLE;
	int known_column_type   = 0;
	int number_of_values    = 0;
	int result              = 0;
	int value_iterator      = 0;

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
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
		result = libesedb_record_get_utf16_column_name_size(
		          record,
		          value_iterator,
		          &column_name_size,
		          error );

#else
		result = libesedb_record_get_utf8_column_name_size(
		          record,
		          value_iterator,
		          &column_name_size,
		          error );
#endif
		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve column name size of value: %d.",
			 function,
			 value_iterator );

			return( -1 );
		}
		/* It is assumed that the column name cannot be larger than 255 characters
		 * otherwise using dynamic allocation is more appropriate
		 */
		if( column_name_size > 256 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: column name size value exceeds maximum.",
			 function );

			return( -1 );
		}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
		result = libesedb_record_get_utf16_column_name(
		          record,
		          value_iterator,
		          (uint16_t *) column_name,
		          column_name_size,
		          error );
#else
		result = libesedb_record_get_utf8_column_name(
		          record,
		          value_iterator,
		          (uint8_t *) column_name,
		          column_name_size,
		          error );
#endif
		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve column name of value: %d.",
			 function,
			 value_iterator );

			return( -1 );
		}
		if( libesedb_record_get_column_type(
		     record,
		     value_iterator,
		     &column_type,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve column type of value: %d.",
			 function,
			 value_iterator );

			return( -1 );
		}
		known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_UNDEFINED;

		if( column_type == LIBESEDB_COLUMN_TYPE_CURRENCY )
		{
			if( ( column_name_size > 1 )
			 && ( column_name_size < 8 ) )
			{
				if( column_name[ 0 ] == (libcstring_character_t) 'T' )
				{
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_FILETIME;
				}
				else if( column_name[ 0 ] == (libcstring_character_t) 'Q' )
				{
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_64BIT;
				}
			}
		}
		else if( ( column_type == LIBESEDB_COLUMN_TYPE_BINARY_DATA )
		      || ( column_type == LIBESEDB_COLUMN_TYPE_LARGE_BINARY_DATA ) )
		{
			if( ( column_name_size > 1 )
			 && ( column_name_size < 8 ) )
			{
				if( column_name[ 0 ] == (libcstring_character_t) 'L' )
				{
/* TODO
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_32BIT;
*/
				}
				else if( column_name[ 0 ] == (libcstring_character_t) 'S' )
				{
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_STRING;
				}
				else if( column_name[ 0 ] == (libcstring_character_t) 'T' )
				{
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_FILETIME;
				}
				else if( column_name[ 0 ] == (libcstring_character_t) 'Q' )
				{
					byte_order        = _BYTE_STREAM_ENDIAN_BIG;
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_64BIT;
				}
				else if( column_name_size == 5 )
				{
					if( libcstring_system_string_compare(
					     column_name,
					     _LIBCSTRING_SYSTEM_STRING( "Ne58" ),
					     4 ) == 0 )
					{
						known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_SID;
					}
					else if( libcstring_system_string_compare(
					          column_name,
					          _LIBCSTRING_SYSTEM_STRING( "Ne59" ),
					          4 ) == 0 )
					{
						known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_SID;
					}
				}
				else if( column_name_size == 6 )
				{
					if( libcstring_system_string_compare(
					     column_name,
					     _LIBCSTRING_SYSTEM_STRING( "N3616" ),
					     5 ) == 0 )
					{
						known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_MAPI_ENTRYID;
					}
					else if( libcstring_system_string_compare(
					          column_name,
					          _LIBCSTRING_SYSTEM_STRING( "N36d0" ),
					          5 ) == 0 )
					{
						known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_MAPI_ENTRYID;
					}
					else if( libcstring_system_string_compare(
					          column_name,
					          _LIBCSTRING_SYSTEM_STRING( "N36d1" ),
					          5 ) == 0 )
					{
						known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_MAPI_ENTRYID;
					}
					else if( libcstring_system_string_compare(
					          column_name,
					          _LIBCSTRING_SYSTEM_STRING( "N36d2" ),
					          5 ) == 0 )
					{
						known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_MAPI_ENTRYID;
					}
					else if( libcstring_system_string_compare(
					          column_name,
					          _LIBCSTRING_SYSTEM_STRING( "N36d3" ),
					          5 ) == 0 )
					{
						known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_MAPI_ENTRYID;
					}
					else if( libcstring_system_string_compare(
					          column_name,
					          _LIBCSTRING_SYSTEM_STRING( "N36d4" ),
					          5 ) == 0 )
					{
						known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_MAPI_ENTRYID;
					}
					else if( libcstring_system_string_compare(
					          column_name,
					          _LIBCSTRING_SYSTEM_STRING( "N36d5" ),
					          5 ) == 0 )
					{
						known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_MAPI_ENTRYID;
					}
					else if( libcstring_system_string_compare(
					          column_name,
					          _LIBCSTRING_SYSTEM_STRING( "N36d7" ),
					          5 ) == 0 )
					{
						known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_MAPI_ENTRYID;
					}
					else if( libcstring_system_string_compare(
					          column_name,
					          _LIBCSTRING_SYSTEM_STRING( "N36dc" ),
					          5 ) == 0 )
					{
						known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_MAPI_ENTRYID;
					}
					else if( libcstring_system_string_compare(
					          column_name,
					          _LIBCSTRING_SYSTEM_STRING( "N3880" ),
					          5 ) == 0 )
					{
						known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_GUID;
					}
				}
				/* TODO add support for multi value entry identifiers MN36d8 and MN36e4 */
			}
		}
		if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_32BIT )
		{
			result = export_exchange_record_value_32bit(
				  record,
				  value_iterator,
				  byte_order,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_64BIT )
		{
			result = export_exchange_record_value_64bit(
				  record,
				  value_iterator,
				  byte_order,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_FILETIME )
		{
			result = export_exchange_record_value_filetime(
				  record,
				  value_iterator,
				  byte_order,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_GUID )
		{
			result = export_exchange_record_value_guid(
				  record,
				  value_iterator,
				  byte_order,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_MAPI_ENTRYID )
		{
			result = export_exchange_record_value_mapi_entryid(
				  record,
				  value_iterator,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_SID )
		{
			result = export_exchange_record_value_sid(
				  record,
				  value_iterator,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_STRING )
		{
			result = export_exchange_record_value_string(
				  record,
				  value_iterator,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_UNDEFINED )
		{
			result = export_handle_export_record_value(
				  record,
				  value_iterator,
				  record_file_stream,
				  log_handle,
				  error );
		}
		if( result != 1 )
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

/* Exports the values in a Global table record
 * Returns 1 if successful or -1 on error
 */
int export_exchange_record_global(
     libesedb_record_t *record,
     FILE *record_file_stream,
     log_handle_t *log_handle,
     liberror_error_t **error )
{
	libcstring_system_character_t column_name[ 256 ];

	static char *function   = "export_exchange_record_global";
	size_t column_name_size = 0;
	uint32_t column_type    = 0;
	uint8_t byte_order      = _BYTE_STREAM_ENDIAN_LITTLE;
	int known_column_type   = 0;
	int number_of_values    = 0;
	int result              = 0;
	int value_iterator      = 0;

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
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
		result = libesedb_record_get_utf16_column_name_size(
		          record,
		          value_iterator,
		          &column_name_size,
		          error );

#else
		result = libesedb_record_get_utf8_column_name_size(
		          record,
		          value_iterator,
		          &column_name_size,
		          error );
#endif
		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve column name size of value: %d.",
			 function,
			 value_iterator );

			return( -1 );
		}
		/* It is assumed that the column name cannot be larger than 255 characters
		 * otherwise using dynamic allocation is more appropriate
		 */
		if( column_name_size > 256 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: column name size value exceeds maximum.",
			 function );

			return( -1 );
		}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
		result = libesedb_record_get_utf16_column_name(
		          record,
		          value_iterator,
		          (uint16_t *) column_name,
		          column_name_size,
		          error );
#else
		result = libesedb_record_get_utf8_column_name(
		          record,
		          value_iterator,
		          (uint8_t *) column_name,
		          column_name_size,
		          error );
#endif
		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve column name of value: %d.",
			 function,
			 value_iterator );

			return( -1 );
		}
		if( libesedb_record_get_column_type(
		     record,
		     value_iterator,
		     &column_type,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve column type of value: %d.",
			 function,
			 value_iterator );

			return( -1 );
		}
		known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_UNDEFINED;

		if( column_type == LIBESEDB_COLUMN_TYPE_CURRENCY )
		{
			if( ( column_name_size > 1 )
			 && ( column_name_size < 8 ) )
			{
				if( column_name[ 0 ] == (libcstring_character_t) 'T' )
				{
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_FILETIME;
				}
				else if( column_name[ 0 ] == (libcstring_character_t) 'Q' )
				{
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_64BIT;
				}
			}
		}
		else if( ( column_type == LIBESEDB_COLUMN_TYPE_BINARY_DATA )
		      || ( column_type == LIBESEDB_COLUMN_TYPE_LARGE_BINARY_DATA ) )
		{
			if( ( column_name_size > 1 )
			 && ( column_name_size < 8 ) )
			{
				if( column_name[ 0 ] == (libcstring_character_t) 'L' )
				{
/* TODO
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_32BIT;
*/
				}
				else if( column_name[ 0 ] == (libcstring_character_t) 'S' )
				{
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_STRING;
				}
				else if( column_name[ 0 ] == (libcstring_character_t) 'T' )
				{
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_FILETIME;
				}
				else if( column_name[ 0 ] == (libcstring_character_t) 'Q' )
				{
					byte_order        = _BYTE_STREAM_ENDIAN_BIG;
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_64BIT;
				}
				else if( column_name_size == 6 )
				{
					if( libcstring_system_string_compare(
					     column_name,
					     _LIBCSTRING_SYSTEM_STRING( "N6762" ),
					     5 ) == 0 )
					{
						known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_GUID;
					}
					else if( libcstring_system_string_compare(
					          column_name,
					          _LIBCSTRING_SYSTEM_STRING( "N6768" ),
					          5 ) == 0 )
					{
						known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_STRING;
					}
					else if( libcstring_system_string_compare(
					          column_name,
					          _LIBCSTRING_SYSTEM_STRING( "N676a" ),
					          5 ) == 0 )
					{
						known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_GUID;
					}
					else if( libcstring_system_string_compare(
					          column_name,
					          _LIBCSTRING_SYSTEM_STRING( "N677f" ),
					          5 ) == 0 )
					{
						known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_BINARY_DATA;
					}
				}
				else if( column_name_size == 7 )
				{
					if( libcstring_system_string_compare(
					     column_name,
					     _LIBCSTRING_SYSTEM_STRING( "MN667f" ),
					     6 ) == 0 )
					{
						known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_MAPI_MULTI_VALUE;
					}
				}
			}
		}
		if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_BINARY_DATA )
		{
			result = export_exchange_record_binary_data(
				  record,
				  value_iterator,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_32BIT )
		{
			result = export_exchange_record_value_32bit(
				  record,
				  value_iterator,
				  byte_order,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_64BIT )
		{
			result = export_exchange_record_value_64bit(
				  record,
				  value_iterator,
				  byte_order,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_FILETIME )
		{
			result = export_exchange_record_value_filetime(
				  record,
				  value_iterator,
				  byte_order,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_GUID )
		{
			result = export_exchange_record_value_guid(
				  record,
				  value_iterator,
				  byte_order,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_MAPI_MULTI_VALUE )
		{
			result = export_exchange_record_value_mapi_multi_value(
				  record,
				  value_iterator,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_STRING )
		{
			result = export_exchange_record_value_string(
				  record,
				  value_iterator,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_UNDEFINED )
		{
			result = export_handle_export_record_value(
				  record,
				  value_iterator,
				  record_file_stream,
				  log_handle,
				  error );
		}
		if( result != 1 )
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


/* Exports the values in a Mailbox table record
 * Returns 1 if successful or -1 on error
 */
int export_exchange_record_mailbox(
     libesedb_record_t *record,
     FILE *record_file_stream,
     log_handle_t *log_handle,
     liberror_error_t **error )
{
	libcstring_system_character_t column_name[ 256 ];

	static char *function   = "export_exchange_record_mailbox";
	size_t column_name_size = 0;
	uint32_t column_type    = 0;
	uint8_t byte_order      = _BYTE_STREAM_ENDIAN_LITTLE;
	int known_column_type   = 0;
	int number_of_values    = 0;
	int result              = 0;
	int value_iterator      = 0;

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
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
		result = libesedb_record_get_utf16_column_name_size(
		          record,
		          value_iterator,
		          &column_name_size,
		          error );

#else
		result = libesedb_record_get_utf8_column_name_size(
		          record,
		          value_iterator,
		          &column_name_size,
		          error );
#endif
		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve column name size of value: %d.",
			 function,
			 value_iterator );

			return( -1 );
		}
		/* It is assumed that the column name cannot be larger than 255 characters
		 * otherwise using dynamic allocation is more appropriate
		 */
		if( column_name_size > 256 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: column name size value exceeds maximum.",
			 function );

			return( -1 );
		}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
		result = libesedb_record_get_utf16_column_name(
		          record,
		          value_iterator,
		          (uint16_t *) column_name,
		          column_name_size,
		          error );
#else
		result = libesedb_record_get_utf8_column_name(
		          record,
		          value_iterator,
		          (uint8_t *) column_name,
		          column_name_size,
		          error );
#endif
		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve column name of value: %d.",
			 function,
			 value_iterator );

			return( -1 );
		}
		if( libesedb_record_get_column_type(
		     record,
		     value_iterator,
		     &column_type,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve column type of value: %d.",
			 function,
			 value_iterator );

			return( -1 );
		}
		known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_UNDEFINED;

		if( column_type == LIBESEDB_COLUMN_TYPE_CURRENCY )
		{
			if( ( column_name_size > 1 )
			 && ( column_name_size < 8 ) )
			{
				if( column_name[ 0 ] == (libcstring_character_t) 'T' )
				{
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_FILETIME;
				}
				else if( column_name[ 0 ] == (libcstring_character_t) 'Q' )
				{
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_64BIT;
				}
			}
		}
		else if( ( column_type == LIBESEDB_COLUMN_TYPE_BINARY_DATA )
		      || ( column_type == LIBESEDB_COLUMN_TYPE_LARGE_BINARY_DATA ) )
		{
			if( ( column_name_size > 1 )
			 && ( column_name_size < 8 ) )
			{
				if( column_name[ 0 ] == (libcstring_character_t) 'L' )
				{
/* TODO
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_32BIT;
*/
				}
				else if( column_name[ 0 ] == (libcstring_character_t) 'S' )
				{
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_STRING;
				}
				else if( column_name[ 0 ] == (libcstring_character_t) 'T' )
				{
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_FILETIME;
				}
				else if( column_name[ 0 ] == (libcstring_character_t) 'Q' )
				{
					byte_order        = _BYTE_STREAM_ENDIAN_BIG;
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_64BIT;
				}
				else if( column_name_size == 6 )
				{
					if( libcstring_system_string_compare(
					     column_name,
					     _LIBCSTRING_SYSTEM_STRING( "N66a0" ),
					     5 ) == 0 )
					{
						known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_SID;
					}
					else if( libcstring_system_string_compare(
					          column_name,
					          _LIBCSTRING_SYSTEM_STRING( "N676a" ),
					          5 ) == 0 )
					{
						known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_GUID;
					}
					else if( libcstring_system_string_compare(
					          column_name,
					          _LIBCSTRING_SYSTEM_STRING( "N676c" ),
					          5 ) == 0 )
					{
						known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_GUID;
					}
				}
			}
		}
		if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_32BIT )
		{
			result = export_exchange_record_value_32bit(
				  record,
				  value_iterator,
				  byte_order,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_64BIT )
		{
			result = export_exchange_record_value_64bit(
				  record,
				  value_iterator,
				  byte_order,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_FILETIME )
		{
			result = export_exchange_record_value_filetime(
				  record,
				  value_iterator,
				  byte_order,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_GUID )
		{
			result = export_exchange_record_value_guid(
				  record,
				  value_iterator,
				  byte_order,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_SID )
		{
			result = export_exchange_record_value_sid(
				  record,
				  value_iterator,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_STRING )
		{
			result = export_exchange_record_value_string(
				  record,
				  value_iterator,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_UNDEFINED )
		{
			result = export_handle_export_record_value(
				  record,
				  value_iterator,
				  record_file_stream,
				  log_handle,
				  error );
		}
		if( result != 1 )
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

/* Exports the values in a Msg table record
 * Returns 1 if successful or -1 on error
 */
int export_exchange_record_msg(
     libesedb_record_t *record,
     FILE *record_file_stream,
     log_handle_t *log_handle,
     liberror_error_t **error )
{
	libcstring_system_character_t column_name[ 256 ];

	static char *function   = "export_exchange_record_msg";
	size_t column_name_size = 0;
	uint32_t column_type    = 0;
	uint8_t byte_order      = _BYTE_STREAM_ENDIAN_LITTLE;
	int known_column_type   = 0;
	int number_of_values    = 0;
	int result              = 0;
	int value_iterator      = 0;

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
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
		result = libesedb_record_get_utf16_column_name_size(
		          record,
		          value_iterator,
		          &column_name_size,
		          error );

#else
		result = libesedb_record_get_utf8_column_name_size(
		          record,
		          value_iterator,
		          &column_name_size,
		          error );
#endif
		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve column name size of value: %d.",
			 function,
			 value_iterator );

			return( -1 );
		}
		/* It is assumed that the column name cannot be larger than 255 characters
		 * otherwise using dynamic allocation is more appropriate
		 */
		if( column_name_size > 256 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: column name size value exceeds maximum.",
			 function );

			return( -1 );
		}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
		result = libesedb_record_get_utf16_column_name(
		          record,
		          value_iterator,
		          (uint16_t *) column_name,
		          column_name_size,
		          error );
#else
		result = libesedb_record_get_utf8_column_name(
		          record,
		          value_iterator,
		          (uint8_t *) column_name,
		          column_name_size,
		          error );
#endif
		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve column name of value: %d.",
			 function,
			 value_iterator );

			return( -1 );
		}
		if( libesedb_record_get_column_type(
		     record,
		     value_iterator,
		     &column_type,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve column type of value: %d.",
			 function,
			 value_iterator );

			return( -1 );
		}
		known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_UNDEFINED;

		if( column_type == LIBESEDB_COLUMN_TYPE_CURRENCY )
		{
			if( ( column_name_size > 1 )
			 && ( column_name_size < 8 ) )
			{
				if( column_name[ 0 ] == (libcstring_character_t) 'T' )
				{
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_FILETIME;
				}
				else if( column_name[ 0 ] == (libcstring_character_t) 'Q' )
				{
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_64BIT;
				}
			}
		}
		else if( ( column_type == LIBESEDB_COLUMN_TYPE_BINARY_DATA )
		      || ( column_type == LIBESEDB_COLUMN_TYPE_LARGE_BINARY_DATA ) )
		{
			if( ( column_name_size > 1 )
			 && ( column_name_size < 8 ) )
			{
				if( column_name[ 0 ] == (libcstring_character_t) 'L' )
				{
/* TODO
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_32BIT;
*/
				}
				else if( column_name[ 0 ] == (libcstring_character_t) 'S' )
				{
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_STRING;
				}
				else if( column_name[ 0 ] == (libcstring_character_t) 'T' )
				{
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_FILETIME;
				}
				else if( column_name[ 0 ] == (libcstring_character_t) 'Q' )
				{
					byte_order        = _BYTE_STREAM_ENDIAN_BIG;
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_64BIT;
				}
				else if( column_name_size == 6 )
				{
					if( libcstring_system_string_compare(
					     column_name,
					     _LIBCSTRING_SYSTEM_STRING( "N300b" ),
					     5 ) == 0 )
					{
						known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_GUID;
					}
					else if( libcstring_system_string_compare(
					          column_name,
					          _LIBCSTRING_SYSTEM_STRING( "N6720" ),
					          5 ) == 0 )
					{
						known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_STRING;
					}
				}
			}
		}
		if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_32BIT )
		{
			result = export_exchange_record_value_32bit(
				  record,
				  value_iterator,
				  byte_order,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_64BIT )
		{
			result = export_exchange_record_value_64bit(
				  record,
				  value_iterator,
				  byte_order,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_FILETIME )
		{
			result = export_exchange_record_value_filetime(
				  record,
				  value_iterator,
				  byte_order,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_GUID )
		{
			result = export_exchange_record_value_guid(
				  record,
				  value_iterator,
				  byte_order,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_SID )
		{
			result = export_exchange_record_value_sid(
				  record,
				  value_iterator,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_STRING )
		{
			result = export_exchange_record_value_string(
				  record,
				  value_iterator,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_UNDEFINED )
		{
			result = export_handle_export_record_value(
				  record,
				  value_iterator,
				  record_file_stream,
				  log_handle,
				  error );
		}
		if( result != 1 )
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

/* Exports the values in a PerUserRead table record
 * Returns 1 if successful or -1 on error
 */
int export_exchange_record_per_user_read(
     libesedb_record_t *record,
     FILE *record_file_stream,
     log_handle_t *log_handle,
     liberror_error_t **error )
{
	libcstring_system_character_t column_name[ 256 ];

	static char *function   = "export_exchange_record_per_user_read";
	size_t column_name_size = 0;
	uint32_t column_type    = 0;
	uint8_t byte_order      = _BYTE_STREAM_ENDIAN_LITTLE;
	int known_column_type   = 0;
	int number_of_values    = 0;
	int result              = 0;
	int value_iterator      = 0;

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
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
		result = libesedb_record_get_utf16_column_name_size(
		          record,
		          value_iterator,
		          &column_name_size,
		          error );

#else
		result = libesedb_record_get_utf8_column_name_size(
		          record,
		          value_iterator,
		          &column_name_size,
		          error );
#endif
		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve column name size of value: %d.",
			 function,
			 value_iterator );

			return( -1 );
		}
		/* It is assumed that the column name cannot be larger than 255 characters
		 * otherwise using dynamic allocation is more appropriate
		 */
		if( column_name_size > 256 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: column name size value exceeds maximum.",
			 function );

			return( -1 );
		}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
		result = libesedb_record_get_utf16_column_name(
		          record,
		          value_iterator,
		          (uint16_t *) column_name,
		          column_name_size,
		          error );
#else
		result = libesedb_record_get_utf8_column_name(
		          record,
		          value_iterator,
		          (uint8_t *) column_name,
		          column_name_size,
		          error );
#endif
		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve column name of value: %d.",
			 function,
			 value_iterator );

			return( -1 );
		}
		if( libesedb_record_get_column_type(
		     record,
		     value_iterator,
		     &column_type,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve column type of value: %d.",
			 function,
			 value_iterator );

			return( -1 );
		}
		known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_UNDEFINED;

		if( column_type == LIBESEDB_COLUMN_TYPE_CURRENCY )
		{
			if( ( column_name_size > 1 )
			 && ( column_name_size < 8 ) )
			{
				if( column_name[ 0 ] == (libcstring_character_t) 'T' )
				{
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_FILETIME;
				}
				else if( column_name[ 0 ] == (libcstring_character_t) 'Q' )
				{
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_64BIT;
				}
			}
		}
		else if( ( column_type == LIBESEDB_COLUMN_TYPE_BINARY_DATA )
		      || ( column_type == LIBESEDB_COLUMN_TYPE_LARGE_BINARY_DATA ) )
		{
			if( ( column_name_size > 1 )
			 && ( column_name_size < 8 ) )
			{
				if( column_name[ 0 ] == (libcstring_character_t) 'T' )
				{
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_FILETIME;
				}
				else if( column_name[ 0 ] == (libcstring_character_t) 'Q' )
				{
					byte_order        = _BYTE_STREAM_ENDIAN_BIG;
					known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_64BIT;
				}
				else if( column_name_size == 6 )
				{
					if( libcstring_system_string_compare(
					     column_name,
					     _LIBCSTRING_SYSTEM_STRING( "N676c" ),
					     5 ) == 0 )
					{
						known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_GUID;
					}
					else if( libcstring_system_string_compare(
					          column_name,
					          _LIBCSTRING_SYSTEM_STRING( "N67d0" ),
					          5 ) == 0 )
					{
						known_column_type = EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_GUID;
					}
				}
			}
		}
		if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_INTEGER_64BIT )
		{
			result = export_exchange_record_value_64bit(
				  record,
				  value_iterator,
				  byte_order,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_FILETIME )
		{
			result = export_exchange_record_value_filetime(
				  record,
				  value_iterator,
				  byte_order,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_GUID )
		{
			result = export_exchange_record_value_guid(
				  record,
				  value_iterator,
				  byte_order,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == EXPORT_EXCHANGE_KNOWN_COLUMN_TYPE_UNDEFINED )
		{
			result = export_handle_export_record_value(
				  record,
				  value_iterator,
				  record_file_stream,
				  log_handle,
				  error );
		}
		if( result != 1 )
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

