/*                               -*- Mode: C -*- 
 * tests.c --- tests driver
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Tue Jun  7 14:30:10 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Wed Jul 13 15:46:54 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 */

#include "libdev.h"
#include "vector.h"
#include "list.h"
#include "bitset.h"

/*****************************************************************
 * banner -                                                      *
 *****************************************************************/

static void
banner( void )
{
  printf( "* libvdev - tests driver *\n" );
}

/*****************************************************************
 * vector_test -                                                 *
 *****************************************************************/

typedef struct {
  int value;
} int_t;

void
vector_test( void )
{
  vector_t *vs= dev_new_vector2( 1000, 100 );

  printf( "vector:\n" );

  printf( "  add\n" );
  for ( int i=0; i<10000; i++ ) {
    int_t *v = ( int_t * ) dev_malloc( sizeof( int_t ) );
    v->value = i;
    dev_vector_add( vs, v );
  }

  printf( "  get\n" );
  for ( int i=0; i< dev_vector_size( vs ); i++ ) {
    int_t *v = ( int_t * ) dev_vector_get( vs, i );
    assert( v->value == i );
  }

  printf( "  remove\n" );
  for ( int i=dev_vector_size( vs ) - 1; i>=100 ; i-- ) {
    int_t *v = ( int_t * ) dev_vector_remove( vs );
    assert( v->value == i );
    dev_free( v );
  }

  printf( "  serve\n" );
  for ( int i=0; dev_vector_size( vs ) != 0; i++ ) {
    int_t *v = ( int_t * ) dev_vector_serve( vs );
    assert( v->value == i );
    dev_free( v );
  }

  dev_free_vector( vs, free );

  printf( "done!\n\n" );

}

void
list_test( void )
{
  list_t *l= dev_new_list();

  printf( "testing list ...\n" );

  printf( "  add\n" );
  for ( int i=0; i<10000; i++ ) {
    int_t *v = ( int_t * ) dev_malloc( sizeof( int_t ) );
    v->value = i;
    dev_list_add( l, v );
  }

  printf( "  get\n" );
  for ( int i=0; i< dev_list_size( l ); i++ ) {
    int_t *v = ( int_t * ) dev_list_get( l, i );
    assert( v->value == i );
  }

  printf( "  remove\n" );
  for ( int i=dev_list_size( l ) - 1; i>=100 ; i-- ) {
    int_t *v = ( int_t * ) dev_list_remove( l );
    assert( v->value == i );
    dev_free( v );
  }

  printf( "  serve\n" );
  for ( int i=0; dev_list_size( l ) != 0; i++ ) {
    int_t *v = ( int_t * ) dev_list_serve( l );
    assert( v->value == i );
    dev_free( v );
  }

  printf( "free\n\n" );
  dev_free_list( l, free );

  printf( "done!\n\n" );

}

/*****************************************************************
 * bitset_test -                                                 *
 *****************************************************************/

void
bitset_test( void )
{
  printf( "testing bitset ...\n" );

  for ( int k=4; k<129; k=2*k ) {
    bitset_t *b = dev_new_bitset( k );
    for ( int j=0; j<k; j++ ) {
      dev_bitset_set( b, j );
      printf( "%s\n", dev_bitset_tostring( b ) );
      dev_bitset_clear( b, j );
      printf( "%s\n", dev_bitset_tostring( b ) );
    }
    dev_free_bitset( b );
  }

  bitset_t *b = dev_new_bitset( 48 );

  for ( int j=0; j<48; j=j+3 ) {
    dev_bitset_set( b, j );
    printf( "%s\n", dev_bitset_tostring( b ) );

    bitset_t *c = dev_clone_bitset( b );

    printf( "%s\n", dev_bitset_tostring( c ) );
    printf( "%d\n", dev_bitset_cardinality( c ) );

    if ( ! dev_bitset_equals( b, c ) )
      dev_die( "dev_bitset_equals failed" );

    dev_free_bitset( c );
  }

  printf( "done\n" );
}

/*****************************************************************
 * main - main program                                           *
 *****************************************************************/

int
main( void )
{
  char *buffer;

  dev_init();

  banner();

  printf( "\n" );

  buffer = dev_malloc( 1000000 );

  dev_free( buffer );

  dev_set_debug_level( 4 );

  buffer = dev_malloc( 1000000 );

  dev_free( buffer );

  printf( "\n" );

  dev_set_debug_level( 2 );

  vector_test();

  list_test();

  bitset_test();

  exit( EXIT_SUCCESS );
}

