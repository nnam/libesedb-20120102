/*
 * Character type string functions
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

#if !defined( _LIBCSTRING_STRING_H )
#define _LIBCSTRING_STRING_H

#include <common.h>
#include <types.h>

#include "libcstring_definitions.h"
#include "libcstring_narrow_string.h"
#include "libcstring_types.h"
#include "libcstring_wide_string.h"

#if defined( _cplusplus )
extern "C" {
#endif

#if defined( LIBCSTRING_HAVE_16BIT_CHARACTER )
#error 16-bit character type string functions not yet supported

#else
#define _LIBCSTRING_STRING( string ) \
	(libcstring_character_t *) string

#define libcstring_string_allocate( size ) \
	(uint8_t *) libcstring_narrow_string_allocate( size )

#define libcstring_string_compare( string1, string2, size ) \
	libcstring_narrow_string_compare( (char *) string1, (char *) string2, size )

#define libcstring_string_compare_no_case( string1, string2, size ) \
	libcstring_narrow_string_compare_no_case( (char *) string1, (char *) string2, size )

#define libcstring_string_copy( destination, source, size ) \
	(libcstring_character_t *) libcstring_narrow_string_copy( (char *) destination, (char *) source, size )

#define libcstring_string_length( string ) \
	libcstring_narrow_string_length( (char *) string )

#define libcstring_string_search_character( string, character, size ) \
	(libcstring_character_t *) libcstring_narrow_string_search_character( (char *) string, (int) character, size )

#define libcstring_string_search_character_reverse( string, character, size ) \
	(libcstring_character_t *) libcstring_narrow_string_search_character_reverse( (char *) string, (int) character, size )

#define libcstring_string_search_string( string, substring, size ) \
	(libcstring_character_t *) libcstring_narrow_string_search_string( (char *) string, (char *) substring, size )

#if defined( __BORLANDC__ ) && ( __BORLANDC__ < 0x0560 )
#define libcstring_string_snprintf \
        libcstring_narrow_string_snprintf

#else
#define libcstring_string_snprintf( target, size, ... ) \
	libcstring_narrow_string_snprintf( (char *) target, size, __VA_ARGS__ )
#endif

#endif /* defined( LIBCSTRING_HAVE_16BIT_CHARACTER ) */

#if defined( _cplusplus )
}
#endif

#endif

