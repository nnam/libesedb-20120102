/*
 * Catalog definition functions
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
#include <memory.h>
#include <types.h>

#include <libcstring.h>
#include <liberror.h>
#include <libnotify.h>

#include "libesedb_catalog_definition.h"
#include "libesedb_codepage.h"
#include "libesedb_column_type.h"
#include "libesedb_debug.h"
#include "libesedb_definitions.h"
#include "libesedb_lcid.h"
#include "libesedb_libuna.h"
#include "libesedb_unused.h"

#include "esedb_page_values.h"

/* Creates a catalog definition
 * Returns 1 if successful or -1 on error
 */
int libesedb_catalog_definition_initialize(
     libesedb_catalog_definition_t **catalog_definition,
     liberror_error_t **error )
{
	static char *function = "libesedb_catalog_definition_initialize";

	if( catalog_definition == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid catalog definition.",
		 function );

		return( -1 );
	}
	if( *catalog_definition != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid catalog definition value already set.",
		 function );

		return( -1 );
	}
	*catalog_definition = memory_allocate_structure(
	                       libesedb_catalog_definition_t );

	if( *catalog_definition == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create catalog definition.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *catalog_definition,
	     0,
	     sizeof( libesedb_catalog_definition_t ) ) == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear catalog definition.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( *catalog_definition != NULL )
	{
		memory_free(
		 *catalog_definition );

		*catalog_definition = NULL;
	}
	return( -1 );
}

/* Frees the catalog definition
 * Returns 1 if successful or -1 on error
 */
int libesedb_catalog_definition_free(
     libesedb_catalog_definition_t **catalog_definition,
     liberror_error_t **error )
{
	static char *function = "libesedb_catalog_definition_free";

	if( catalog_definition == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid catalog definition.",
		 function );

		return( -1 );
	}
	if( *catalog_definition != NULL )
	{
		if( ( *catalog_definition )->name != NULL )
		{
			memory_free(
			 ( *catalog_definition )->name );
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( ( *catalog_definition )->name_string != NULL )
		{
			memory_free(
			 ( *catalog_definition )->name_string );
		}
#endif
		if( ( *catalog_definition )->template_name != NULL )
		{
			memory_free(
			 ( *catalog_definition )->template_name );
		}
		if( ( *catalog_definition )->default_value != NULL )
		{
			memory_free(
			 ( *catalog_definition )->default_value );
		}
		memory_free(
		 *catalog_definition );

		*catalog_definition = NULL;
	}
	return( 1 );
}

/* Reads the catalog definition from the definition data
 * Returns 1 if successful or -1 on error
 */
int libesedb_catalog_definition_read(
     libesedb_catalog_definition_t *catalog_definition,
     uint8_t *definition_data,
     size_t definition_data_size,
     int ascii_codepage LIBESEDB_ATTRIBUTE_UNUSED,
     liberror_error_t **error )
{
	uint8_t *fixed_size_data_type_value_data            = NULL;
	uint8_t *variable_size_data_type_size_data          = NULL;
	uint8_t *variable_size_data_type_value_data         = NULL;
	static char *function                               = "libesedb_catalog_definition_read";
	uint16_t calculated_variable_size_data_types_offset = 0;
	uint16_t data_type_number                           = 0;
	uint16_t previous_variable_size_data_type_size      = 0;
	uint16_t variable_size_data_type_size               = 0;
	uint16_t variable_size_data_types_offset            = 0;
	uint8_t last_fixed_size_data_type                   = 0;
	uint8_t last_variable_size_data_type                = 0;
	uint8_t number_of_variable_size_data_types          = 0;
	uint8_t variable_size_data_type_iterator            = 0;

#if defined( HAVE_DEBUG_OUTPUT )
	libcstring_system_character_t *value_string         = 0;
	size_t value_string_size                            = 0;
	uint32_t value_32bit                                = 0;
	uint16_t record_offset                              = 0;
	uint16_t value_16bit                                = 0;
	int result                                          = 0;
#endif

	LIBESEDB_UNREFERENCED_PARAMETER( ascii_codepage )

	if( catalog_definition == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid catalog definition.",
		 function );

		return( -1 );
	}
	if( definition_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid definition data.",
		 function );

		return( -1 );
	}
	if( definition_data_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid definition data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( definition_data_size < sizeof( esedb_data_definition_header_t ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: definition data too small.",
		 function );

		return( -1 );
	}
	last_fixed_size_data_type    = ( (esedb_data_definition_header_t *) definition_data )->last_fixed_size_data_type;
	last_variable_size_data_type = ( (esedb_data_definition_header_t *) definition_data )->last_variable_size_data_type;

	byte_stream_copy_to_uint16_little_endian(
	 ( (esedb_data_definition_header_t *) definition_data )->variable_size_data_types_offset,
	 variable_size_data_types_offset );

#if defined( HAVE_DEBUG_OUTPUT )
	if( libnotify_verbose != 0 )
	{
		libnotify_printf(
		 "%s: last fixed size data type\t\t\t\t: %" PRIu8 "\n",
		 function,
		 last_fixed_size_data_type );

		libnotify_printf(
		 "%s: last variable size data type\t\t\t\t: %" PRIu8 "\n",
		 function,
		 last_variable_size_data_type );

		libnotify_printf(
		 "%s: variable size data types offset\t\t\t: %" PRIu16 "\n",
		 function,
		 variable_size_data_types_offset );
	}
#endif

	/* As far as the documentation states
	 * the column data FIELD structure is 16 bytes of size
	 */
	if( last_fixed_size_data_type < 5 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: last fixed size data type too small.",
		 function );

		return( -1 );
	}
	if( last_fixed_size_data_type > 11 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported last fixed size data type: %" PRIu8 ".",
		 function,
		 last_fixed_size_data_type );

		return( -1 );
	}
	if( last_variable_size_data_type > 127 )
	{
		number_of_variable_size_data_types = last_variable_size_data_type - 127;
	}
	calculated_variable_size_data_types_offset += sizeof( esedb_data_definition_header_t );

	/* Use a fall through to determine the size of the fixed size data types
	 */
	switch( last_fixed_size_data_type )
	{
		case 11:
			calculated_variable_size_data_types_offset += 2;
		case 10:
			calculated_variable_size_data_types_offset += 4;
		case 9:
			calculated_variable_size_data_types_offset += 2;
		case 8:
			if( last_variable_size_data_type > 127 )
			{
				calculated_variable_size_data_types_offset += 1 * number_of_variable_size_data_types;
			}
		case 7:
			calculated_variable_size_data_types_offset += 4;
		case 6:
			calculated_variable_size_data_types_offset += 4;
		case 5:
			calculated_variable_size_data_types_offset += 4;
		case 4:
			calculated_variable_size_data_types_offset += 4;
		case 3:
			calculated_variable_size_data_types_offset += 4;
		case 2:
			calculated_variable_size_data_types_offset += 2;
		case 1:
			calculated_variable_size_data_types_offset += 4;
			break;
	}
	if( variable_size_data_types_offset > definition_data_size )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: variable size data types offset exceeds definition data.",
		 function );

		return( -1 );
	}
	fixed_size_data_type_value_data = &( definition_data[ sizeof( esedb_data_definition_header_t ) ] );

	byte_stream_copy_to_uint32_little_endian(
	 ( (esedb_data_definition_t *) fixed_size_data_type_value_data )->father_data_page_object_identifier,
	 catalog_definition->father_data_page_object_identifier );

	byte_stream_copy_to_uint16_little_endian(
	 ( (esedb_data_definition_t *) fixed_size_data_type_value_data )->type,
	 catalog_definition->type );

	byte_stream_copy_to_uint32_little_endian(
	 ( (esedb_data_definition_t *) fixed_size_data_type_value_data )->identifier,
	 catalog_definition->identifier );

	if( catalog_definition->type == LIBESEDB_CATALOG_DEFINITION_TYPE_COLUMN )
	{
		byte_stream_copy_to_uint32_little_endian(
		 ( (esedb_data_definition_t *) fixed_size_data_type_value_data )->column_type,
		 catalog_definition->column_type );
	}
	else
	{
		byte_stream_copy_to_uint32_little_endian(
		 ( (esedb_data_definition_t *) fixed_size_data_type_value_data )->father_data_page_number,
		 catalog_definition->father_data_page_number );
	}
	byte_stream_copy_to_uint32_little_endian(
	 ( (esedb_data_definition_t *) fixed_size_data_type_value_data )->space_usage,
	 catalog_definition->size );

	if( catalog_definition->type == LIBESEDB_CATALOG_DEFINITION_TYPE_COLUMN )
	{
		byte_stream_copy_to_uint32_little_endian(
		 ( (esedb_data_definition_t *) fixed_size_data_type_value_data )->codepage,
		 catalog_definition->codepage );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libnotify_verbose != 0 )
	{
		data_type_number = 1;

		libnotify_printf(
		 "%s: (%03" PRIu16 ") father data page (FDP) object identifier\t: %" PRIu32 "\n",
		 function,
		 data_type_number++,
		 catalog_definition->father_data_page_object_identifier );

		libnotify_printf(
		 "%s: (%03" PRIu16 ") type\t\t\t\t\t\t: 0x%04" PRIx16 " ",
		 function,
		 data_type_number++,
		 catalog_definition->type );
		libesedb_debug_print_page_value_definition_type(
		 catalog_definition->type );
		libnotify_printf(
		 "\n" );

		libnotify_printf(
		 "%s: (%03" PRIu16 ") identifier\t\t\t\t\t: %" PRIu32 "\n",
		 function,
		 data_type_number++,
		 catalog_definition->identifier );

		if( catalog_definition->type == LIBESEDB_CATALOG_DEFINITION_TYPE_COLUMN )
		{
			libnotify_printf(
			 "%s: (%03" PRIu16 ") column type\t\t\t\t\t: %" PRIu32 " (%s) %s\n",
			 function,
			 data_type_number++,
			 catalog_definition->column_type,
			 libesedb_column_type_get_identifier(
			  catalog_definition->column_type ),
			 libesedb_column_type_get_description(
			  catalog_definition->column_type ) );
		}
		else
		{
			libnotify_printf(
			 "%s: (%03" PRIu16 ") father data page (FDP) number\t\t\t: %" PRIu32 "\n",
			 function,
			 data_type_number++,
			 catalog_definition->father_data_page_number );
		}
		libnotify_printf(
		 "%s: (%03" PRIu16 ") space usage\t\t\t\t\t: %" PRIu32 "\n",
		 function,
		 data_type_number++,
		 catalog_definition->size );

		byte_stream_copy_to_uint32_little_endian(
		 ( (esedb_data_definition_t *) fixed_size_data_type_value_data )->flags,
		 value_32bit );

		if( last_fixed_size_data_type >= 6 )
		{
			if( catalog_definition->type == LIBESEDB_CATALOG_DEFINITION_TYPE_COLUMN )
			{
				libnotify_printf(
				 "%s: (%03" PRIu16 ") flags\t\t\t\t\t\t: 0x%08" PRIx32 "\n",
				 function,
				 data_type_number++,
				 value_32bit );
				libesedb_debug_print_column_group_of_bits(
				 value_32bit );
				libnotify_printf(
				 "\n" );
			}
			else if( catalog_definition->type == LIBESEDB_CATALOG_DEFINITION_TYPE_INDEX )
			{
				libnotify_printf(
				 "%s: (%03" PRIu16 ") flags\t\t\t\t\t\t: 0x%08" PRIx32 "\n",
				 function,
				 data_type_number++,
				 value_32bit );
				libesedb_debug_print_index_group_of_bits(
				 value_32bit );
				libnotify_printf(
				 "\n" );
			}
			else
			{
				libnotify_printf(
				 "%s: (%03" PRIu16 ") flags\t\t\t\t\t\t: 0x%08" PRIx32 "\n",
				 function,
				 data_type_number++,
				 value_32bit );
			}
		}
		if( last_fixed_size_data_type >= 7 )
		{
			if( catalog_definition->type == LIBESEDB_CATALOG_DEFINITION_TYPE_COLUMN )
			{
				libnotify_printf(
				 "%s: (%03" PRIu16 ") codepage\t\t\t\t\t: %" PRIu32 "",
				 function,
				 data_type_number++,
				 catalog_definition->codepage );

				if( catalog_definition->codepage != 0 )
				{
					libnotify_printf(
					 " (%s) %s",
					 libesedb_codepage_get_identifier(
					  catalog_definition->codepage ),
					 libesedb_codepage_get_description(
					  catalog_definition->codepage ) );
				}
				libnotify_printf(
				 "\n" );
			}
			else if( catalog_definition->type == LIBESEDB_CATALOG_DEFINITION_TYPE_INDEX )
			{
				byte_stream_copy_to_uint32_little_endian(
				 ( (esedb_data_definition_t *) fixed_size_data_type_value_data )->locale_identifier,
				 value_32bit );

				libnotify_printf(
				 "%s: (%03" PRIu16 ") locale identifier\t\t\t\t: 0x%08" PRIx32 " (%s)\n",
				 function,
				 data_type_number++,
				 value_32bit,
				 libesedb_lcid_language_tag_get_identifier(
				  (uint16_t) value_32bit ),
				 libesedb_lcid_language_tag_get_description(
				  (uint16_t) value_32bit ) );
			}
			else
			{
				byte_stream_copy_to_uint32_little_endian(
				 ( (esedb_data_definition_t *) fixed_size_data_type_value_data )->number_of_pages,
				 value_32bit );

				libnotify_printf(
				 "%s: (%03" PRIu16 ") number of pages\t\t\t\t\t: %" PRIu32 "\n",
				 function,
				 data_type_number++,
				 value_32bit );
			}
		}
		if( last_fixed_size_data_type >= 8 )
		{
			libnotify_printf(
			 "%s: (%03" PRIu16 ") root flag\t\t\t\t\t: 0x%02" PRIx8 "\n",
			 function,
			 data_type_number++,
			 ( (esedb_data_definition_t *) fixed_size_data_type_value_data )->root_flag );
		}
		if( last_fixed_size_data_type >= 9 )
		{
			byte_stream_copy_to_uint16_little_endian(
			 ( (esedb_data_definition_t *) fixed_size_data_type_value_data )->record_offset,
			 record_offset );

			libnotify_printf(
			 "%s: (%03" PRIu16 ") record offset\t\t\t\t\t: %" PRIu16 "\n",
			 function,
			 data_type_number++,
			 record_offset );
		}
		if( last_fixed_size_data_type >= 10 )
		{
			byte_stream_copy_to_uint32_little_endian(
			 ( (esedb_data_definition_t *) fixed_size_data_type_value_data )->lc_map_flags,
			 value_32bit );

			libnotify_printf(
			 "%s: (%03" PRIu16 ") locale map (LCMAP) flags\t\t\t: 0x%08" PRIx32 "\n",
			 function,
			 data_type_number++,
			 value_32bit );
		}
		if( last_fixed_size_data_type >= 11 )
		{
			byte_stream_copy_to_uint16_little_endian(
			 ( (esedb_data_definition_t *) fixed_size_data_type_value_data )->key_most,
			 value_16bit );

			libnotify_printf(
			 "%s: (%03" PRIu16 ") key most\t\t\t\t\t: 0x04%" PRIx16 "\n",
			 function,
			 data_type_number++,
			 value_16bit );
		}
		libnotify_printf(
		 "\n" );
	}
#endif

#if defined( HAVE_DEBUG_OUTPUT )
	if( ( libnotify_verbose != 0 )
	 && ( variable_size_data_types_offset > calculated_variable_size_data_types_offset ) )
	{
		libnotify_printf(
		 "%s: trailing data:\n",
		 function );
		libnotify_print_data(
		 &( definition_data[ calculated_variable_size_data_types_offset ] ),
		 variable_size_data_types_offset - calculated_variable_size_data_types_offset );
	}
#endif
	if( number_of_variable_size_data_types > 0 )
	{
		variable_size_data_type_size_data  = &( definition_data[ variable_size_data_types_offset ] );
		variable_size_data_type_value_data = &( variable_size_data_type_size_data[ number_of_variable_size_data_types * 2 ] );

		data_type_number = 128;

		for( variable_size_data_type_iterator = 0;
		     variable_size_data_type_iterator < number_of_variable_size_data_types;
		     variable_size_data_type_iterator++ )
		{
			byte_stream_copy_to_uint16_little_endian(
			 variable_size_data_type_size_data,
			 variable_size_data_type_size );

			variable_size_data_type_size_data += 2;

#if defined( HAVE_DEBUG_OUTPUT )
			if( libnotify_verbose != 0 )
			{
				libnotify_printf(
				 "%s: (%03" PRIu16 ") variable size data type size\t\t\t: 0x%04" PRIx16 " (%" PRIu16 ")\n",
				 function,
				 data_type_number,
				 variable_size_data_type_size,
				 ( ( variable_size_data_type_size & 0x8000 ) != 0 ) ? 0 : ( variable_size_data_type_size & 0x7fff ) - previous_variable_size_data_type_size );
			}
#endif

			switch( data_type_number )
			{
				case 128:
					/* The MSB signifies that the variable size data type is empty
					 */
					if( ( variable_size_data_type_size & 0x8000 ) == 0 )
					{
						catalog_definition->name_size = (size_t) ( variable_size_data_type_size - previous_variable_size_data_type_size );

						catalog_definition->name = (uint8_t *) memory_allocate(
										        sizeof( uint8_t ) * catalog_definition->name_size );

						if( catalog_definition->name == NULL )
						{
							liberror_error_set(
							 error,
							 LIBERROR_ERROR_DOMAIN_MEMORY,
							 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
							 "%s: unable to create name.",
							 function );

							catalog_definition->name_size = 0;

							return( -1 );
						}
						if( memory_copy(
						     catalog_definition->name,
						     &( variable_size_data_type_value_data[ previous_variable_size_data_type_size ] ),
						     catalog_definition->name_size ) == NULL )
						{
							liberror_error_set(
							 error,
							 LIBERROR_ERROR_DOMAIN_MEMORY,
							 LIBERROR_MEMORY_ERROR_COPY_FAILED,
							 "%s: unable to set name.",
							 function );

							memory_free(
							 catalog_definition->name );

							catalog_definition->name      = NULL;
							catalog_definition->name_size = 0;

							return( -1 );
						}
#if defined( HAVE_DEBUG_OUTPUT )
						if( libnotify_verbose != 0 )
						{
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
							result = libuna_utf16_string_size_from_byte_stream(
							          catalog_definition->name,
							          catalog_definition->name_size,
							          ascii_codepage,
							          &value_string_size,
							          error );
#else
							result = libuna_utf8_string_size_from_byte_stream(
							          catalog_definition->name,
							          catalog_definition->name_size,
							          ascii_codepage,
							          &value_string_size,
							          error );
#endif

							if( result != 1 )
							{
								liberror_error_set(
								 error,
								 LIBERROR_ERROR_DOMAIN_RUNTIME,
								 LIBERROR_RUNTIME_ERROR_GET_FAILED,
								 "%s: unable to determine size of name string.",
								 function );

								return( -1 );
							}
							catalog_definition->name_string = libcstring_system_string_allocate(
							                                   value_string_size );

							if( catalog_definition->name_string == NULL )
							{
								liberror_error_set(
								 error,
								 LIBERROR_ERROR_DOMAIN_MEMORY,
								 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
								 "%s: unable to create name string.",
								 function );

								return( -1 );
							}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
							result = libuna_utf16_string_copy_from_byte_stream(
							          (libuna_utf16_character_t *) catalog_definition->name_string,
							          value_string_size,
							          catalog_definition->name,
							          catalog_definition->name_size,
							          ascii_codepage,
							          error );
#else
							result = libuna_utf8_string_copy_from_byte_stream(
							          (libuna_utf8_character_t *) catalog_definition->name_string,
							          value_string_size,
							          catalog_definition->name,
							          catalog_definition->name_size,
							          ascii_codepage,
							          error );
#endif

							if( result != 1 )
							{
								liberror_error_set(
								 error,
								 LIBERROR_ERROR_DOMAIN_CONVERSION,
								 LIBERROR_CONVERSION_ERROR_GENERIC,
								 "%s: unable to set name string.",
								 function );

								memory_free(
								 catalog_definition->name_string );

								catalog_definition->name_string = NULL;

								return( -1 );
							}
							libnotify_printf(
							 "%s: (%03" PRIu8 ") name\t\t\t\t\t\t: %" PRIs_LIBCSTRING_SYSTEM "\n",
							 function,
							 data_type_number,
							 catalog_definition->name_string );
						}
#endif
					}
#if defined( HAVE_DEBUG_OUTPUT )
					else if( libnotify_verbose != 0 )
					{
						libnotify_printf(
						 "%s: (%03" PRIu8 ") name\t\t\t\t\t\t: <NULL>\n",
						 function,
						 data_type_number );
					}
#endif
					break;

#if defined( HAVE_DEBUG_OUTPUT )
				case 129:
					if( libnotify_verbose != 0 )
					{
						/* The MSB signifies that the variable size data type is empty
						 */
						if( ( variable_size_data_type_size & 0x8000 ) == 0 )
						{
							libnotify_printf(
							 "%s: (%03" PRIu8 ") stats:\n",
							 function,
							 data_type_number );
							libnotify_print_data(
							 &( variable_size_data_type_value_data[ previous_variable_size_data_type_size ] ),
							 variable_size_data_type_size - previous_variable_size_data_type_size );
						}
						else
						{
							libnotify_printf(
							 "%s: (%03" PRIu8 ") stats\t\t\t\t\t\t: <NULL>\n",
							 function,
							 data_type_number );
						}
					}
					break;
#endif

				case 130:
					/* The MSB signifies that the variable size data type is empty
					 */
					if( ( variable_size_data_type_size & 0x8000 ) == 0 )
					{
						catalog_definition->template_name_size = (size_t) ( variable_size_data_type_size - previous_variable_size_data_type_size );

						catalog_definition->template_name = (uint8_t *) memory_allocate(
						                                                 sizeof( uint8_t ) * catalog_definition->template_name_size );

						if( catalog_definition->template_name == NULL )
						{
							liberror_error_set(
							 error,
							 LIBERROR_ERROR_DOMAIN_MEMORY,
							 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
							 "%s: unable to create template name.",
							 function );

							catalog_definition->template_name_size = 0;

							return( -1 );
						}
						if( memory_copy(
						     catalog_definition->template_name,
						     &( variable_size_data_type_value_data[ previous_variable_size_data_type_size ] ),
						     catalog_definition->template_name_size ) == NULL )
						{
							liberror_error_set(
							 error,
							 LIBERROR_ERROR_DOMAIN_MEMORY,
							 LIBERROR_MEMORY_ERROR_COPY_FAILED,
							 "%s: unable to set template name.",
							 function );

							memory_free(
							 catalog_definition->template_name );

							catalog_definition->template_name      = NULL;
							catalog_definition->template_name_size = 0;

							return( -1 );
						}
#if defined( HAVE_DEBUG_OUTPUT )
						if( libnotify_verbose != 0 )
						{
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
							result = libuna_utf16_string_size_from_byte_stream(
							          catalog_definition->template_name,
							          catalog_definition->template_name_size,
							          ascii_codepage,
							          &value_string_size,
							          error );
#else
							result = libuna_utf8_string_size_from_byte_stream(
							          catalog_definition->template_name,
							          catalog_definition->template_name_size,
							          ascii_codepage,
							          &value_string_size,
							          error );
#endif

							if( result != 1 )
							{
								liberror_error_set(
								 error,
								 LIBERROR_ERROR_DOMAIN_RUNTIME,
								 LIBERROR_RUNTIME_ERROR_GET_FAILED,
								 "%s: unable to determine size of template name string.",
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
								 "%s: unable to create template name string.",
								 function );

								return( -1 );
							}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
							result = libuna_utf16_string_copy_from_byte_stream(
							          (libuna_utf16_character_t *) value_string,
							          value_string_size,
							          catalog_definition->template_name,
							          catalog_definition->template_name_size,
							          ascii_codepage,
							          error );
#else
							result = libuna_utf8_string_copy_from_byte_stream(
							          (libuna_utf8_character_t *) value_string,
							          value_string_size,
							          catalog_definition->template_name,
							          catalog_definition->template_name_size,
							          ascii_codepage,
							          error );
#endif

							if( result != 1 )
							{
								liberror_error_set(
								 error,
								 LIBERROR_ERROR_DOMAIN_CONVERSION,
								 LIBERROR_CONVERSION_ERROR_GENERIC,
								 "%s: unable to set template name string.",
								 function );

								memory_free(
								 value_string );

								return( -1 );
							}
							libnotify_printf(
							 "%s: (%03" PRIu8 ") template name\t\t\t\t\t: %" PRIs_LIBCSTRING_SYSTEM "\n",
							 function,
							 data_type_number,
							 value_string );

							memory_free(
							 value_string );
						}
#endif
					}
#if defined( HAVE_DEBUG_OUTPUT )
					else if( libnotify_verbose != 0 )
					{
						libnotify_printf(
						 "%s: (%03" PRIu8 ") template name\t\t\t\t\t: <NULL>\n",
						 function,
						 data_type_number );
					}
#endif
					break;

				case 131:
					/* The MSB signifies that the variable size data type is empty
					 */
					if( ( variable_size_data_type_size & 0x8000 ) == 0 )
					{
						catalog_definition->default_value_size = (size_t) ( variable_size_data_type_size - previous_variable_size_data_type_size );

						catalog_definition->default_value = (uint8_t *) memory_allocate(
												 sizeof( uint8_t ) * catalog_definition->default_value_size );

						if( catalog_definition->default_value == NULL )
						{
							liberror_error_set(
							 error,
							 LIBERROR_ERROR_DOMAIN_MEMORY,
							 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
							 "%s: unable to create default value.",
							 function );

							catalog_definition->default_value_size = 0;

							return( -1 );
						}
						if( memory_copy(
						     catalog_definition->default_value,
						     &( variable_size_data_type_value_data[ previous_variable_size_data_type_size ] ),
						     catalog_definition->default_value_size ) == NULL )
						{
							liberror_error_set(
							 error,
							 LIBERROR_ERROR_DOMAIN_MEMORY,
							 LIBERROR_MEMORY_ERROR_COPY_FAILED,
							 "%s: unable to set default value.",
							 function );

							memory_free(
							 catalog_definition->default_value );

							catalog_definition->default_value      = NULL;
							catalog_definition->default_value_size = 0;

							return( -1 );
						}
#if defined( HAVE_DEBUG_OUTPUT )
						if( libnotify_verbose != 0 )
						{
							libnotify_printf(
							 "%s: (%03" PRIu8 ") default value:\n",
							 function,
							 data_type_number );
							libnotify_print_data(
							 catalog_definition->default_value,
							 catalog_definition->default_value_size );
						}
#endif
					}
#if defined( HAVE_DEBUG_OUTPUT )
					else if( libnotify_verbose != 0 )
					{
						libnotify_printf(
						 "%s: (%03" PRIu8 ") default value\t\t\t\t\t: <NULL>\n",
						 function,
						 data_type_number );
					}
#endif
					break;

#if defined( HAVE_DEBUG_OUTPUT )
				case 132:
					if( libnotify_verbose != 0 )
					{
						/* The MSB signifies that the variable size data type is empty
						 */
						if( ( variable_size_data_type_size & 0x8000 ) == 0 )
						{
							libnotify_printf(
							 "%s: (%03" PRIu8 ") KeyFldIDs:\n",
							 function,
							 data_type_number );
							libnotify_print_data(
							 &( variable_size_data_type_value_data[ previous_variable_size_data_type_size ] ),
							 variable_size_data_type_size - previous_variable_size_data_type_size );
						}
						else
						{
							libnotify_printf(
							 "%s: (%03" PRIu8 ") KeyFldIDs\t\t\t\t\t: <NULL>\n",
							 function,
							 data_type_number );
						}
					}
					break;

				case 133:
					if( libnotify_verbose != 0 )
					{
						/* The MSB signifies that the variable size data type is empty
						 */
						if( ( variable_size_data_type_size & 0x8000 ) == 0 )
						{
							libnotify_printf(
							 "%s: (%03" PRIu8 ") VarSegMac:\n",
							 function,
							 data_type_number );
							libnotify_print_data(
							 &( variable_size_data_type_value_data[ previous_variable_size_data_type_size ] ),
							 variable_size_data_type_size - previous_variable_size_data_type_size );
						}
						else
						{
							libnotify_printf(
							 "%s: (%03" PRIu8 ") VarSegMac\t\t\t\t\t: <NULL>\n",
							 function,
							 data_type_number );
						}
					}
					break;

				case 134:
					if( libnotify_verbose != 0 )
					{
						/* The MSB signifies that the variable size data type is empty
						 */
						if( ( variable_size_data_type_size & 0x8000 ) == 0 )
						{
							libnotify_printf(
							 "%s: (%03" PRIu8 ") ConditionalColumns:\n",
							 function,
							 data_type_number );
							libnotify_print_data(
							 &( variable_size_data_type_value_data[ previous_variable_size_data_type_size ] ),
							 variable_size_data_type_size - previous_variable_size_data_type_size );
						}
						else
						{
							libnotify_printf(
							 "%s: (%03" PRIu8 ") ConditionalColumns\t\t\t\t: <NULL>\n",
							 function,
							 data_type_number );
						}
					}
					break;

				case 135:
					if( libnotify_verbose != 0 )
					{
						/* The MSB signifies that the variable size data type is empty
						 */
						if( ( variable_size_data_type_size & 0x8000 ) == 0 )
						{
							libnotify_printf(
							 "%s: (%03" PRIu8 ") TupleLimits:\n",
							 function,
							 data_type_number );
							libnotify_print_data(
							 &( variable_size_data_type_value_data[ previous_variable_size_data_type_size ] ),
							 variable_size_data_type_size - previous_variable_size_data_type_size );
						}
						else
						{
							libnotify_printf(
							 "%s: (%03" PRIu8 ") TupleLimits\t\t\t\t\t: <NULL>\n",
							 function,
							 data_type_number );
						}
					}
					break;

				case 136:
					if( libnotify_verbose != 0 )
					{
						/* The MSB signifies that the variable size data type is empty
						 */
						if( ( variable_size_data_type_size & 0x8000 ) == 0 )
						{
							libnotify_printf(
							 "%s: (%03" PRIu8 ") Version:\n",
							 function,
							 data_type_number );
							libnotify_print_data(
							 &( variable_size_data_type_value_data[ previous_variable_size_data_type_size ] ),
							 variable_size_data_type_size - previous_variable_size_data_type_size );
						}
						else
						{
							libnotify_printf(
							 "%s: (%03" PRIu8 ") Version\t\t\t\t\t\t: <NULL>\n",
							 function,
							 data_type_number );
						}
					}
					break;
#endif

				default:
#if defined( HAVE_DEBUG_OUTPUT )
					if( libnotify_verbose != 0 )
					{
						/* The MSB signifies that the variable size data type is empty
						 */
						if( ( variable_size_data_type_size & 0x8000 ) == 0 )
						{
							libnotify_printf(
							 "%s: (%03" PRIu16 ") variable size data type:\n",
							 function,
							 data_type_number );
							libnotify_print_data(
							 &( variable_size_data_type_value_data[ previous_variable_size_data_type_size ] ),
							 variable_size_data_type_size - previous_variable_size_data_type_size );
						}
						else
						{
							libnotify_printf(
							 "%s: (%03" PRIu16 ") variable size data type\t\t\t: <NULL>\n",
							 function,
							 data_type_number );
						}
					}
#endif
					break;
			}
			/* The MSB signifies that the variable size data type is empty
			 */
			if( ( variable_size_data_type_size & 0x8000 ) == 0 )
			{
				previous_variable_size_data_type_size = variable_size_data_type_size;
			}
			data_type_number++;
		}
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libnotify_verbose != 0 )
	{
		libnotify_printf(
		 "\n" );
	}
#endif

	return( 1 );
}

/* Retrieves the catalog definition identifier
 * Returns 1 if successful or -1 on error
 */
int libesedb_catalog_definition_get_identifier(
     libesedb_catalog_definition_t *catalog_definition,
     uint32_t *identifier,
     liberror_error_t **error )
{
	static char *function = "libesedb_catalog_definition_get_identifier";

	if( catalog_definition == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid catalog definition.",
		 function );

		return( -1 );
	}
	if( identifier == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid identifier.",
		 function );

		return( -1 );
	}
	*identifier = catalog_definition->identifier;

	return( 1 );
}

/* Retrieves the catalog definition column type
 * Returns 1 if successful or -1 on error
 */
int libesedb_catalog_definition_get_column_type(
     libesedb_catalog_definition_t *catalog_definition,
     uint32_t *column_type,
     liberror_error_t **error )
{
	static char *function = "libesedb_catalog_definition_get_column_type";

	if( catalog_definition == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid catalog definition.",
		 function );

		return( -1 );
	}
	if( column_type == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid column type.",
		 function );

		return( -1 );
	}
	*column_type = catalog_definition->column_type;

	return( 1 );
}

/* Retrieves the size of the UTF-8 encoded string string of the catalog definition name
 * The returned size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
int libesedb_catalog_definition_get_utf8_name_size(
     libesedb_catalog_definition_t *catalog_definition,
     size_t *utf8_string_size,
     int ascii_codepage,
     liberror_error_t **error )
{
	static char *function = "libesedb_catalog_definition_get_utf8_name_size";
	int result            = 0;

	if( catalog_definition == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid catalog definition.",
		 function );

		return( -1 );
	}
	result = libuna_utf8_string_size_from_byte_stream(
		  catalog_definition->name,
		  catalog_definition->name_size,
		  ascii_codepage,
		  utf8_string_size,
		  error );

	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve UTF-8 string size.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the UTF-8 encoded string of the catalog definition name
 * The size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libesedb_catalog_definition_get_utf8_name(
     libesedb_catalog_definition_t *catalog_definition,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     int ascii_codepage,
     liberror_error_t **error )
{
	static char *function = "libesedb_catalog_definition_get_utf8_name";
	int result            = 0;

	if( catalog_definition == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid catalog definition.",
		 function );

		return( -1 );
	}
	result = libuna_utf8_string_copy_from_byte_stream(
		  utf8_string,
		  utf8_string_size,
		  catalog_definition->name,
		  catalog_definition->name_size,
		  ascii_codepage,
		  error );

	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set UTF-8 string.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the size of the UTF-16 encoded string of the catalog definition name
 * The returned size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
int libesedb_catalog_definition_get_utf16_name_size(
     libesedb_catalog_definition_t *catalog_definition,
     size_t *utf16_string_size,
     int ascii_codepage,
     liberror_error_t **error )
{
	static char *function = "libesedb_catalog_definition_get_utf16_name_size";
	int result            = 0;

	if( catalog_definition == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid catalog definition.",
		 function );

		return( -1 );
	}
	result = libuna_utf16_string_size_from_byte_stream(
		  catalog_definition->name,
		  catalog_definition->name_size,
		  ascii_codepage,
		  utf16_string_size,
		  error );

	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve UTF-16 string size.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the UTF-16 encoded string of the catalog definition name
 * The size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libesedb_catalog_definition_get_utf16_name(
     libesedb_catalog_definition_t *catalog_definition,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     int ascii_codepage,
     liberror_error_t **error )
{
	static char *function = "libesedb_catalog_definition_get_utf16_name";
	int result            = 0;

	if( catalog_definition == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid catalog definition.",
		 function );

		return( -1 );
	}
	result = libuna_utf16_string_copy_from_byte_stream(
		  utf16_string,
		  utf16_string_size,
		  catalog_definition->name,
		  catalog_definition->name_size,
		  ascii_codepage,
		  error );

	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set UTF-16 string.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the size of the UTF-8 encoded string string of the catalog definition template name
 * The returned size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
int libesedb_catalog_definition_get_utf8_template_name_size(
     libesedb_catalog_definition_t *catalog_definition,
     size_t *utf8_string_size,
     int ascii_codepage,
     liberror_error_t **error )
{
	static char *function = "libesedb_catalog_definition_get_utf8_template_name_size";
	int result            = 0;

	if( catalog_definition == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid catalog definition.",
		 function );

		return( -1 );
	}
	if( catalog_definition->template_name == NULL )
	{
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
		*utf8_string_size = 0;
	}
	else
	{
		result = libuna_utf8_string_size_from_byte_stream(
			  catalog_definition->template_name,
			  catalog_definition->template_name_size,
			  ascii_codepage,
			  utf8_string_size,
			  error );

		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve UTF-8 string size.",
			 function );

			return( -1 );
		}
	}
	return( 1 );
}

/* Retrieves the UTF-8 encoded string of the catalog definition template name
 * The size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libesedb_catalog_definition_get_utf8_template_name(
     libesedb_catalog_definition_t *catalog_definition,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     int ascii_codepage,
     liberror_error_t **error )
{
	static char *function = "libesedb_catalog_definition_get_utf8_template_name";
	int result            = 0;

	if( catalog_definition == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid catalog definition.",
		 function );

		return( -1 );
	}
	if( catalog_definition->template_name != NULL )
	{
		result = libuna_utf8_string_copy_from_byte_stream(
			  utf8_string,
			  utf8_string_size,
			  catalog_definition->template_name,
			  catalog_definition->template_name_size,
			  ascii_codepage,
			  error );

		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set UTF-8 string.",
			 function );

			return( -1 );
		}
	}
	return( 1 );
}

/* Retrieves the size of the UTF-16 encoded string of the catalog definition template name
 * The returned size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
int libesedb_catalog_definition_get_utf16_template_name_size(
     libesedb_catalog_definition_t *catalog_definition,
     size_t *utf16_string_size,
     int ascii_codepage,
     liberror_error_t **error )
{
	static char *function = "libesedb_catalog_definition_get_utf16_template_name_size";
	int result            = 0;

	if( catalog_definition == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid catalog definition.",
		 function );

		return( -1 );
	}
	if( catalog_definition->template_name == NULL )
	{
		if( utf16_string_size == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
			 "%s: invalid UTF-16 string size.",
			 function );

			return( -1 );
		}
		*utf16_string_size = 0;
	}
	else
	{
		result = libuna_utf16_string_size_from_byte_stream(
			  catalog_definition->template_name,
			  catalog_definition->template_name_size,
			  ascii_codepage,
			  utf16_string_size,
			  error );

		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve UTF-16 string size.",
			 function );

			return( -1 );
		}
	}
	return( 1 );
}

/* Retrieves the UTF-16 encoded string of the catalog definition template name
 * The size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libesedb_catalog_definition_get_utf16_template_name(
     libesedb_catalog_definition_t *catalog_definition,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     int ascii_codepage,
     liberror_error_t **error )
{
	static char *function = "libesedb_catalog_definition_get_utf16_template_name";
	int result            = 0;

	if( catalog_definition == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid catalog definition.",
		 function );

		return( -1 );
	}
	if( catalog_definition->template_name != NULL )
	{
		result = libuna_utf16_string_copy_from_byte_stream(
			  utf16_string,
			  utf16_string_size,
			  catalog_definition->template_name,
			  catalog_definition->template_name_size,
			  ascii_codepage,
			  error );

		if( result != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set UTF-16 string.",
			 function );

			return( -1 );
		}
	}
	return( 1 );
}

