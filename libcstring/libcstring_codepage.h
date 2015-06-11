/*
 * Codepage functions
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

#if !defined( _LIBCSTRING_CODEPAGE_H )
#define _LIBCSTRING_CODEPAGE_H

#include <common.h>
#include <types.h>

#if defined( __cplusplus )
extern "C" {
#endif

/* The codepage definitions
 */
enum LIBCSTRING_CODEPAGES
{
	LIBCSTRING_CODEPAGE_ASCII			= 20127,

	LIBCSTRING_CODEPAGE_ISO_8859_1			= 28591,
	LIBCSTRING_CODEPAGE_ISO_8859_2			= 28592,
	LIBCSTRING_CODEPAGE_ISO_8859_3			= 28593,
	LIBCSTRING_CODEPAGE_ISO_8859_4			= 28594,
	LIBCSTRING_CODEPAGE_ISO_8859_5			= 28595,
	LIBCSTRING_CODEPAGE_ISO_8859_6			= 28596,
	LIBCSTRING_CODEPAGE_ISO_8859_7			= 28597,
	LIBCSTRING_CODEPAGE_ISO_8859_8			= 28598,
	LIBCSTRING_CODEPAGE_ISO_8859_9			= 28599,
	LIBCSTRING_CODEPAGE_ISO_8859_10			= 28600,
	LIBCSTRING_CODEPAGE_ISO_8859_11			= 28601,
	LIBCSTRING_CODEPAGE_ISO_8859_13			= 28603,
	LIBCSTRING_CODEPAGE_ISO_8859_14			= 28604,
	LIBCSTRING_CODEPAGE_ISO_8859_15			= 28605,
	LIBCSTRING_CODEPAGE_ISO_8859_16			= 28606,

	LIBCSTRING_CODEPAGE_WINDOWS_874			= 874,
	LIBCSTRING_CODEPAGE_WINDOWS_932			= 932,
	LIBCSTRING_CODEPAGE_WINDOWS_936			= 936,
	LIBCSTRING_CODEPAGE_WINDOWS_1250		= 1250,
	LIBCSTRING_CODEPAGE_WINDOWS_1251		= 1251,
	LIBCSTRING_CODEPAGE_WINDOWS_1252		= 1252,
	LIBCSTRING_CODEPAGE_WINDOWS_1253		= 1253,
	LIBCSTRING_CODEPAGE_WINDOWS_1254		= 1254,
	LIBCSTRING_CODEPAGE_WINDOWS_1255		= 1255,
	LIBCSTRING_CODEPAGE_WINDOWS_1256		= 1256,
	LIBCSTRING_CODEPAGE_WINDOWS_1257		= 1257,
	LIBCSTRING_CODEPAGE_WINDOWS_1258		= 1258
};

#if defined( __cplusplus )
}
#endif

#endif

