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

#if !defined( _LIBFDATA_INTERNAL_TREE_H )
#define _LIBFDATA_INTERNAL_TREE_H

#include <common.h>
#include <types.h>

#include <liberror.h>

#include "libfdata_array_type.h"
#include "libfdata_extern.h"
#include "libfdata_libbfio.h"
#include "libfdata_libfcache.h"
#include "libfdata_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libfdata_internal_tree libfdata_internal_tree_t;

struct libfdata_internal_tree
{
	/* The root node
	 */
	libfdata_tree_node_t *root_node;

	/* The flags
	 */
	uint8_t flags;

	/* The io handle
	 */
	intptr_t *io_handle;

	/* The free io handle function
	 */
	int (*free_io_handle)(
	       intptr_t **io_handle,
	       liberror_error_t **error );

	/* The clone (duplicate) io handle function
	 */
	int (*clone_io_handle)(
	       intptr_t **destination_io_handle,
	       intptr_t *source_io_handle,
	       liberror_error_t **error );

	/* The read node data function
	 */
	int (*read_node_data)(
	       intptr_t *io_handle,
	       libbfio_handle_t *file_io_handle,
	       libfdata_tree_node_t *node,
	       libfcache_cache_t *cache,
	       off64_t node_data_offset,
	       size64_t node_data_size,
               uint8_t read_flags,
	       liberror_error_t **error );

	/* The read sub nodes function
	 */
	int (*read_sub_nodes)(
	       intptr_t *io_handle,
	       libbfio_handle_t *file_io_handle,
	       libfdata_tree_node_t *node,
	       libfcache_cache_t *cache,
	       off64_t sub_nodes_offset,
	       size64_t sub_nodes_size,
               uint8_t read_flags,
	       liberror_error_t **error );
};

LIBFDATA_EXTERN \
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
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_tree_free(
     libfdata_tree_t **tree,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_tree_clone(
     libfdata_tree_t **destination_tree,
     libfdata_tree_t *source_tree,
     liberror_error_t **error );

/* Node value functions
 */
int libfdata_tree_get_node_value(
     libfdata_tree_t *tree,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     libfdata_tree_node_t *node,
     intptr_t **node_value,
     uint8_t read_flags,
     liberror_error_t **error );

int libfdata_tree_set_node_value(
     libfdata_tree_t *tree,
     libfcache_cache_t *cache,
     libfdata_tree_node_t *node,
     intptr_t *node_value,
     int (*free_node_value)(
            intptr_t **node_value,
            liberror_error_t **error ),
     uint8_t flags,
     liberror_error_t **error );

/* Sub node functions
 */
int libfdata_tree_read_sub_nodes(
     libfdata_tree_t *tree,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     libfdata_tree_node_t *node,
     uint8_t read_flags,
     liberror_error_t **error );

/* Root node functions
 */
LIBFDATA_EXTERN \
int libfdata_tree_get_root_node(
     libfdata_tree_t *tree,
     libfdata_tree_node_t **root_node,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_tree_set_root_node(
     libfdata_tree_t *tree,
     off64_t node_data_offset,
     size64_t node_data_size,
     liberror_error_t **error );

/* Leaf node functions
 */
LIBFDATA_EXTERN \
int libfdata_tree_get_number_of_leaf_nodes(
     libfdata_tree_t *tree,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     int *number_of_leaf_nodes,
     uint8_t read_flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_tree_get_leaf_node_by_index(
     libfdata_tree_t *tree,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     int leaf_node_index,
     libfdata_tree_node_t **leaf_node,
     uint8_t read_flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_tree_get_number_of_deleted_leaf_nodes(
     libfdata_tree_t *tree,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     int *number_of_deleted_leaf_nodes,
     uint8_t read_flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_tree_get_deleted_leaf_node_by_index(
     libfdata_tree_t *tree,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     int deleted_leaf_node_index,
     libfdata_tree_node_t **deleted_leaf_node,
     uint8_t read_flags,
     liberror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

