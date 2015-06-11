/*
 * Definitions
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

#if !defined( _LIBCSTRING_DEFINITIONS_H )
#define _LIBCSTRING_DEFINITIONS_H

#include <common.h>

/* Detect if the code is being compiled with Windows Unicode support
 */
#if defined( WINAPI ) && ( defined( _UNICODE ) || defined( UNICODE ) )
#define LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER		1
#endif

#define LIBCSTRING_VERSION				20120102

/* The libcstring version string
 */
#define LIBCSTRING_VERSION_STRING			"20120102"

#if defined( LIBCSTRING_HAVE_WIDE_CHARACTER )
#define PRIc_LIBCSTRING					"lc"
#define PRIs_LIBCSTRING					"ls"

#else
#define PRIc_LIBCSTRING					"c"
#define PRIs_LIBCSTRING					"s"

#endif

#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
#define PRIc_LIBCSTRING_SYSTEM				"lc"
#define PRIs_LIBCSTRING_SYSTEM				"ls"

#else
#define PRIc_LIBCSTRING_SYSTEM				"c"
#define PRIs_LIBCSTRING_SYSTEM				"s"

#endif

#endif

