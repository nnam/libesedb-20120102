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

#if !defined( _LIBESEDB_CATALOG_DEFINITION_H )
#define _LIBESEDB_CATALOG_DEFINITION_H

#include <common.h>
#include <types.h>

#include <libcstring.h>
#include <liberror.h>

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libesedb_catalog_definition libesedb_catalog_definition_t;

struct libesedb_catalog_definition
{
	/* The father data page (FDP) object identifier
	 */
	uint32_t father_data_page_object_identifier;

	/* The type
	 */
	uint16_t type;

	/* The identifier
	 */
	uint32_t identifier;

	union
	{
		/* The father data page (FDP) number 
		 */
		uint32_t father_data_page_number;

		/* The column type
		 */
		uint32_t column_type;
	};

	/* The size (or space usage)
	 */
	uint32_t size;

	/* The codepage
	 */
	uint32_t codepage;

	/* The name
	 */
	uint8_t *name;

	/* The name size
	 */
	size_t name_size;

#if defined( HAVE_DEBUG_OUTPUT )
	/* The name string
	 */
	libcstring_system_character_t *name_string;
#endif

	/* The template name
	 */
	uint8_t *template_name;

	/* The template name size
	 */
	size_t template_name_size;

	/* The default value
	 */
	uint8_t *default_value;

	/* The default value size
	 */
	size_t default_value_size;
};

int libesedb_catalog_definition_initialize(
     libesedb_catalog_definition_t **catalog_definition,
     liberror_error_t **error );

int libesedb_catalog_definition_free(
     libesedb_catalog_definition_t **catalog_definition,
     liberror_error_t **error );

int libesedb_catalog_definition_read(
     libesedb_catalog_definition_t *catalog_definition,
     uint8_t *definition_data,
     size_t definition_data_size,
     int ascii_codepage,
     liberror_error_t **error );

int libesedb_catalog_definition_get_identifier(
     libesedb_catalog_definition_t *catalog_definition,
     uint32_t *identifier,
     liberror_error_t **error );

int libesedb_catalog_definition_get_column_type(
     libesedb_catalog_definition_t *catalog_definition,
     uint32_t *column_type,
     liberror_error_t **error );

int libesedb_catalog_definition_get_utf8_name_size(
     libesedb_catalog_definition_t *catalog_definition,
     size_t *utf8_string_size,
     int ascii_codepage,
     liberror_error_t **error );

int libesedb_catalog_definition_get_utf8_name(
     libesedb_catalog_definition_t *catalog_definition,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     int ascii_codepage,
     liberror_error_t **error );

int libesedb_catalog_definition_get_utf16_name_size(
     libesedb_catalog_definition_t *catalog_definition,
     size_t *utf16_string_size,
     int ascii_codepage,
     liberror_error_t **error );

int libesedb_catalog_definition_get_utf16_name(
     libesedb_catalog_definition_t *catalog_definition,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     int ascii_codepage,
     liberror_error_t **error );

int libesedb_catalog_definition_get_utf8_template_name_size(
     libesedb_catalog_definition_t *catalog_definition,
     size_t *utf8_string_size,
     int ascii_codepage,
     liberror_error_t **error );

int libesedb_catalog_definition_get_utf8_template_name(
     libesedb_catalog_definition_t *catalog_definition,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     int ascii_codepage,
     liberror_error_t **error );

int libesedb_catalog_definition_get_utf16_template_name_size(
     libesedb_catalog_definition_t *catalog_definition,
     size_t *utf16_string_size,
     int ascii_codepage,
     liberror_error_t **error );

int libesedb_catalog_definition_get_utf16_template_name(
     libesedb_catalog_definition_t *catalog_definition,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     int ascii_codepage,
     liberror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

