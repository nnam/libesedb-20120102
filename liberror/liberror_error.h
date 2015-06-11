/*
 * Error functions
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

#if !defined( _LIBERROR_INTERNAL_ERROR_H )
#define _LIBERROR_INTERNAL_ERROR_H

#include <common.h>
#include <types.h>

#include <libcstring.h>

#include <stdio.h>

#include "liberror_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

#define LIBERROR_MESSAGE_INCREMENT_SIZE		64
#define LIBERROR_MESSAGE_MAXIMUM_SIZE		4096

typedef struct liberror_internal_error liberror_internal_error_t;

struct liberror_internal_error
{
	/* The error domain
	 */
	int domain;

	/* The error code
	 */
	int code;

	/* The number of messages
	 */
	int number_of_messages;

	/* A dynamic array containing the message strings
	 */
	libcstring_system_character_t **messages;

	/* A dynamic array containing the message string lengths
	 * without the end-of-string character
	 */
	size_t *lengths;
};

void liberror_error_free(
      liberror_error_t **error );

void liberror_error_set(
      liberror_error_t **error,
      int error_domain,
      int error_code,
      const char *format,
      ... );

int liberror_error_matches(
     liberror_error_t *error,
     int error_domain,
     int error_code );

int liberror_error_fprint(
     liberror_error_t *error,
     FILE *stream );

int liberror_error_sprint(
     liberror_error_t *error,
     char *string,
     size_t size );

int liberror_error_backtrace_fprint(
     liberror_error_t *error,
     FILE *stream );

int liberror_error_backtrace_sprint(
     liberror_error_t *error,
     char *string,
     size_t size );

#if defined( __cplusplus )
}
#endif

#endif

