/*
 * CRC-32 functions
 * Based on RFC 1952
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

#if !defined( _LIBFMAPI_CRC_H )
#define _LIBFMAPI_CRC_H

#include <common.h>
#include <types.h>

#include <liberror.h>

#if defined( __cplusplus )
extern "C" {
#endif

void libfmapi_initialize_crc32_table(
      void );

int libfmapi_crc32_calculate(
     uint32_t *crc32,
     uint8_t *buffer,
     size_t size,
     uint32_t initial_value,
     liberror_error_t **error );

int libfmapi_crc32_weak_calculate(
     uint32_t *crc32,
     uint8_t *buffer,
     size_t size,
     uint32_t initial_value,
     liberror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

