/*
 * The internal definitions
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

#if !defined( LIBFDATETIME_INTERNAL_DEFINITIONS_H )
#define LIBFDATETIME_INTERNAL_DEFINITIONS_H

#include <common.h>
#include <types.h>

/* Define HAVE_LOCAL_LIBFDATETIME for local use of libfdatetime
 */
#if !defined( HAVE_LOCAL_LIBFDATETIME )
#include <libfdatetime/definitions.h>

/* The definitions in <libfdatetime/definitions.h> are copied here
 * for local use of libfdatetime
 */
#else
#include <byte_stream.h>

#define LIBFDATETIME_VERSION					20120102

/* The version string
 */
#define LIBFDATETIME_VERSION_STRING				"20120102"

/* The byte order definitions
 */
#define LIBFDATETIME_ENDIAN_BIG					_BYTE_STREAM_ENDIAN_BIG
#define LIBFDATETIME_ENDIAN_LITTLE				_BYTE_STREAM_ENDIAN_LITTLE

/* The string format definition flags
 */
enum LIBFDATETIME_STRING_FORMAT_FLAGS
{
	LIBFDATETIME_STRING_FORMAT_FLAG_DATE			= 0x01,
	LIBFDATETIME_STRING_FORMAT_FLAG_TIME			= 0x02,
	LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MICRO_SECONDS	= 0x04,
	LIBFDATETIME_STRING_FORMAT_FLAG_DURATION		= 0x08
};

#define LIBFDATETIME_STRING_FORMAT_FLAG_DATE_TIME \
	( LIBFDATETIME_STRING_FORMAT_FLAG_DATE | LIBFDATETIME_STRING_FORMAT_FLAG_TIME )

#define LIBFDATETIME_STRING_FORMAT_FLAG_DATE_TIME_MICRO_SECONDS \
	( LIBFDATETIME_STRING_FORMAT_FLAG_DATE | LIBFDATETIME_STRING_FORMAT_FLAG_TIME | LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MICRO_SECONDS )

/* The date and time format definitions
 */
enum LIBFDATETIME_DATE_TIME_FORMATS
{
        LIBFDATETIME_DATE_TIME_FORMAT_CTIME			= (int) 'c',
        LIBFDATETIME_DATE_TIME_FORMAT_ISO8601			= (int) 'i'
};

#endif

#endif

