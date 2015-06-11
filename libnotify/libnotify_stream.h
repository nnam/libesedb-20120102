/*
 * Notification stream functions
 *
 * Copyright (c) 2008-2012, Joachim Metz <jbmetz@users.sourceforge.net>
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

#if !defined( _LIBNOTIFY_STREAM_H )
#define _LIBNOTIFY_STREAM_H

#include <common.h>
#include <types.h>

#include <liberror.h>

#include <stdio.h>

#if defined( __cplusplus )
extern "C" {
#endif

extern FILE *libnotify_stream;

int libnotify_set_stream(
     FILE *stream,
     liberror_error_t **error );

int libnotify_stream_open(
     const char *filename,
     liberror_error_t **error );

int libnotify_stream_close(
     liberror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

