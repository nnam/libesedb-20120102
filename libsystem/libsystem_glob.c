/*
 * Globbing functions
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

#include <common.h>
#include <memory.h>
#include <types.h>

#include <libcstring.h>
#include <liberror.h>

#if defined( HAVE_ERRNO_H ) || defined( WINAPI )
#include <errno.h>
#endif

#if defined( HAVE_STDLIB_H ) || defined( WINAPI )
#include <stdlib.h>
#endif

#if defined( HAVE_IO_H ) || defined( WINAPI )
#include <io.h>
#endif

#include "libsystem_find.h"
#include "libsystem_glob.h"
#include "libsystem_path.h"

#if !defined( LIBSYSTEM_HAVE_GLOB )

/* Initializes a new glob
 * Returns 1 if successful or -1 on error
 */
int libsystem_glob_initialize(
     libsystem_glob_t **glob,
     liberror_error_t **error )
{
	static char *function = "libsystem_glob_initialize";

	if( glob == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid glob.",
		 function );

		return( -1 );
	}
	if( *glob == NULL )
	{
		*glob = memory_allocate_structure(
		         libsystem_glob_t );

		if( *glob == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create glob.",
			 function );

			goto on_error;
		}
		if( memory_set(
		     *glob,
		     0,
		     sizeof( libsystem_glob_t ) ) == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_SET_FAILED,
			 "%s: unable to clear glob.",
			 function );

			goto on_error;
		}
	}
	return( 1 );

on_error:
	if( *glob != NULL )
	{
		memory_free(
		 *glob );

		*glob = NULL;
	}
	return( -1 );
}

/* Frees memory of a glob
 * Returns 1 if successful or -1 on error
 */
int libsystem_glob_free(
     libsystem_glob_t **glob,
     liberror_error_t **error )
{
	static char *function = "libsystem_glob_free";
	int result_iterator   = 0;

	if( glob == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid glob.",
		 function );

		return( -1 );
	}
	if( *glob != NULL )
	{
		if( ( *glob )->result != NULL )
		{
			for( result_iterator = 0;
			     result_iterator < ( *glob )->number_of_results;
			     result_iterator++ )
			{
				if( ( *glob )->result[ result_iterator ] != NULL )
				{
					memory_free(
					 ( *glob )->result[ result_iterator ] );
				}
			}
			memory_free(
			 ( *glob )->result );
		}
		memory_free(
		 *glob );

		*glob = NULL;
	}
	return( 1 );
}

/* Resizes the glob
 * Returns 1 if successful or -1 on error
 */
int libsystem_glob_resize(
     libsystem_glob_t *glob,
     int new_number_of_results,
     liberror_error_t **error )
{
	void *reallocation    = NULL;
	static char *function = "libsystem_glob_resize";
	size_t previous_size  = 0;
	size_t new_size       = 0;

	if( glob == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid glob.",
		 function );

		return( -1 );
	}
	if( glob->number_of_results >= new_number_of_results )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: new number of results less equal than current.",
		 function );

		return( -1 );
	}
	previous_size = sizeof( libcstring_system_character_t * ) * glob->number_of_results;
	new_size      = sizeof( libcstring_system_character_t * ) * new_number_of_results;

	if( ( previous_size > (size_t) SSIZE_MAX )
	 || ( new_size > (size_t) SSIZE_MAX ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid size value exceeds maximum.",
		 function );

		return( -1 );
	}
	reallocation = memory_reallocate(
	                glob->result,
	                new_size );

	if( reallocation == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to reallocate glob results.",
		 function );

		return( -1 );
	}
	glob->result = (libcstring_system_character_t **) reallocation;

	if( memory_set(
	     &( glob->result[ glob->number_of_results ] ),
	     0,
	     new_size - previous_size ) == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear glob.",
		 function );

		return( -1 );
	}
	glob->number_of_results = new_number_of_results;

	return( 1 );
}

/* Resolves filenames with wildcards (globs)
 * Returns the number of results if successful or -1 on error
 */
int libsystem_glob_resolve(
     libsystem_glob_t *glob,
     libcstring_system_character_t * const patterns[],
     int number_of_patterns,
     liberror_error_t **error )
{
#if defined( HAVE_IO_H ) || defined( WINAPI )
	libsystem_find_data_t find_data;

	libcstring_system_character_t find_path[ _MAX_PATH ];
	libcstring_system_character_t find_drive[ _MAX_DRIVE ];
	libcstring_system_character_t find_directory[ _MAX_DIR ];
	libcstring_system_character_t find_name[ _MAX_FNAME ];
	libcstring_system_character_t find_extension[ _MAX_EXT ];

	intptr_t find_handle    = 0;
#endif
	static char *function   = "libsystem_glob_resolve";
	size_t find_path_length = 0;
	int globs_found         = 0;
	int iterator            = 0;

	if( glob == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid glob.",
		 function );

		return( -1 );
	}
	for( iterator = 0;
	     iterator < number_of_patterns;
	     iterator++ )
	{
		if( patterns[ iterator ] == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing pattern value.",
			 function );

			return( -1 );
		}
#if defined( _MSC_VER )
		if( libsystem_path_split(
		     patterns[ iterator ],
		     find_drive,
		     _MAX_DRIVE,
		     find_directory,
		     _MAX_DIR,
		     find_name,
		     _MAX_FNAME,
		     find_extension,
		     _MAX_EXT ) != 0 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to split path.",
			 function );

			return( -1 );
		}
#else
		libsystem_path_split(
		 patterns[ iterator ],
		 find_drive,
		 _MAX_DRIVE,
		 find_directory,
		 _MAX_DIR,
		 find_name,
		 _MAX_FNAME,
		 find_extension,
		 _MAX_EXT );
#endif
		find_handle = libsystem_find_first(
					   patterns[ iterator ],
					   &find_data );

		if( find_handle != -1 )
		{
			do
			{
				if( libsystem_glob_resize(
				     glob,
				     glob->number_of_results + 1,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_RESIZE_FAILED,
					 "%s: unable to resize glob.",
					 function );

					return( -1 );
				}
#if defined( _MSC_VER )
				if( libsystem_path_make(
				     find_path,
				     _MAX_PATH,
				     find_drive,
				     find_directory,
				     find_data.name,
				     _LIBCSTRING_SYSTEM_STRING( "" )  ) != 0 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to make path.",
					 function );

					return( -1 );
				}
#elif defined( __BORLANDC__ ) && __BORLANDC__ <= 0x0520
				libsystem_path_make(
				 find_path,
				 _MAX_PATH,
				 find_drive,
				 find_directory,
				 find_data.ff_name,
				 _LIBCSTRING_SYSTEM_STRING( "" ) );

#else
				libsystem_path_make(
				 find_path,
				 _MAX_PATH,
				 find_drive,
				 find_directory,
				 find_data.name,
				 _LIBCSTRING_SYSTEM_STRING( "" ) );
#endif

				find_path_length = libcstring_system_string_length(
				                    find_path );

				glob->result[ glob->number_of_results - 1 ] = (libcstring_system_character_t *) memory_allocate(
				                                                                                 sizeof( libcstring_system_character_t ) * ( find_path_length + 1 ) );

				if( glob->result[ glob->number_of_results - 1 ] == NULL )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_MEMORY,
					 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
					 "%s: unable to create glob result.",
					 function );

					return( -1 );
				}
				if( libcstring_system_string_copy(
				     glob->result[ glob->number_of_results - 1 ],
				     find_path,
				     find_path_length ) == NULL )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set glob result.",
					 function );

					memory_free(
					 glob->result[ glob->number_of_results - 1 ] );

					glob->result[ glob->number_of_results - 1 ] = NULL;

					return( -1 );
				}
				( glob->result[ glob->number_of_results - 1 ] )[ find_path_length ] = 0;

				globs_found++;

				if( globs_found > (int32_t) UINT16_MAX )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
					 "%s: too many globs found.",
					 function );

					return( -1 );
				}
			}
			while( libsystem_find_next(
			        find_handle,
			        &find_data ) == 0 );

			if( errno != ENOENT )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: error finding next file entry.",
				 function );

				return( -1 );
			}
			if( libsystem_find_close(
			     find_handle ) != 0 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: error closing find handle.",
				 function );

				return( -1 );
			}
		}
		else if( errno != ENOENT )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: error finding file entry.",
			 function );

			return( -1 );
		}
	}
	return( 1 );
}

#endif

