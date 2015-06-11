/*
 * The internal definitions
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

#if !defined( LIBFVALUE_INTERNAL_DEFINITIONS_H )
#define LIBFVALUE_INTERNAL_DEFINITIONS_H

#include <common.h>
#include <types.h>

#define LIBFVALUE_VERSION					20120102

/* The libfvalue version string
 */
#define LIBFVALUE_VERSION_STRING				"20120102"

/* The endian definitions
 */
#define LIBFVALUE_ENDIAN_NATIVE					(uint8_t) 'n'
#define LIBFVALUE_ENDIAN_BIG					_BYTE_STREAM_ENDIAN_BIG
#define LIBFVALUE_ENDIAN_LITTLE					_BYTE_STREAM_ENDIAN_LITTLE

/* The value flags definitions
 */
enum LIBFVALUE_VALUE_FLAGS
{
	/* The identifier is not managed by the value
	 * the identifier is expected to be available
	 * during the life-time of the value
	 */
        LIBFVALUE_VALUE_FLAG_IDENTIFIER_NON_MANAGED		= 0x00,

	/* The identifier is managed by the value
	 */
        LIBFVALUE_VALUE_FLAG_IDENTIFIER_MANAGED			= 0x01,

	/* The data is not managed by the value
	 * the data is expected to be available
	 * during the life-time of the value
	 */
        LIBFVALUE_VALUE_FLAG_DATA_NON_MANAGED			= 0x00,

	/* The data is managed by the value
	 */
        LIBFVALUE_VALUE_FLAG_DATA_MANAGED			= 0x02,

	/* The metadata is not managed by the value
	 * the metadata is expected to be available
	 * during the life-time of the value
	 */
        LIBFVALUE_VALUE_FLAG_METADATA_NON_MANAGED		= 0x00,

	/* The metadata is managed by the value
	 */
        LIBFVALUE_VALUE_FLAG_METADATA_MANAGED			= 0x04
};

/* The value identifier flags definitions
 */
enum LIBFVALUE_VALUE_IDENTIFIER_FLAGS
{
	/* The identifier is not managed by the value
	 *
	 * if LIBFVALUE_VALUE_FLAG_IDENTIFIER_MANAGED is not set
	 * the identifier is expected to be available
	 * during the life-time of the value
	 *
	 * if LIBFVALUE_VALUE_FLAG_IDENTIFIER_MANAGED is set
	 * a copy of the identifier will be managed by the value
	 */
        LIBFVALUE_VALUE_IDENTIFIER_FLAG_NON_MANAGED		= 0x00,

	/* The identifier is managed by the value
	 * This will also set LIBFVALUE_VALUE_FLAG_IDENTIFIER_MANAGED in the value
	 */
        LIBFVALUE_VALUE_IDENTIFIER_FLAG_MANAGED			= 0x01,

	/* The identifier is not cloned, but passed as a reference
	 */
        LIBFVALUE_VALUE_IDENTIFIER_FLAG_CLONE_BY_REFERENCE	= 0x10
};

/* The value data flags definitions
 */
enum LIBFVALUE_VALUE_DATA_FLAGS
{
	/* The data is not managed by the value
	 *
	 * if LIBFVALUE_VALUE_FLAG_DATA_MANAGED is not set
	 * the data is expected to be available
	 * during the life-time of the value
	 *
	 * if LIBFVALUE_VALUE_FLAG_DATA_MANAGED is set
	 * a copy of the data will be managed by the value
	 */
        LIBFVALUE_VALUE_DATA_FLAG_NON_MANAGED			= 0x00,

	/* The data is managed by the value
	 * This will also set LIBFVALUE_VALUE_FLAG_DATA_MANAGED in the value
	 */
        LIBFVALUE_VALUE_DATA_FLAG_MANAGED			= 0x02,

	/* The data is not cloned, but passed as a reference
	 */
        LIBFVALUE_VALUE_DATA_FLAG_CLONE_BY_REFERENCE		= 0x10
};

/* The value metadata flags definitions
 */
enum LIBFVALUE_VALUE_METADATA_FLAGS
{
	/* The metadata is not managed by the value
	 *
	 * if LIBFVALUE_VALUE_FLAG_METADATA_MANAGED is not set
	 * the metadata is expected to be available
	 * during the life-time of the value
	 *
	 * if LIBFVALUE_VALUE_FLAG_METADATA_MANAGED is set
	 * a copy of the metadata will be managed by the value
	 */
        LIBFVALUE_VALUE_METADATA_FLAG_NON_MANAGED		= 0x00,

	/* The metadata is managed by the value
	 * This will also set LIBFVALUE_VALUE_FLAG_METADATA_MANAGED in the value
	 */
        LIBFVALUE_VALUE_METADATA_FLAG_MANAGED			= 0x04,

	/* The metadata is not cloned, but passed as a reference
	 */
        LIBFVALUE_VALUE_METADATA_FLAG_CLONE_BY_REFERENCE	= 0x10
};

/* The value class definitions
 */
enum LIBFVALUE_VALUE_CLASSES
{
	LIBFVALUE_VALUE_CLASS_UNDEFINED				= 0,
	LIBFVALUE_VALUE_CLASS_BINARY,
	LIBFVALUE_VALUE_CLASS_BOOLEAN,
	LIBFVALUE_VALUE_CLASS_DATETIME,
	LIBFVALUE_VALUE_CLASS_ENUMERATION,
	LIBFVALUE_VALUE_CLASS_FLOATING_POINT,
	LIBFVALUE_VALUE_CLASS_INTEGER,
	LIBFVALUE_VALUE_CLASS_STRING
};

/* The value type definitions
 */
enum LIBFVALUE_VALUE_TYPES
{
	LIBFVALUE_VALUE_TYPE_UNDEFINED				= 0,

	LIBFVALUE_VALUE_TYPE_BINARY_DATA,

	LIBFVALUE_VALUE_TYPE_BOOLEAN,

	LIBFVALUE_VALUE_TYPE_ENUMERATION,

	LIBFVALUE_VALUE_TYPE_INTEGER_8BIT,
	LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_8BIT,
	LIBFVALUE_VALUE_TYPE_INTEGER_16BIT,
	LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_16BIT,
	LIBFVALUE_VALUE_TYPE_INTEGER_32BIT,
	LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_32BIT,
	LIBFVALUE_VALUE_TYPE_INTEGER_64BIT,
	LIBFVALUE_VALUE_TYPE_UNSIGNED_INTEGER_64BIT,

	LIBFVALUE_VALUE_TYPE_FLOATING_POINT_32BIT,
	LIBFVALUE_VALUE_TYPE_FLOATING_POINT_64BIT,

	LIBFVALUE_VALUE_TYPE_STRING_BYTE_STREAM,
	LIBFVALUE_VALUE_TYPE_STRING_UTF8,
	LIBFVALUE_VALUE_TYPE_STRING_UTF16,
	LIBFVALUE_VALUE_TYPE_STRING_UTF32,

	LIBFVALUE_VALUE_TYPE_FATDATE,
	LIBFVALUE_VALUE_TYPE_FATDATETIME,
	LIBFVALUE_VALUE_TYPE_FATTIME,
	LIBFVALUE_VALUE_TYPE_FILETIME,
	LIBFVALUE_VALUE_TYPE_NSF_TIMEDATE,
	LIBFVALUE_VALUE_TYPE_HFSTIME,
	LIBFVALUE_VALUE_TYPE_POSIX_TIME,

	LIBFVALUE_VALUE_TYPE_GUID
};

/* The value format definitions
 */
enum LIBFVALUE_VALUE_FORMATS
{
	LIBFVALUE_VALUE_FORMAT_UNDEFINED			= 0,

	LIBFVALUE_VALUE_FORMAT_DATE,
	LIBFVALUE_VALUE_FORMAT_TIME,
	LIBFVALUE_VALUE_FORMAT_DATE_TIME,
	LIBFVALUE_VALUE_FORMAT_DURATION,

	LIBFVALUE_VALUE_FORMAT_DECIMAL,
	LIBFVALUE_VALUE_FORMAT_HEXADECIMAL
};

/* The table flags definitions
 */
enum LIBFVALUE_TABLE_FLAGS
{
	/* Allow a partial match of the identifier
	 */
        LIBFVALUE_TABLE_FLAG_ALLOW_PARTIAL_MATCH		= 0x01
};

/* The debug print flags definitions
 */
enum LIBFVALUE_DEBUG_PRINT_FLAGS
{
	/* Print the value with a type description
	 */
        LIBFVALUE_DEBUG_PRINT_FLAG_WITH_TYPE_DESCRIPTION	= 0x01
};

#endif

