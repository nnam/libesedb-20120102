/*
 * The internal definitions
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

#if !defined( LIBERROR_INTERNAL_DEFINITIONS_H )
#define LIBERROR_INTERNAL_DEFINITIONS_H

#include <common.h>
#include <types.h>

/* The error domains
 */
enum LIBERROR_ERROR_DOMAINS
{
	LIBERROR_ERROR_DOMAIN_ARGUMENTS			= (int) 'a',
	LIBERROR_ERROR_DOMAIN_CONVERSION		= (int) 'c',
	LIBERROR_ERROR_DOMAIN_COMPRESSION		= (int) 'C',
	LIBERROR_ERROR_DOMAIN_ENCRYPTION		= (int) 'E',
	LIBERROR_ERROR_DOMAIN_IO			= (int) 'I',
	LIBERROR_ERROR_DOMAIN_INPUT			= (int) 'i',
	LIBERROR_ERROR_DOMAIN_MEMORY			= (int) 'm',
	LIBERROR_ERROR_DOMAIN_OUTPUT			= (int) 'o',
	LIBERROR_ERROR_DOMAIN_RUNTIME			= (int) 'r',
};

/* The argument error codes
 * to signify errors regarding arguments passed to a function
 */
enum LIBERROR_ARGUMENT_ERROR
{
	LIBERROR_ARGUMENT_ERROR_GENERIC			= 0,

	/* The argument contains an invalid value
	 */
	LIBERROR_ARGUMENT_ERROR_INVALID_VALUE		= 1,

	/* The argument contains a value less than zero
	 */
	LIBERROR_ARGUMENT_ERROR_VALUE_LESS_THAN_ZERO	= 2,

	/* The argument contains a value zero or less
	 */
	LIBERROR_ARGUMENT_ERROR_VALUE_ZERO_OR_LESS	= 3,

	/* The argument contains a value that exceeds the maximum
	 * for the specific type
	 */
	LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM	= 4,

	/* The argument contains a value that is too small
	 */
	LIBERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL		= 5,

	/* The argument contains a value that is too large
	 */
	LIBERROR_ARGUMENT_ERROR_VALUE_TOO_LARGE		= 6,

	/* The argument contains a value that is out of bound
	 */
	LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS	= 7,

	/* The argument contains a value that is not supported
	 */
	LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE	= 8,

	/* The argument contains a value that conficts with another argument
	 */
	LIBERROR_ARGUMENT_ERROR_CONFLICTING_VALUE	= 9
};

/* TODO deprecated remove after a while */
#define LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_RANGE	LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS

/* The conversion error codes
 * to signify errors regarding conversions
 */
enum LIBERROR_CONVERSION_ERROR
{
	LIBERROR_CONVERSION_ERROR_GENERIC		= 0,

	/* The conversion failed on the input
	 */
	LIBERROR_CONVERSION_ERROR_INPUT_FAILED		= 1,

	/* The conversion failed on the output
	 */
	LIBERROR_CONVERSION_ERROR_OUTPUT_FAILED		= 2
};

/* The compression error codes
 * to signify errors regarding compression
 */
enum LIBERROR_COMPRESSION_ERROR
{
	LIBERROR_COMPRESSION_ERROR_GENERIC		= 0,

	/* The compression failed
	 */
	LIBERROR_COMPRESSION_ERROR_COMPRESS_FAILED	= 1,

	/* The decompression failed
	 */
	LIBERROR_COMPRESSION_ERROR_DECOMPRESS_FAILED	= 2
};

/* TODO deprecated remove after a while */
#define LIBERROR_COMPRESSION_ERROR_UNCOMPRESS_FAILED	LIBERROR_COMPRESSION_ERROR_DECOMPRESS_FAILED

/* The encryption error codes
 * to signify errors regarding encryption
 */
enum LIBERROR_ENCRYPTION_ERROR
{
	LIBERROR_ENCRYPTION_ERROR_GENERIC		= 0,

	/* The encryption failed
	 */
	LIBERROR_ENCRYPTION_ERROR_ENCRYPT_FAILED	= 1,

	/* The decryption failed
	 */
	LIBERROR_ENCRYPTION_ERROR_DECRYPT_FAILED	= 2
};

/* The input/output error codes
 * to signify errors regarding input/output
 */
enum LIBERROR_IO_ERROR
{
	LIBERROR_IO_ERROR_GENERIC			= 0,

	/* The open failed
	 */
	LIBERROR_IO_ERROR_OPEN_FAILED			= 1,

	/* The close failed
	 */
	LIBERROR_IO_ERROR_CLOSE_FAILED			= 2,

	/* The seek failed
	 */
	LIBERROR_IO_ERROR_SEEK_FAILED			= 3,

	/* The read failed
	 */
	LIBERROR_IO_ERROR_READ_FAILED			= 4,

	/* The write failed
	 */
	LIBERROR_IO_ERROR_WRITE_FAILED			= 5,

	/* Access denied
	 */
	LIBERROR_IO_ERROR_ACCESS_DENIED			= 6,

	/* The resource is invalid i.e. a missing file
	 */
	LIBERROR_IO_ERROR_INVALID_RESOURCE		= 7,

	/* The ioctl failed
	 */
	LIBERROR_IO_ERROR_IOCTL_FAILED			= 8,

	/* The unlink failed
	 */
	LIBERROR_IO_ERROR_UNLINK_FAILED			= 9
};

/* The input error codes
 * to signify errors regarding handing input data
 */
enum LIBERROR_INPUT_ERROR
{
	LIBERROR_INPUT_ERROR_GENERIC			= 0,

	/* The input contains invalid data
	 */
	LIBERROR_INPUT_ERROR_INVALID_DATA		= 1,

	/* The input contains an unsupported signature
	 */
	LIBERROR_INPUT_ERROR_SIGNATURE_MISMATCH		= 2,

	/* A checksum in the input did not match
	 */
	LIBERROR_INPUT_ERROR_CHECKSUM_MISMATCH		= 3,

	/* A value in the input did not match a previously
	 * read value or calculated value
	 */
	LIBERROR_INPUT_ERROR_VALUE_MISMATCH		= 4
};

/* TODO deprecated remove after a while */
#define LIBERROR_INPUT_ERROR_CRC_MISMATCH		LIBERROR_INPUT_ERROR_CHECKSUM_MISMATCH

/* The memory error codes
 * to signify errors regarding memory
 */
enum LIBERROR_MEMORY_ERROR
{
	LIBERROR_MEMORY_ERROR_GENERIC			= 0,

	/* There is insufficient memory available
	 */
	LIBERROR_MEMORY_ERROR_INSUFFICIENT		= 1,

	/* The memory failed to be copied
	 */
	LIBERROR_MEMORY_ERROR_COPY_FAILED		= 2,

	/* The memory failed to be set
	 */
	LIBERROR_MEMORY_ERROR_SET_FAILED		= 3
};

/* The runtime error codes
 * to signify errors regarding runtime processing
 */
enum LIBERROR_RUNTIME_ERROR
{
	LIBERROR_RUNTIME_ERROR_GENERIC			= 0,

	/* The value is missing
	 */
	LIBERROR_RUNTIME_ERROR_VALUE_MISSING		= 1,

	/* The value was already set
	 */
	LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET	= 2,

	/* The creation and/or initialization of an internal structure failed
	 */
	LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED	= 3,

	/* The resize of an internal structure failed
	 */
	LIBERROR_RUNTIME_ERROR_RESIZE_FAILED		= 4,

	/* The free and/or finalization of an internal structure failed
	 */
	LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED		= 5,

	/* The value could not be determined
	 */
	LIBERROR_RUNTIME_ERROR_GET_FAILED		= 6,

	/* The value could not be set
	 */
	LIBERROR_RUNTIME_ERROR_SET_FAILED		= 7,

	/* The value could not be appended/prepended
	 */
	LIBERROR_RUNTIME_ERROR_APPEND_FAILED		= 8,

	/* The value could not be copied
	 */
	LIBERROR_RUNTIME_ERROR_COPY_FAILED		= 9,

	/* The value could not be removed
	 */
	LIBERROR_RUNTIME_ERROR_REMOVE_FAILED		= 10,

	/* The value could not be printed
	 */
	LIBERROR_RUNTIME_ERROR_PRINT_FAILED		= 11,

	/* The value was out of bounds
	 */
	LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS	= 12,

	/* The value exceeds the maximum for its specific type
	 */
	LIBERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM	= 13,

	/* The value is unsupported
	 */
	LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE	= 14,

	/* An aborted was requested
	 */
	LIBERROR_RUNTIME_ERROR_ABORT_REQUESTED		= 15
};

/* TODO deprecated remove after a while */
#define LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_RANGE	LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS

/* The output error codes
 */
enum LIBERROR_OUTPUT_ERROR
{
	LIBERROR_OUTPUT_ERROR_GENERIC			= 0,

	/* There is insuficient space to write the output
	 */
	LIBERROR_OUTPUT_ERROR_INSUFFICIENT_SPACE	= 1
};

#endif

