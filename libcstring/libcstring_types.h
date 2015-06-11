/*
 * Type definitions
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

#if !defined( _LIBCSTRING_TYPES_H )
#define _LIBCSTRING_TYPES_H

#include <common.h>
#include <types.h>

/* The wide character include file (wchar.h)
 * is included by types.h
 */

#include "libcstring_definitions.h"

#if defined( _cplusplus )
extern "C" {
#endif

#if defined( LIBCSTRING_HAVE_16BIT_CHARACTER )

/* The character type is 16-bit
 * A string contains UTF-16
 */
typedef uint16_t libcstring_character_t;

#else

/* The character type is 8-bit
 * A string contains UTF-8
 */
typedef uint8_t libcstring_character_t;

#endif

#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )

/* The system character type is wide
 * A system string contains either UTF-16 or UTF-32
 */
typedef wchar_t libcstring_system_character_t;
typedef wint_t libcstring_system_integer_t;

#else

/* The system character type is narrow
 * A system string contains either UTF-8 or extended ASCII with a codepage
 */
typedef char libcstring_system_character_t;
typedef int libcstring_system_integer_t;

#endif

#if defined( _cplusplus )
}
#endif

#endif

