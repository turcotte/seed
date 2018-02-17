/*                               -*- Mode: C -*- 
 * construct.c --- constructs an enhanced suffix array (aka virtual tree - vtree)
 * Author          : Truong Nguyen and Marcel Turcotte 
 * Created On      : August 2003
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Tue Jul 19 15:36:53 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 *
 * __References__
 *
 * @InProceedings{Karkkainen2003,
 *   author =	 {K\"arkk\"ainen, Juha and Sanders, Peter},
 *   title =	 {Simple Linear Work Suffix Array Construction},
 *   booktitle = {Annual Symposium on Combinatorial Pattern Matching},
 *   pages =	 {943--955},
 *   year =	 2003,
 *   volume =	 2719,
 *   series =	 {Lecture Notes in Computer Science},
 *   address =	 {Berlin},
 *   publisher = {Springer-Verlag},
 *   note = {\url{http://www.springerlink.com/openurl.asp?genre=article&issn=0302-9743&volume=2719&spage=943}}
 * }
 *
 * @InProceedings{Kasai2001,
 *   author =	 {Kasai, Toru and Lee, Gunho and Arimura, Hiroki and
 *               Arikawa, Setsuo and Park, Kunsoo},
 *   title =	 {Linear-Time Longest-Common-Prefix Computation in
 *               Suffix Arrays and Its Applications},
 *   booktitle = {Annual Symposium on Combinatorial Pattern Matching},
 *   pages =	 {181--192},
 *   year =	 2001,
 *   volume =	 2089,
 *   series =	 {Lecture Notes in Computer Science},
 *   address =	 {Berline},
 *   publisher = {Springer-Verlag},
 *   note =      {\url{http://www.springerlink.com/openurl.asp?genre=article&issn=0302-9743&volume=2089&spage=181}}
 * }
 */

#include "libdev.h"
#include "ivector.h"
#include "libvtree.h"

#include <string.h>

/*****************************************************************
 * vtree_set_id -                                                *
 *****************************************************************/

void
vtree_set_id( vtree_t *v, int id )
{
  v->id = id;
}

/*****************************************************************
 * vtree_get_id -                                                *
 *****************************************************************/

int
vtree_get_id( vtree_t *v )
{
  return v->id;
}


/*****************************************************************
 * vtree_init - allocates memory for all the arrays              *
 * dtext :                                                       *
 *****************************************************************/

vtree_t *
vtree_init( dstring_t *dtext )
{
  vtree_t *v;
  int n = dtext->length;

  size_t array_size = ( n + 1 ) 
* sizeof( pos_t ); /* check this */

  v = ( vtree_t * ) dev_malloc( sizeof( vtree_t ) );

  v->suftab = ( pos_t * ) dev_malloc( array_size );
  v->lcptab = ( pos_t * ) dev_malloc( array_size );
  v->isuftab = ( pos_t * ) dev_malloc( array_size );
  v->bwtab = ( symbol_t * ) dev_malloc( n * sizeof( symbol_t ) );
  v->childtab = ( node_t * ) dev_malloc( ( n + 1 ) * sizeof( node_t ) );

  v->text = ( symbol_t * ) dev_malloc( ( n + 3 ) * sizeof( symbol_t ) );
  for ( int i=0; i<n; i++ )
    v->text[ i ] = dtext->text[ i ];

  v->text[ n ] = v->text[ n+1 ] = v->text[ n+2 ] = 0;

  v->length = dtext->length;

  v->alphabet_size = dtext->alphabet->size;

  v->id = -1;

  return v;
}

/*****************************************************************
 * vtree_free - frees the memory for the associated vtree v      *
 *****************************************************************/

void
vtree_free( vtree_t *v )
{
  dev_free( v->suftab );
  dev_free( v->isuftab );
  dev_free( v->lcptab );
  dev_free( v->bwtab );
  dev_free( v->childtab );
  dev_free( v->text );
  dev_free( v );
}

/*****************************************************************
 * create_childtab_updown - compute the up/down values for the   *
 * child-table.                                                  *
 *****************************************************************/

#define push( elem ) dev_ivector_add( stack, elem )
#define pop() dev_ivector_remove( stack )
#define peek() dev_ivector_get_last( stack )

static void 
create_childtab_updown( vtree_t *v )
{
  ivector_t *stack = dev_new_ivector();
  pos_t lastIndex = -1;
  pos_t top = 0;

  for ( pos_t i=0; i <= v->length; i++ ) { /* initialization */
    v->childtab[ i ].up = -1; 
    v->childtab[ i ].down = -1; 
  }

  push( top );

  for ( pos_t i=1; i <= v->length; i++ ) {
    
    while ( v->lcptab[ i ] < v->lcptab[ top ] ) {
      assert( dev_ivector_size( stack ) > 0 );
      lastIndex = pop();
      top = peek();
      if ( v->lcptab[ i ] <= v->lcptab[ top ] && v->lcptab[ top ] != v->lcptab[ lastIndex ] ) {
	v->childtab[ top ].down = lastIndex;
      }
    }

    if ( lastIndex != -1 ) {
      v->childtab[ i ].up = lastIndex;
      lastIndex = -1;
    }

    push( i );
    top = i;
  }

  dev_free_ivector( stack );
}

#undef push
#undef pop
#undef peek

/*****************************************************************
 * create_childtab_next - compute the next l-Index for the       *
 * child-table.                                                  *
 *****************************************************************/

#define push( elem ) dev_ivector_add( stack, elem )
#define pop() dev_ivector_remove( stack )
#define peek() dev_ivector_get_last( stack )

static void
create_childtab_next( vtree_t *v )
{
  ivector_t *stack = dev_new_ivector();
  pos_t lastIndex = -1;
  pos_t top = 0;

  for ( pos_t i=0; i <= v->length; i++ ) { /* initialization */
    v->childtab[ i ].next = -1; 
  }

  push( top );

  for ( pos_t i=1; i <= v->length; i++ ) {
    
    while ( v->lcptab[ i ] < v->lcptab[ top ] ) {
      pop();
      top = peek();
    }

    if ( v->lcptab[ i ] == v->lcptab[ top ] ) {
      lastIndex = pop();
      v->childtab[ lastIndex ].next = i;
    }

    push( i );
    top = i;

  }

  dev_free_ivector( stack );
}

#undef push
#undef pop
#undef peek

/*****************************************************************
 * create_childtab - create the child-table                      *
 *****************************************************************/

static inline void
create_childtab( vtree_t *v )
{
  create_childtab_updown( v );
  create_childtab_next( v );
}

/*****************************************************************
 * create_bw_array - computes the Burrows and Wheeler transfor-  *
 * mation.                                                       *
 *****************************************************************/

static void 
create_bw_array( vtree_t *v )
{
   pos_t i;

   for( i = 0; i < v->length; i++ ) {

      if( v->suftab[ i ] == 0 )
	v->bwtab[ i ] = -1;
      else
	v->bwtab[ i ] = v->text[ v->suftab[ i ] - 1 ];
   }
}

/*****************************************************************
 * create_lcp_array - creates LCP array for adjacent prefixes    *
 *                                                               *
 *****************************************************************/

static void 
create_lcp_array( vtree_t *v )
{
   pos_t i, adjlcp = 0, prev;

   v->lcptab[ 0 ] = 0; /* by definition */
   v->lcptab[ v->length ] = 0;

   for( i = 0; i < v->length; i++ ) {

      if( v->isuftab[ i ] > 0 ) {

         /* Obtain the adjacent (previous) suffix in the suffix array */

         prev = v->suftab[ v->isuftab[ i ] - 1 ];

         /* By Kasai's Theorem 1, only need to start comparing at lcp */

         while( v->text[ i + adjlcp ] == v->text[ prev + adjlcp ] ) {
	   adjlcp++;
         }

         v->lcptab[ v->isuftab[ i ] ] = adjlcp;

         if( adjlcp > 0 ) {
	   adjlcp--;
         }
      }
   }
}

/*****************************************************************
 * leq2 - less than or equal (lexicographic order)               *
 *****************************************************************/

static inline int
leq2( int a1, int a2, int b1, int b2 )
{
  return ( ( a1 < b1 ) || ( a1 == b1 && a2 <= b2 ) );
}

/*****************************************************************
 * leq3 - less than or equal (lexicographic order)               *
 *****************************************************************/

static inline int
leq3( int a1, int a2, int a3, int b1, int b2, int b3 )
{
  return ( ( a1 < b1 ) || ( a1 == b1 && leq2( a2, a3, b2, b3 ) ) );
}

/*****************************************************************
 * radix_pass -                                                  *
 * a : source array                                              *
 * b : destination array                                         *
 * r : key array                                                 *
 * n : length of the arrays                                      *
 * k : number of keys                                            *
 *****************************************************************/

static void
radix_pass( pos_t *a, pos_t *b, pos_t *r, int n, int K )
{

  pos_t c[ K+1 ];

  for ( int i=0; i<=K; i++ ) /* reset counters */
    c[ i ] = 0;

  for ( int i=0; i<n; i++ ) /* count occurrences */
    c[ r[ a[ i ] ] ]++;

  for ( int i=0, sum=0; i<=K; i++ ) { /* exclusive prefix sum */
    int t = c[ i ];
    c[ i ] = sum;
    sum += t;
  }

  for ( int i=0; i<n; i++ ) /* sorting */
    b[ c[ r[ a[ i ] ] ]++ ] = a[ i ];
}

/*****************************************************************
 * create_suffix_array -                                         *
 * s : source array                                              *
 * SA : suffix array                                             *
 * ra : rank array                                               *
 * n : suffix array length                                       *
 * k : number of keys (size of the alphabet)                     *
 *                                                               *
 * Note that this function is invoked recursively.               *
 *****************************************************************/

static void
skew( pos_t *s, pos_t *SA, pos_t *ra, int n, int K )
{
  pos_t n0 = ( n+2 )/3; /* number of mod 0 suffixes */
  pos_t n1 = ( n+1 )/3; /* number of mod 1 suffixes */
  pos_t n2 = n/3;       /* number of mod 2 suffixes */
  pos_t n02 = n0+n2;

  pos_t *s12, *SA12; /* arrays for the mod 1 and mod 2 suffixes */
  pos_t *s0, *SA0; /* arrays for mod 0 suffixes */

  s12 = ( pos_t * ) dev_malloc( ( n02 + 3 ) * sizeof( pos_t ) );
  s12[ n02 ] = s12[ n02+1 ] = s12[ n02+2 ] = 0;

  SA12 = ( pos_t * ) dev_malloc( ( n02 + 3 ) * sizeof( pos_t ) );
  SA12[ n02 ] = SA12[ n02+1 ] = SA12[ n02+2 ] = 0;

  s0 = ( pos_t * ) dev_malloc( n0 * sizeof( pos_t ) );
  SA0 = ( pos_t * ) dev_malloc( n0 * sizeof( pos_t ) );

  /* Generate the positions of the suffixes that are mod 1 and mod 2 */

  for ( int i = 0, j = 0; i < n+( n0-n1 ); i++ ) {
    if ( i%3 != 0 ) {
      s12[ j++ ] = i;
    }
  }

  /* Sort s12 triples based on s[ i+2 ] */
  radix_pass( s12, SA12, s + 2, n02, K );
   
  /* Sort SA12 triples based on s[ i+1 ] */
  radix_pass( SA12, s12, s + 1, n02, K );

  /* Sort s12 triples based on s[ i ] */
  radix_pass( s12, SA12, s, n02, K );

  /* Find the lexicographic names of the triples */

  int name = 0, c0 = -1, c1 = -1, c2 = -1;

  for ( int i=0;  i<n02;  i++ ) {

    if ( s[ SA12[ i ] ]!=c0 || s[ SA12[ i ]+1 ]!=c1 || s[ SA12 [ i ]+ 2 ]!=c2 ) { 
      name++;
      c0 = s[ SA12[ i ] ];
      c1 = s[ SA12[ i ]+1 ];
      c2 = s[ SA12[ i ]+2 ];
    }

    if ( SA12[ i ] % 3 == 1)
      s12[ SA12[ i ]/3 ] = name; /* left half */
    else
      s12[ SA12[ i ]/3 + n0 ] = name; /* right half */

  }

  /* If the highest name assigned is less than the size of the mod 2 
   * suffix array, recursively apply skew. 
   */

  if( name < n02 ) {

    skew( s12, SA12, ra, n02, name );

    for( int i = 0; i < n02; i++ )
      s12[ SA12[ i ] ] = i + 1;

  } else {
      
    /* Generate the suffix array from the name array */
    for( int i=0; i<n02; i++ )
      SA12[ s12[ i ] - 1 ] = i;

  }

  /* Now sort the mod 0 suffixes */

  for( int i=0, j=0; i < n02; i++ ) {
    if( SA12[ i ] < n0 ) {
      s0[ j++ ] = 3*SA12[ i ];
    }
  }

  radix_pass( s0, SA0, s, n0, K );

  /* Merge the sorted SA0 and sorted SA12 suffixes */

  for ( int p = 0, t = n0 - n1, k = 0; k < n; k++ ) {

#define GetI() ( SA12[ t ] < n0 ? SA12[ t ] * 3 + 1 : ( SA12[ t ] - n0 ) * 3 + 2 )

    int i = GetI(); /* pos of current offset 12 suffix */
    int j = SA0[ p ]; /* pos of current offset 0 suffix */

    if ( SA12[ t ] < n0 ?
	leq2( s[ i ], s12[ SA12[ t ] + n0 ], s[ j ], s12[ j/3 ] ) :
	leq3( s[ i ], s[ i + 1 ], s12[ SA12[ t ] - n0 + 1 ], s[ j ], s[ j + 1 ], s12[ j/3 + n0 ] ) ) {
      /* Suffix from SA12 is smaller */
      SA[ k ] = i;
      ra[ i ] = k;
      t++;
      if ( t == n02 ) {	/* done - only SA0 suffixes left */
	for ( k++; p < n0; k++, p++ ) {
	  SA[ k ] = SA0[ p ];
	  ra[ SA0[ p ] ] = k; /* set the rank array too */
	}
      }

    } else { /* Suffix from SA0 is smaller */

      SA[ k ] = j;
      ra[ j ] = k;
      p++;
      
      if ( p == n0 ) { /* done - only SA12 suffixes left */
	for ( k++; t < n02; k++, t++ ) {
	      SA[ k ] = GetI();
	      ra[ GetI() ] = k; /* set the rank array too */
	}
      }
    }
  }

  free( s12 );
  free( SA12 );
  free( SA0 );
  free( s0 );
}

/*****************************************************************
 * create_suffix_array -                                         *
 *****************************************************************/

static inline void
create_suffix_array( vtree_t *v , dstring_t *dtext )
{
  skew( v->text,
	v->suftab,
	v->isuftab,
	dtext->length,
	dtext->alphabet->size );
}

/*****************************************************************
 * vtree_create - creates a suffix array and associated tables   *
 * dtext : a digital string                                      *
 *                                                               *
 * 1. Generate the suffix array and associated data structures   *
 *    based upon the input string;                               *
 *                                                               *
 * 2. Generate a treap and associated data structures based upon *
 *    the adjacent LCP array of the suffix tree;                 *
 *                                                               *
 * 3. Using the treap information, generate all of the rmq data  *
 *    structures required to perform RMQ queries on the adjacent *
 *    LCP array.                                                 *
 *****************************************************************/

vtree_t *
vtree_create( dstring_t *dtext )
{
  vtree_t *v;

  v = vtree_init( dtext );

  create_suffix_array( v, dtext );

  create_lcp_array( v );

  create_bw_array( v );

  create_childtab( v );

  /*
   * TREAP_createTreap( suffInfo->adjLcpArray, suffInfo->len, treapInfo );
   * 
   * TREAP_eulerTour( treapInfo );
   * 
   * RMQ_preProcessData( treapInfo->levelArray, treapInfo->arrayLen,  rmqInfo );
   */

  return v;
}
