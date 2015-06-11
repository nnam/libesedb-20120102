/*
 * The internal type definitions
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

#if !defined( _LIBESEDB_INTERNAL_TYPES_H )
#define _LIBESEDB_INTERNAL_TYPES_H

#include <common.h>
#include <types.h>

/* Define HAVE_LOCAL_LIBESEDB for local use of libesedb
 * The definitions in <libesedb/types.h> are copied here
 * for local use of libesedb
 */
#if defined( HAVE_LOCAL_LIBESEDB )

/* The following type definitions hide internal data structures
 */
#if defined( HAVE_DEBUG_OUTPUT )
typedef struct libesedb_file {}		libesedb_file_t;
typedef struct libesedb_table {}	libesedb_table_t;
typedef struct libesedb_column {}	libesedb_column_t;
typedef struct libesedb_index {}	libesedb_index_t;
typedef struct libesedb_record {}	libesedb_record_t;
typedef struct libesedb_long_value {}	libesedb_long_value_t;
typedef struct libesedb_multi_value {}	libesedb_multi_value_t;

#else
typedef intptr_t libesedb_file_t;
typedef intptr_t libesedb_table_t;
typedef intptr_t libesedb_column_t;
typedef intptr_t libesedb_index_t;
typedef intptr_t libesedb_record_t;
typedef intptr_t libesedb_long_value_t;
typedef intptr_t libesedb_multi_value_t;

#endif

#endif

#endif

