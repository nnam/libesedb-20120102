/*
 * The tree node functions
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

#if !defined( _LIBFDATA_INTERNAL_TREE_NODE_H )
#define _LIBFDATA_INTERNAL_TREE_NODE_H

#include <common.h>
#include <types.h>

#include <liberror.h>

#include "libfdata_array_type.h"
#include "libfdata_date_time.h"
#include "libfdata_extern.h"
#include "libfdata_libbfio.h"
#include "libfdata_libfcache.h"
#include "libfdata_range.h"
#include "libfdata_tree.h"
#include "libfdata_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

#define libfdata_tree_node_calculate_branch_node_cache_entry_index( node_data_offset, number_of_cache_entries ) \
	( number_of_cache_entries > 4 ) ? ( node_data_offset % ( number_of_cache_entries / 4 ) ) : ( node_data_offset % number_of_cache_entries )

#define libfdata_tree_node_calculate_leaf_node_cache_entry_index( node_data_offset, number_of_cache_entries ) \
	( number_of_cache_entries > 4 ) ? ( number_of_cache_entries / 4 ) + ( node_data_offset % ( ( number_of_cache_entries * 3 ) / 4 ) ) : ( node_data_offset % number_of_cache_entries )

typedef struct libfdata_internal_tree_node libfdata_internal_tree_node_t;

struct libfdata_internal_tree_node
{
	/* The tree reference
	 */
	libfdata_tree_t *tree;

	/* The parent (tree) node reference
	 */
	libfdata_tree_node_t *parent_node;

	/* The (element) data range
	 */
	libfdata_range_t *data_range;

	/* The sub nodes (data) offset
	 */
	off64_t sub_nodes_offset;

	/* The sub nodes (data) size
	 */
	size64_t sub_nodes_size;

	/* The time stamp
	 */
	time_t timestamp;

	/* The flags
	 */
	uint8_t flags;

	/* The sub nodes (array)
	 */
	libfdata_array_t *sub_nodes;

	/* The number of leaf nodes
	 */
	int number_of_leaf_nodes;

	/* The number of deleted leaf nodes
	 */
	int number_of_deleted_leaf_nodes;

	/* The relative first leaf node index
	 */
	int first_leaf_node_index;

	/* The relative first deleted leaf node index
	 */
	int first_deleted_leaf_node_index;
};

int libfdata_tree_node_initialize(
     libfdata_tree_node_t **node,
     libfdata_tree_t *tree,
     libfdata_tree_node_t *parent_node,
     liberror_error_t **error );

int libfdata_tree_node_free(
     libfdata_tree_node_t **node,
     liberror_error_t **error );

int libfdata_tree_node_free_single(
     libfdata_tree_node_t **node,
     liberror_error_t **error );

int libfdata_tree_node_set_parent_node(
     libfdata_tree_node_t *node,
     libfdata_tree_node_t *parent_node,
     liberror_error_t **error );

int libfdata_tree_node_is_root(
     libfdata_tree_node_t *node,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_tree_node_get_data_range(
     libfdata_tree_node_t *node,
     off64_t *offset,
     size64_t *size,
     uint32_t *flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_tree_node_set_data_range(
     libfdata_tree_node_t *node,
     off64_t offset,
     size64_t size,
     uint32_t flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_tree_node_get_sub_nodes_range(
     libfdata_tree_node_t *node,
     off64_t *sub_nodes_offset,
     size64_t *sub_nodes_size,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_tree_node_set_sub_nodes_range(
     libfdata_tree_node_t *node,
     off64_t sub_nodes_offset,
     size64_t sub_nodes_size,
     liberror_error_t **error );

int libfdata_tree_node_get_timestamp(
     libfdata_tree_node_t *node,
     time_t *timestamp,
     liberror_error_t **error );

int libfdata_tree_node_get_first_leaf_node_index(
     libfdata_tree_node_t *node,
     int *first_leaf_node_index,
     int *deleted_first_leaf_node_index,
     liberror_error_t **error );

int libfdata_tree_node_set_first_leaf_node_index(
     libfdata_tree_node_t *node,
     int first_leaf_node_index,
     int deleted_first_leaf_node_index,
     liberror_error_t **error );

/* Node value functions
 */
LIBFDATA_EXTERN \
int libfdata_tree_node_get_node_value(
     libfdata_tree_node_t *node,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     intptr_t **node_value,
     uint8_t read_flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_tree_node_set_node_value(
     libfdata_tree_node_t *node,
     libfcache_cache_t *cache,
     intptr_t *node_value,
     int (*free_node_value)(
            intptr_t **node_value,
            liberror_error_t **error ),
     uint8_t flags,
     liberror_error_t **error );

/* Sub node functions
 */
LIBFDATA_EXTERN \
int libfdata_tree_node_resize_sub_nodes(
     libfdata_tree_node_t *node,
     int number_of_sub_nodes,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_tree_node_get_number_of_sub_nodes(
     libfdata_tree_node_t *node,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     int *number_of_sub_nodes,
     uint8_t read_flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_tree_node_get_sub_node_by_index(
     libfdata_tree_node_t *node,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     int sub_node_index,
     libfdata_tree_node_t **sub_node,
     uint8_t read_flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_tree_node_set_sub_node_by_index(
     libfdata_tree_node_t *node,
     int sub_node_index,
     off64_t offset,
     size64_t size,
     uint32_t flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_tree_node_append_sub_node(
     libfdata_tree_node_t *node,
     int *sub_node_index,
     off64_t offset,
     size64_t size,
     uint32_t flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_tree_node_insert_sub_node(
     libfdata_tree_node_t *node,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     int *sub_node_index,
     off64_t offset,
     size64_t size,
     uint32_t flags,
     int (*node_value_compare_function)(
            intptr_t *first_node_value,
            intptr_t *second_node_value,
            liberror_error_t **error ),
     uint8_t insert_flags,
     uint8_t read_flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_tree_node_split_sub_nodes(
     libfdata_tree_node_t *node,
     int number_of_splits,
     liberror_error_t **error );

/* Deleted node functions
 */
LIBFDATA_EXTERN \
int libfdata_tree_node_is_deleted(
     libfdata_tree_node_t *node,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_tree_node_set_deleted(
     libfdata_tree_node_t *node,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_tree_node_set_deleted_sub_node(
     libfdata_tree_node_t *node,
     int sub_node_index,
     liberror_error_t **error );

/* Leaf node functions
 */
int libfdata_tree_node_set_calculate_leaf_node_values(
     libfdata_tree_node_t *node,
     liberror_error_t **error );

int libfdata_tree_node_read_leaf_node_values(
     libfdata_tree_node_t *node,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     uint8_t read_flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_tree_node_is_leaf(
     libfdata_tree_node_t *node,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     uint8_t read_flags,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_tree_node_set_leaf(
     libfdata_tree_node_t *node,
     liberror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_tree_node_set_leaf_sub_node(
     libfdata_tree_node_t *node,
     int sub_node_index,
     liberror_error_t **error );

int libfdata_tree_node_get_number_of_leaf_nodes(
     libfdata_tree_node_t *node,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     int *number_of_leaf_nodes,
     uint8_t read_flags,
     liberror_error_t **error );

int libfdata_tree_node_get_leaf_node_by_index(
     libfdata_tree_node_t *node,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     int leaf_node_index,
     libfdata_tree_node_t **leaf_node,
     uint8_t read_flags,
     liberror_error_t **error );

int libfdata_tree_node_get_number_of_deleted_leaf_nodes(
     libfdata_tree_node_t *node,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *cache,
     int *number_of_deleted_leaf_nodes,
     uint8_t read_flags,
     liberror_error_t **error );

int libfdata_tree_node_get_deleted_leaf_node_by_index(
     libfdata_tree_node_t *node,
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

