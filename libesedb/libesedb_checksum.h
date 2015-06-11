/*
 * Checksum functions
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

#if !defined( _LIBESEDB_CHECKSUM_H )
#define _LIBESEDB_CHECKSUM_H

#include <common.h>
#include <types.h>

#include <liberror.h>

#if defined( __cplusplus )
extern "C" {
#endif

int libesedb_checksum_calculate_little_endian_ecc32(
     uint32_t *ecc_checksum_value,
     uint32_t *xor_checksum_value,
     const uint8_t *buffer,
     size_t size,
     size_t offset,
     uint32_t initial_value,
     liberror_error_t **error );

int libesedb_checksum_calculate_little_endian_xor32(
     uint32_t *checksum_value,
     const uint8_t *buffer,
     size_t size,
     uint32_t initial_value,
     liberror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif
