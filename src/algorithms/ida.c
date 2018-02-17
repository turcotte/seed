/*                               -*- Mode: C -*- 
 * ida.c --- iterative deepening algorithm
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Mon Jun 13 11:16:27 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Tue Aug  9 18:25:34 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 *
 * Things to do:
 * - save_matches should also save all the parameters
 * - adding new stopping criteria: number of nodes, size, number of motifs, ...
 * - saving ct files
 * - finer control of the motif generation/matching: num_mismatch_per_stem, ...
 * - variable range expressions
 */

#include "libdev.h"
#include "list.h"
#include "vector.h"
#include "libvtree.h"
#include "seq.h"
#include "stems.h"
#include "motif.h"
#include "ida.h"
#include "misc.h"

#include <string.h>

#ifdef __sun
#include <procfs.h>
#include <unistd.h>
#endif

/*****************************************************************
 * make_all_vtrees -                                             *
 *****************************************************************/

vector_t *
make_all_vtrees( char *seqs[], int num_seqs )
{
  vector_t *vs = dev_new_vector( num_seqs, 1 );

  for ( int i=0; i < num_seqs; i++ ) {

    dstring_t *ds = dev_digitalize( &bio_nuc_alphabet, seqs[ i ] );

    vtree_t *v = vtree_create( ds );

    vtree_set_id( v, i );

    dev_vector_add( vs, v );

    dev_free_dstring( ds );
  }

  dev_vector_trim( vs );

  return vs;
}

/*****************************************************************
 * calculate_support -                                           *
 *****************************************************************/

void
calculate_support( motif_t *m, vector_t *vs, param_t *params )
{
  int matches = 0, n = dev_vector_size( vs );

  for ( int i=0; i < n; i++ )
    if ( occurs( ( vtree_t * ) dev_vector_get( vs, i ), m, params ) )
      matches++;

  m->support = ( float ) matches / ( float ) n;

}

/*****************************************************************
 * filter_by_support -                                           *
 *****************************************************************/

list_t *
filter_by_support( list_t *in, vector_t *vs, param_t *params )
{
  list_t *out = dev_new_list();

  dev_log( 1, "[ filter_by_support ]" );

  while ( dev_list_size( in ) > 0 ) {

    motif_t *m = dev_list_serve( in );

    calculate_support( m, vs, params );

    if ( m->support >= params->min_support )
      dev_list_add( out, m );
    else
      free_motif( m );

  }

  dev_log( 1, "[ size of the motif list is %d ]", dev_list_size( out ) );

  return out;
}

/*****************************************************************
 * filter_keep_longest_stems -                                   *
 *****************************************************************/

list_t *
filter_keep_longest_stems( list_t *in, param_t *params )
{
  list_t *out = dev_new_list();

  if ( params->skip_keep_longest_stems ) {

    while ( dev_list_size( in ) > 0 )
      dev_list_add( out, ( motif_t * ) dev_list_serve( in ) );

  } else {

    dev_log( 1, "[ filter_keep_longest_stems ]" );

    while ( dev_list_size( in ) > 0 ) {

      motif_t *m1 = ( motif_t * ) dev_list_serve( in );

      int within = FALSE, num_elems = dev_list_size( in );

      for ( int i=0; i < num_elems && ( ! within ); i++ ) {

	motif_t *m2 = ( motif_t * ) dev_list_serve( in );

	if ( stem_within( m1, m2 ) )
	  within = TRUE;
	else if ( stem_within( m2, m1 ) )
	  free_motif( m2 );
	else
	  dev_list_add( in, m2 );
      }

      if ( within )
	free_motif( m1 );
      else
	dev_list_add( out, m1 );
    }

    dev_log( 1, "[ size of the motif list is %d ]", dev_list_size( out ) );

  }

  return out;
}

/*****************************************************************
 * fix_all - makes new motifs by instantiating generic positions *
 *                                                               *
 * Pre-condition: input consists of single stem motifs.          *
 *****************************************************************/

list_t *
fix_all( list_t *open, vector_t *vs, param_t *params )
{
  list_t *out = dev_new_list();

  while ( dev_list_size( open ) > 0 ) {

    motif_t *m = dev_list_serve( open );
    dev_list_add( out, m );

    if ( m->num_fixed_pos < params->max_fixed_pos ) {

      expression_t *e = m->expression;

      int leftmost = dev_bitset_leftmost_one( e->mask );

      for ( int i = leftmost + 1; i < e->length ; i++ ) {

	motif_t *new = clone_motif( m );

	dev_bitset_set( new->expression->mask, i );

	new->num_fixed_pos++;
	calculate_support( new, vs, params );

	if ( new->support < params->min_support )
	  free_motif( new );
	else if ( new->num_fixed_pos < params->max_fixed_pos && i < ( e->length - 1 ))
	  dev_list_add( open, new );
	else
	  dev_list_add( out, new );

      } /* end of for */
 
    } /* end of if */

  } /* end of while */

  return out;
}

/*****************************************************************
 * fix_all2 - makes new motifs by instantiating generic          *
 * positions. This algorithm differs from fix_all in the         *
 * following ways: 1) returns a vector, rather than a list,      *
 * orders the elements such that all the motifs derived from the *
 * same parent motif are contiguous in the vector, and 3)        *
 * numbers the elements.                                         *
 *                                                               *
 * Pre-condition: input consists of single stem motifs.          *
 *****************************************************************/

vector_t *
fix_all2( list_t *open, vector_t *vs, param_t *params )
{
  vector_t *out = dev_new_vector( 1000, 200 ); /* tune me! */
  list_t *tmp = dev_new_list(), *res = NULL;
  motif_t *m;

  dev_log( 1, "[ fix_all ]" );

  if ( time_limit_exceeded( params ) )
    while ( dev_list_size( open ) > 0 )
      dev_vector_add( out, dev_list_serve( open ) );

  while ( dev_list_size( open ) > 0 ) {

    int first = dev_vector_size( out ), last;

    m = dev_list_serve( open );

    dev_list_add( tmp, m );

    res = fix_all( tmp, vs, params );

    last = first + dev_list_size( tmp );

    while ( dev_list_size( res ) > 0 ) {

      m = dev_list_serve( res );

      m->next = last;

      if ( dev_get_debug_level() >= 2 )
	report_motif( m );

      dev_vector_add( out, m );
    }
    
    if ( time_limit_exceeded( params ) )
      while ( dev_list_size( open ) > 0 )
	dev_vector_add( out, dev_list_serve( open ) );
  }

  if ( res != NULL )
    dev_free_list( res, ( void ( * )( void * ) ) free_motif );

  dev_free_list( tmp, ( void ( * )( void * ) ) free_motif );

  dev_log( 1, "[ size of the motif list is %d ]", dev_vector_size( out ) );

  return out;
}

/*****************************************************************
 * combine_allall - makes new motifs by combining two existing   *
 * motifs.                                                       *
 *****************************************************************/

void
combine_allall( vector_t *motifs, vector_t *vs, param_t *params )
{
  int n = dev_vector_size( motifs ), first = 0, last = n, num_stem = 1;
  int done = params->max_num_stem < 2;

  dev_log( 1, "[ combine_all ]" );

  if ( time_limit_exceeded( params ) )
    done = TRUE;

  while ( ! done ) {

    dev_log( 1, "[ generating all %d stems motifs ]", ( num_stem + 1 ) );
    dev_log( 1, "[ size of the motif list is %d ]", dev_vector_size( motifs ) );

    for ( int i=first; i < last; i++ ) {

      motif_t *current = dev_vector_get( motifs, i );

      for ( int j=current->next; j<n && !done; j++ ) {

	motif_t *new = combine( current, dev_vector_get( motifs, j ) );

	if ( new != NULL ) {

	  calculate_support( new, vs, params );

	  if ( new->support < params->min_support ) {

	    free_motif( new );

	  } else {

	    if ( dev_get_debug_level() >= 2 )
	      report_motif( new );

	    dev_vector_add( motifs, new );
	  }
	}

	if ( time_limit_exceeded( params ) )
	  done = TRUE;
      }
    }

    num_stem++;

    if ( dev_vector_size( motifs ) == last || num_stem == params->max_num_stem ) {

      done = TRUE;

    } else {

      first = last;
      last = dev_vector_size( motifs );

    }
  }

  dev_log( 1, "[ done ]" );
  dev_log( 1, "[ size of the motif list is %d ]", dev_vector_size( motifs ) );
}

/*****************************************************************
 * ida_discover -                                                *
 *****************************************************************/

vector_t *
postprocess( vector_t *in, param_t *params )
{
  int n = dev_vector_size( in );
  vector_t *out = dev_new_vector( 100, 100 );
  char **seqs; /* cached motif string representations */
  char **secs;

  seqs = ( char ** ) dev_malloc( n * sizeof( char * ) );
  secs = ( char ** ) dev_malloc( n * sizeof( char * ) );

  dev_log( 1, "[ postprocess ]" );

  for ( int i=0; i<n; i++ )
    seqs[ i ] = secs[ i ] = NULL;

  for ( int i = ( n-1 ); i >= 0; i-- ) {

    int failed = FALSE;

    motif_t *m = dev_vector_remove( in );

    if ( m->num_stem < params->min_num_stem )
      failed = TRUE;

    if ( motif_num_base_pair( m ) < params->min_base_pair )
      failed = TRUE;

    for ( int j=0; j < i && ( ! failed ) ; j++ ) {

      motif_t *other = dev_vector_get( in, j );

      if ( m->num_fixed_pos == other->num_fixed_pos ) {

	if ( seqs[ i ] == NULL )
	  motif_to_string( m, &seqs[ i ], &secs[ i ] );

	if ( seqs[ j ] == NULL )
	  motif_to_string( other, &seqs[ j ], &secs[ j ] );

	failed = ( strcmp( seqs[ i ], seqs[ j ] ) == 0 ) && ( strcmp( secs[ i ], secs[ j ] ) == 0 );

	if ( failed && dev_get_debug_level() >= 2 ) {
	  dev_log( 2, "[ removing redundant motif ]" );
	  report_motif( m );
	  report_motif( other );
	}

      }

    }

    if ( failed ) {
      free_motif( m );
    } else {
      dev_vector_add( out, m );
    }
  }

  for ( int i=0; i<n; i++ )
    if ( seqs[ i ] != NULL ) {
      dev_free( seqs[ i ] );
      dev_free( secs[ i ] );
    }

  dev_free( seqs );
  dev_free( secs );

  dev_log( 1, "[ size of the motif list is %d ]", dev_vector_size( out ) );

  return out;
}

/*****************************************************************
 * display_statistics -                                          *
 *****************************************************************/

static void
display_statistics( param_t *params )
{
  char *asc_time;

  dev_log( 1, "[ elapsed time %s ]", asc_cpu_time( &asc_time, params ) );

  dev_free( asc_time );

  dev_log( 1, "[ total number of match operations is %ld ]", params->match_count );

#ifdef __sun
  char *msg;
  pstatus_t info;
  FILE * fd;
  ssize_t size;

  fd = fopen( "/proc/self/status", "r" );

  if ( fd == 0 )
    return; /* silently returns */

  size = fread( &info, sizeof( pstatus_t ), 1, fd );

  if ( size == 0 )
    return; /* silently returns */

  dev_log( 1, "[ memory usage is %s ]", dev_asc_memory_usage( &msg, info.pr_brksize ) );
  dev_free( msg );
#endif

#ifdef HAS_MALLINFO
  char *msg;
  dev_log( 1, "[ memory usage is %s ]", dev_asc_memory_usage( &msg, dev_get_memory_usage() ) );
  dev_free( msg );
#endif

}

/*****************************************************************
 * ida_discover -                                                *
 *****************************************************************/

void
ida_discover( char *seqs[], int num_seqs, param_t *params ) 
{
  vector_t *vs, *m3, *m4;
  list_t *m0, *m1, *m2;
  dstring_t *dseed;

  dseed = dev_digitalize( &bio_nuc_alphabet, seqs[ params->seed ] );

  vs = make_all_vtrees( seqs, num_seqs );

  m0 = find_all_stems( dseed, params );

  m1 = filter_by_support( m0, vs, params );

  m2 = filter_keep_longest_stems( m1, params );

  m3 = fix_all2( m2, vs, params );

  combine_allall( m3, vs, params );

  display_statistics( params );

  m4 = postprocess( m3, params );

  if ( params->match_file != NULL )
    save_matches( m4, vs, params );

  if ( params->save_as_ct ) {
    save_matches_as_ct( m4, vs, params );
  } else if ( params->save_motifs ) {
    save_motifs( m4, vs, params );
  }

  /* cleaning up */

  dev_free_dstring( dseed );
  dev_free_vector( vs, ( void ( * )( void * ) ) vtree_free );
  dev_free_list( m0, ( void ( * )( void * ) ) free_motif );
  dev_free_list( m1, ( void ( * )( void * ) ) free_motif );
  dev_free_list( m2, ( void ( * )( void * ) ) free_motif );
  dev_free_vector( m3, ( void ( * )( void * ) ) free_motif );
  dev_free_vector( m4, ( void ( * )( void * ) ) free_motif );
}
