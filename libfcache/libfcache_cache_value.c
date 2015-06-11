/*
 * Cache value functions
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

#include <common.h>
#include <memory.h>
#include <types.h>

#include <liberror.h>

#include "libfcache_cache_value.h"
#include "libfcache_date_time.h"
#include "libfcache_definitions.h"

/* Creates a cache value
 * Returns 1 if successful or -1 on error
 */
int libfcache_cache_value_initialize(
     libfcache_cache_value_t **cache_value,
     liberror_error_t **error )
{
	static char *function = "libfcache_cache_value_initialize";

	if( cache_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid cache value.",
		 function );

		return( -1 );
	}
	if( *cache_value != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid cache value value already set.",
		 function );

		return( -1 );
	}
	*cache_value = memory_allocate_structure(
	                libfcache_cache_value_t );

	if( *cache_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create cache value.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *cache_value,
	     0,
	     sizeof( libfcache_cache_value_t ) ) == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear cache value.",
		 function );

		goto on_error;
	}
	( *cache_value )->offset = (off64_t) -1;

	return( 1 );

on_error:
	if( *cache_value != NULL )
	{
		memory_free(
		 *cache_value );

		*cache_value = NULL;
	}
	return( -1 );
}

/* Frees the cache value
 * Returns 1 if successful or -1 on error
 */
int libfcache_cache_value_free(
     libfcache_cache_value_t **cache_value,
     liberror_error_t **error )
{
	static char *function = "libfcache_cache_value_free";
	int result            = 1;

	if( cache_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid cache value.",
		 function );

		return( -1 );
	}
	if( *cache_value != NULL )
	{
		if( ( ( *cache_value )->flags & LIBFCACHE_CACHE_VALUE_FLAG_MANAGED ) != 0 )
		{
			if( ( *cache_value )->value != NULL )
			{
				if( ( *cache_value )->free_value == NULL )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: invalid cache value - missing free value function.",
					 function );

					result = -1;
				}
				else if( ( *cache_value )->free_value(
					  &( ( *cache_value )->value ),
					  error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
					 "%s: unable to free value.",
					 function );

					result = -1;
				}
			}
		}
		memory_free(
		 *cache_value );

		*cache_value = NULL;
	}
	return( result );
}

/* Retrieves the cache value identifier
 * Returns 1 if successful or -1 on error
 */
int libfcache_cache_value_get_identifier(
     libfcache_cache_value_t *cache_value,
     off64_t *offset,
     time_t *timestamp,
     liberror_error_t **error )
{
	static char *function = "libfcache_cache_value_get_identifier";

	if( cache_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid cache value.",
		 function );

		return( -1 );
	}
	if( offset == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid offset.",
		 function );

		return( -1 );
	}
	if( timestamp == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid timestamp.",
		 function );

		return( -1 );
	}
	*offset    = cache_value->offset;
	*timestamp = cache_value->timestamp;

	return( 1 );
}

/* Sets the cache value identifier
 * Returns 1 if successful or -1 on error
 */
int libfcache_cache_value_set_identifier(
     libfcache_cache_value_t *cache_value,
     off64_t offset,
     time_t timestamp,
     liberror_error_t **error )
{
	static char *function = "libfcache_cache_value_set_identifier";

	if( cache_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid cache value.",
		 function );

		return( -1 );
	}
	cache_value->offset    = offset;
	cache_value->timestamp = timestamp;

	return( 1 );
}

/* Retrieves the cache value
 * Returns 1 if successful or -1 on error
 */
int libfcache_cache_value_get_value(
     libfcache_cache_value_t *cache_value,
     intptr_t **value,
     liberror_error_t **error )
{
	static char *function = "libfcache_cache_value_get_value";

	if( cache_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid cache value.",
		 function );

		return( -1 );
	}
	if( value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	*value = cache_value->value;

	return( 1 );
}

/* Sets the cache value
 * Returns 1 if successful or -1 on error
 */
int libfcache_cache_value_set_value(
     libfcache_cache_value_t *cache_value,
     intptr_t *value,
     int (*free_value)(
            intptr_t **value,
            liberror_error_t **error ),
     uint8_t flags,
     liberror_error_t **error )
{
	static char *function = "libfcache_cache_value_set_value";

	if( cache_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid cache value.",
		 function );

		return( -1 );
	}
	if( free_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid free value function.",
		 function );

		return( -1 );
	}
	if( ( cache_value->flags & LIBFCACHE_CACHE_VALUE_FLAG_MANAGED ) != 0 )
	{
		if( cache_value->value != NULL )
		{
			if( cache_value->free_value == NULL )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
				 "%s: invalid cache value - missing free value function.",
				 function );

				return( -1 );
			}
			if( cache_value->free_value(
			     &( cache_value->value ),
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free value.",
				 function );

				return( -1 );
			}
		}
		cache_value->flags &= ~( LIBFCACHE_CACHE_VALUE_FLAG_MANAGED );
	}
	cache_value->value      = value;
	cache_value->free_value = free_value;
	cache_value->flags     |= flags;

	return( 1 );
}

