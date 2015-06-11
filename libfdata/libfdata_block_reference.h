/*
 * Block data reference functions
 *
 * Copyright (c) 2010-2012, Joachim Metz <jbmetz@users.sourceforge.net>
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

#if !defined( _LIBFDATA_BLOCK_REFERENCE_H )
#define _LIBFDATA_BLOCK_REFERENCE_H

#include <common.h>
#include <types.h>

#include <liberror.h>

#include "libfdata_extern.h"
#include "libfdata_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

LIBFDATA_EXTERN \
int libfdata_block_reference_initialize(
     libfdata_reference_t **reference,
     libfdata_block_t *block,
     uint8_t flags,
     liberror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

