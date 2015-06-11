/*
 * Error functions
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

#if !defined( _LIBESEDB_INTERNAL_ERROR_H )
#define _LIBESEDB_INTERNAL_ERROR_H

#include <common.h>
#include <types.h>

#include <stdio.h>

#if !defined( HAVE_LOCAL_LIBESEDB )
#include <libesedb/error.h>
#endif

#include "libesedb_extern.h"

#if defined( __cplusplus )
extern "C" {
#endif

#if !defined( HAVE_LOCAL_LIBESEDB )

LIBESEDB_EXTERN \
void libesedb_error_free(
      libesedb_error_t **error );

LIBESEDB_EXTERN \
int libesedb_error_fprint(
     libesedb_error_t *error,
     FILE *stream );

LIBESEDB_EXTERN \
int libesedb_error_sprint(
     libesedb_error_t *error,
     char *string,
     size_t size );

LIBESEDB_EXTERN \
int libesedb_error_backtrace_fprint(
     libesedb_error_t *error,
     FILE *stream );

LIBESEDB_EXTERN \
int libesedb_error_backtrace_sprint(
     libesedb_error_t *error,
     char *string,
     size_t size );

#endif

#if defined( __cplusplus )
}
#endif

#endif

