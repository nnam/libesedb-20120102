/* 
 * Internationalization (i18n) functions
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

#include <common.h>
#include <types.h>

#include <liberror.h>

#if defined( HAVE_LIBINTL_H )
#include <libintl.h>
#endif

#include "libesedb_i18n.h"

static int libesedb_i18n_initialized = 0;

/* Initializes library internationalization functions
 */
int libesedb_i18n_initialize(
     liberror_error_t **error )
{
	static char *function = "libesedb_i18n_initialize";

	if( libesedb_i18n_initialized == 0 )
	{
#if defined( HAVE_BINDTEXTDOMAIN )
		if( bindtextdomain(
		     "libesedb",
		     LOCALEDIR ) == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to bind text domain.",
			 function );

			return( -1 );
		}
#endif
		libesedb_i18n_initialized = 1;
	}
	return( 1 );
}

