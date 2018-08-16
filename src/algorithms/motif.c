/*                               -*- Mode: C -*- 
 * motif.c --- secondary structure expressions and motifs
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Thu Jul  7 14:32:49 2005
 * Last Modified By: turcotte
 * Last Modified On: Wed Feb 21 13:40:18 2018
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 */

#include "ivector.h"
#include "list.h"
#include "seq.h"
#include "motif.h"
#include <string.h>
#include <stdio.h>

#ifdef RNALIB
#include  <math.h>
#include  "fold.h"
#include  "fold_vars.h"
#endif

/*****************************************************************
 * get_sym_5_to_3 -                                              *
 *****************************************************************/

symbol_t
get_sym_5_to_3( expression_t *e, int offset )
{
  symbol_t joker = SYM_NUC_N;
  int ioffset;

  switch ( e->type ) {
  case left:

    if ( dev_bitset_get( e->mask, offset ) )
      return e->dstring->text[ e->start + offset ];
    else
      return joker;

  case right:

    ioffset = e->length - offset - 1;
    if ( dev_bitset_get( e->mask, ioffset ) )
      return e->dstring->text[ e->start - e->length + 1 + offset ];
    else
      return joker;

  case range:

    return joker;

  default:
    dev_die( "unknown element %d", e->type );
  }
  
  return -1; /* never reached! */
}

/*****************************************************************
 * new_stem_motif -                                              *
 *****************************************************************/

motif_t *
new_stem_motif( int i, int j, int length, int m, dstring_t *ds )
{
  expression_t *e5, *em, *e3;

  e5 = ( expression_t * ) dev_malloc( sizeof( expression_t ) );
  
  e5->type = left;
  e5->start = i;
  e5->length = length;
  e5->range = 0;
  e5->mask = dev_new_bitset( length );
  e5->dstring = ds;
  e5->mismatch = m;

  em = ( expression_t * ) dev_malloc( sizeof( expression_t ) );
  
  em->type = range;
  em->start = i+length;
  em->length =  j - i - 2 * length + 1;
  em->range = 0;
  em->mask = NULL;
  em->dstring = ds;
  em->mismatch = 0;

  e3 = ( expression_t * ) dev_malloc( sizeof( expression_t ) );
  
  e3->type = right;
  e3->start = j;
  e3->length = length;
  e3->range = 0;
  e3->mask = e5->mask; /* shared! */
  e3->dstring = ds;
  e3->mismatch = m;

  /* connectivity */

  e5->nested = em;
  e5->adjacent = e3;

  em->nested = NULL;
  em->adjacent = e3;

  e3->nested = e5;
  e3->adjacent = NULL;

  motif_t *result = ( motif_t * ) dev_malloc( sizeof( motif_t ) );

  result->expression = e5;
  result->num_fixed_pos = 0;
  result->num_stem = 1;
  result->next = -1;
  result->support = -1.0;

  return result;
}

/*****************************************************************
 * expression_next -                                             *
 *****************************************************************/

expression_t *
expression_next( expression_t *e )
{
  if ( e->type == left )
    return e->nested;

  return e->adjacent;
}

/*****************************************************************
 * free_expression -                                             *
 *****************************************************************/

void
free_expression( expression_t *e )
{

  while ( e != NULL ) {

    expression_t *tmp;

    tmp = e;

    e = expression_next( e );

    if ( tmp->type == left )
      dev_free_bitset( tmp->mask );

    dev_free( tmp );

  }

}

/*****************************************************************
 * free_motif -                                                  *
 *****************************************************************/

void
free_motif( motif_t *m )
{
  free_expression( m->expression );
  dev_free( m );
}

/*****************************************************************
 * clone_expression -                                            *
 *****************************************************************/

expression_t *
clone_expression( expression_t *e, expression_t *old_adj, expression_t *new_adj )
{
  // pre-condition:

  assert( e != NULL );

  expression_t *result = ( expression_t * ) dev_malloc( sizeof( expression_t ) );

  result->type = e->type;
  result->start = e->start;
  result->length = e->length;
  result->range = e->range;
  result->dstring = e->dstring; /* shared! */
  result->mismatch = e->mismatch;

  switch ( e->type ) {
  case left: {

    result->mask = dev_clone_bitset( e->mask );

    expression_t *old_e3 = e->adjacent;
    expression_t *new_e3 = ( expression_t * ) dev_malloc( sizeof( expression_t ) );

    new_e3->type = old_e3->type;
    new_e3->start = old_e3->start;
    new_e3->length = old_e3->length;
    new_e3->range = old_e3->range;
    new_e3->dstring = old_e3->dstring; /* shared! */
    new_e3->mask = result->mask; /* shared */
    new_e3->mismatch = old_e3->mismatch;

    new_e3->nested = result;

    if ( old_e3->adjacent == old_adj )
      new_e3->adjacent = new_adj;
    else
      new_e3->adjacent = clone_expression( old_e3->adjacent, old_adj, new_adj );

    result->nested = clone_expression( e->nested, old_e3, new_e3 );
    result->adjacent = new_e3;

    break;
  }
  case range: {

    if ( e->adjacent == old_adj )
      result->adjacent = new_adj;
    else
      result->adjacent = clone_expression( e->adjacent, old_adj, new_adj );

    break;
  }
  default:
    dev_die( "unknown element %d", e->type );
  }
  return result;
}

/*****************************************************************
 * clone_motif -                                                 *
 *****************************************************************/

motif_t *
clone_motif( motif_t *m )
{
  motif_t *result = ( motif_t * ) dev_malloc( sizeof( motif_t ) );

  result->num_fixed_pos = m->num_fixed_pos;
  result->num_stem = m->num_stem;
  result->next = m->next;
  result->support = m->support;

  result->expression = clone_expression( m->expression, NULL, NULL );

  return result;
}

/*****************************************************************
 * add_match - creates and adds matches to a list                *
 *****************************************************************/

void
add_match( list_t *matches,
	   vtree_t *v,
	   interval2_t *interval,
	   symbol_t *sbuf,
	   char *bbuf,
	   int length,
	   int save_all )
{
  int n = save_all ? ( interval->j - interval->i + 1 ) : 1;

  sbuf[ length ] = bio_nuc_alphabet.size;
  bbuf[ length ] = '\0';

  for ( int k=0; k<n; k++ ) {

    char *sequence, *structure;
    match_t *m;

    sequence = dev_malloc( length+1 );

    for ( int pos=0; pos<length; pos++ )
      sequence[ pos ] = dev_decode( &bio_nuc_alphabet, sbuf[ pos ] );
    sequence[ length ] = '\0';

    structure = dev_malloc( length+1 );

    for ( int pos=0; pos<length; pos++ )
      structure[ pos ] = bbuf[ pos ];
    structure[ length ] = '\0';

    m = ( match_t * ) dev_malloc( sizeof( match_t ) );

    m->offset = v->suftab[ interval->i ];
    m->length = length;
    m->sequence = sequence;
    m->structure = structure;
    m->id = v->id;

    dev_list_add( matches, m );

  }
}

/*****************************************************************
 * Mutually recursive functions                                  *
 *****************************************************************/

static int 
match_node( vtree_t *v, 
	    interval2_t *interval, 
	    expression_t *e, 
	    int pos, int offset, int m, int save_all, int decision_mode,
	    symbol_t *sbuf, char *bbuf, int ibuf,
	    ivector_t *stack, 
	    list_t *matches,
	    param_t *params );

static int 
match_edge( vtree_t *v, 
	    interval2_t *interval, 
	    expression_t *e, 
	    int pos, int offset, int m, int save_all, int decision_mode,
	    symbol_t *sbuf, char *bbuf, int ibuf,
	    ivector_t *stack, 
	    list_t *matches,
	    param_t *params );

/*****************************************************************
 * match_edge - recursive function matching an expression within *
 * an edge label.                                                *
 *****************************************************************/

int
match_edge( vtree_t *v, 
	    interval2_t *interval, 
	    expression_t *e,
	    int pos, int offset, int m, int save_all, int decision_mode,
	    symbol_t *sbuf, char *bbuf, int ibuf,
	    ivector_t *stack, 
	    list_t *matches,
	    param_t *params )
{
  symbol_t a, b, c;
  int result = FALSE;

  /* end of expression? */

  if ( e == NULL ) { 

    if ( dev_ivector_size( stack ) != 0 )
      dev_die( "internal error, invalid expression" );

    if ( ! decision_mode )
      add_match( matches, v, interval, sbuf, bbuf, ibuf, save_all );

    return TRUE;
  }

  /* at an internal node? */

  if ( interval->i != interval->j && pos == vtree_getlcp( v, interval->i, interval->j ) ) {
    return match_node( v, interval, e, pos, offset, m, save_all, decision_mode, sbuf, bbuf, ibuf, stack, matches, params );
  }

  /* else */

  switch ( e->type ) {
  case left:

    if ( offset >= e->length )
      return match_edge( v, interval, e->nested, pos, 0, m, save_all, decision_mode, sbuf, bbuf, ibuf, stack, matches, params );

    a = v->text[ v->suftab[ interval->i ] + pos ];

    if ( a == 0 )
      return FALSE;

    /* reached the end of the input? */

    if ( ister( a ) )
      return FALSE;

    b = get_sym_5_to_3( e, offset );

    if ( dev_isspecial( &bio_nuc_alphabet, b ) ) {
      dev_log( 4, "not a valid expression, contains a terminator" ); /* fix me */
      return FALSE;
    }

    if ( ( ! bio_nuc_cmp( a, b ) ) && ( ++m > params->max_mismatch ) ) {

      result = FALSE;

    } else {

      if ( sbuf != NULL ) {
	sbuf[ ibuf ] = a;
	bbuf[ ibuf ] = '(';
	ibuf++;
      }

      dev_ivector_add( stack, a );

      result = match_edge( v, interval, e, pos+1, offset+1, m, save_all, decision_mode, sbuf, bbuf, ibuf, stack, matches, params );

      dev_ivector_remove( stack );
    }
    break;

  case right:

    if ( offset >= e->length )
      return match_edge( v, interval, e->adjacent, pos, 0, m, save_all, decision_mode, sbuf, bbuf, ibuf, stack, matches, params );

    a = v->text[ v->suftab[ interval->i ] + pos ];

    if ( a == 0 )
      return FALSE;

    /* reached the end of the input? */

    if ( ister( a ) )
      return FALSE;

    b = get_sym_5_to_3( e, offset );

    if ( dev_isspecial( &bio_nuc_alphabet, b ) ) {
      dev_log( 4, "not a valid expression, contains a terminator" ); /* fix me */
      return FALSE;
    }

    c = dev_ivector_remove( stack );

    if ( ( ( ! bio_nuc_cmp( a, b ) ) || ( ! bio_nuc_isbp( c, a, ! params->nogu ) ) ) && ( ++m > params->max_mismatch ) ) {

      result = FALSE;

    } else {

      if ( sbuf != NULL ) {
	sbuf[ ibuf ] = a;
	bbuf[ ibuf ] = ')';
	ibuf++;
      }

      result = match_edge( v, interval, e, pos+1, offset+1, m, save_all, decision_mode, sbuf, bbuf, ibuf, stack, matches, params );
    }

    dev_ivector_add( stack, c );

    break;

  case range:

    if ( offset >= e->length ) {

      result = match_edge( v, interval, e->adjacent, pos, 0, m, save_all, decision_mode, sbuf, bbuf, ibuf, stack, matches, params );

      if ( ( ( ! result ) || ( save_all ) ) && ( offset < e->length + params->range ) ) {

	a = v->text[ v->suftab[ interval->i ] + pos ];

	if ( sbuf != NULL ) { 
	  sbuf[ ibuf ] = a; 
	  bbuf[ ibuf ] = '.'; 
	  ibuf++; 
	}
      
	result = match_edge( v, interval, e, pos+1, offset+1, m, save_all, decision_mode, sbuf, bbuf, ibuf, stack, matches, params );

      }

    } else {

      a = v->text[ v->suftab[ interval->i ] + pos ];

      if ( a == 0 )
	return FALSE;

      /* reached the end of the input? */

      if ( ister( a ) )
	return FALSE;

      if ( sbuf != NULL ) { sbuf[ ibuf ] = a; bbuf[ ibuf ] = '.'; ibuf++; }

      result = match_edge( v, interval, e, pos+1, offset+1, m, save_all, decision_mode, sbuf, bbuf, ibuf, stack, matches, params );

    }

    break;

  default:
    dev_die( "unknown element %d", e->type );
  }

  return result;
}

/*****************************************************************
 * match_node - recursive function matching an expression at an  *
 * an internal node.                                             *
 *****************************************************************/

int
match_node( vtree_t *v, 
	    interval2_t *interval, 
	    expression_t *e, 
	    int pos, int offset, int m, int save_all, int decision_mode,
	    symbol_t *sbuf, char *bbuf, int ibuf,
	    ivector_t *stack, 
	    list_t *matches,
	    param_t *params )
{
  vector_t *childs;
  int queryFound = FALSE;

  childs = vtree_getChildIntervals( v, interval );

  while ( dev_vector_size( childs ) > 0 && ( ( ! queryFound ) || save_all ) ) {

    interval2_t *child = ( interval2_t * ) dev_vector_serve( childs );
  
    if ( match_edge( v, child, e, pos, offset, m, save_all, decision_mode, sbuf, bbuf, ibuf, stack, matches, params ) )
      queryFound = TRUE;

    dev_free( child );

  }

  dev_free_vector( childs, dev_free );

  return queryFound;
}

/*****************************************************************
 * match - returns a list of matches                             *
 *****************************************************************/

list_t *
match( vtree_t *v, motif_t *m, int save_all, param_t *params )
{
  interval2_t *i0;
  ivector_t *stack;
  symbol_t *sbuf;
  char *bbuf;
  list_t *matches;

  i0 = new_interval2( 0, v->length );
  sbuf = ( symbol_t * ) dev_malloc( ( v->length + 1 ) * sizeof( symbol_t ) );
  bbuf = ( char * ) dev_malloc( ( v->length + 1 ) * sizeof( char ) );
  stack = dev_new_ivector();
  matches = dev_new_list();

  ( void ) match_node( v, i0, m->expression, 0, 0, 0, save_all, FALSE, sbuf, bbuf, 0, stack, matches, params );

  dev_free( i0 );
  dev_free( sbuf );
  dev_free( bbuf );
  dev_free_ivector( stack );

  return matches;
}

/*****************************************************************
 * occurs - returns true if the input sequence (represented here *
 * by its enhanced suffix array) contains at least one match of  *
 * the motif.                                                    *
 *****************************************************************/

int
occurs( vtree_t *v, motif_t *m, param_t *params )
{
  interval2_t *i0 = new_interval2( 0, v->length );

  ivector_t *stack = dev_new_ivector();

  int result = match_node( v, i0, m->expression, 0, 0, 0, FALSE, TRUE, NULL, NULL, 0, stack, NULL, params );

  dev_free( i0 );
  dev_free_ivector( stack );

  params->match_count++;

  return result;
}

/*****************************************************************
 * free_match -                                                  *
 *****************************************************************/

void
free_match( match_t *m )
{
  assert( m != NULL );
  
  dev_free( m->sequence );
  dev_free( m->structure );
  dev_free( m );
}

/*****************************************************************
 * element_start -                                               *
 *****************************************************************/

static pos_t
element_start( expression_t *e )
{
  assert( e != NULL );

  if ( e->type == left || e->type == range )
    return e->start;
  else if ( e->type == right )
    return e->start - e->length + 1;
  else
    dev_die( "unknown element type: %d", e->type );

  return -1; /* never reached */
}

/*****************************************************************
 * expression_start                                              *
 *****************************************************************/

static pos_t
expression_start( expression_t *e )
{
  assert( e != NULL );

  if ( e->type == left || e->type == range )
    return e->start;
  else if ( e->type == right )
    return e->nested->start; /* start of left hand side */
  else
    dev_die( "unknown element type: %d", e->type );

  return -1; /* never reached */
}


/*****************************************************************
 * motif_start -                                                 *
 *****************************************************************/

static inline pos_t
motif_start( motif_t *m )
{
  assert( m != NULL );
  return expression_start( m->expression );
}

/*****************************************************************
 * element_end -                                                 *
 *****************************************************************/

static pos_t
element_end( expression_t *e )
{
  pos_t end = -1;

  assert( e != NULL );

  if ( e->type == left || e->type == range )
    return e->start + e->length - 1;
  else if ( e->type == right )
    return e->start;
  else
    dev_die( "unknown element type: %d", e->type );

  return end;
}

/*****************************************************************
 * expression_end -                                              *
 *****************************************************************/

static pos_t
expression_end( expression_t *e )
{
  pos_t end = -1;

  assert( e != NULL );

  while ( e->adjacent != NULL )
    e = e->adjacent;

  if ( e->type == left || e->type == range )
    return e->start + e->length - 1;
  else if ( e->type == right )
    return e->start;
  else
    dev_die( "unknown element type: %d", e->type );

  return end;
}

/*****************************************************************
 * motif_end -                                                   *
 *****************************************************************/

static inline pos_t
motif_end( motif_t *m )
{
  assert( m != NULL );
  return expression_end( m->expression );
}

/*****************************************************************
 * element_before -                                              *
 *****************************************************************/

int
element_before( expression_t *a, expression_t *b )
{
  return element_end( a ) < element_start( b );
}

/*****************************************************************
 * expression_before -                                           *
 *****************************************************************/

int
expression_before( expression_t *a, expression_t *b )
{
  return expression_end( a ) < expression_start( b );
}

/*****************************************************************
 * motif_before -                                                *
 *****************************************************************/

int
motif_before( motif_t *a, motif_t *b )
{
  return motif_end( a ) < motif_start( b );
}

/*****************************************************************
 * expression_append -                                           *
 *****************************************************************/

expression_t *
expression_append( expression_t *a, expression_t *b )
{
  expression_t *ea = clone_expression( a, NULL, NULL );
  expression_t *eb = clone_expression( b, NULL, NULL );

  expression_t *e = ea;

  while ( e->adjacent != NULL )
    e = e->adjacent;

  expression_t *connector = ( expression_t * ) dev_malloc( sizeof( expression_t ) );
  
  connector->type = range;
  connector->start = expression_end( ea ) + 1;
  connector->length =  expression_start( eb ) - expression_end( ea ) - 1;
  connector->range = 0;
  connector->mask = NULL;
  connector->dstring = e->dstring;
  connector->mismatch = 0;

  /* connectivity */

  connector->nested = NULL;
  connector->adjacent = eb;

  e->adjacent = connector;

  return ea;
}

/*****************************************************************
 * motif_append -                                                *
 *****************************************************************/

static motif_t *
motif_append( motif_t *a, motif_t *b )
{
  
  motif_t *result = ( motif_t * ) dev_malloc( sizeof( motif_t ) );

  result->expression = expression_append( a->expression, b->expression );

  result->num_fixed_pos = a->num_fixed_pos + b->num_fixed_pos;
  result->num_stem = a->num_stem + b->num_stem;
  result->next = -1;
  result->support = -1.0;

  return result;
}

/*****************************************************************
 * replace_range_by_stem -                                       *
 *****************************************************************/

static void
replace_range_by_stem( expression_t *previous, expression_t *stem )
{
  assert( previous != NULL );

  expression_t *p;

  if ( previous->type == left )
    p = previous->nested;
  else
    p = previous->adjacent;

  expression_t *adjacent = p->adjacent;

  assert( adjacent != NULL );

  /* left connector */

  expression_t *cleft = ( expression_t * ) dev_malloc( sizeof( expression_t ) );
  
  cleft->type = range;
  cleft->start = element_end( previous ) + 1;
  cleft->length =  expression_start( stem ) - element_end( previous ) - 1;
  cleft->range = 0;
  cleft->mask = NULL;
  cleft->dstring = previous->dstring; /* shared! */
  cleft->mismatch = 0;

  cleft->nested = NULL;
  cleft->adjacent = stem;

  if ( previous->type == left )
    previous->nested = cleft;
  else if ( previous->type == right )
    previous->adjacent = cleft;
  else
    dev_die( "internal erro, two consecutive range expressions" );

  /* right connector */

  expression_t *cright = ( expression_t * ) dev_malloc( sizeof( expression_t ) );
  
  cright->type = range;
  cright->start = expression_end( stem ) + 1;
  cright->length =  element_start( adjacent ) - expression_end( stem ) - 1;
  cright->range = 0;
  cright->mask = NULL;
  cright->dstring = stem->dstring;
  cright->mismatch = 0;

  cright->nested = NULL;
  cright->adjacent = adjacent;

  assert( stem->type == left && stem->adjacent->type == right );
  stem->adjacent->adjacent = cright;

  if ( adjacent->type == range )
    dev_die( "internal error, consecutive range expressions" );

  p->nested = NULL;
  p->adjacent = NULL;

  free_expression( p );
}

/*****************************************************************
 * motif_insert - inserts motif b into motif a. It is assumed    *
 * that motif b consists of a single stem. Therefore, the opera- *
 * tion consists in finding a range expression that can be       *
 * replaced by motif (and suitable connectors).                  *
 *****************************************************************/

static motif_t *
motif_insert( motif_t *a, motif_t *b )
{
  assert( b->num_stem == 1 );

  expression_t *ea = clone_expression( a->expression, NULL, NULL );
  expression_t *eb = clone_expression( b->expression, NULL, NULL );

  expression_t *pa = ea;

  int done = FALSE;

  while ( ! done ) {

    expression_t *next;

    if ( pa->type == left )
      next = pa->nested;
    else
      next = pa->adjacent;

    if ( next == NULL ) {

      free_expression( ea );
      free_expression( eb );

      ea = NULL;
      done = TRUE;

    } else if ( next->type == range && 
		expression_start( eb ) >= element_start( next ) && 
		expression_end( eb ) <= element_end( next ) ) {

      replace_range_by_stem( pa, eb );
      done = TRUE;

    } else if ( ! element_before( next, eb ) ) {

      free_expression( ea );
      free_expression( eb );

      ea = NULL;
      done = TRUE;

    } else if ( pa->type == left && element_before( eb, pa->adjacent ) ) {

      pa = pa->nested;

    } else {

      pa = pa->adjacent;

    }
  }

  if ( ea == NULL )
    return NULL;
    
  motif_t *result = ( motif_t * ) dev_malloc( sizeof( motif_t ) );

  result->expression = ea;

  result->num_fixed_pos = a->num_fixed_pos + b->num_fixed_pos;
  result->num_stem = a->num_stem + b->num_stem;
  result->next = -1;
  result->support = -1.0;

  return result;
}

/*****************************************************************
 * combine -                                                     *
 *****************************************************************/

motif_t *
combine( motif_t *a, motif_t *b )
{
  motif_t *result = NULL;

  if ( motif_before( a, b ) ) {

    result = motif_append( a, b );

  } else if ( motif_before( b, a ) ) {

    result = motif_append( b, a );

  } else {

    result = motif_insert( a, b );

  }

  if ( result != NULL )
    result->next = b->next;

  return result;
}

/*****************************************************************
 * stem_within -                                                 *
 *****************************************************************/

int
stem_within( motif_t *a, motif_t *b )
{
  assert( a->expression->type == left );
  assert( b->expression->type == left );

  int als = element_start( a->expression );
  int ale = element_end( a->expression );

  int ars = element_start( a->expression->adjacent );
  int are = element_end( a->expression->adjacent );

  int bls = element_start( b->expression );
  int ble = element_end( b->expression );

  int brs = element_start( b->expression->adjacent );
  int bre = element_end( b->expression->adjacent );

  return ( als >= bls && ale <= ble && ars >= brs && are <= bre );
}

/*****************************************************************
 * element_is_equivalent - returns true if element b is equi-    *
 * valent to element a (or vice versa).                          *
 *****************************************************************/

/* static int */
/* element_is_equivalent( expression_t *a, expression_t *b ) */
/* { */

/*   if ( a->type != b->type ) */
/*     return FALSE; */

/*   switch ( a->type ) { */
/*   case left: */

/*     if ( ( a->length != b->length ) ) */
/*       return FALSE; */

/*     for ( int offset=0; offset < a->length; offset++ ) */
/*       if ( get_sym_5_to_3( a, offset ) != get_sym_5_to_3( b, offset ) ) */
/* 	return FALSE; */

/*     return TRUE; */

/*   case range: */

/*     if ( ( a->length == b->length ) && ( a->range == b->range ) ) */
/*       return TRUE; */
/*     else */
/*       return FALSE; */

/*   case right: */

/*     return TRUE; /\* left hand side was validated first *\/ */
    
/*   default: */
/*     dev_die( "unknown element %d", a->type ); */
/*   } */

/*   dev_die( "internal error, this statement should never be reached" ); */

/*   return FALSE; */
/* } */

/*****************************************************************
 * expression_is_equivalent - returns true if expression b is    *
 * equivalent to expression a (or vice versa).                   *
 *****************************************************************/

/* static int */
/* expression_is_equivalent( expression_t *a, expression_t *b ) */
/* { */

/*   if ( a == NULL && b == NULL ) */
/*     return TRUE; */
/*   else if ( a == NULL || b == NULL ) */
/*     return FALSE; */

/*   return element_is_equivalent( a, b ) && */
/*     expression_is_equivalent( expression_next( a ), expression_next( b ) ); */
/* } */

/*****************************************************************
 * motif_is_equivalent - returns true if motif b is equivalent   *
 * to motif a (or vice versa).                                   *
 *****************************************************************/

int
motif_is_equivalent( motif_t *a, motif_t *b )
{

  if ( ( a->num_fixed_pos != b->num_fixed_pos ) ||
       ( a->num_stem != b->num_stem ) )
    return FALSE;

  int result;

  /* result = expression_is_equivalent( a->expression, b->expression );
   *
   * The above call does detect cases where two distinct expressions
   * are generating the same list of base pairs.
   *
   * (a)  ((((((.....))))))
   *     
   * (b)  ((((((.....))))))
   *              =
   *      (((...........)))
   *              + 
   *         (((.....)))
   */

  char *a_seq, *a_sec, *b_seq, *b_sec;
  
  motif_to_string( a, &a_seq, &a_sec );
  motif_to_string( b, &b_seq, &b_sec );

  result = ( strcmp( a_seq, b_seq ) == 0 ) && ( strcmp( a_sec, b_sec ) == 0 );

  dev_free( a_seq );
  dev_free( a_sec );
  dev_free( b_seq );
  dev_free( b_sec );

  return result;
}

/*****************************************************************
 * expression_num_base_pair - returns the total number of base   *
 * pairs.                                                        *
 *****************************************************************/

static int
expression_num_base_pair( expression_t *e )
{
  if ( e == NULL )
    return 0;

  int result = expression_num_base_pair( expression_next( e ) );

  if ( e->type == left )
    result += e->length;

  return result;
}

/*****************************************************************
 * motif_num_base_pair - returns the total number of base pairs. *
 *****************************************************************/

int
motif_num_base_pair( motif_t *m )
{
  return expression_num_base_pair( m->expression );
}

/*****************************************************************
 * motif_to_string - returns a string representation of this     *
 * motif were sbuf is filled with the sequence information and   *
 * bbuf is the bracket notation.                                 *
 *                                                               *
 * Note: the caller is responsible for deallocating the memory   *
 * associated with sbuf and bbuf.                                *
 *****************************************************************/

void
motif_to_string( motif_t *m, char **sbuf, char **bbuf )
{
  assert( m->expression != NULL );

  int pos = 0, n = motif_end( m ) - motif_start( m ) + 1;

  char *t = ( char * ) dev_malloc( n+1 );
  char *s = ( char * ) dev_malloc( n+1 );

  expression_t *e = m->expression;

  while ( e != NULL ) {

    switch ( e->type ) {

    case left: 

      for ( int k=0; k<e->length; k++ ) {
	t[ pos ] = dev_decode( &bio_nuc_alphabet, get_sym_5_to_3( e, k ) );
	s[ pos ] = '(';
	pos++;
      }
      e = e->nested;
      break;

    case right:

      for ( int k=0; k<e->length; k++ ) {
	t[ pos ] = dev_decode( &bio_nuc_alphabet, get_sym_5_to_3( e, k ) );
	s[ pos ] = ')';
	pos++;
      }
      e = e->adjacent;
      break;

    case range:

      for ( int k=0; k<e->length; k++ ) {
	t[ pos ] = dev_decode( &bio_nuc_alphabet, get_sym_5_to_3( e, k ) );
	s[ pos ] = '.';
	pos++;
      }
      e = e->adjacent;
      break;

    default:
      dev_die( "unknown element %d", e->type );
    }
  }   

  t[ pos ] = '\0';
  s[ pos ] = '\0';

  *sbuf = t;
  *bbuf = s;
}

/*****************************************************************
 * report_motif -                                                *
 *****************************************************************/

void
report_motif( motif_t *m )
{
  assert( m->expression != NULL );

  dstring_t *ds = m->expression->dstring;

  int n = ds->length;

  char *seq = dev_decode_dstring( ds );

  printf( "%s\n", seq );
   
  char *t = ( char * ) dev_malloc( n+1 );
  char *s = ( char * ) dev_malloc( n+1 );

  for ( int k=0; k<n; k++ ) {
    s[ k ] = ' ';
  }
  s[ n ] = '\0';

  for ( int k=0; k<n; k++ ) {
    t[ k ] = ' ';
  }
  t[ n ] = '\0';

  expression_t *e = m->expression;
  int size = 0, mismatch = 0;

  while ( e != NULL ) {

    switch ( e->type ) {

    case left: 

      for ( int k=0; k<e->length; k++ ) {
	t[ e->start + k ] = dev_decode( &bio_nuc_alphabet, get_sym_5_to_3( e, k ) );
	s[ e->start + k ] = '(';
	size++;
      }

      mismatch += e->mismatch;
      e = e->nested;
      break;

    case right:

      for ( int k=0; k<e->length; k++ ) {
	t[ e->start - e->length + k + 1 ] = dev_decode( &bio_nuc_alphabet, get_sym_5_to_3( e, k ) );
	s[ e->start - e->length + k + 1 ] = ')';
	size++;
      }

      e = e->adjacent;
      break;

    case range:

      for ( int k=0; k<e->length; k++ ) {
	t[ e->start + k ] = dev_decode( &bio_nuc_alphabet, get_sym_5_to_3( e, k ) );
	s[ e->start + k ] = '.';
	size++;
      }

      mismatch += e->mismatch;
      e = e->adjacent;
      break;

    default:
      dev_die( "unknown element %d", e->type );
    }
 }   

  printf( "%s\n%s (%d/%d)\n", t, s, mismatch, size );

  dev_free( t );
  dev_free( s );
  dev_free( seq );
}

/*****************************************************************
 * remove_non_canonical_bp -                                     *
 *****************************************************************/

#ifdef RNALIB

void static
remove_non_canonical_bp( char *seq, char *sec, param_t *params )
{
  int n = strlen( sec ), i = 0;

  ivector_t *stack = dev_new_ivector( n/2, 1 );
  
  while ( i<n ) {

    if ( sec[ i ] == '(' ) {

      dev_ivector_add( stack, i );

    } else if ( sec[ i ] == ')' ) {

      int j = dev_ivector_remove( stack );

      symbol_t a = dev_encode( &bio_nuc_alphabet, seq[ j ] );
      symbol_t b = dev_encode( &bio_nuc_alphabet, seq[ i ] );

      if ( bio_is_char_class( a ) || 
	   bio_is_char_class( b ) || 
	   ( ! bio_nuc_isbp( a, b, ! params->nogu ) ) ) {
	sec[ i ] = sec[ j ] = '.';
      }
    }
    i++;
  }

  if ( dev_ivector_size( stack ) != 0 )
    dev_die( "internal error, unbalanced parentheses" );

  dev_free_ivector( stack );
}

#endif

/*****************************************************************
 * save_matches -                                                *
 *****************************************************************/

void
save_matches( vector_t *ms, vector_t *vs, param_t *params )
{
  dev_log( 1, "[ save_matches ]" );

  FILE *fh;

  fh = dev_fopen( params->match_file, "w" );

  fprintf( fh, "<motifs>\n" );

  for ( int i=0; i < dev_vector_size( ms ); i++ ) {

    motif_t *m = ( motif_t * ) dev_vector_get( ms, i );

    fprintf( fh, "  <motif id=\"%d\">\n", i );

    char *seq, *sec;

    motif_to_string( m, &seq, &sec );

    fprintf( fh, "    <seq>%s</seq>\n", seq );
    fprintf( fh, "    <sec>%s</sec>\n", sec );

    for ( int k=0; k < dev_vector_size( vs ); k++ ) {

      vtree_t *v;
      list_t *matches;

      v = ( vtree_t * ) dev_vector_get( vs, k );

      matches = match( v, m, params->save_all_matches, params );

      assert( ! ( dev_list_size( matches ) == 0 && k == params->seed ) );

      while ( dev_list_size( matches ) > 0 ) {

	match_t *match = ( match_t * ) dev_list_serve( matches );
	float e = 0.0;

#ifdef RNALIB
	{
	  temperature = 37.0;
	  char *structure = dev_strcpy( match->structure );
	  remove_non_canonical_bp( match->sequence, structure, params );
	  e = energy_of_struct( match->sequence, structure );
	}
#endif
	fprintf( fh, "    <match id=\"%d\">\n", k );
	fprintf( fh, "       <offset>%d</offset>\n", match->offset );
	fprintf( fh, "       <energy>%.1f</energy>\n", e );
	fprintf( fh, "       <seq>%s</seq>\n", match->sequence );
	fprintf( fh, "       <sec>%s</sec>\n", match->structure );
	fprintf( fh, "    </match>\n" );

	free_match( match );
      }
      
      dev_free_list( matches, ( void ( * )( void * ) ) free_match );
    }

    dev_free( seq );
    dev_free( sec );

    fprintf( fh, "  </motif>\n" );
  }

  save_params( fh, "  ", params );

  fprintf( fh, "</motifs>\n" );

  fclose( fh );
}

/*****************************************************************
 * create_base_pair_pos_array - returns an array ps such that    *
 * ps[ i ] = j if sec is a valid bracket representation of a     *
 * secondary structure indicating that bases i and j are paired, *
 * and ps[ i ] = -1 otherwise.                                   *
 *****************************************************************/

int *
create_base_pair_pos_array( char *sec )
{
  int n = strlen( sec ), j;
  ivector_t *stack;
  int *ps = ( int * ) dev_malloc( n * sizeof( int ) );

  for ( int p=0; p<n; p++ )
    ps[ p ] = -1;

  stack = dev_new_ivector2( n/2, 1 );

  j=0;

  while ( j<n ) {

    if ( sec[ j ] == '(' ) {

      dev_ivector_add( stack, j );

    } else if ( sec[ j ] == ')' ) {
  
      int i = dev_ivector_remove( stack );

      ps[ i ] = j;
      ps[ j ] = i;

    }

    j++;
  }

  return ps;
}

/*****************************************************************
 * save_match_as_ct -                                            *
 *****************************************************************/

void
save_match_as_ct( match_t *match, vtree_t *v, char *dir, char *name, param_t *params )
{
  FILE *fh;
  int n = v->length - 1, *ps;
  float e = 0.0;

  char *filename = dev_new_filename( dir, name, ".ct" );

  fh = dev_fopen( filename, "w" );

#ifdef RNALIB
  {
    temperature = 37.0;
    char *structure = dev_strcpy( match->structure );
    remove_non_canonical_bp( match->sequence, structure, params );
    e = energy_of_struct( match->sequence, structure );
  }
#endif

  char *sequence = dev_to_string( &bio_nuc_alphabet, v->text );

  char *structure = dev_malloc( n+1 );
  for ( int i=0; i < n; i++ )
    structure[ i ] = '.';
  structure[ n ] = '\0';
  for ( int k=0; k < match->length; k++ )
    structure[ match->offset + k ] = match->structure[ k ];

  ps = create_base_pair_pos_array( structure );

  fprintf( fh, "%5d   dG = %.1f  [initially %7.1f]    %s\n", n, e, e, name );

  for ( int i=0; i<n; i++ )
    fprintf( fh, "%5d %c %5d %5d %5d %5d\n", i+1, sequence[ i ], i, i+2, ( ps[ i ] + 1 ), i+1 );

  dev_free( filename );
  dev_free( sequence );
  dev_free( structure );
  dev_free( ps );

  fclose( fh );
}

/*****************************************************************
 * save_motif -                                                  *
 *****************************************************************/

void
save_motif( motif_t *m, int i, char *dir )
{
  FILE *fh;
  char *seq, *sec, *filename = dev_new_filename( dir, "motif", ".xml" );

  assert( m->expression != NULL );

  motif_to_string( m, &seq, &sec );

  fh = dev_fopen( filename, "w" );
 
  fprintf( fh, "<motif id=\"%d\" offset=\"%d\">\n", i, m->expression->start );
  fprintf( fh, "  <seq>%s</seq>\n", seq );
  fprintf( fh, "  <sec>%s</sec>\n", sec );
  fprintf( fh, "</motif>\n" );
 
  dev_free( seq );
  dev_free( sec );

  fclose( fh );
}

/*****************************************************************
 * make_dirname -                                                *
 *****************************************************************/

#define SECTION_SIZE 10000

static char *
make_dirname( int i, int n, param_t *params )
{
  char dir[ 8 ], *dirname;

  if ( n < SECTION_SIZE ) {

    sprintf( dir, "%06d", i );
    dirname = dev_new_filename( params->destination, dir, NULL );

    if ( dev_isdir( dirname ) )
      dev_log( 3, "[ directory exists %s ]", dirname );
    else
      dev_mkdir_or_die( dirname );

  } else {

    char section_dir[ 3 ], *section_dirname;

    sprintf( section_dir, "%02d", i / SECTION_SIZE );

    section_dirname = dev_new_filename( params->destination, section_dir, NULL );

    if ( ! dev_isdir( section_dirname ) )
      dev_mkdir_or_die( section_dirname );

    sprintf( dir, "%04d", i % SECTION_SIZE );

    dirname = dev_new_filename( section_dirname, dir, NULL );

    if ( dev_isdir( dirname ) )
      dev_log( 3, "[ directory exists %s ]", dirname );
    else
      dev_mkdir_or_die( dirname );

    dev_free( section_dirname );

  }

  return dirname;
}

/*****************************************************************
 * save_matches_as_ct -                                          *
 *****************************************************************/

void
save_matches_as_ct( vector_t *ms, vector_t *vs, param_t *params )
{
  int n = dev_vector_size( ms );

  dev_log( 1, "[ save_matches_as_ct ]" );

  for ( int i=0; i < n; i++ ) {

    char *dirname;

    motif_t *m = ( motif_t * ) dev_vector_get( ms, i );

    dirname = make_dirname( i, n, params );

    save_motif( m, i, dirname ); 

    for ( int k=0; k < dev_vector_size( vs ); k++ ) {

      vtree_t *v;
      list_t *matches;

      v = ( vtree_t * ) dev_vector_get( vs, k );

      matches = match( v, m, params->save_all_matches, params );

      int l = 0;
      while ( dev_list_size( matches ) > 0 ) {

	match_t *match = ( match_t * ) dev_list_serve( matches );
	char name[ 14 ];

	sprintf( name, "%06d-%06d", match->id, l );

	save_match_as_ct( match, v, dirname, name, params );

	free_match( match );

	l++;
      }
      dev_free_list( matches, ( void ( * )( void * ) ) free_match );
    }

    dev_free( dirname );
  }

  FILE *fh;
  char *params_file;
  params_file = dev_new_filename( params->destination, "params", ".xml" );
  fh = dev_fopen( params_file, "w" );
  save_params( fh, "", params );
  fclose( fh );
}

/*****************************************************************
 * save_motifs -                                                 *
 *****************************************************************/

void
save_motifs( vector_t *ms, vector_t *vs, param_t *params )
{
  int n = dev_vector_size( ms );

  dev_log( 1, "[ save_motifs ]" );

  for ( int i=0; i < n; i++ ) {

    motif_t *m = ( motif_t * ) dev_vector_get( ms, i );

    char *dirname;

    dirname = make_dirname( i, n, params );

    save_motif( m, i, dirname ); 

    dev_free( dirname );

  }

  FILE *fh;
  char *params_file;
  params_file = dev_new_filename( params->destination, "params", ".xml" );
  fh = dev_fopen( params_file, "w" );
  save_params( fh, "", params );

  fclose( fh );
}

