/*                               -*- Mode: C -*- 
 * vector.c --- growable array of generic elements
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Sun Jun 19 13:50:02 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Wed Jul 13 15:47:00 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 */

#include "libdev.h"
#include "vector.h"

/*****************************************************************
 * dev_new_vector - allocates and initialises a new vector       *
 *****************************************************************/

vector_t *
dev_new_vector()
{
  vector_t *xs = ( vector_t * ) dev_malloc( sizeof( vector_t ) );

  xs->elems = ( void * ) dev_malloc( DEV_VECTOR_INITIAL_CAPACITY * sizeof( void * ) );
  xs->capacity = DEV_VECTOR_INITIAL_CAPACITY;
  xs->increment = DEV_VECTOR_CAPACITY_INCREMENT;
  xs->count = 0;

  return xs;
}

/*****************************************************************
 * dev_new_vector2 - allocates and initialises a new vector      *
 * c : initial capacity                                          *
 * i : increment                                                 *
 *****************************************************************/

vector_t *
dev_new_vector2( int c, int i )
{
  vector_t *xs = ( vector_t * ) dev_malloc( sizeof( vector_t ) );

  xs->elems = ( void * ) dev_malloc( c * sizeof( void * ) );
  xs->capacity = c;
  xs->increment = i;
  xs->count = 0;

  return xs;
}

/*****************************************************************
 * dev_vector_size - returns the size of this vector             *
 * xs : input vector                                             *
 *****************************************************************/

int
dev_vector_size( vector_t *xs )
{
  return xs->count;
}

/*****************************************************************
 * dev_vector_is_empty - returns if this vector is empty         *
 * xs : input vector                                             *
 *****************************************************************/

int
dev_vector_is_empty( vector_t *xs )
{
  return xs->count == 0;
}

/*****************************************************************
 * dev_vector_get - returns the element at the specified index   *
 * xs : input vector                                             *
 * index : index into this vector                                *
 *****************************************************************/

void *
dev_vector_get( vector_t *xs, int index )
{
  assert( index >= 0 && index < xs->count );

  return xs->elems[ index ];
}

/*****************************************************************
 * dev_vector_get_first - returns the first element              *
 * xs : input vector                                             *
 *****************************************************************/

void *
dev_vector_get_first( vector_t *xs )
{
  assert( xs->count > 0 );

  return xs->elems[ 0 ];
}

/*****************************************************************
 * dev_vector_get_last - returns the last element                *
 * xs : input vector                                             *
 *****************************************************************/

void *
dev_vector_get_last( vector_t *xs )
{
  assert( xs->count > 0 );

  return xs->elems[ xs->count - 1 ];
}

/*****************************************************************
 * dev_vector_set - sets the element at the specified index      *
 * xs : input vector                                             *
 * index : index into this vector                                *
 * elem : the element to be added at position index              *
 *****************************************************************/

void
dev_vector_set( vector_t *xs, int index, void *elem )
{
  assert( index >= 0 && index < xs->count );

  xs->elems[ index ] = elem;
}

/*****************************************************************
 * increase_capacity_if_needed -                                 *
 * xs : input vector                                             *
 *****************************************************************/

static inline void
increase_capacity_if_needed( vector_t *xs )
{
  if ( xs->count < xs->capacity )
    return;

  int c = xs->capacity + xs->increment;
  void **old, **elems = ( void ** ) dev_malloc( c * sizeof( void * ) );

  for ( int i=0; i<xs->count; i++ )
    elems[ i ] = xs->elems[ i ];

  old = xs->elems;
  xs->elems = elems;
  xs->capacity = c;

  dev_free( old );
}

/*****************************************************************
 * dev_vector_add - adds an element at the end of this vector    *
 * xs : input vector                                             *
 * elem : the element to be added                                *
 *****************************************************************/

void
dev_vector_add( vector_t *xs, void *elem )
{
  increase_capacity_if_needed( xs );

  xs->elems[ xs->count++ ] = elem;
}

/*****************************************************************
 * dev_vector_remove - removes the last element                  *
 * xs : input vector                                             *
 * removes and returns the last element                          *
 *****************************************************************/

void *
dev_vector_remove( vector_t *xs )
{
  void *elem;

  assert( xs->count > 0 );

  xs->count--;
  elem = xs->elems[ xs->count ];
  xs->elems[ xs->count ] = NULL;

  return elem;
}

/*****************************************************************
 * dev_vector_serve - removes and returns the first element      *
 * xs : input vector                                             *
 * removes and returns the first element                         *
 *****************************************************************/

void *
dev_vector_serve( vector_t *xs )
{
  void *elem;

  assert( xs->count > 0 );

  elem = xs->elems[ 0 ];

  for ( int i=1; i < xs->count; i++ )
    xs->elems[ i-1 ] = xs->elems[ i ];

  xs->count--;

  return elem;
}

/*****************************************************************
 * dev_vector_to_array - returns an array containing all the     *
 * elments in this vector.                                       *
 * xs : input vector                                             *
 * returns an array containing all the elements of this vector   *
 *****************************************************************/

void **
dev_vector_to_array( vector_t *xs )
{
  void **elems = ( void ** ) dev_malloc( xs->count * sizeof( void * ) );

  for ( int i=0; i < xs->count; i++ )
    elems[ i ] = xs->elems[ i ];

  return elems;
}

/*****************************************************************
 * dev_free_vector - frees memory space                          *
 * xs : input vector                                             *
 *****************************************************************/

void
dev_free_vector( vector_t *xs, void ( *free_elem )( void *e ) )
{
  for ( int i=0; i<xs->count; i++ )
    free_elem( xs->elems[ i ] );

  dev_free( xs->elems );
  dev_free( xs );
}

/*****************************************************************
 * dev_vector_trim - trims to current size                       *
 * xs : input vector                                             *
 *****************************************************************/

void
dev_vector_trim( vector_t *xs )
{
  xs->elems = ( void * ) dev_realloc( xs->elems, xs->count * sizeof( void * ) );
  xs->capacity = xs->count;
}

