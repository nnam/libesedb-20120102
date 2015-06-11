/*
 * Support functions
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

#if !defined( _LIBESEDB_SUPPORT_H )
#define _LIBESEDB_SUPPORT_H

#include <common.h>
#include <types.h>

#include <liberror.h>

#include "libesedb_extern.h"
#include "libesedb_libbfio.h"

#if defined( __cplusplus )
extern "C" {
#endif

#if !defined( HAVE_LOCAL_LIBESEDB )

LIBESEDB_EXTERN \
const char *libesedb_get_version(
             void );

LIBESEDB_EXTERN \
int libesedb_get_access_flags_read(
     void );

LIBESEDB_EXTERN \
int libesedb_get_codepage(
     int *codepage,
     liberror_error_t **error );

LIBESEDB_EXTERN \
int libesedb_set_codepage(
     int codepage,
     liberror_error_t **error );

#endif /* !defined( HAVE_LOCAL_LIBESEDB ) */

LIBESEDB_EXTERN \
int libesedb_check_file_signature(
     const char *filename,
     liberror_error_t **error );

#if defined( HAVE_WIDE_CHARACTER_TYPE )
LIBESEDB_EXTERN \
int libesedb_check_file_signature_wide(
     const wchar_t *filename,
     liberror_error_t **error );
#endif

LIBESEDB_EXTERN \
int libesedb_check_file_signature_file_io_handle(
     libbfio_handle_t *bfio_handle,
     liberror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

