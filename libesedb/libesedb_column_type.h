/*
 * Column type functions
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

#if !defined( _LIBESEDB_COLUMN_TYPE_H )
#define _LIBESEDB_COLUMN_TYPE_H

#include <common.h>
#include <types.h>

#if defined( __cplusplus )
extern "C" {
#endif

#if defined( HAVE_DEBUG_OUTPUT )

typedef struct libesedb_column_type libesedb_column_type_t;

struct libesedb_column_type
{
	/* The column_type
	 */
	uint32_t column_type;

	/* The identifier
	 */
	const char *identifier;

	/* The description
	 */
	const char *description;
};
const char *libesedb_column_type_get_identifier(
             uint32_t column_type );

const char *libesedb_column_type_get_description(
             uint32_t column_type );

#endif

#if defined( __cplusplus )
}
#endif

#endif

