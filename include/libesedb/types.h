/*
 * Type definitions for libesedb
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

#if !defined( _LIBESEDB_TYPES_H )
#define _LIBESEDB_TYPES_H

#include <libesedb/features.h>

/* Microsoft C/C++ compiler
 */
#if defined( _MSC_VER )

/* Define _LIBESEDB_TYPES_H_INTEGERS to avoid conflict
 */
#if !defined( _LIBESEDB_TYPES_H_INTEGERS )
#define _LIBESEDB_TYPES_H_INTEGERS

#ifdef __cplusplus
extern "C" {
#endif

/* The Microsoft Visual Studio C++ compiler does not have <inttypes.h> or <stdint.h>
 * therefore it does not support the (u)int#_t type definitions
 * it has __int# defintions instead
 */
typedef __int8			int8_t;
typedef unsigned __int8		uint8_t;
typedef __int16			int16_t;
typedef unsigned __int16	uint16_t;
typedef __int32			int32_t;
typedef unsigned __int32	uint32_t;
typedef __int64			int64_t;
typedef unsigned __int64	uint64_t;

#if defined( _WIN64 )
typedef __int64			ssize_t;
#else
typedef __int32			ssize_t;
#endif

#ifdef __cplusplus
}
#endif

#endif /* !defined( _LIBESEDB_TYPES_H_INTEGERS ) */

/* Borland C/C++ compiler
 */
#elif defined( __BORLANDC__ )
#if __BORLANDC__ <= 0x0560

/* Define _LIBESEDB_TYPES_H_INTEGERS to avoid conflict
 */
#if !defined( _LIBESEDB_TYPES_H_INTEGERS )
#define _LIBESEDB_TYPES_H_INTEGERS

#ifdef __cplusplus
extern "C" {
#endif

/* Earlier versions of the Borland C++ Builder compiler
 * do not have <stdint.h> therefore they do not support
 * the (u)int#_t type definitions they have __int# defintions instead
 */
typedef __int8			int8_t;
typedef unsigned __int8		uint8_t;
typedef __int16			int16_t;
typedef unsigned __int16	uint16_t;
typedef __int32			int32_t;
typedef unsigned __int32	uint32_t;
typedef __int64			int64_t;
typedef unsigned __int64	uint64_t;

#if defined( _WIN64 )
typedef __int64			ssize_t;
typedef unsigned __int64	intptr_t;
#else
typedef __int32			ssize_t;
typedef unsigned __int32	intptr_t;
#endif

#ifdef __cplusplus
}
#endif

#endif /* !defined( _LIBESEDB_TYPES_H_INTEGERS ) */

#else
#include <stdint.h>
#endif /* __BORLANDC__ <= 0x0560 */

/* Other compilers
 */
#else

#if 1 || defined( HAVE_SYS_TYPES_H )
#include <sys/types.h>

#else
#error Missing system type definitions (sys/types.h)
#endif

/* Type definitions for compilers that have access to
 * <inttypes.h> or <stdint.h>
 */
#if 1 || defined( HAVE_INTTYPES_H )
#include <inttypes.h>

#elif 1 || defined( HAVE_STDINT_H )
#include <stdint.h>

#else
#error Missing integer type definitions (inttypes.h, stdint.h)
#endif

#endif /* Compiler specific integer type definitions */

#if defined( LIBESEDB_HAVE_WIDE_CHARACTER_TYPE )

#if defined( _MSC_VER ) || defined( __MINGW32_VERSION )
#include <wchar.h>

#elif defined( __BORLANDC__ )
#if __BORLANDC__ <= 0x0520
#include <string.h>

#else
#include <wchar.h>
#endif /* __BORLANDC__ <= 0x0520 */

#elif 0 || defined( HAVE_WCHAR_H )

/* __USE_UNIX98 is required to add swprintf definition
 */
#if !defined( __USE_UNIX98 )
#define __USE_UNIX98
#define LIBESEDB_DEFINITION_UNIX98
#endif

/* __USE_ISOC99 is required to add wcstoll and wcstuoll definition
 */
#if !defined( __USE_ISOC99 )
#define __USE_ISOC99
#define LIBESEDB_DEFINITION_ISOC99
#endif

#include <wchar.h>

#if defined( LIBESEDB_DEFINITION_UNIX98 )
#undef __USE_UNIX98
#undef LIBESEDB_DEFINITION_UNIX98
#endif

#if defined( LIBESEDB_DEFINITION_ISOC99 )
#undef __USE_ISOC99
#undef LIBESEDB_DEFINITION_ISOC99
#endif

#endif

#endif

#ifdef __cplusplus
extern "C" {
#endif

#if ! 0 || ! HAVE_SIZE32_T
typedef uint32_t size32_t;
#endif

#if ! 0 || ! HAVE_SSIZE32_T
typedef int32_t ssize32_t;
#endif

#if ! 0 || ! HAVE_SIZE64_T
typedef uint64_t size64_t;
#endif

#if ! 0 || ! HAVE_SSIZE64_T
typedef int64_t ssize64_t;
#endif

#if ! 0 || ! HAVE_OFF64_T
typedef int64_t off64_t;
#endif

/* The following type definitions hide internal data structures
 */
typedef intptr_t libesedb_file_t;
typedef intptr_t libesedb_table_t;
typedef intptr_t libesedb_column_t;
typedef intptr_t libesedb_index_t;
typedef intptr_t libesedb_record_t;
typedef intptr_t libesedb_long_value_t;
typedef intptr_t libesedb_multi_value_t;

#ifdef __cplusplus
}
#endif

#endif

