/*                               -*- Mode: C -*- 
 * ivector.c --- growable array of pos_t values
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Wed Jun 22 17:26:56 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Wed Jul 13 15:45:59 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 */

#include "libdev.h"
#include "ivector.h"

/*****************************************************************
 * dev_new_ivector - allocates and initialises a new ivector     *
 *****************************************************************/

ivector_t *
dev_new_ivector()
{
  return dev_new_ivector2( DEV_IVECTOR_INITIAL_CAPACITY, DEV_IVECTOR_CAPACITY_INCREMENT );
}

/*****************************************************************
 * dev_new_ivector2 - allocates and initialises a new ivector    *
 * c : initial capacity                                          *
 * i : increment                                                 *
 *****************************************************************/

ivector_t *
dev_new_ivector2( int c, int i )
{
  ivector_t *xs = ( ivector_t * ) dev_malloc( sizeof( ivector_t ) );

  xs->elems = ( pos_t * ) dev_malloc( c * sizeof( pos_t ) );
  xs->capacity = c;
  xs->increment = i;
  xs->count = 0;

  return xs;
}

/*****************************************************************
 * dev_ivector_size - returns the size of this ivector           *
 * xs : input ivector                                            *
 *****************************************************************/

int
dev_ivector_size( ivector_t *xs )
{
  return xs->count;
}

/*****************************************************************
 * dev_ivector_is_empty - returns if this ivector is empty       *
 * xs : input ivector                                            *
 *****************************************************************/

int
dev_ivector_is_empty( ivector_t *xs )
{
  return xs->count == 0;
}

/*****************************************************************
 * dev_ivector_get - returns the element at the specified index  *
 * xs : input ivector                                            *
 * index : index into this ivector                               *
 *****************************************************************/

pos_t
dev_ivector_get( ivector_t *xs, int index )
{
  assert( index >= 0 && index < xs->count );

  return xs->elems[ index ];
}

/*****************************************************************
 * dev_ivector_get_first - returns the first element             *
 * xs : input ivector                                            *
 *****************************************************************/

pos_t
dev_ivector_get_first( ivector_t *xs )
{
  assert( xs->count > 0 );

  return xs->elems[ 0 ];
}

/*****************************************************************
 * dev_ivector_get_last - returns the last element               *
 * xs : input ivector                                            *
 *****************************************************************/

pos_t
dev_ivector_get_last( ivector_t *xs )
{
  assert( xs->count > 0 );

  return xs->elems[ xs->count - 1 ];
}

/*****************************************************************
 * dev_ivector_set - sets the element at the specified index     *
 * xs : input ivector                                            *
 * index : index into this ivector                               *
 * elem : the element to be added at position index              *
 *****************************************************************/

void
dev_ivector_set( ivector_t *xs, int index, pos_t elem )
{
  assert( index >= 0 && index < xs->count );

  xs->elems[ index ] = elem;
}

/*****************************************************************
 * increase_capacity_if_needed -                                 *
 * xs : input ivector                                            *
 *****************************************************************/

static inline void
increase_capacity_if_needed( ivector_t *xs )
{
  if ( xs->count < xs->capacity )
    return;

  int c = xs->capacity + xs->increment;
  pos_t *old, *elems = ( pos_t * ) dev_malloc( c * sizeof( pos_t ) );

  for ( int i=0; i<xs->count; i++ )
    elems[ i ] = xs->elems[ i ];

  old = xs->elems;
  xs->elems = elems;
  xs->capacity = c;

  dev_free( old );
}

/*****************************************************************
 * dev_ivector_add - adds an element at the end of this ivector  *
 * xs : input ivector                                            *
 * elem : the element to be added                                *
 *****************************************************************/

void
dev_ivector_add( ivector_t *xs, pos_t elem )
{
  increase_capacity_if_needed( xs );

  xs->elems[ xs->count++ ] = elem;
}

/*****************************************************************
 * dev_ivector_remove - removes the last element                 *
 * xs : input ivector                                            *
 * removes and returns the last element                          *
 *****************************************************************/

pos_t
dev_ivector_remove( ivector_t *xs )
{
  pos_t elem;

  assert( xs->count > 0 );

  xs->count--;
  elem = xs->elems[ xs->count ];

  return elem;
}

/*****************************************************************
 * dev_ivector_serve - removes and returns the first element     *
 * xs : input ivector                                            *
 * removes and returns the first element                         *
 *****************************************************************/

pos_t
dev_ivector_serve( ivector_t *xs )
{
  pos_t elem;

  assert( xs->count > 0 );

  elem = xs->elems[ 0 ];

  for ( int i=1; i < xs->count; i++ )
    xs->elems[ i-1 ] = xs->elems[ i ];

  xs->count--;

  return elem;
}

/*****************************************************************
 * dev_ivector_to_array - returns an array containing all the    *
 * elments in this ivector.                                      *
 * xs : input ivector                                            *
 * returns an array containing all the elements of this ivector  *
 *****************************************************************/

pos_t *
dev_ivector_to_array( ivector_t *xs )
{
  pos_t *elems = ( pos_t * ) dev_malloc( xs->count * sizeof( pos_t ) );

  for ( int i=0; i < xs->count; i++ )
    elems[ i ] = xs->elems[ i ];

  return elems;
}

/*****************************************************************
 * dev_free_ivector - frees memory space                         *
 * xs : input ivector                                            *
 *****************************************************************/

void
dev_free_ivector( ivector_t *xs )
{
  dev_free( xs->elems );
  dev_free( xs );
}

/*****************************************************************
 * dev_ivector_trim - trims to current size                      *
 * xs : input ivector                                            *
 *****************************************************************/

void
dev_ivector_trim( ivector_t *xs )
{
  xs->elems = ( pos_t * ) dev_realloc( xs->elems, xs->count * sizeof( pos_t ) );
  xs->capacity = xs->count;
}

