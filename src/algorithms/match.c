/*                               -*- Mode: C -*- 
 * match.c --- pattern matching routines
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Fri Jun 24 15:35:55 2005
 * Last Modified By: turcotte
 * Last Modified On: Wed Feb 21 13:29:25 2018
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 *
 * This program is actually not part of Seed.  It was created as a
 * proof of concept for the development of a suffix array based RNA
 * secondary structure pattern matcher.
 */

#include "libdev.h"
#include "vector.h"
#include "ivector.h"
#include "seq.h"
#include "libvtree.h"

#include <stdlib.h>
#include <time.h>
#include <string.h>

#define LEFT_PAREN '('
#define RIGHT_PAREN ')'
#define SEQ_ELEM '.'

/*****************************************************************
 * pattern_t -                                                   *
 *****************************************************************/

typedef struct {
  symbol_t *sequence;
  char *structure;
  int length;
} pattern_t;

/*****************************************************************
 * report_matches -                                              *
 *****************************************************************/

void
report_matches( vtree_t *v, interval2_t *interval, pattern_t *p, int m )
{
  printf( "query found at:\n" );

  for ( pos_t k=interval->i; k<=interval->j; k++ ) {
    printf( "  pos = %d, seq = ", v->suftab[ k ] );
    for ( pos_t i=0; i<p->length; i++ )
      printf( "%c", bio_nuc_tochar( v->text[ v->suftab[ k ] + i ] ) );
    if ( m > 0 )
      printf( ", mismatch = %d", m );
    printf( "\n" );
  }
}

/*****************************************************************
 * match_sec_struc_edge - recursive function matching a secondary*
 * structure within an edge label.                               *
 *****************************************************************/

static int match_sec_struc_node( vtree_t *v, interval2_t *interval, pattern_t *p, int pos, int mismatch, int m, int count, ivector_t *stack );

int
match_sec_struc_edge( vtree_t *v, interval2_t *interval, pattern_t *p, int pos, int max, int mismatch, int m, int count, ivector_t *stack )
{
  symbol_t a, b, left;
  char s;
  int result = -1;

  if ( pos == p->length ) {

    if ( dev_ivector_size( stack ) != 0 )
      dev_die( "invalid input pattern" );

    if ( count )
      return interval->j - interval->i + 1;
    else
      report_matches( v, interval, p, m );

    return TRUE;
  }

  if ( pos == max )
    return match_sec_struc_node( v, interval, p, pos, mismatch, m, count, stack );

  a = v->text[ v->suftab[ interval->i ] + pos ];
  b = p->sequence[ pos ];

  if ( ister( a ) )
    return FALSE;

  s = p->structure[ pos ];

  switch ( s ) {
  case LEFT_PAREN:

    if ( ( ! bio_nuc_cmp( a, b ) ) && ( ++m > mismatch ) ) {
      result = FALSE;
    } else {
      dev_ivector_add( stack, a );
      result = match_sec_struc_edge( v, interval, p, pos+1, max, mismatch, m, count, stack );
      dev_ivector_remove( stack );
    }
    break;

  case RIGHT_PAREN:
    left = dev_ivector_remove( stack );

    if ( ( ( ! bio_nuc_cmp( a, b ) ) || ( ! bio_nuc_isbp( left, a, TRUE ) ) ) && ( ++m > mismatch ) ) {
      result = FALSE;
    } else {
      result = match_sec_struc_edge( v, interval, p, pos+1, max, mismatch, m, count, stack );
    }
    dev_ivector_add( stack, left );
    break;

  case SEQ_ELEM:
    if ( ( ! bio_nuc_cmp( a, b ) ) && ( ++m > mismatch ) ) {
      result = FALSE;
    } else {
      result = match_sec_struc_edge( v, interval, p, pos+1, max, mismatch, m, count, stack );
    }
    break;

  default:
    dev_die( "unknown structure pattern: %s", s );
  }

  return result;
}

/*****************************************************************
 * match_sec_struc_node - recursive function matching a secondary*
 * structure at an internal node                                 *
 *****************************************************************/

int
match_sec_struc_node( vtree_t *v, interval2_t *interval, pattern_t *p, int pos, int mismatch, int m, int count, ivector_t *stack )
{
  vector_t *childs;
  int queryFound = FALSE;

  childs = vtree_getChildIntervals( v, interval );

  for ( int c=0; c < dev_vector_size( childs ); c++ ) {

    interval2_t *child = ( interval2_t * ) dev_vector_get( childs, c );
    pos_t min = p->length;

    if ( child->i != child->j ) {
      pos_t l = vtree_getlcp( v, child->i, child->j );
      min = MIN( l, p->length );
    }

    if ( match_sec_struc_edge( v, child, p, pos, min, mismatch, m, count, stack ) )
      queryFound = TRUE;

    dev_free( child );

  }

  return queryFound;
}

/*****************************************************************
 * match_sec_struc -                                             *
 * returns true if an occurrence the pattern was found           * 
 *****************************************************************/

int
match_sec_struc( vtree_t *v, char *sequence, char *structure, int mismatch, int count )
{
  interval2_t *i0 = new_interval2( 0, v->length );
  pattern_t pat;
  dstring_t *ds;
  ivector_t *stack = dev_new_ivector();
  int result;

  ds = dev_digitalize( &bio_nuc_alphabet, sequence );

  pat.sequence = ds->text;
  pat.structure = structure;
  pat.length = ds->length-1;

  result = match_sec_struc_node( v, i0, &pat, 0, mismatch, 0, count, stack );

  dev_free_dstring( ds );
  dev_free( i0 );
  dev_free_ivector( stack );

  return result;
}

/*****************************************************************
 * isnuc_or_struc --                                             *
 * returns true if c is a nucleotide or a structural element.    * 
 *****************************************************************/

int
isnuc_or_struc( char c )
{
  return isnuc( c ) || c == '(' || c == ')' || c == '.';
}

/*****************************************************************
 * main -                                                        *
 *****************************************************************/

int
main( int argc, char *argv[] )
{
  dstring_t *dstring;
  vtree_t *vtree;

  clock_t t0, t1;

  char **seq_pat, **seq_db, **desc_pat, **desc_db;

  int num_seqs;

  if ( argc != 3 ) {
    fprintf( stderr, "Usage: match pat.fa db.fa\n" );
    exit( EXIT_FAILURE );
  }

  if ( bio_read_fasta( argv[ 1 ], &seq_pat, &desc_pat, isnuc_or_struc ) != 2 ) {
    fprintf( stderr, "not a valid pattern %s\n", argv[ 1 ] );
    exit( EXIT_FAILURE );
  }    

  ( void ) clock();

  num_seqs = bio_read_fasta( argv[ 2 ], &seq_db, &desc_db, isnuc );

  for ( int i=0; i<num_seqs; i++ ) {

    int result;

    printf( "%s\n", desc_db[ i ] );

    dstring = dev_digitalize( &bio_nuc_alphabet, seq_db[ i ] );
    
    printf( "vtree_create ... " );

    t0 =  clock();

    vtree = vtree_create( dstring );

    t1 = clock();

    printf( "%lf s\n", ( ( double ) ( t1 - t0 ) ) / ( double ) CLOCKS_PER_SEC );

    printf( "match_sec_struc ... " );

    t0 =  clock();

    long iter = 1;

    for ( int j=0; j < iter; j++ )
      result = match_sec_struc( vtree, seq_pat[ 0 ], seq_pat[ 1 ], 0, TRUE );

    t1 = clock();

    printf( "%lf s\n", ( ( double ) ( t1 - t0 ) ) / ( double ) CLOCKS_PER_SEC / ( double ) iter );

    if ( result )
      printf( "a match was found!\n" );

    dev_free_dstring( dstring );

    vtree_free( vtree );

    printf( "\n" );
  }

  dev_free_array( (void **) seq_pat, 2 );
  dev_free_array( (void **) desc_pat, 2 );

  dev_free_array( (void **) seq_db, num_seqs );
  dev_free_array( (void **) desc_db, num_seqs );

  return EXIT_SUCCESS;
}
