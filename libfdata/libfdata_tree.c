/*
 * The tree functions
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
#include <libnotify.h>

#include "libfdata_date_time.h"
#include "libfdata_definitions.h"
#include "libfdata_libbfio.h"
#include "libfdata_libfcache.h"
#include "libfdata_tree.h"
#include "libfdata_tree_node.h"
#include "libfdata_types.h"

/* Initializes the tree
 *
 * If the flag LIBFDATA_FLAG_IO_HANDLE_MANAGED is set the tree
 * takes over management of the IO handle and the IO handle is freed when
 * no longer needed
 *
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_initialize(
     libfdata_tree_t **tree,
     intptr_t *io_handle,
     int (*free_io_handle)(
            intptr_t **io_handle,
            liberror_error_t **error ),
     int (*clone_io_handle)(
            intptr_t **destination_io_handle,
            intptr_t *source_io_handle,
            liberror_error_t **error ),
     int (*read_node_data)(
            intptr_t *io_handle,
            libbfio_handle_t *file_io_handle,
            libfdata_tree_node_t *node,
            libfcache_cache_t *cache,
            off64_t node_data_offset,
            size64_t node_data_size,
            uint8_t read_flags,
            liberror_error_t **error ),
     int (*read_sub_nodes)(
            intptr_t *io_handle,
            libbfio_handle_t *file_io_handle,
            libfdata_tree_node_t *node,
            libfcache_cache_t *cache,
            off64_t sub_nodes_offset,
            size64_t sub_nodes_size,
            uint8_t read_flags,
            liberror_error_t **error ),
     uint8_t flags,
     liberror_error_t **error )
{
	libfdata_internal_tree_t *internal_tree = NULL;
	static char *function                   = "libfdata_tree_initialize";

	if( tree == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree.",
		 function );

		return( -1 );
	}
	if( *tree != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid tree value already set.",
		 function );

		return( -1 );
	}
	if( read_node_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid read node data function.",
		 function );

		return( -1 );
	}
	if( read_sub_nodes == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid read sub nodes function.",
		 function );

		return( -1 );
	}
	internal_tree = memory_allocate_structure(
	                 libfdata_internal_tree_t );

	if( internal_tree == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create tree.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_tree,
	     0,
	     sizeof( libfdata_internal_tree_t ) ) == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear tree.",
		 function );

		goto on_error;
	}
	internal_tree->flags          |= flags;
	internal_tree->io_handle       = io_handle;
	internal_tree->free_io_handle  = free_io_handle;
	internal_tree->clone_io_handle = clone_io_handle;
	internal_tree->read_node_data  = read_node_data;
	internal_tree->read_sub_nodes  = read_sub_nodes;

	*tree = (libfdata_tree_t *) internal_tree;

	return( 1 );

on_error:
	if( internal_tree != NULL )
	{
		memory_free(
		 internal_tree );
	}
	return( -1 );
}

/* Frees the tree
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_free(
     libfdata_tree_t **tree,
     liberror_error_t **error )
{
	libfdata_internal_tree_t *internal_tree = NULL;
	static char *function                   = "libfdata_tree_free";
	int result                              = 1;

	if( tree == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree.",
		 function );

		return( -1 );
	}
	if( *tree != NULL )
	{
		internal_tree = (libfdata_internal_tree_t *) *tree;
		*tree         = NULL;

		if( internal_tree->root_node != NULL )
		{
			if( libfdata_tree_node_free(
			     &( internal_tree->root_node ),
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free root node.",
				 function );

				result = -1;
			}
		}
		if( ( internal_tree->flags & LIBFDATA_FLAG_IO_HANDLE_MANAGED ) != 0 )
		{
			if( internal_tree->io_handle != NULL )
			{
				if( internal_tree->free_io_handle != NULL )
				{
					if( internal_tree->free_io_handle(
					     &( internal_tree->io_handle ),
					     error ) != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
						 "%s: unable to free IO handle.",
						 function );

						result = -1;
					}
				}
			}
		}
		memory_free(
		 internal_tree );
	}
	return( result );
}

/* Clones (duplicates) the tree
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_clone(
     libfdata_tree_t **destination_tree,
     libfdata_tree_t *source_tree,
     liberror_error_t **error )
{
	libfdata_internal_tree_t *internal_source_tree = NULL;
	libfdata_tree_node_t *source_tree_root_node    = NULL;
	intptr_t *destination_io_handle                = NULL;
	static char *function                          = "libfdata_tree_clone";
	off64_t node_data_offset                       = 0;
	size64_t node_data_size                        = 0;
	uint32_t node_data_flags                       = 0;

	if( destination_tree == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid destination tree.",
		 function );

		return( -1 );
	}
	if( *destination_tree != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: destination tree already set.",
		 function );

		return( -1 );
	}
	if( source_tree == NULL )
	{
		*destination_tree = NULL;

		return( 1 );
	}
	internal_source_tree = (libfdata_internal_tree_t *) source_tree;

	if( internal_source_tree->io_handle != NULL )
	{
		if( internal_source_tree->free_io_handle == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid source tree - missing free IO handle function.",
			 function );

			goto on_error;
		}
		if( internal_source_tree->clone_io_handle == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid source tree - missing clone IO handle function.",
			 function );

			goto on_error;
		}
		if( internal_source_tree->clone_io_handle(
		     &destination_io_handle,
		     internal_source_tree->io_handle,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to clone IO handle.",
			 function );

			goto on_error;
		}
	}
	if( libfdata_tree_initialize(
	     destination_tree,
	     destination_io_handle,
	     internal_source_tree->free_io_handle,
	     internal_source_tree->clone_io_handle,
	     internal_source_tree->read_node_data,
	     internal_source_tree->read_sub_nodes,
	     LIBFDATA_FLAG_IO_HANDLE_MANAGED,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create destination tree.",
		 function );

		goto on_error;
	}
	destination_io_handle = NULL;

	if( libfdata_tree_get_root_node(
	     source_tree,
	     &source_tree_root_node,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve source tree root node.",
		 function );

		goto on_error;
	}
	if( libfdata_tree_node_get_data_range(
	     source_tree_root_node,
	     &node_data_offset,
	     &node_data_size,
	     &node_data_flags,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve source tree root node data range.",
		 function );

		goto on_error;
	}
	if( libfdata_tree_set_root_node(
	     *destination_tree,
	     node_data_offset,
	     node_data_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set destination tree root node.",
		 function );

		goto on_error;
	}
	/* TODO or clone all the tree nodes ? */

	return( 1 );

on_error:
	if( *destination_tree != NULL )
	{
		libfdata_tree_free(
		 destination_tree,
		 NULL );
	}
	if( destination_io_handle != NULL )
	{
		internal_source_tree->free_io_handle(
		 &destination_io_handle,
		 NULL );
	}
	return( -1 );
}

/* Retrieves the node value of the node
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_get_node_value(
     libfdata_tree_t *tree,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     libfdata_tree_node_t *node,
     intptr_t **node_value,
     uint8_t read_flags,
     liberror_error_t **error )
{
	libfcache_cache_value_t *cache_value     = NULL;
	libfdata_internal_tree_t *internal_tree = NULL;
	static char *function                   = "libfdata_tree_get_node_value";
	off64_t cache_value_offset              = (off64_t) -1;
	off64_t node_data_offset                = 0;
	size64_t node_data_size                 = 0;
	time_t cache_value_timestamp            = 0;
	time_t node_timestamp                   = 0;
	uint32_t node_data_flags                = 0;
	int cache_entry_index                   = -1;
	int number_of_cache_entries             = 0;
	int result                              = 0;

	if( tree == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree.",
		 function );

		return( -1 );
	}
	internal_tree = (libfdata_internal_tree_t *) tree;

	if( internal_tree->read_node_data == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid tree - missing read node data function.",
		 function );

		return( -1 );
	}
	if( libfdata_tree_node_get_data_range(
	     node,
	     &node_data_offset,
	     &node_data_size,
	     &node_data_flags,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve data range from tree node.",
		 function );

		return( -1 );
	}
	if( libfcache_cache_get_number_of_entries(
	     cache,
	     &number_of_cache_entries,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of cache entries.",
		 function );

		return( -1 );
	}
	if( number_of_cache_entries <= 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid number of cache entries value out of bounds.",
		 function );

		return( -1 );
	}
	if( ( read_flags & LIBFDATA_READ_FLAG_IGNORE_CACHE ) == 0 )
	{
		if( ( ( ( (libfdata_internal_tree_node_t *) node )->flags & LIBFDATA_TREE_NODE_FLAG_IS_VIRTUAL ) == 0 )
		 && ( ( ( (libfdata_internal_tree_node_t *) node )->flags & LIBFDATA_TREE_NODE_FLAG_IS_LEAF ) != 0 ) )
		{
			cache_entry_index = libfdata_tree_node_calculate_leaf_node_cache_entry_index(
			                     node_data_offset,
			                     number_of_cache_entries );
		}
		else
		{
			cache_entry_index = libfdata_tree_node_calculate_branch_node_cache_entry_index(
			                     node_data_offset,
			                     number_of_cache_entries );
		}
		if( libfcache_cache_get_value_by_index(
		     cache,
		     cache_entry_index,
		     &cache_value,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve cache entry: %d from cache.",
			 function,
			 cache_entry_index );

			return( -1 );
		}
		if( cache_value != NULL )
		{
			if( libfdata_tree_node_get_timestamp(
			     node,
			     &node_timestamp,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve time stamp from tree node.",
				 function );

				return( -1 );
			}
			if( libfcache_cache_value_get_identifier(
			     cache_value,
			     &cache_value_offset,
			     &cache_value_timestamp,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve cache value identifier.",
				 function );

				return( -1 );
			}
		}
		if( ( node_data_offset == cache_value_offset )
		 && ( node_timestamp == cache_value_timestamp ) )
		{
			result = 1;
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libnotify_verbose != 0 )
		{
			if( result == 0 )
			{
				libnotify_printf(
				 "%s: cache: 0x%08" PRIjx " miss (entry: %d, want: %" PRIi64 ", got: %" PRIi64 ")\n",
				 function,
				 (intptr_t) cache,
				 cache_entry_index,
				 node_data_offset,
				 cache_value_offset );
			}
			else
			{
				libnotify_printf(
				 "%s: cache: 0x%08" PRIjx " hit (entry: %d)\n",
				 function,
				 (intptr_t) cache,
				 cache_entry_index );
			}
		}
#endif
	}
	if( result == 0 )
	{
#if defined( HAVE_DEBUG_OUTPUT )
		if( libnotify_verbose != 0 )
		{
			libnotify_printf(
			 "%s: reading node data at offset: %" PRIi64 " (0x%08" PRIx64 ") of size: %" PRIu64 "\n",
			 function,
			 node_data_offset,
			 node_data_offset,
			 node_data_size );
		}
#endif
		/* Read the node data from the file IO handle
		 */
		if( internal_tree->read_node_data(
		     internal_tree->io_handle,
		     file_io_handle,
		     node,
		     cache,
		     node_data_offset,
		     node_data_size,
		     read_flags,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read node data at offset: %" PRIi64 ".",
			 function,
			 node_data_offset );

			return( -1 );
		}
		if( ( ( ( (libfdata_internal_tree_node_t *) node )->flags & LIBFDATA_TREE_NODE_FLAG_IS_VIRTUAL ) == 0 )
		 && ( ( ( (libfdata_internal_tree_node_t *) node )->flags & LIBFDATA_TREE_NODE_FLAG_IS_LEAF ) != 0 ) )
		{
			cache_entry_index = libfdata_tree_node_calculate_leaf_node_cache_entry_index(
			                     node_data_offset,
			                     number_of_cache_entries );
		}
		else
		{
			cache_entry_index = libfdata_tree_node_calculate_branch_node_cache_entry_index(
			                     node_data_offset,
			                     number_of_cache_entries );
		}
		if( libfcache_cache_get_value_by_index(
		     cache,
		     cache_entry_index,
		     &cache_value,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve cache entry: %d from cache.",
			 function,
			 cache_entry_index );

			return( -1 );
		}
		if( cache_value != NULL )
		{
			if( libfdata_tree_node_get_timestamp(
			     node,
			     &node_timestamp,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve time stamp from tree node.",
				 function );

				return( -1 );
			}
			if( libfcache_cache_value_get_identifier(
			     cache_value,
			     &cache_value_offset,
			     &cache_value_timestamp,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve cache value identifier.",
				 function );

				return( -1 );
			}
		}
		if( ( node_data_offset != cache_value_offset )
		 || ( node_timestamp != cache_value_timestamp ) )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing cache value.",
			 function );

			return( -1 );
		}
	}
	if( libfcache_cache_value_get_value(
	     cache_value,
	     node_value,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve node value.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Sets the node value of the node
 *
 * If the flag LIBFDATA_TREE_NODE_VALUE_FLAG_MANAGED is set the tree
 * takes over management of the value and the value is freed when
 * no longer needed.
 *
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_set_node_value(
     libfdata_tree_t *tree,
     libfcache_cache_t *cache,
     libfdata_tree_node_t *node,
     intptr_t *node_value,
     int (*free_node_value)(
            intptr_t **node_value,
            liberror_error_t **error ),
     uint8_t flags,
     liberror_error_t **error )
{
	static char *function       = "libfdata_tree_set_node_value";
	off64_t node_data_offset    = 0;
	size64_t node_data_size     = 0;
	time_t node_timestamp       = 0;
	uint32_t node_data_flags    = 0;
	int cache_entry_index       = -1;
	int number_of_cache_entries = 0;

	if( tree == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree.",
		 function );

		return( -1 );
	}
	if( libfdata_tree_node_get_data_range(
	     node,
	     &node_data_offset,
	     &node_data_size,
	     &node_data_flags,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve data range from tree node.",
		 function );

		return( -1 );
	}
	if( libfdata_tree_node_get_timestamp(
	     node,
	     &node_timestamp,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve timestamp from tree node.",
		 function );

		return( -1 );
	}
	if( libfcache_cache_get_number_of_entries(
	     cache,
	     &number_of_cache_entries,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of cache entries.",
		 function );

		return( -1 );
	}
	if( number_of_cache_entries <= 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid number of cache entries value out of bounds.",
		 function );

		return( -1 );
	}
	if( ( ( ( (libfdata_internal_tree_node_t *) node )->flags & LIBFDATA_TREE_NODE_FLAG_IS_VIRTUAL ) == 0 )
	 && ( ( ( (libfdata_internal_tree_node_t *) node )->flags & LIBFDATA_TREE_NODE_FLAG_IS_LEAF ) != 0 ) )
	{
		cache_entry_index = libfdata_tree_node_calculate_leaf_node_cache_entry_index(
				     node_data_offset,
				     number_of_cache_entries );
	}
	else
	{
		cache_entry_index = libfdata_tree_node_calculate_branch_node_cache_entry_index(
				     node_data_offset,
				     number_of_cache_entries );
	}
	if( libfcache_cache_set_value_by_index(
	     cache,
	     cache_entry_index,
	     (int64_t) node_data_offset,
	     node_timestamp,
	     node_value,
	     free_node_value,
	     flags,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set value in cache entry: %d.",
		 function,
		 cache_entry_index );

		return( -1 );
	}
	return( 1 );
}

/* Reads the sub nodes of the node
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_read_sub_nodes(
     libfdata_tree_t *tree,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     libfdata_tree_node_t *node,
     uint8_t read_flags,
     liberror_error_t **error )
{
	libfdata_internal_tree_t *internal_tree = NULL;
	static char *function                   = "libfdata_tree_read_sub_nodes";
	off64_t sub_nodes_offset                = 0;
	size64_t sub_nodes_size                 = 0;

	if( tree == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree.",
		 function );

		return( -1 );
	}
	internal_tree = (libfdata_internal_tree_t *) tree;

	if( internal_tree->read_sub_nodes == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid tree - missing read sub nodes function.",
		 function );

		return( -1 );
	}
	if( libfdata_tree_node_get_sub_nodes_range(
	     node,
	     &sub_nodes_offset,
	     &sub_nodes_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve sub nodes range.",
		 function );

		return( -1 );
	}
	/* Read the sub nodes from the file IO handle
	 */
	if( internal_tree->read_sub_nodes(
	     internal_tree->io_handle,
	     file_io_handle,
	     node,
	     cache,
	     sub_nodes_offset,
	     sub_nodes_size,
	     read_flags,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read sub nodes at offset: %" PRIi64 ".",
		 function,
		 sub_nodes_offset );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the offset and size of the root node
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_get_root_node(
     libfdata_tree_t *tree,
     libfdata_tree_node_t **root_node,
     liberror_error_t **error )
{
	libfdata_internal_tree_t *internal_tree = NULL;
	static char *function                   = "libfdata_tree_get_root_node";

	if( tree == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree.",
		 function );

		return( -1 );
	}
	internal_tree = (libfdata_internal_tree_t *) tree;

	if( root_node == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid root node.",
		 function );

		return( -1 );
	}
	*root_node = internal_tree->root_node;

	return( 1 );
}

/* Sets the data offset and size of the root node
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_set_root_node(
     libfdata_tree_t *tree,
     off64_t node_data_offset,
     size64_t node_data_size,
     liberror_error_t **error )
{
	libfdata_internal_tree_t *internal_tree = NULL;
	static char *function                   = "libfdata_tree_set_root_node";
	uint32_t node_data_flags                = 0;

	if( tree == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree.",
		 function );

		return( -1 );
	}
	internal_tree = (libfdata_internal_tree_t *) tree;

	if( internal_tree->root_node == NULL )
	{
		if( libfdata_tree_node_initialize(
		     &( internal_tree->root_node ),
		     tree,
		     NULL,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create root node.",
			 function );

			return( -1 );
		}
	}
	if( libfdata_tree_node_set_data_range(
	     internal_tree->root_node,
	     node_data_offset,
	     node_data_size,
	     node_data_flags,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set root node data range.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the number of leaf nodes in the tree
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_get_number_of_leaf_nodes(
     libfdata_tree_t *tree,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     int *number_of_leaf_nodes,
     uint8_t read_flags,
     liberror_error_t **error )
{
	libfdata_internal_tree_t *internal_tree = NULL;
	static char *function                   = "libfdata_tree_get_number_of_leaf_nodes";

	if( tree == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree.",
		 function );

		return( -1 );
	}
	internal_tree = (libfdata_internal_tree_t *) tree;

	if( libfdata_tree_node_get_number_of_leaf_nodes(
	     internal_tree->root_node,
	     file_io_handle,
	     cache,
	     number_of_leaf_nodes,
	     read_flags,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of leaf nodes from root node.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves a leaf nodes at a specific index
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_get_leaf_node_by_index(
     libfdata_tree_t *tree,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     int leaf_node_index,
     libfdata_tree_node_t **leaf_node,
     uint8_t read_flags,
     liberror_error_t **error )
{
	libfdata_internal_tree_t *internal_tree = NULL;
	static char *function                   = "libfdata_tree_get_leaf_node_by_index";

	if( tree == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree.",
		 function );

		return( -1 );
	}
	internal_tree = (libfdata_internal_tree_t *) tree;

	if( libfdata_tree_node_get_leaf_node_by_index(
	     internal_tree->root_node,
	     file_io_handle,
	     cache,
	     leaf_node_index,
	     leaf_node,
	     read_flags,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve leaf node: %d from root node.",
		 function,
		 leaf_node_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the number of deleted leaf nodes in the tree
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_get_number_of_deleted_leaf_nodes(
     libfdata_tree_t *tree,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     int *number_of_deleted_leaf_nodes,
     uint8_t read_flags,
     liberror_error_t **error )
{
	libfdata_internal_tree_t *internal_tree = NULL;
	static char *function                   = "libfdata_tree_get_number_of_deleted_leaf_nodes";

	if( tree == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree.",
		 function );

		return( -1 );
	}
	internal_tree = (libfdata_internal_tree_t *) tree;

	if( libfdata_tree_node_get_number_of_deleted_leaf_nodes(
	     internal_tree->root_node,
	     file_io_handle,
	     cache,
	     number_of_deleted_leaf_nodes,
	     read_flags,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of deleted leaf nodes from root node.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves a deleted leaf nodes at a specific index
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_get_deleted_leaf_node_by_index(
     libfdata_tree_t *tree,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     int deleted_leaf_node_index,
     libfdata_tree_node_t **deleted_leaf_node,
     uint8_t read_flags,
     liberror_error_t **error )
{
	libfdata_internal_tree_t *internal_tree = NULL;
	static char *function                   = "libfdata_tree_get_deleted_leaf_node_by_index";

	if( tree == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree.",
		 function );

		return( -1 );
	}
	internal_tree = (libfdata_internal_tree_t *) tree;

	if( libfdata_tree_node_get_deleted_leaf_node_by_index(
	     internal_tree->root_node,
	     file_io_handle,
	     cache,
	     deleted_leaf_node_index,
	     deleted_leaf_node,
	     read_flags,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of deleted leaf node: %d from root node.",
		 function,
		 deleted_leaf_node_index );

		return( -1 );
	}
	return( 1 );
}

