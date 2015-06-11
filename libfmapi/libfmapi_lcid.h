/*
 * LCID functions
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

#if !defined( _LIBFMAPI_LCID_H )
#define _LIBFMAPI_LCID_H

#include <common.h>
#include <types.h>

#if defined( __cplusplus )
extern "C" {
#endif

#if defined( HAVE_DEBUG_OUTPUT )

typedef struct libfmapi_lcid_language_tag libfmapi_lcid_language_tag_t;

struct libfmapi_lcid_language_tag
{
	/* The LCID language tag value
	 */
	uint16_t lcid_language_tag;

	/* The identifier
	 */
	const char *identifier;

	/* The description
	 */
	const char *description;
};
const char *libfmapi_lcid_language_tag_get_identifier(
             uint16_t lcid_language_tag );

const char *libfmapi_lcid_language_tag_get_description(
             uint16_t lcid_language_tag );

#endif

#if defined( __cplusplus )
}
#endif

#endif

