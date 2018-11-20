/*                               -*- Mode: C -*- 
 * tests.c --- tests driver
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Thu Jun 16 11:25:30 2005
 * Last Modified By: turcotte
 * Last Modified On: Tue Nov 20 09:16:25 2018
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 */

#include "libdev.h"
#include "vector.h"
#include "libvtree.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

/*****************************************************************
 * alphabet - lower case letters alphabet                        *
 *****************************************************************/

static code_t codes[] =
  { { '*',  0 },
    { 'a',  1 },
    { 'b',  2 },
    { 'c',  3 },
    { 'd',  4 },
    { 'e',  5 },
    { 'f',  6 },
    { 'g',  7 },
    { 'h',  8 },
    { 'i',  9 },
    { 'j', 10 },
    { 'k', 11 },
    { 'l', 12 },
    { 'm', 13 },
    { 'n', 14 },
    { 'o', 15 },
    { 'p', 16 },
    { 'q', 17 },
    { 'r', 18 },
    { 's', 19 },
    { 't', 20 },
    { 'u', 21 },
    { 'v', 22 },
    { 'w', 23 },
    { 'x', 24 },
    { 'y', 25 },
    { 'z', 26 } };

static alphabet_t lowercase = { codes, 27, 27 };

/*****************************************************************
 * banner - displays the name of the library                     *
 *****************************************************************/

static void
banner()
{
  printf( "* libvtree - tests driver *\n\n" );
}

/*****************************************************************
 * is_wild_card -                                                *
 *****************************************************************/

static inline int
is_wild_card( symbol_t a )
{
  return a == 0;
}

/*****************************************************************
 * trivial_cmp -                                                 *
 *****************************************************************/

static inline int
trivial_cmp( symbol_t a, symbol_t b )
{
  return ( is_wild_card( a ) || is_wild_card( b ) || a == b );
}

/*****************************************************************
 * display - prints out the various tables                       *
 *****************************************************************/

static void
display( char *s, dstring_t *ds, vtree_t *v )
{
  printf( "\n" );
  printf( "       " );
  for ( int i=0; i<v->length; i++ )
    printf( " %2d ", i );
  printf( "\n" );

  printf( "s    = " );
  for ( int i=0; i<ds->length; i++ )
    if ( dev_isspecial( &lowercase, ds->text[ i ] ) )
      printf( "[ $]" );
    else
      printf( "[ %c]", dev_decode( &lowercase, ds->text[ i ] ) );
  printf( "\n" );

  printf( "ds   = " );
  for ( int i=0; i<ds->length; i++ )
    printf( "[%2d]", ds->text[ i ] );
  printf( "\n" );

  printf( "sa   = " );
  for ( int i=0; i<v->length; i++ )
    printf( "[%2d]", v->suftab[ i ] );
  printf( "\n" );

  printf( "ra   = " );
  for ( int i=0; i<v->length; i++ )
    printf( "[%2d]", v->isuftab[ i ] );
  printf( "\n" );

  printf( "lcp  = " );
  for ( int i=0; i<v->length; i++ )
    printf( "[%2d]", v->lcptab[ i ] );
  printf( "\n" );

  printf( "bw   = " );
  for ( int i=0; i<v->length; i++ )
    printf( "[%2d]", v->bwtab[ i ] );
  printf( "\n" );

  printf( "up   = " );
  for ( int i=0; i<v->length; i++ )
    printf( "[%2d]", v->childtab[ i ].up );
  printf( "\n" );

  printf( "down = " );
  for ( int i=0; i<v->length; i++ )
    printf( "[%2d]", v->childtab[ i ].down );
  printf( "\n" );

  printf( "next = " );
  for ( int i=0; i<v->length; i++ )
    printf( "[%2d]", v->childtab[ i ].next );
  printf( "\n" );

}

/*****************************************************************
 * display2 - prints out the various tables                      *
 *****************************************************************/

static void
display2( char *s, dstring_t *ds, vtree_t *v )
{
  printf( "\n" );

  printf( "      s  ds  sa  ra lcp  bw   ^   v   > \n" );

  for ( int i=0; i<ds->length; i++ ) {

    char c;

    if  ( dev_isspecial( &lowercase, ds->text[ i ] ) )
      c = '$';
    else
      c = dev_decode( &lowercase, ds->text[ i ] );

    printf( " %2d ", i );
    printf( "[%2c]", c );
    printf( "[%2d]", ds->text[ i ] );
    printf( "[%2d]", v->suftab[ i ] );
    printf( "[%2d]", v->isuftab[ i ] );
    printf( "[%2d]", v->lcptab[ i ] );
    printf( "[%2d]", v->bwtab[ i ] );
    printf( "[%2d]", v->childtab[ i ].up );
    printf( "[%2d]", v->childtab[ i ].down );
    printf( "[%2d]", v->childtab[ i ].next );

    printf( " " );

    for ( int j=v->suftab[ i ]; j<v->length; j++ ) {

      if  ( dev_isspecial( &lowercase, ds->text[ j ] ) )
	c = '$';
      else
	c = dev_decode( &lowercase, ds->text[ j ] );

      printf( "%c", c );

    }
    
    printf( "\n" );

  }
}

/*****************************************************************
 * print_s - prints a symbols string                             *
 *****************************************************************/

static void
print_s( symbol_t *s, int n )
{
  printf( "s = " );
  for ( int i=0; i<n; i++ )
    printf( "[%2d]", s[ i ] );
  printf( "\n" );
}

/*****************************************************************
 * isPermutation - checks if SA is a permutation                 *
 *****************************************************************/

static int
isPermutation( pos_t *SA, int n ) 
{
  int perm = TRUE;

  int *seen = ( int * ) dev_malloc( n * sizeof( int ) );

  for ( int i=0; i<n; i++ ) 
    seen[ i ] = 0;

  for ( int i=0; i<n; i++ ) 
    seen[ SA[ i ] ] = 1;

  for ( int i=0; i<n && perm; i++ ) 
    if ( !seen[ i ] ) 
      perm = FALSE;

  free( seen );

  return perm;
}

/*****************************************************************
 * isRank - checks if ra is the inverse suffix array             *
 *****************************************************************/

static int
isRank( pos_t *ra, pos_t *SA, int n ) 
{
  for ( int i=0; i<n; i++ )
    if ( ra[ SA[ i ] ] != i )
      return 0;

  return 1;
}

/*****************************************************************
 * sleq - string less than or equals                             *
 *****************************************************************/

static int
sleq( symbol_t *s1, symbol_t *s2 ) {
  if ( s1[ 0 ] < s2[ 0 ] ) 
    return 1;

  if ( s1[ 0 ] > s2[ 0 ] )
    return 0;

  return sleq( s1+1, s2+1 );
} 

/*****************************************************************
 * isSorted - returns true if SA is the suffix array of S        *
 *****************************************************************/

static int
isSorted( pos_t *SA, symbol_t *s, int n ) {

  for ( int i = 0;  i < n-1;  i++ )
    if ( !sleq( s+SA[i], s+SA[i+1] ) )
      return 0;

  return 1;  
}

/*****************************************************************
 * generate_and_test - generates and tests all the strings of    *
 * length 2 to 8 over a 4 letter alphabet.                       *
 *****************************************************************/

static void
generate_and_test() {

  int nmax = 8, b = 4;

  dev_log( 0, "testing all the strings of length 2..8 over a 4 letters alphabet" );

  for ( int n=2; n<=nmax;  n++) {

    int N = ( int ) ( pow( ( double) b, (double) n ) + 0.5 );

    symbol_t *s = ( symbol_t * ) dev_malloc( ( n+3 ) * sizeof( symbol_t ) );

    for ( int i=0;  i<n;  i++ ) 
      s[ i ] = 1;

    s[ n ] = s[ n+1 ] = s[ n+2 ] = 0;

    for ( int j=0; j<N;  j++ ) {

      vtree_t *v;
      dstring_t ds;

      ds.text = s;
      ds.length = n;
      ds.alphabet = &lowercase;

      v = vtree_create( &ds );

      assert( isSorted( v->suftab, v->text, n ) );
      assert( isPermutation( v->suftab, n ) );
      assert( isRank( v->isuftab, v->suftab, n ) );

      vtree_free( v );

      int i; // Generate next s
      for ( i=0; s[ i ]==b; i++) 
	s[ i ]=1;
      s[ i ]++;
    }

    dev_free( s );
  }
  dev_log( 0, "done!" );
}

/*****************************************************************
 * f3 - a function applied to all the interior nodes of the vtree*
 *****************************************************************/

static void
f3( vtree_t *v, interval3_t *i )
{
  printf( "node <%d,%d,%d>\n", i->lcp, i->lb, i->rb );
}

/*****************************************************************
 * f4 - a function applied to all the interior nodes of the vtree*
 *****************************************************************/

static void
f4( vtree_t *v, interval4_t *i )
{
  printf( "node <%d,%d,%d, [ ...(%d)... ]>\n", i->lcp, i->lb, i->rb, dev_vector_size( i->childList ) );
}

/*****************************************************************
 * traverse_topdown -                                            *
 *****************************************************************/

static void
traverse_topdown( vtree_t *v, interval2_t *i0 )
{
  vector_t *childs;

  printf( "node <%d, %d>\n", i0->i, i0->j );

  if ( i0->i == i0->j )
    return;

  childs = vtree_getChildIntervals( v, i0 );

  for ( int k=0; k < dev_vector_size( childs ); k++ ) {

    interval2_t *child = ( interval2_t * ) dev_vector_get( childs, k );

    traverse_topdown( v, child );

    dev_free( child );

  }

  dev_free_vector( childs, free );

}

/*****************************************************************
 * main - main program                                           *
 *****************************************************************/

int
main( void )
{
  char *s1 = "mississippi", *s2 = "acaaacatat";
  dstring_t *ds, *pattern;
  vtree_t *v;
  vector_t *rs, *nodes;
  int n;

  dev_init();

  banner();

  generate_and_test();

  ds = dev_digitalize( &lowercase, s1 );
  v = vtree_create( ds );
  display2( s1, ds, v );

  vtree_free( v );

  ds = dev_digitalize( &lowercase, s2 );
  v = vtree_create( ds );
  display2( s2, ds, v );

  printf( "\n" );
  vtree_traverse_and_process( v, f4 );

  printf( "\n" );
  rs = vtree_findall_smax_repeats( v );
  for ( int i=0; i < dev_vector_size( rs ); i++ ) {
    interval2_t *interval = dev_vector_get( rs, i );
    printf( "repeat <%d,%d>\n", interval->i, interval->j );
  }

  /* 
  
  printf( "\n:: top down\n" );

  traverse_topdown( v, new_interval2( 0, v->length ) );

  */

  printf( "\n:: exact match\n" );

  n = strlen( s2 );
  for ( int p=0; p<n; p++ )
    for ( int l=1; l<(n-p); l++ ) {
      char *buffer;

      buffer = dev_malloc( n-p+1 );
      for ( int i=0; i<l; i++ )
	buffer[ i ] = s2[ p+i ];
      buffer[ l ] = '\0';

      printf( "input = %s\n", buffer );
      pattern= dev_digitalize( &lowercase, buffer );
      pattern->length =   pattern->length - 1;
      vtree_find_exact_match( v, pattern );
    }

  printf( "input = %s\n", "cac" );
  pattern= dev_digitalize( &lowercase, "cac" );
  pattern->length =   pattern->length - 1;
  vtree_find_exact_match( v, pattern );

  vtree_free( v );

  printf( "\n" );

  exit( EXIT_SUCCESS );
}
