/* 
 * Windows Search database export functions
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
#include <byte_stream.h>
#include <file_stream.h>
#include <memory.h>
#include <types.h>

#include <libcstring.h>
#include <liberror.h>

#include <libsystem.h>

#include "esedbtools_libesedb.h"
#include "esedbtools_libfdatetime.h"
#include "esedbtools_libuna.h"
#include "export.h"
#include "export_handle.h"
#include "windows_search.h"

/* Define this if the debug output is not verbose enough
#define HAVE_EXTRA_DEBUG_OUTPUT
*/
#define HAVE_EXTRA_DEBUG_OUTPUT

enum WINDOWS_SEARCH_KNOWN_COLUMN_TYPES
{
	WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_UNDEFINED,
	WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_INTEGER_32BIT,
	WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_INTEGER_64BIT,
	WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_FILETIME,
	WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED,
	WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_UTF16_LITTLE_ENDIAN,
};

enum WINDOWS_SEARCH_FORMATS
{
	WINDOWS_SEARCH_FORMAT_DECIMAL,
	WINDOWS_SEARCH_FORMAT_HEXADECIMAL,
};

/* Decode data using Windows Search encoding
 * Returns 1 on success or -1 on error
 */
int windows_search_decode(
     uint8_t *data,
     size_t data_size,
     uint8_t *encoded_data, 
     size_t encoded_data_size,
     liberror_error_t **error )
{
	static char *function        = "windows_search_decode";
	size_t data_iterator         = 0;
	size_t encoded_data_iterator = 0;
	uint32_t bitmask32           = 0;
	uint8_t bitmask              = 0;

	if( encoded_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid encoded data.",
		 function );

		return( -1 );
	}
	if( encoded_data_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid encoded data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( data_size < encoded_data_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: data size value too small.",
		 function );

		return( -1 );
	}
	bitmask32 = 0x05000113 ^ (uint32_t) encoded_data_size;

	for( encoded_data_iterator = 0;
	     encoded_data_iterator < encoded_data_size;
	     encoded_data_iterator++ )
	{
		switch( encoded_data_iterator & 0x03 )
		{
			case 3:
				bitmask = (uint8_t) ( ( bitmask32 >> 24 ) & 0xff );
				break;
			case 2:
				bitmask = (uint8_t) ( ( bitmask32 >> 16 ) & 0xff );
				break;
			case 1:
				bitmask = (uint8_t) ( ( bitmask32 >> 8 ) & 0xff );
				break;
			default:
				bitmask = (uint8_t) ( bitmask32 & 0xff );
				break;
		}
		bitmask ^= encoded_data_iterator;

		data[ data_iterator++ ] = encoded_data[ encoded_data_iterator ]
		                        ^ bitmask;
	}
	return( 1 );
}

/* Determines the uncompressed size of a run-length compressed UTF-16 string
 * Returns 1 on success or -1 on error
 */
int windows_search_get_run_length_uncompressed_utf16_string_size(
     uint8_t *compressed_data,
     size_t compressed_data_size,
     size_t *uncompressed_data_size,
     liberror_error_t **error )
{
	static char *function           = "windows_search_get_run_length_uncompressed_utf16_string_size";
	size_t compressed_data_iterator = 0;
	uint8_t compression_size        = 0;

	if( compressed_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid compressed data.",
		 function );

		return( -1 );
	}
	if( compressed_data_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid compressed data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( uncompressed_data_size == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid uncompressed data size.",
		 function );

		return( -1 );
	}
	*uncompressed_data_size = 0;

	while( compressed_data_iterator < compressed_data_size )
	{
		if( compressed_data_iterator >= compressed_data_size )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: compressed data size value too small.",
			 function );

			*uncompressed_data_size = 0;

			return( -1 );
		}
		compression_size = compressed_data[ compressed_data_iterator++ ];

		/* Check if the last byte in the compressed string was the compression size
		 * or the run-length byte value
		 */
		if( ( compressed_data_iterator + 1 ) >= compressed_data_size )
		{
			break;
		}
		/* Check if the compressed string was cut-short at the end
		 */
		if( ( compressed_data_iterator + 1 + compression_size ) > compressed_data_size )
		{
#if defined( HAVE_DEBUG_OUTPUT )
fprintf( stderr, "MARKER: %zd, %d, %zd, %zd\n",
 compressed_data_iterator, compression_size, compressed_data_size,
 compressed_data_size - compressed_data_iterator - 1 );
#endif
			compression_size = (uint8_t) ( compressed_data_size - compressed_data_iterator - 1 );
		}
		*uncompressed_data_size  += compression_size * 2;
		compressed_data_iterator += compression_size + 1;
	}
	if( compressed_data_iterator > compressed_data_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: compressed data size value too small.",
		 function );

		*uncompressed_data_size = 0;

		return( -1 );
	}
	return( 1 );
}

/* Decompresses a run-length compressed UTF-16 string
 * Returns 1 on success or -1 on error
 */
int windows_search_decompress_run_length_compressed_utf16_string(
     uint8_t *uncompressed_data,
     size_t uncompressed_data_size,
     uint8_t *compressed_data,
     size_t compressed_data_size,
     liberror_error_t **error )
{
	static char *function             = "windows_search_decompress_run_length_compressed_utf16_string";
	size_t compressed_data_iterator   = 0;
	size_t uncompressed_data_iterator = 0;
	uint8_t compression_size          = 0;
	uint8_t compression_byte          = 0;

	if( uncompressed_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid uncompressed data.",
		 function );

		return( -1 );
	}
	if( uncompressed_data_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid uncompressed data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( compressed_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid compressed data.",
		 function );

		return( -1 );
	}
	if( compressed_data_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid compressed data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	while( compressed_data_iterator < compressed_data_size )
	{
		if( compressed_data_iterator >= compressed_data_size )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: compressed data size value too small.",
			 function );

			return( -1 );
		}
		compression_size = compressed_data[ compressed_data_iterator++ ];

		/* Check if the last byte in the compressed string was the compression size
		 * or the run-length byte value
		 */
		if( ( compressed_data_iterator + 1 ) >= compressed_data_size )
		{
			break;
		}
		/* Check if the compressed string was cut-short at the end
		 */
		if( ( compressed_data_iterator + 1 + compression_size ) > compressed_data_size )
		{
			compression_size = (uint8_t) ( compressed_data_size - compressed_data_iterator - 1 );
		}
		if( compressed_data_iterator >= compressed_data_size )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: compressed data size value too small.",
			 function );

			return( -1 );
		}
		compression_byte = compressed_data[ compressed_data_iterator++ ];

		while( compression_size > 0 )
		{
			if( compressed_data_iterator >= compressed_data_size )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: compressed data size value too small.",
				 function );

				return( -1 );
			}
			if( ( uncompressed_data_iterator + 1 ) >= uncompressed_data_size )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: uncompressed data size value too small.",
				 function );

				return( -1 );
			}
			uncompressed_data[ uncompressed_data_iterator++ ] = compressed_data[ compressed_data_iterator++ ];
			uncompressed_data[ uncompressed_data_iterator++ ] = compression_byte;

			compression_size--;
		}
	}
	return( 1 );
}

/* Determines the uncompressed data size of a run-length compressed UTF-16 string
 * Returns 1 on success or -1 on error
 */
int windows_search_get_byte_index_uncompressed_data_size(
     uint8_t *compressed_data,
     size_t compressed_data_size,
     size_t *uncompressed_data_size,
     liberror_error_t **error )
{
	static char *function                  = "windows_search_get_byte_index_uncompressed_size";
	uint16_t stored_uncompressed_data_size = 0;

	if( compressed_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid compressed data.",
		 function );

		return( -1 );
	}
	if( compressed_data_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid compressed data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( uncompressed_data_size == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid uncompressed data size.",
		 function );

		return( -1 );
	}
	/* The first 2 bytes contain the uncompressed data size
	 */
	byte_stream_copy_to_uint16_little_endian(
	 compressed_data,
	 stored_uncompressed_data_size );

	*uncompressed_data_size = (size_t) stored_uncompressed_data_size;

	return( 1 );
}

/* Decompresses byte-index compressed data
 * Returns 1 on success or -1 on error
 */
int windows_search_decompress_byte_indexed_compressed_data(
     uint8_t *uncompressed_data,
     size_t uncompressed_data_size,
     uint8_t *compressed_data, 
     size_t compressed_data_size,
     liberror_error_t **error )
{
	uint16_t compression_value_table[ 2048 ];

	uint32_t nibble_count_table[ 16 ]       = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	uint32_t total_nibble_count_table[ 16 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	static char *function                   = "windows_search_decompress_byte_indexed_compressed_data";
	size_t compressed_data_iterator         = 0;
	size_t compression_iterator             = 0;
	size_t uncompressed_data_iterator       = 0;

	uint32_t compressed_data_bit_stream     = 0;
	uint32_t compression_offset             = 0;
	uint32_t nibble_count                   = 0;
	uint32_t total_nibble_count             = 0;
	uint32_t value_32bit                    = 0;
	int32_t compression_value_table_index   = 0;
	uint16_t compression_size               = 0;
	uint16_t compression_value              = 0;
	uint16_t stored_uncompressed_data_size  = 0;
	uint16_t value_0x0400                   = 0;
	uint16_t value_0x0800                   = 0;
	uint16_t value_0x2000                   = 0;
	uint8_t nibble_count_table_index        = 0;
	int8_t number_of_bits_available         = 0;
	int8_t number_of_bits_used              = 0;

	if( uncompressed_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid uncompressed data.",
		 function );

		return( -1 );
	}
	if( uncompressed_data_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid uncompressed data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( compressed_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid compressed data.",
		 function );

		return( -1 );
	}
	if( compressed_data_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid compressed data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( compressed_data_size <= 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: compressed data size value too small.",
		 function );

		return( -1 );
	}
	if( memset(
	     compression_value_table,
	     0,
	     2048 * 2 ) == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear compression value table.",
		 function );

		return( -1 );
	}
#if defined( HAVE_DEBUG_OUTPUT ) && defined( HAVE_EXTRA_DEBUG_OUTPUT )
	if( libsystem_notify_verbose != 0 )
	{
		libsystem_notify_printf(
		 "%s: compressed data header:\n",
		 function );
		libsystem_notify_print_data(
		 compressed_data,
		 258 );
	}
#endif
	/* Byte 0 - 1 contain the uncompressed data size
	 */
	byte_stream_copy_to_uint16_little_endian(
	 compressed_data,
	 stored_uncompressed_data_size );

	if( uncompressed_data_size < stored_uncompressed_data_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: uncompressed data size value too small.",
		 function );

		return( -1 );
	}
	/* Byte 2 - 257 contain the compression table
	 *
	 * The table contains a compression value for every byte
	 * bits 0 - 3 contain ???
	 * bits 4 - 7 contain the number of bits used to store the compressed data
	 */
	for( compressed_data_iterator = 0;
	     compressed_data_iterator < 256;
	     compressed_data_iterator++ )
	{
		nibble_count_table_index = compressed_data[ 2 + compressed_data_iterator ];

		nibble_count_table[ nibble_count_table_index & 0x0f ] += 1;
		nibble_count_table[ nibble_count_table_index >> 4 ]   += 1;
	}
#if defined( HAVE_DEBUG_OUTPUT ) && defined( HAVE_EXTRA_DEBUG_OUTPUT )
	if( libsystem_notify_verbose != 0 )
	{
		libsystem_notify_printf(
		 "%s: uncompressed data size:\t%" PRIu16 "\n",
		 function,
		 stored_uncompressed_data_size );

		for( nibble_count_table_index = 0;
		     nibble_count_table_index < 16;
		     nibble_count_table_index++ )
		{
			libsystem_notify_printf(
			 "%s: nibble count table index: %02d value:\t\t0x%08" PRIx32 " (%" PRIu32 ")\n",
			 function,
			 nibble_count_table_index,
			 nibble_count_table[ nibble_count_table_index ],
			 nibble_count_table[ nibble_count_table_index ] );
		}
		libsystem_notify_printf(
		 "\n" );
	}
#endif
	if( nibble_count_table[ 0 ] >= 0x01ff )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: first nibble count table entry value exceeds maximum.",
		 function );

		return( -1 );
	}
	/* Make copy of the nibble count table
	 */
	for( nibble_count_table_index = 0;
	     nibble_count_table_index < 16;
	     nibble_count_table_index++ )
	{
		total_nibble_count_table[ nibble_count_table_index ] = nibble_count_table[ nibble_count_table_index ];
	}
	/* TODO why this loop */
	nibble_count = 0;

	for( nibble_count_table_index = 15;
	     nibble_count_table_index > 0;
	     nibble_count_table_index-- )
	{
		nibble_count += total_nibble_count_table[ nibble_count_table_index ];

		if( nibble_count == 1 )
		{
			break;
		}
		nibble_count >>= 1;
	}
	if( nibble_count != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: nibble count value exceeds maximum.",
		 function );

		return( -1 );
	}
	/* Determine the total nible counts
	 */
	nibble_count = 0;

#if defined( HAVE_DEBUG_OUTPUT ) && defined( HAVE_EXTRA_DEBUG_OUTPUT )
	if( libsystem_notify_verbose != 0 )
	{
		libsystem_notify_printf(
		 "%s: total nibble count table index: %02d value:\t0x%08" PRIx32 " (%" PRIu32 ")\n",
		 function,
		 0,
		 total_nibble_count_table[ 0 ],
		 total_nibble_count_table[ 0 ] );
	}
#endif

	for( nibble_count_table_index = 1;
	     nibble_count_table_index < 16;
	     nibble_count_table_index++ )
	{
		total_nibble_count_table[ nibble_count_table_index ] += nibble_count;
		nibble_count                                          = total_nibble_count_table[ nibble_count_table_index ];

#if defined( HAVE_DEBUG_OUTPUT ) && defined( HAVE_EXTRA_DEBUG_OUTPUT )
		if( libsystem_notify_verbose != 0 )
		{
			libsystem_notify_printf(
			 "%s: total nibble count table index: %02d value:\t0x%08" PRIx32 " (%" PRIu32 ")\n",
			 function,
			 nibble_count_table_index,
			 total_nibble_count_table[ nibble_count_table_index ],
			 total_nibble_count_table[ nibble_count_table_index ] );
		}
#endif
	}
#if defined( HAVE_DEBUG_OUTPUT ) && defined( HAVE_EXTRA_DEBUG_OUTPUT )
	if( libsystem_notify_verbose != 0 )
	{
		libsystem_notify_printf(
		 "\n" );
	}
#endif

	total_nibble_count = nibble_count;

	/* Fill the compression value table
	 */
	value_0x2000 = 0x2000;

	while( value_0x2000 > 0 )
	{
		value_0x2000 -= 0x10;

		compressed_data_iterator = value_0x2000 >> 5;

		nibble_count_table_index = compressed_data[ 2 + compressed_data_iterator ] >> 4;

		if( nibble_count_table_index > 0 )
		{
			total_nibble_count_table[ nibble_count_table_index ] -= 1;
			compression_value_table_index                         = total_nibble_count_table[ nibble_count_table_index ];

			if( compression_value_table_index > 2048 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: compression value table index value exceeds maximum.",
				 function );

				return( -1 );
			}
			compression_value_table[ compression_value_table_index ] = value_0x2000 | nibble_count_table_index;
		}
		value_0x2000 -= 0x10;

		compressed_data_iterator = value_0x2000 >> 5;

		nibble_count_table_index = compressed_data[ 2 + compressed_data_iterator ] & 0x0f;

		if( nibble_count_table_index > 0 )
		{
			total_nibble_count_table[ nibble_count_table_index ] -= 1;
			compression_value_table_index                         = total_nibble_count_table[ nibble_count_table_index ];

			if( compression_value_table_index > 2048 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: compression value table index value exceeds maximum.",
				 function );

				return( -1 );
			}
			compression_value_table[ compression_value_table_index ] = value_0x2000 | nibble_count_table_index;
		}
	}
	compression_value_table_index = 0x0800;
	value_0x0800                  = 0x0800;
	value_0x0400                  = 0x0400;

	if( total_nibble_count > 2048 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: total nibble count value exceeds maximum.",
		 function );

		return( -1 );
	}
	for( nibble_count_table_index = 15;
	     nibble_count_table_index > 10;
	     nibble_count_table_index-- )
	{
		if( value_0x0800 > compression_value_table_index )
		{
			value_0x0800                  -= 2;
			compression_value_table_index -= 1;

			compression_value_table[ compression_value_table_index ] = value_0x0800 | 0x8000;
		}
		for( nibble_count = nibble_count_table[ nibble_count_table_index ];
		     nibble_count > 0;
		     nibble_count-- )
		{
			total_nibble_count -= 1;

			compression_value              = compression_value_table[ total_nibble_count ];
			compression_value_table_index -= 1;

			compression_value_table[ compression_value_table_index ] = compression_value;
		}
	}
	while( value_0x0800 > compression_value_table_index )
	{
		value_0x0800 -= 2;
		value_0x0400 -= 1;

		compression_value_table[ value_0x0400 ] = value_0x0800 | 0x8000;
	}
	while( total_nibble_count > 0 )
	{
		total_nibble_count -= 1;

		compression_value             = compression_value_table[ total_nibble_count ];
		compression_value_table_index = value_0x0400 - ( 0x0400 >> ( compression_value & 0x0f ) );

		do
		{
			value_0x0400 -= 1;

			compression_value_table[ value_0x0400 ] = compression_value;
		}
		while( value_0x0400 > compression_value_table_index );
	}

#if defined( HAVE_DEBUG_OUTPUT ) && defined( HAVE_EXTRA_DEBUG_OUTPUT )
	if( libsystem_notify_verbose != 0 )
	{
		libsystem_notify_printf(
		 "%s: compression value table:\n",
		 function );
		libsystem_notify_print_data(
		 (uint8_t *) compression_value_table,
		 2 * 2048 );
	}
#endif
	/* Byte 258 - end contain the compression data bit stream
	 */
	compressed_data_iterator = 2 + 0x100;

	if( ( compressed_data_iterator + 3 ) >= compressed_data_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: compressed data size value too small.",
		 function );

		return( -1 );
	}
	/* Read the data as 16-bit little endian values
	 */
	compressed_data_bit_stream   = compressed_data[ compressed_data_iterator + 1 ];
	compressed_data_bit_stream <<= 8;
	compressed_data_bit_stream  += compressed_data[ compressed_data_iterator ];
	compressed_data_bit_stream <<= 8;
	compressed_data_bit_stream  += compressed_data[ compressed_data_iterator + 3 ];
	compressed_data_bit_stream <<= 8;
	compressed_data_bit_stream  += compressed_data[ compressed_data_iterator + 2 ];

	compressed_data_iterator += 4;

	number_of_bits_available = 0x10;

	/* The compression data is stored a 16-bit little-endian values
	 * it contains a bit stream which contains the following values
	 * starting with the first bit in the stream
	 * 0 - 9 compression value table index (where 0 is the MSB of the value)
	 */
	while( compressed_data_iterator < compressed_data_size )
	{
		/* Read a 10-bit table index from the decoded data
		 * maximum index of 1023
		 */
		compression_value_table_index = compressed_data_bit_stream >> 0x16;

		/* Check if the table entry contains an ignore index flag (bit 15)
		 */
		if( ( compression_value_table[ compression_value_table_index ] & 0x8000 ) != 0 )
		{
			/* Ignore the 10-bit index
			 */
			compressed_data_bit_stream <<= 10;

			do
			{
				compression_value_table_index = compression_value_table[ compression_value_table_index ] & 0x7fff;

				/* Add the MSB of the compressed data bit stream to the
				 * compression value table index
				 */
				compression_value_table_index += compressed_data_bit_stream >> 31;

				/* Ignore 1 bit for empty compression values
				 */
				compressed_data_bit_stream <<= 1;

				if( compression_value_table_index > 2048 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
					 "%s: compression value table index value exceeds maximum.",
					 function );

					return( -1 );
				}
			}
			while( compression_value_table[ compression_value_table_index ] == 0 );

			/* Retrieve the number of bits used (lower 4-bit) of from the table entry
			 */
			number_of_bits_used = (int8_t) ( compression_value_table[ compression_value_table_index ] & 0x0f );

			/* Retrieve the compression value from the table entry
			 */
			compression_value = compression_value_table[ compression_value_table_index ] >> 4;

			number_of_bits_available -= number_of_bits_used;
		}
		else
		{
			/* Retrieve the number of bits used (lower 4-bit) of from the table entry
			 */
			number_of_bits_used = (int8_t) ( compression_value_table[ compression_value_table_index ] & 0x0f );

			/* Retrieve the compression value from the table entry
			 */
			compression_value = compression_value_table[ compression_value_table_index ] >> 4;

			number_of_bits_available    -= number_of_bits_used;
			compressed_data_bit_stream <<= number_of_bits_used;
		}
		if( number_of_bits_available < 0 )
		{
			number_of_bits_used = -1 * number_of_bits_available;

			if( ( compressed_data_iterator + 1 ) >= compressed_data_size )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: compressed data size value too small.",
				 function );

				return( -1 );
			}
			/* Read the data as 16-bit little endian values
			 */
			value_32bit   = compressed_data[ compressed_data_iterator + 1 ];
			value_32bit <<= 8;
			value_32bit  += compressed_data[ compressed_data_iterator ];

			compressed_data_iterator += 2;

			value_32bit               <<= number_of_bits_used;
			compressed_data_bit_stream += value_32bit;

			number_of_bits_available += 0x10;
		}
		/* Check if the table entry contains a compression tuple flag (bit 12)
		 */
		if( ( compression_value_table[ compression_value_table_index ] & 0x1000 ) != 0 )
		{
			/* Retrieve the size of the compression (bit 4-7) from the table entry
			 */
			compression_size = (uint16_t) ( ( compression_value_table[ compression_value_table_index ] >> 4 ) & 0x0f );

			/* Retrieve the size of the compression (bit 8-11) from the table entry
			 */
			number_of_bits_used = (int8_t) ( ( compression_value_table[ compression_value_table_index ] >> 8 ) & 0x0f );

			/* Break if the end of the compressed data is reached
			 * and both the compression size and number of bits used for the compression offset are 0
			 */
			if( ( compressed_data_iterator == compressed_data_size )
			 && ( compression_size == 0 )
			 && ( number_of_bits_used == 0 ) )
			{
				break;
			}
			/* Retrieve the compression offset from the decoded data
			 */
			compression_offset = ( compressed_data_bit_stream >> 1 ) | 0x80000000;

			compression_offset = ( compression_offset >> ( 31 - number_of_bits_used ) );

			compressed_data_bit_stream <<= number_of_bits_used;
			number_of_bits_available    -= number_of_bits_used;

			if( compression_size == 0x0f )
			{
				if( compressed_data_iterator >= compressed_data_size )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
					 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
					 "%s: compressed data size value too small.",
					 function );

					return( -1 );
				}
				compression_size += compressed_data[ compressed_data_iterator ];

				compressed_data_iterator += 1;
			}
			if( compression_size == ( 0xff + 0x0f ) )
			{
				if( ( compressed_data_iterator + 1 ) >= compressed_data_size )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
					 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
					 "%s: compressed data size value too small.",
					 function );

					return( -1 );
				}
				byte_stream_copy_to_uint16_little_endian(
				 &( compressed_data[ compressed_data_iterator ] ),
				 compression_size );

				compressed_data_iterator += 2;

				if( compression_size < ( 0xff + 0x0f ) )
				{
					/* TODO error */
					return( -1 );
				}
			}
			compression_size += 3;

			if( number_of_bits_available < 0 )
			{
				number_of_bits_used = -1 * number_of_bits_available;

				if( ( compressed_data_iterator + 1 ) >= compressed_data_size )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
					 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
					 "%s: compressed data size value too small.",
					 function );

					return( -1 );
				}
				/* Read the data as 16-bit little endian values
				 */
				value_32bit   = compressed_data[ compressed_data_iterator + 1 ];
				value_32bit <<= 8;
				value_32bit  += compressed_data[ compressed_data_iterator ];

				compressed_data_iterator += 2;

				value_32bit               <<= number_of_bits_used;
				compressed_data_bit_stream += value_32bit;

				number_of_bits_available += 0x10;
			}
			if( ( uncompressed_data_iterator + compression_size ) > uncompressed_data_size )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: uncompressed data size value too small.",
				 function );

				return( -1 );
			}
			if( compression_offset > uncompressed_data_iterator )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: compression offset value exceeds uncompressed data iterator.",
				 function );

				return( -1 );
			}
			compression_iterator = uncompressed_data_iterator - compression_offset;

			while( compression_size > 0 )
			{
				uncompressed_data[ uncompressed_data_iterator++ ] = uncompressed_data[ compression_iterator++ ];

				compression_size--;
			}
		}
		else
		{
			if( uncompressed_data_iterator >= uncompressed_data_size )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: uncompressed data size value too small.",
				 function );

				return( -1 );
			}
			uncompressed_data[ uncompressed_data_iterator++ ] = (uint8_t) ( compression_value & 0xff );
		}
	}
#if defined( HAVE_DEBUG_OUTPUT ) && defined( HAVE_EXTRA_DEBUG_OUTPUT )
	if( libsystem_notify_verbose != 0 )
	{
		libsystem_notify_printf(
		 "%s: uncompressed data:\n",
		 function );
		libsystem_notify_print_data(
		 uncompressed_data,
		 uncompressed_data_iterator );
	}
#endif
	return( 1 );
}

/* Exports a compressed string
 * Returns 1 if successful or -1 on error
 */
int windows_search_export_compressed_string_value(
     uint8_t *value_data,
     size_t value_data_size,
     int ascii_codepage,
     FILE *record_file_stream,
     liberror_error_t **error )
{
	libcstring_system_character_t *value_string = NULL;
	uint8_t *decoded_value_data                 = NULL;
	uint8_t *decompressed_value_data            = NULL;
	uint8_t *narrow_value_string                = NULL;
	uint8_t *value_utf16_stream                 = NULL;
	static char *function                       = "windows_search_export_compressed_string_value";
	size_t decoded_value_data_size              = 0;
	size_t decompressed_value_data_size         = 0;
	size_t value_string_size                    = 0;
	size_t value_utf16_stream_size              = 0;
	uint8_t compression_type                    = 0;
	int result                                  = 0;

	if( value_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value data.",
		 function );

		return( -1 );
	}
	if( value_data_size == 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_ZERO_OR_LESS,
		 "%s: invalid value data size value zero or less.",
		 function );

		return( 1 );
	}
	if( value_data_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid value data size value exceeds maximum.",
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
	decoded_value_data_size = value_data_size;

	decoded_value_data = (uint8_t *) memory_allocate(
					  sizeof( uint8_t ) * decoded_value_data_size );

	if( decoded_value_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create decoded value data.",
		 function );

		return( -1 );
	}
	if( windows_search_decode(
	     decoded_value_data,
	     decoded_value_data_size,
	     value_data,
	     value_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to decode value data.",
		 function );

		memory_free(
		 decoded_value_data );

		return( -1 );
	}
/* TODO test purposes
#if defined( HAVE_DEBUG_OUTPUT ) && defined( HAVE_EXTRA_DEBUG_OUTPUT )
	if( libsystem_notify_verbose != 0 )
	{
		libsystem_notify_printf(
		 "%s: value data:\n",
		 function );
		libsystem_notify_print_data(
		 value_data,
		 value_data_size );
	}
#endif
*/
#if defined( HAVE_DEBUG_OUTPUT ) && defined( HAVE_EXTRA_DEBUG_OUTPUT )
	if( libsystem_notify_verbose != 0 )
	{
		libsystem_notify_printf(
		 "%s: decoded data:\n",
		 function );
		libsystem_notify_print_data(
		 decoded_value_data,
		 decoded_value_data_size );
	}
#endif
	compression_type = decoded_value_data[ 0 ];

	/* Byte-index compressed data
	 */
	if( ( compression_type & 0x02 ) != 0 )
	{
		if( windows_search_get_byte_index_uncompressed_data_size(
		     &( decoded_value_data[ 1 ] ),
		     decoded_value_data_size - 1,
		     &decompressed_value_data_size,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve byte-index compressed data size.",
			 function );

			memory_free(
			 decoded_value_data );

			return( -1 );
		}
		decompressed_value_data_size += 1;

		decompressed_value_data = (uint8_t *) memory_allocate(
		                                       sizeof( uint8_t ) * decompressed_value_data_size );

		if( decompressed_value_data == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create decompressed value data.",
			 function );

			memory_free(
			 decoded_value_data );

			return( -1 );
		}
		/* Add the first byte of the decoded data - 2 to have the
		 * decompressed data look like decoded data for chained decompression
		 */
		decompressed_value_data[ 0 ] = decoded_value_data[ 0 ] - 2;

		result = windows_search_decompress_byte_indexed_compressed_data(
		          &( decompressed_value_data[ 1 ] ),
		          decompressed_value_data_size - 1,
		          &( decoded_value_data[ 1 ] ),
		          decoded_value_data_size - 1,
		          error );

		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to decompress byte-index compressed data.",
			 function );

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

			memory_free(
			 decompressed_value_data );
		}
		else
		{
#if defined( HAVE_DEBUG_OUTPUT ) && defined( HAVE_EXTRA_DEBUG_OUTPUT )
			if( libsystem_notify_verbose != 0 )
			{
				libsystem_notify_printf(
				 "%s: decompressed data:\n",
				 function );
				libsystem_notify_print_data(
				 decompressed_value_data,
				 decompressed_value_data_size );
			}
#endif
			memory_free(
			 decoded_value_data );

			decoded_value_data      = decompressed_value_data;
			decoded_value_data_size = decompressed_value_data_size;

			compression_type &= ~( 0x02 );
		}
	}
	/* Run-length compressed UTF-16 little-endian string
	 */
	if( compression_type == 0 )
	{
		if( windows_search_get_run_length_uncompressed_utf16_string_size(
		     &( decoded_value_data[ 1 ] ),
		     decoded_value_data_size - 1,
		     &value_utf16_stream_size,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve run-length uncompressed UTF-16 string size.",
			 function );

			if( ( error != NULL )
			 && ( *error != NULL ) )
			{
				libsystem_notify_print_error_backtrace(
				 *error );
			}
			liberror_error_free(
			 error );

			memory_free(
			 decoded_value_data );
		}
		if( value_utf16_stream_size > 0 )
		{
			value_utf16_stream = (uint8_t *) memory_allocate(
							  sizeof( uint8_t ) * value_utf16_stream_size );

			if( value_utf16_stream == NULL )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_MEMORY,
				 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
				 "%s: unable to create value UTF-16 stream.",
				 function );

				memory_free(
				 decoded_value_data );

				return( -1 );
			}
			if( windows_search_decompress_run_length_compressed_utf16_string(
			     value_utf16_stream,
			     value_utf16_stream_size,
			     &( decoded_value_data[ 1 ] ),
			     decoded_value_data_size - 1,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to decompress run-length compressed UTF-16 string.",
				 function );

				memory_free(
				 value_utf16_stream );
				memory_free(
				 decoded_value_data );

				return( -1 );
			}
#if defined( HAVE_DEBUG_OUTPUT ) && defined( HAVE_EXTRA_DEBUG_OUTPUT )
			if( libsystem_notify_verbose != 0 )
			{
				libsystem_notify_printf(
				 "%s: decompressed data:\n",
				 function );
				libsystem_notify_print_data(
				 value_utf16_stream,
				 value_utf16_stream_size );
			}
#endif
			/* Sometimes the UTF-16 stream is cut-off in the surrogate high range
			 * The last 2 bytes are ignored otherwise libuna will not convert
			 * the stream to a string
			 */
			if( ( ( value_utf16_stream[ value_utf16_stream_size - 1 ] ) >= 0xd8 )
			 && ( ( value_utf16_stream[ value_utf16_stream_size - 1 ] ) <= 0xdb ) )
			{
				value_utf16_stream_size -= 2;
			}
			memory_free(
			 decoded_value_data );

#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
			result = libuna_utf16_string_size_from_utf16_stream(
				  value_utf16_stream,
				  value_utf16_stream_size,
				  LIBUNA_ENDIAN_LITTLE,
				  &value_string_size,
				  error );
#else
			result = libuna_utf8_string_size_from_utf16_stream(
				  value_utf16_stream,
				  value_utf16_stream_size,
				  LIBUNA_ENDIAN_LITTLE,
				  &value_string_size,
				  error );
#endif
			if( result != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine size of value UTF-16 stream.",
				 function );

				memory_free(
				 value_utf16_stream );

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

				memory_free(
				 value_utf16_stream );

				return( -1 );
			}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
			result = libuna_utf16_string_copy_from_utf16_stream(
				  (uint16_t *) value_string,
				  value_string_size,
				  value_utf16_stream,
				  value_utf16_stream_size,
				  LIBUNA_ENDIAN_LITTLE,
				  error );
#else
			result = libuna_utf8_string_copy_from_utf16_stream(
				  (uint8_t *) value_string,
				  value_string_size,
				  value_utf16_stream,
				  value_utf16_stream_size,
				  LIBUNA_ENDIAN_LITTLE,
				  error );
#endif
			if( result != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve value string.",
				 function );

				memory_free(
				 value_string );
				memory_free(
				 value_utf16_stream );

				return( -1 );
			}
			memory_free(
			 value_utf16_stream );

			export_text(
			 value_string,
			 value_string_size,
			 record_file_stream );

			memory_free(
			 value_string );
		}
	}
	/* 8-bit compressed UTF-16 little-endian string
	 */
	else if( compression_type == 1 )
	{
		if( libuna_utf8_string_size_from_byte_stream(
		     &( decoded_value_data[ 1 ] ),
		     decoded_value_data_size - 1,
		     ascii_codepage,
		     &value_string_size,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to determine size of value string.",
			 function );

			memory_free(
			 decoded_value_data );

			return( -1 );
		}
		narrow_value_string = (uint8_t *) memory_allocate(
		                                   sizeof( uint8_t ) * value_string_size );

		if( narrow_value_string == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create value string.",
			 function );

			memory_free(
			 decoded_value_data );

			return( -1 );
		}
		if( libuna_utf8_string_copy_from_byte_stream(
		     narrow_value_string,
		     value_string_size,
		     &( decoded_value_data[ 1 ] ),
		     decoded_value_data_size - 1,
		     ascii_codepage,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve value string.",
			 function );

			memory_free(
			 narrow_value_string );
			memory_free(
			 decoded_value_data );

			return( -1 );
		}
		memory_free(
		 decoded_value_data );

		export_narrow_text(
		 (char *) narrow_value_string,
		 value_string_size,
		 record_file_stream );

		memory_free(
		 narrow_value_string );
	}
	/* uncompressed data
	 */
	else if( compression_type == 4 )
	{
		export_narrow_text(
		 (char *) &( decoded_value_data[ 1 ] ),
		 decoded_value_data_size - 1,
		 record_file_stream );

		memory_free(
		 decoded_value_data );
	}
	else
	{
		if( libsystem_notify_verbose != 0 )
		{
			libsystem_notify_printf(
			 "unsupported compression type: 0x%02" PRIx8 "\n",
			 compression_type );

#if defined( HAVE_DEBUG_OUTPUT )
			libsystem_notify_print_data(
			 value_data,
			 value_data_size );
#endif
		}
		memory_free(
		 decoded_value_data );
	}
	return( 1 );
}

/* Exports a 32-bit value in a binary data table record value
 * Returns 1 if successful or -1 on error
 */
int windows_search_export_record_value_32bit(
     libesedb_record_t *record,
     int record_value_entry,
     uint8_t byte_order,
     FILE *record_file_stream,
     liberror_error_t **error )
{
	uint8_t *value_data    = NULL;
	static char *function  = "windows_search_export_record_value_32bit";
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
int windows_search_export_record_value_64bit(
     libesedb_record_t *record,
     int record_value_entry,
     uint8_t byte_order,
     uint8_t format,
     FILE *record_file_stream,
     liberror_error_t **error )
{
	uint8_t *value_data    = NULL;
	static char *function  = "windows_search_export_record_value_64bit";
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
	if( ( format != WINDOWS_SEARCH_FORMAT_DECIMAL )
	 && ( format != WINDOWS_SEARCH_FORMAT_HEXADECIMAL ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported format: 0x%02" PRIx8 "",
		 function,
		 format );

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
	if( ( column_type != LIBESEDB_COLUMN_TYPE_CURRENCY )
	 && ( column_type != LIBESEDB_COLUMN_TYPE_BINARY_DATA )
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
			if( memory_compare(
			     value_data,
			     "********",
			     8 ) == 0 )
			{
				fprintf(
				 record_file_stream,
				 "********" );
			}
			else
			{
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
				if( format == WINDOWS_SEARCH_FORMAT_DECIMAL )
				{
					fprintf(
					 record_file_stream,
					 "%" PRIu64 "",
					 value_64bit );
				}
				else if( format == WINDOWS_SEARCH_FORMAT_HEXADECIMAL )
				{
					export_binary_data(
					 value_data,
					 value_data_size,
					 record_file_stream );
				}
			}
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
int windows_search_export_record_value_filetime(
     libesedb_record_t *record,
     int record_value_entry,
     uint8_t byte_order,
     FILE *record_file_stream,
     liberror_error_t **error )
{
	libcstring_system_character_t filetime_string[ 32 ];

	libfdatetime_filetime_t *filetime = NULL;
	uint8_t *value_data               = NULL;
	static char *function             = "windows_search_export_record_value_filetime";
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

/* Exports a compressed string in a binary data table record value
 * Returns 1 if successful or -1 on error
 */
int windows_search_export_record_value_compressed_string(
     libesedb_record_t *record,
     int record_value_entry,
     int ascii_codepage,
     FILE *record_file_stream,
     liberror_error_t **error )
{
	libesedb_long_value_t *long_value   = NULL;
	libesedb_multi_value_t *multi_value = NULL;
	uint8_t *value_data                 = NULL;
	static char *function               = "windows_search_export_record_value_compressed_string";
	size_t value_data_size              = 0;
	uint32_t column_type                = 0;
	uint8_t value_flags                 = 0;
	int long_value_segment_iterator     = 0;
	int multi_value_iterator            = 0;
	int number_of_long_value_segments   = 0;
	int number_of_multi_values          = 0;

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
		 "%s: unable to retrieve value of record entry: %d.",
		 function,
		 record_value_entry );

		return( -1 );
	}
	if( ( value_flags & ~( LIBESEDB_VALUE_FLAG_VARIABLE_SIZE ) ) == 0 )
	{
		if( value_data != NULL )
		{
			if( windows_search_export_compressed_string_value(
			     value_data,
			     value_data_size,
			     ascii_codepage,
			     record_file_stream,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GENERIC,
				 "%s: unable to export compressed string value of record entry: %d.",
				 function,
				 record_value_entry );

				return( -1 );
			}
		}
	}
	else if( ( value_flags & LIBESEDB_VALUE_FLAG_LONG_VALUE ) != 0 )
	{
		if( libesedb_record_get_long_value(
		     record,
		     record_value_entry,
		     &long_value,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve long value of record entry: %d.",
			 function,
			 record_value_entry );

			return( -1 );
		}
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
#if defined( HAVE_DEBUG_OUTPUT )
if( libsystem_notify_verbose != 0 )
{
	libsystem_notify_printf(
	 "LONG VALUE DATA: %d out of %d\n",
	 long_value_segment_iterator + 1,
	 number_of_long_value_segments );
}
#endif

			if( value_data != NULL )
			{
				/* TODO assume data is compressed per segment */
				if( windows_search_export_compressed_string_value(
				     value_data,
				     value_data_size,
				     ascii_codepage,
				     record_file_stream,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GENERIC,
					 "%s: unable to export compressed string of long value segment: %d of record entry: %d.",
					 function,
					 long_value_segment_iterator,
					 record_value_entry );

					libesedb_long_value_free(
					 &long_value,
					 NULL );

					return( -1 );
				}
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
	}
	/* TODO handle 0x10 flag */
	else if( ( ( value_flags & LIBESEDB_VALUE_FLAG_MULTI_VALUE ) != 0 )
	      && ( ( value_flags & 0x10 ) == 0 ) )
	{
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
				/* TODO what about non string multi values ?
				 */
				if( windows_search_export_compressed_string_value(
				     value_data,
				     value_data_size,
				     ascii_codepage,
				     record_file_stream,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GENERIC,
					 "%s: unable to export compressed string of multi value: %d of record entry: %d.",
					 function,
					 multi_value_iterator,
					 record_value_entry );

					libesedb_multi_value_free(
					 &multi_value,
					 NULL );

					return( -1 );
				}
				if( multi_value_iterator < ( number_of_multi_values - 1 ) )
				{
					fprintf(
					 record_file_stream,
					 "; " );
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

/* Exports an UTF-16 string in a binary data table record value
 * Returns 1 if successful or -1 on error
 */
int windows_search_export_record_value_utf16_string(
     libesedb_record_t *record,
     int record_value_entry,
     uint8_t byte_order,
     FILE *record_file_stream,
     liberror_error_t **error )
{
	libcstring_system_character_t *value_string = NULL;
	uint8_t *value_data                         = NULL;
	static char *function                       = "windows_search_export_record_value_utf16_string";
	size_t value_data_size                      = 0;
	size_t value_string_size                    = 0;
	uint32_t column_type                        = 0;
	uint8_t value_flags                         = 0;
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
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
			result = libuna_utf16_string_size_from_utf16_stream(
			          value_data,
			          value_data_size,
			          byte_order,
			          &value_string_size,
			          error );
#else
			result = libuna_utf8_string_size_from_utf16_stream(
			          value_data,
			          value_data_size,
			          byte_order,
			          &value_string_size,
			          error );
#endif
			if( result != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine size of value string: %d.",
				 function,
				 record_value_entry );

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
			result = libuna_utf16_string_copy_from_utf16_stream(
			          (uint16_t *) value_string,
			          value_string_size,
			          value_data,
			          value_data_size,
			          byte_order,
			          error );
#else
			result = libuna_utf8_string_copy_from_utf16_stream(
			          (uint8_t *) value_string,
			          value_string_size,
			          value_data,
			          value_data_size,
			          byte_order,
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

/* Exports the values in a SystemIndex_0A table record
 * Returns 1 if successful or -1 on error
 */
int windows_search_export_record_systemindex_0a(
     libesedb_record_t *record,
     int ascii_codepage,
     FILE *record_file_stream,
     log_handle_t *log_handle,
     liberror_error_t **error )
{
	libcstring_system_character_t column_name[ 256 ];

	static char *function   = "windows_search_export_record_systemindex_0a";
	size_t column_name_size = 0;
	uint32_t column_type    = 0;
	uint8_t byte_order      = _BYTE_STREAM_ENDIAN_BIG;
	uint8_t format          = 0;
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
		known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_UNDEFINED;

		if( column_type == LIBESEDB_COLUMN_TYPE_INTEGER_32BIT_SIGNED )
		{
			if( libcstring_system_string_compare(
			     column_name,
			     _LIBCSTRING_SYSTEM_STRING( "__SDID" ),
			     6 ) == 0 )
			{
				/* The byte order is set because the SystemIndex_0A table in the
				 * Windows Search XP and 7 database contains binary values in big-endian
				 * In the Windows Search XP database the __SDID column is of type binary data.
				 * In the Windows Search Vista data base the __SDID column is of type integer 32-bit signed.
				 * Int Windows Search 7 the __SDID column is no longer present
				 */
				byte_order = _BYTE_STREAM_ENDIAN_LITTLE;
			}
		}
		if( ( column_type == LIBESEDB_COLUMN_TYPE_CURRENCY )
		 || ( column_type == LIBESEDB_COLUMN_TYPE_BINARY_DATA )
		 || ( column_type == LIBESEDB_COLUMN_TYPE_LARGE_BINARY_DATA ) )
		{
			if( column_name_size == 24 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "System_ThumbnailCacheId" ),
				     23 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_INTEGER_64BIT;
					format            = WINDOWS_SEARCH_FORMAT_HEXADECIMAL;
				}
			}
		}
		/* Only check for known columns of the binary data type
		 * some columns get their type reassigned over time
		 */
		if( ( column_type == LIBESEDB_COLUMN_TYPE_BINARY_DATA )
		 || ( column_type == LIBESEDB_COLUMN_TYPE_LARGE_BINARY_DATA ) )
		{
			if( column_name_size == 7 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "__SDID" ),
				     6 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_INTEGER_32BIT;
				}
			}
			else if( column_name_size == 12 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "System_Kind" ),
				     11 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Size" ),
				          11 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_INTEGER_64BIT;
					format            = WINDOWS_SEARCH_FORMAT_DECIMAL;
				}
			}
			else if( column_name_size == 13 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "System_Title" ),
				     12 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
			}
			else if( column_name_size == 14 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "System_Author" ),
				     13 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
			}
			else if( column_name_size == 15 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "System_Comment" ),
				     14 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_DueDate" ),
				          14 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_FILETIME;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_EndDate" ),
				          14 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_FILETIME;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_ItemUrl" ),
				          14 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Subject" ),
				          14 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
			}
			else if( column_name_size == 16 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "System_FileName" ),
				     15 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Identity" ),
				          15 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_ItemDate" ),
				          15 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_FILETIME;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_ItemName" ),
				          15 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_ItemType" ),
				          15 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING(  "System_KindText" ),
				          15 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_MIMEType" ),
				          15 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
			}
			else if( column_name_size == 17 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "System_Copyright" ),
				     16 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_FileOwner" ),
				          16 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
			}
			else if( column_name_size == 18 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "System_Media_MCDI" ),
				     17 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_RatingText" ),
				          17 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
			}
			else if( column_name_size == 19 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "System_DateCreated" ),
				     18 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_FILETIME;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_ItemAuthors" ),
				          18 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Music_Genre" ),
				          18 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_ParsingName" ),
				          18 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
			}
			else if( column_name_size == 20 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "System_ComputerName" ),
				     19 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_DateAccessed" ),
				          19 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_FILETIME;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_DateAcquired" ),
				          19 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_FILETIME;
				}
				else if( libcstring_system_string_compare(
					  column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_DateImported" ),
					  19 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_FILETIME;
				}
				else if( libcstring_system_string_compare(
					  column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_DateModified" ),
					  19 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_FILETIME;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_ItemTypeText" ),
				          19 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Music_Artist" ),
				          19 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Search_Store" ),
				          19 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
			}
			else if( column_name_size == 21 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "System_FileExtension" ),
				     20 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Message_Store" ),
				          20 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
			}
			else if( column_name_size == 22 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "System_FlagStatusText" ),
				     21 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_ImportanceText" ),
				          21 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_ItemNamePrefix" ),
				          21 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Media_SubTitle" ),
				          21 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Message_ToName" ),
				          21 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Music_Composer" ),
				          21 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
			}
			else if( column_name_size == 23 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "System_ItemNameDisplay" ),
				     22 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_ItemPathDisplay" ),
				          22 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Media_ContentID" ),
				          22 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Media_Publisher" ),
				          22 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Music_PartOfSet" ),
				          22 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Photo_DateTaken" ),
				          22 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_FILETIME;
				}
			}
			else if( column_name_size == 24 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "System_Image_Dimensions" ),
				     23 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_ItemParticipants" ),
				          23 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Message_DateSent" ),
				          23 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_FILETIME;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Message_FromName" ),
				          23 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Music_AlbumTitle" ),
				          23 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
			}
			else if( column_name_size == 25 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "System_Calendar_Location" ),
				     24 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Media_DateEncoded" ),
				          24 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_FILETIME;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Message_CcAddress" ),
				          24 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Message_ToAddress" ),
				          24 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Music_AlbumArtist" ),
				          24 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Search_GatherTime" ),
				          24 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_FILETIME;
				}
			}
			else if( column_name_size == 26 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "System_Document_DateSaved" ),
				     25 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_FILETIME;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Media_CollectionID" ),
				          25 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Media_DateReleased" ),
				          25 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Message_SenderName" ),
				          25 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Search_AutoSummary" ),
				          25 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
			}
			else if( column_name_size == 27 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "System_Message_FromAddress" ),
				     26 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
			}
			else if( column_name_size == 28 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "System_Document_DateCreated" ),
				     27 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_FILETIME;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Media_ClassPrimaryID" ),
				          27 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Message_DateReceived" ),
				          27 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_FILETIME;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Message_MessageClass" ),
				          27 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
			}
			else if( column_name_size == 29 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "System_ItemFolderNameDisplay" ),
				     28 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_ItemFolderPathDisplay" ),
				          28 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_ItemPathDisplayNarrow" ),
				          28 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Message_SenderAddress" ),
				          28 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
			}
			else if( column_name_size == 30 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "System_Link_TargetParsingPath" ),
				     29 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Media_ClassSecondaryID" ),
				          29 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Message_ConversationID" ),
				          29 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_RecordedTV_EpisodeName" ),
				          29 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
			}
			else if( column_name_size == 31 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "System_Calendar_ShowTimeAsText" ),
				     30 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
				else if( libcstring_system_string_compare(
				          column_name,
				          _LIBCSTRING_SYSTEM_STRING( "System_Media_CollectionGroupID" ),
				          30 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
			}
			else if( column_name_size == 32 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "System_RecordedTV_RecordingTime" ),
				     31 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_FILETIME;
				}
			}
			else if( column_name_size == 35 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "System_ItemFolderPathDisplayNarrow" ),
				     34 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
			}
			else if( column_name_size == 37 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "System_RecordedTV_ProgramDescription" ),
				     36 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED;
				}
			}
		}
		if( known_column_type == WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_INTEGER_32BIT )
		{
			result = windows_search_export_record_value_32bit(
				  record,
				  value_iterator,
				  byte_order,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_INTEGER_64BIT )
		{
			result = windows_search_export_record_value_64bit(
				  record,
				  value_iterator,
				  byte_order,
				  format,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_FILETIME )
		{
			result = windows_search_export_record_value_filetime(
				  record,
				  value_iterator,
				  byte_order,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_COMPRESSED )
		{
			result = windows_search_export_record_value_compressed_string(
				  record,
				  value_iterator,
				  ascii_codepage,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_UNDEFINED )
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

/* Exports the values in a SystemIndex_Gthr table record
 * Returns 1 if successful or -1 on error
 */
int windows_search_export_record_systemindex_gthr(
     libesedb_record_t *record,
     FILE *record_file_stream,
     log_handle_t *log_handle,
     liberror_error_t **error )
{
	libcstring_system_character_t column_name[ 256 ];

	static char *function   = "windows_search_export_record_systemindex_gthr";
	size_t column_name_size = 0;
	uint32_t column_type    = 0;
	int known_column_type   = 0;
	int number_of_values    = 0;
	int result              = 0;
	int value_iterator      = 0;
	uint8_t byte_order      = _BYTE_STREAM_ENDIAN_LITTLE;

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
		known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_UNDEFINED;

		/* Only check for known columns of the binary data type
		 * some columns get their type reassigned over time
		 */
		if( ( column_type == LIBESEDB_COLUMN_TYPE_BINARY_DATA )
		 || ( column_type == LIBESEDB_COLUMN_TYPE_LARGE_BINARY_DATA ) )
		{
			if( column_name_size == 10 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "FileName1" ),
				     9 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_UTF16_LITTLE_ENDIAN;
				}
				else if( libcstring_system_string_compare(
					 column_name,
					 _LIBCSTRING_SYSTEM_STRING( "FileName2" ),
					 9 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_UTF16_LITTLE_ENDIAN;
				}
			}
			else if( column_name_size == 13 )
			{
				if( libcstring_system_string_compare(
				     column_name,
				     _LIBCSTRING_SYSTEM_STRING( "LastModified" ),
				     12 ) == 0 )
				{
					known_column_type = WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_FILETIME;

					byte_order = _BYTE_STREAM_ENDIAN_BIG;
				}
			}
		}
		if( known_column_type == WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_FILETIME )
		{
			result = windows_search_export_record_value_filetime(
				  record,
				  value_iterator,
				  byte_order,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_STRING_UTF16_LITTLE_ENDIAN )
		{
			result = windows_search_export_record_value_utf16_string(
				  record,
				  value_iterator,
				  LIBUNA_ENDIAN_LITTLE,
				  record_file_stream,
				  error );
		}
		else if( known_column_type == WINDOWS_SEARCH_KNOWN_COLUMN_TYPE_UNDEFINED )
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

