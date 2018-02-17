/*                               -*- Mode: C -*- 
 * vector.h --- growable array of generic elements
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Sun Jun 19 13:50:02 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Wed Jul 13 15:47:06 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 */

#ifndef VECTOR_H
#define VECTOR_H

/*****************************************************************
 * Constants                                                     *
 *****************************************************************/

#define DEV_VECTOR_CAPACITY_INCREMENT 10
#define DEV_VECTOR_INITIAL_CAPACITY 10

/*****************************************************************
 * vector_t - growable array of generic objects                  *
 *****************************************************************/

typedef struct {
  void **elems;
  int capacity;
  int increment;
  int count;
} vector_t;

/*****************************************************************
 * Interface (exported)                                          *
 *****************************************************************/

extern vector_t *dev_new_vector();

extern vector_t *dev_new_vector2( int c, int i );

extern int dev_vector_size( vector_t *xs );

extern int dev_vector_is_empty( vector_t *xs );

extern void *dev_vector_get( vector_t *xs, int index );

extern void *dev_vector_get_first( vector_t *xs );

extern void *dev_vector_get_last( vector_t *xs );

extern void dev_vector_set( vector_t *xs, int index, void *elem );

extern void dev_vector_add( vector_t *xs, void *elem );

extern void *dev_vector_remove( vector_t *xs );

extern void *dev_vector_serve( vector_t *xs );

extern void **dev_vector_to_array( vector_t *xs );

extern void dev_free_vector( vector_t *xs, void ( *free_elem )( void * ) );

extern void dev_vector_trim( vector_t *xs );

#endif
