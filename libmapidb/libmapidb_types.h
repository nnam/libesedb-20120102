/*
 * The internal type definitions
 *
 * Copyright (c) 2011-2012, Joachim Metz <jbmetz@users.sourceforge.net>
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

#if !defined( _LIBMAPIDB_INTERNAL_TYPES_H )
#define _LIBMAPIDB_INTERNAL_TYPES_H

#include <common.h>
#include <types.h>

/* Define HAVE_LOCAL_LIBMAPIDB for local use of libmapidb
 * The definitions in <libmapidb/types.h> are copied here
 * for local use of libmapidb
 */
#if defined( HAVE_LOCAL_LIBMAPIDB )

/* The following type definitions hide internal data structures
 */
typedef intptr_t libmapidb_database_t;
typedef intptr_t libmapidb_folder_t;

#endif

#endif

