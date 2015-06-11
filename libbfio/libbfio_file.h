/*
 * File functions
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

#if !defined( _LIBBFIO_FILE_H )
#define _LIBBFIO_FILE_H

#include <common.h>
#include <types.h>

#include <libcstring.h>
#include <liberror.h>

#include "libbfio_extern.h"
#include "libbfio_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libbfio_file_io_handle libbfio_file_io_handle_t;

struct libbfio_file_io_handle
{
	/* The name
	 */
	libcstring_system_character_t *name;

	/* The size of the name
	 */
	size_t name_size;

#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	/* The file handle
	 */
	HANDLE file_handle;
#else
	/* The file descriptor
	 */
	int file_descriptor;
#endif

	/* The access flags
	 */
	int access_flags;
};

int libbfio_file_io_handle_initialize(
     libbfio_file_io_handle_t **file_io_handle,
     liberror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_file_initialize(
     libbfio_handle_t **handle,
     liberror_error_t **error );

int libbfio_file_io_handle_free(
     libbfio_file_io_handle_t **file_io_handle,
     liberror_error_t **error );

int libbfio_file_io_handle_clone(
     libbfio_file_io_handle_t **destination_file_io_handle,
     libbfio_file_io_handle_t *source_file_io_handle,
     liberror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_file_get_name_size(
     libbfio_handle_t *handle,
     size_t *name_size,
     liberror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_file_get_name(
     libbfio_handle_t *handle,
     char *name,
     size_t name_size,
     liberror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_file_set_name(
     libbfio_handle_t *handle,
     const char *name,
     size_t name_length,
     liberror_error_t **error );

int libbfio_file_io_handle_get_name_size(
     libbfio_file_io_handle_t *file_io_handle,
     size_t *name_size,
     liberror_error_t **error );

int libbfio_file_io_handle_get_name(
     libbfio_file_io_handle_t *file_io_handle,
     char *name,
     size_t name_size,
     liberror_error_t **error );

int libbfio_file_io_handle_set_name(
     libbfio_file_io_handle_t *file_io_handle,
     const char *name,
     size_t name_length,
     liberror_error_t **error );

#if defined( HAVE_WIDE_CHARACTER_TYPE )
LIBBFIO_EXTERN \
int libbfio_file_get_name_size_wide(
     libbfio_handle_t *handle,
     size_t *name_size,
     liberror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_file_get_name_wide(
     libbfio_handle_t *handle,
     wchar_t *name,
     size_t name_size,
     liberror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_file_set_name_wide(
     libbfio_handle_t *handle,
     const wchar_t *name,
     size_t name_length,
     liberror_error_t **error );

int libbfio_file_io_handle_get_name_size_wide(
     libbfio_file_io_handle_t *file_io_handle,
     size_t *name_size,
     liberror_error_t **error );

int libbfio_file_io_handle_get_name_wide(
     libbfio_file_io_handle_t *file_io_handle,
     wchar_t *name,
     size_t name_size,
     liberror_error_t **error );

int libbfio_file_io_handle_set_name_wide(
     libbfio_file_io_handle_t *file_io_handle,
     const wchar_t *name,
     size_t name_length,
     liberror_error_t **error );

#endif /* defined( HAVE_WIDE_CHARACTER_TYPE ) */

int libbfio_file_open(
     libbfio_file_io_handle_t *file_io_handle,
     int access_flags,
     liberror_error_t **error );

int libbfio_file_close(
     libbfio_file_io_handle_t *file_io_handle,
     liberror_error_t **error );

ssize_t libbfio_file_read(
         libbfio_file_io_handle_t *file_io_handle,
         uint8_t *buffer,
         size_t size,
         liberror_error_t **error );

ssize_t libbfio_file_write(
         libbfio_file_io_handle_t *file_io_handle,
         const uint8_t *buffer,
         size_t size,
         liberror_error_t **error );

#if defined( WINAPI ) && ( WINVER < 0x0500 )
BOOL libbfio_SetFilePointerEx(
      HANDLE file_handle,
      LARGE_INTEGER distance_to_move_large_integer,
      LARGE_INTEGER *new_file_pointer_large_integer,
      DWORD move_method );
#endif

off64_t libbfio_file_seek_offset(
         libbfio_file_io_handle_t *file_io_handle,
         off64_t offset,
         int whence,
         liberror_error_t **error );

int libbfio_file_exists(
     libbfio_file_io_handle_t *file_io_handle,
     liberror_error_t **error );

int libbfio_file_is_open(
     libbfio_file_io_handle_t *file_io_handle,
     liberror_error_t **error );

#if defined( WINAPI ) && ( WINVER < 0x0500 )
BOOL libbfio_GetFileSizeEx(
      HANDLE file_handle,
      LARGE_INTEGER *file_size_large_integer );
#endif

int libbfio_file_get_size(
     libbfio_file_io_handle_t *file_io_handle,
     size64_t *size,
     liberror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

