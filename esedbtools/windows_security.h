/* 
 * Windows Security database export functions
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

#if !defined( _WINDOWS_SECURITY_H )
#define _WINDOWS_SECURITY_H

#include <common.h>
#include <file_stream.h>
#include <types.h>

#include <liberror.h>

#include "esedbtools_libesedb.h"
#include "log_handle.h"

#if defined( __cplusplus )
extern "C" {
#endif

int windows_security_export_record_value_filetime(
     libesedb_record_t *record,
     int record_value_entry,
     uint8_t byte_order,
     FILE *record_file_stream,
     liberror_error_t **error );

int windows_security_export_record_value_utf16_string(
     libesedb_record_t *record,
     int record_value_entry,
     uint8_t byte_order,
     FILE *record_file_stream,
     liberror_error_t **error );

int windows_security_export_record_smtblversion(
     libesedb_record_t *record,
     FILE *record_file_stream,
     log_handle_t *log_handle,
     liberror_error_t **error );

int windows_security_export_record_smtblsection(
     libesedb_record_t *record,
     FILE *record_file_stream,
     log_handle_t *log_handle,
     liberror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

