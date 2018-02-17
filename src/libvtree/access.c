/*                               -*- Mode: C -*- 
 * access.c --- access functions for an enhanced suffix array (vtree)
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Mon Jun 20 20:09:40 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Fri Jul 15 14:01:07 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 */

#include "libdev.h"
#include "vector.h"
#include "libvtree.h"

/*****************************************************************
 * new_interval3 - allocates and initialises an interval3 struct *
 *****************************************************************/

static inline interval3_t *
new_interval3( pos_t lcp, pos_t lb, pos_t rb )
{
  interval3_t *i = ( interval3_t * ) dev_malloc( sizeof( interval3_t ) );

  i->lcp = lcp;
  i->lb = lb;
  i->rb = rb;

  return i;
}

/*****************************************************************
 * vtree_traverse_with_array - bottom-up traversal of the vtree  *
 * v : enhanced suffix array                                     *
 * f : the function to be applied at each the interior nodes     *
 *                                                               *
 * @InProceedings{Kasai2001,                                     *
 *   author =	 {Kasai, Toru and Lee, Gunho and Arimura, Hiroki *
 *               and Arikawa, Setsuo and Park, Kunsoo},		 *
 *   title =	 {Linear-Time Longest-Common-Prefix Computation  *
 *               in Suffix Arrays and Its Applications},	 *
 *   booktitle = {Annual Symposium on Combinatorial Pattern 	 *
                 Matching},					 *
 *   pages =	 {181--192},					 *
 *   year =	 2001,						 *
 *   volume =	 2089,						 *
 *   series =	 {Lecture Notes in Computer Science},		 *
 *   address =	 {Berline},					 *
 *   publisher = {Springer-Verlag},				 *
 *   note =      {\url{http://www.springerlink.com/openurl.asp?\ *
 *   genre=article&issn=0302-9743&volume=2089&spage=181}}        *
 * }                                                             *
 *****************************************************************/

#define push( elem ) dev_vector_add( stack, elem )
#define pop() dev_vector_remove( stack )
#define peek() dev_vector_get_last( stack )

void
vtree_traverse_with_array( vtree_t *v, void ( *f )( vtree_t *, interval3_t * ) )
{
  vector_t *stack = ( vector_t * ) dev_new_vector( 5, 5 );
  interval3_t *top, *interval = NULL;

  top = new_interval3( 0, 0, -1 );
  push( top );

  for ( pos_t i=1; i<= v->length; i++ ) {

    pos_t lb = i-1;

    while ( v->lcptab[ i ] < top->lcp ) {

      top->rb = i-1;

      if ( interval != NULL )
	dev_free( interval );

      interval = pop();

      f( v, interval );

      top = peek();

      lb = top->lb;
    }

    if ( v->lcptab[ i ] > top->lcp ) {
      top = new_interval3( v->lcptab[ i ], lb, -1 );
      push( top );
    }

  }

  top = pop();
  dev_free( top );

  dev_free_vector( stack, free );
}

#undef push
#undef pop
#undef peek

/*****************************************************************
 * new_interval4 - allocates and initialises an interval4 struct *
 *****************************************************************/

static inline interval4_t *
new_interval4( pos_t lcp, pos_t lb, pos_t rb )
{
  interval4_t *i = ( interval4_t * ) dev_malloc( sizeof( interval4_t ) );

  i->lcp = lcp;
  i->lb = lb;
  i->rb = rb;

  i->childList = dev_new_vector();

  return i;
}

/*****************************************************************
 * vtree_traverse_and_process - bottom-up traversal of the vtree *
 * v : enhanced suffix array                                     *
 * f : the function to be applied at each the interior nodes     *
 *                                                               *
 * __Algorithm 4.4:__                                            *
 * @article{985389,                                              *
 *  author = {Mohamed Ibrahim Abouelhoda and Stefan Kurtz and 	 *
    Enno Ohlebusch},						 *
 *  title = {Replacing suffix trees with enhanced suffix arrays},*
 *  journal = {J. of Discrete Algorithms},			 *
 *  volume = {2},						 *
 *  number = {1},						 *
 *  year = {2004},						 *
 *  issn = {1570-8667},						 *
 *  pages = {53--86},						 *
 *  doi = {http://dx.doi.org/10.1016/S1570-8667(03)00065-0},	 *
 *  publisher = {Elsevier Science Publishers B. V.},		 *
 *  address = {Amsterdam, The Netherlands, The Netherlands}	 *
 *  }								 *
 *****************************************************************/

#define push( elem ) dev_vector_add( stack, elem )
#define pop() dev_vector_remove( stack )
#define peek() dev_vector_get_last( stack )
#define add( list, elem ) dev_vector_add( list, elem )

void
vtree_traverse_and_process( vtree_t *v, void ( *f )( vtree_t *, interval4_t * ) )
{
  vector_t *stack = ( vector_t * ) dev_new_vector( 5, 5 );
  interval4_t *top, *lastInterval = NULL;

  top = new_interval4( 0, 0, -1 );
  push( top );

  for ( pos_t i=1; i <= v->length; i++ ) {

    pos_t lb = i-1;

    while ( v->lcptab[ i ] < top->lcp ) {

      top->rb = i-1;

      if ( lastInterval != NULL )
	dev_free( lastInterval );

      lastInterval = pop();
      top = peek();

      f( v, lastInterval );

      lb = lastInterval->lb;

      if ( v->lcptab[ i ] <= top->lcp ) {
	add( top->childList, lastInterval );
	lastInterval = NULL;
      }

    }

    if ( v->lcptab[ i ] > top->lcp ) {

      top = new_interval4( v->lcptab[ i ], lb, -1 );

      if ( lastInterval != NULL ) {
	add( top->childList, lastInterval );
	lastInterval = NULL;
      }

      push( top );

    }
  }

  top = pop();
  dev_free( top );

  dev_free_vector( stack, free );
}

#undef push
#undef pop
#undef peek
#undef add

/*****************************************************************
 * new_interval2 - allocates and initialises an interval2 struct *
 *****************************************************************/

interval2_t *
new_interval2( pos_t i, pos_t j )
{
  interval2_t *interval = ( interval2_t * ) dev_malloc( sizeof( interval2_t ) );

  interval->i = i;
  interval->j = j;

  return interval;
}

/*****************************************************************
 * vtree_getChildIntervals - returns the child intervals of i,j  *
 * v : enhanced suffix array                                     *
 *                                                               *
 * __Algorithm 6.7:__                                            *
 * @article{985389,                                              *
 *  author = {Mohamed Ibrahim Abouelhoda and Stefan Kurtz and 	 *
    Enno Ohlebusch},						 *
 *  title = {Replacing suffix trees with enhanced suffix arrays},*
 *  journal = {J. of Discrete Algorithms},			 *
 *  volume = {2},						 *
 *  number = {1},						 *
 *  year = {2004},						 *
 *  issn = {1570-8667},						 *
 *  pages = {53--86},						 *
 *  doi = {http://dx.doi.org/10.1016/S1570-8667(03)00065-0},	 *
 *  publisher = {Elsevier Science Publishers B. V.},		 *
 *  address = {Amsterdam, The Netherlands, The Netherlands}	 *
 *  }								 *
 *****************************************************************/

vector_t *
vtree_getChildIntervals( vtree_t *v, interval2_t *i0 )
{
  /* tuned for nucleotides alphabet */
  vector_t *intervalList = dev_new_vector( 5, 5 );
  pos_t i1, i2, val;

  assert( i0->i != i0->j );

  /* special case for 0-[0..n] -- root of the tree */

  if ( i0->j == v->length ) {

    i1 = vtree_get_childtab_next( v, i0->i );
    dev_vector_add( intervalList, new_interval2( i0->i, i1 - 1 ) );

    while ( vtree_get_childtab_next( v, i1 ) != -1 ) {
      i2 = vtree_get_childtab_next( v, i1 );
      dev_vector_add( intervalList, new_interval2( i1, i2 - 1 ) );
      i1 = i2;
    }

    return intervalList;
  }

  /* general case -- internal node */

  val = vtree_get_childtab_up( v, i0->j+1 );

  if ( i0->i < val && val <= i0->j )
    i1 = val;
  else
    i1 = vtree_get_childtab_down( v, i0->i );

  dev_vector_add( intervalList, new_interval2( i0->i, i1 - 1 ) );

  while ( vtree_get_childtab_next( v, i1 ) != -1 ) {
    i2 = vtree_get_childtab_next( v, i1 );
    dev_vector_add( intervalList, new_interval2( i1, i2 - 1 ) );
    i1 = i2;
  }

  dev_vector_add( intervalList, new_interval2( i1, i0->j ) );

  return intervalList;
}

/*****************************************************************
 * vtree_getlcp - returns the lcp-value of an interval           *
 * v : enhanced suffix array                                     *
 *****************************************************************/

pos_t
vtree_getlcp( vtree_t *v, pos_t i, pos_t j )
{
  pos_t val = vtree_get_childtab_up( v, j+1 );

  if ( i < val && val <= j )
    return v->lcptab[ val ];

  return v->lcptab[ vtree_get_childtab_down( v, i ) ];
}

/*****************************************************************
 * trivial_cmp -                                                 *
 *****************************************************************/

static inline int
trivial_cmp( symbol_t a, symbol_t b )
{
  return a == b;
}

/*****************************************************************
 * vtree_getInterval - returns the interval2_t containing a      *
 * v : enhanced suffix array                                     *
 * a : input symbol                                              *
 * cmp : comparator function                                     *
 *****************************************************************/

interval2_t *
vtree_getInterval( vtree_t *v, pos_t i, pos_t j, symbol_t a, int ( *cmp )( symbol_t, symbol_t ) )
{
  pos_t i1, i2, val, l;

  assert( i != j );

  if ( cmp == NULL )
    cmp = trivial_cmp;

  /* special case for 0-[0..n] -- root of the tree */

  if ( j == v->length ) {

    i1 = vtree_get_childtab_next( v, i );

    if ( cmp( v->text[ v->suftab[ i ] ], a ) )
      return new_interval2( i, i1 - 1 );

    while ( vtree_get_childtab_next( v, i1 ) != -1 ) {
      i2 = vtree_get_childtab_next( v, i1 );

      if ( cmp( v->text[ v->suftab[ i1 ] ], a ) )
	return new_interval2( i1, i2 - 1 );

      i1 = i2;
    }

    return NULL;
  }

  /* general case -- internal node */

  val = vtree_get_childtab_up( v, j+1 );
  l = vtree_getlcp( v, i, j );

  if ( i < val && val <= j )
    i1 = val;
  else
    i1 = vtree_get_childtab_down( v, i );

  if ( cmp( v->text[ v->suftab[ i ] + l ], a ) )
    return new_interval2( i, i1 - 1 );

  while ( vtree_get_childtab_next( v, i1 ) != -1 ) {
    i2 = vtree_get_childtab_next( v, i1 );

      if ( cmp( v->text[ v->suftab[ i1 ] + l ], a ) )
	return new_interval2( i1, i2 - 1 );

    i1 = i2;
  }

  if ( cmp( v->text[ v->suftab[ i1 ] + l ], a ) )
    return new_interval2( i1, j );

  return NULL;
}

/*****************************************************************
 * vtree_find_exact_match -                                      *
 * v : enhanced suffix array                                     *
 * p : pattern                                                   *
 *                                                               *
 * __Algorithm 6.8:__                                            *
 * @article{985389,                                              *
 *  author = {Mohamed Ibrahim Abouelhoda and Stefan Kurtz and 	 *
    Enno Ohlebusch},						 *
 *  title = {Replacing suffix trees with enhanced suffix arrays},*
 *  journal = {J. of Discrete Algorithms},			 *
 *  volume = {2},						 *
 *  number = {1},						 *
 *  year = {2004},						 *
 *  issn = {1570-8667},						 *
 *  pages = {53--86},						 *
 *  doi = {http://dx.doi.org/10.1016/S1570-8667(03)00065-0},	 *
 *  publisher = {Elsevier Science Publishers B. V.},		 *
 *  address = {Amsterdam, The Netherlands, The Netherlands}	 *
 *  }								 *
 *****************************************************************/

void
vtree_find_exact_match( vtree_t *v, dstring_t *p )
{
  int c=0;
  int queryFound = TRUE;
  interval2_t *interval;
  pos_t i, j, m = p->length;

  interval = vtree_getInterval( v, 0, v->length, p->text[ c ], NULL );

  while ( ( interval != NULL ) && ( c < m ) && ( queryFound ) ) {

    i = interval->i;
    j = interval->j;

    if ( i != j ) {

      pos_t l = vtree_getlcp( v, i, j );
      pos_t min = MIN( l, m );

      for ( pos_t k=c; k < min && queryFound; k++ )
	if ( v->text[ v->suftab[ i ] + k ] != p->text[ k ] )
	  queryFound = FALSE;

      c = min;

      interval = vtree_getInterval( v, i, j,  p->text[ c ], NULL );

    } else {

      for ( pos_t k=c; k < m && queryFound; k++ )
	if ( v->text[ v->suftab[ i ] + k ] != p->text[ k ] )
	  queryFound = FALSE;
      c = m; /* forces exit */
    }
  }

  if ( ! queryFound ) {

    printf( "pattern P not found" );

  } else {

    printf( "query found a position(s): " );

    for ( pos_t k=i; k<=j; k++ ) {
      if ( k>i )
	printf( ", " );
      printf( "%d", v->suftab[ k ] );
    }
    printf( "\n" );

  }
}

