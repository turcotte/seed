/*                               -*- Mode: C -*- 
 * bitset.c --- implements a vector of bits
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Thu Jul  7 16:08:15 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Wed Jul 13 15:45:48 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 */

#include "libdev.h"
#include "bitset.h"

/*****************************************************************
 * macros                                                        *
 *****************************************************************/

#define BITS_PER_UNIT ( sizeof( unit_t ) * 8 )

/*****************************************************************
 * dev_new_bitset - allocates and initialses a new bitset        *
 *****************************************************************/

bitset_t *
dev_new_bitset( int size )
{
  bitset_t *b;

  assert( size > 0 );

  b = ( bitset_t * ) dev_malloc( sizeof( bitset_t ) );

  b->size = size;

  b->length = ( size-1 ) / BITS_PER_UNIT + 1;

  b->units = ( unit_t * ) dev_malloc( b->length * sizeof( unit_t ) );

  for ( int u=0; u < b->length; u++ )
    b->units[ u ] = (unit_t) 0;

  return b;
}

/*****************************************************************
 * dev_clone_bitset -                                            *
 *****************************************************************/

bitset_t *
dev_clone_bitset( bitset_t *b )
{
  bitset_t *copy;

  copy = ( bitset_t * ) dev_malloc( sizeof( bitset_t ) );

  copy->size = b->size;

  copy->length = b->length;

  copy->units = ( unit_t * ) dev_malloc( copy->length * sizeof( unit_t ) );

  for ( int u=0; u < copy->length; u++ )
    copy->units[ u ] = b->units[ u ];

  return copy;
}

/*****************************************************************
 * dev_free_bitset -                                             *
 *****************************************************************/

void
dev_free_bitset( bitset_t *b )
{
  dev_free( b->units );
  dev_free( b );
}

/*****************************************************************
 * dev_bitset_cardinality - returns the number of bits set       *
 *****************************************************************/

int
dev_bitset_cardinality( bitset_t *b )
{
  int n = 0;

  for ( int i=0; i < b->size; i++ )
    if ( dev_bitset_get( b, i ) )
      n++;

  return n;
}

/*****************************************************************
 * dev_bitset_size - returns the size of this bitset             *
 *****************************************************************/

int
dev_bitset_size( bitset_t *b )
{
  return b->size;
}

/*****************************************************************
 * dev_bitset_and -                                              *
 *****************************************************************/

/*****************************************************************
 * dev_bitset_or -                                               *
 *****************************************************************/

/*****************************************************************
 * dev_bitset_equals -                                           *
 *****************************************************************/

int
dev_bitset_equals( bitset_t *a, bitset_t *b )
{
  if ( a->size != b->size)
    return FALSE;

  for ( int u=0; u < a->length; u++ )
    if ( a->units[ u ] != b->units[ u ] )
      return FALSE;

  return TRUE;
}

/*****************************************************************
 * dev_bitset_set -                                              *
 *****************************************************************/

void
dev_bitset_set( bitset_t *b, int i )
{
  int u = i / BITS_PER_UNIT;
  unit_t mask = (unit_t) 1;

  mask = mask << i;

  b->units[ u ] = b->units[ u ] | mask; 
}

/*****************************************************************
 * dev_bitset_get -                                              *
 *****************************************************************/

int
dev_bitset_get( bitset_t *b, int i )
{
  assert( i >= 0 && i < b->size );

  int u = i / BITS_PER_UNIT;
  unit_t mask = (unit_t) 1;

  mask = mask << i;

  return b->units[ u ] & mask; 
}

/*****************************************************************
 * dev_bitset_clear -                                            *
 *****************************************************************/

void
dev_bitset_clear( bitset_t *b, int i )
{
  int u = i / BITS_PER_UNIT;
  unit_t mask = (unit_t) 1;

  mask = mask << i;

  b->units[ u ] = b->units[ u ] ^ mask; 

}

/*****************************************************************
 * dev_bitset_rightmost_one -                                    *
 *****************************************************************/

int
dev_bitset_rightmost_one( bitset_t *b )
{
  for ( int i=0; i < b->size; i++ )
    if ( dev_bitset_get( b, i ) )
      return i;
  return -1;
}

/*****************************************************************
 * dev_bitset_leftmost_one -                                    *
 *****************************************************************/

int
dev_bitset_leftmost_one( bitset_t *b )
{
  for ( int i=( b->size - 1 ); i>=0; i-- )
    if ( dev_bitset_get( b, i ) )
      return i;
  return -1;
}

/*****************************************************************
 * dev_bitset_tostring -                                         *
 *****************************************************************/

char *
dev_bitset_tostring( bitset_t *b )
{
  char *result;

  result = dev_malloc( b->size + 1 );

  for ( int i=0; i < b->size; i++ )
    result[ b->size - i - 1 ] =  dev_bitset_get( b, i ) ? '1' : '0';

  result[ b->size ] = '\0';

  return result;
}
