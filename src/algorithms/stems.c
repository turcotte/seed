/*                               -*- Mode: C -*- 
 * stems.c --- functions related to the enumeration of stems
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Mon Jul  4 12:29:40 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Fri Aug  5 12:20:20 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 *
 * Things to do:
 * - allowing for consecutive internal mismatches?
 */

#include "libdev.h"
#include "list.h"
#include "seq.h"
#include "libvtree.h"
#include "seed.h"
#include "motif.h"

#include <string.h>

/*****************************************************************
 * make_dpalindrome - creates a digital palindrome, i.e. a       *
 * sequence of symbols consisting of the forward strand, a       *
 * separator, the reverse complement and the terminator.         *
 *****************************************************************/

static dstring_t *
make_dpalindrome( dstring_t *forward )
{
  dstring_t *revcomp, *result;
  
  revcomp = bio_nuc_revcomp( forward );

  result = dev_dstring_append( forward, revcomp );

  dev_free_dstring( revcomp );

  return result;
}

/*****************************************************************
 * get_all_stems - wrapper for vtree_lce allowing for GU pairs   *
 *****************************************************************/

pos_t
get_lce( vtree_t *v, pos_t i, pos_t j, param_t *params )
{
  pos_t lce = 0, ii = i, jj = j, size = 0;
  int done = FALSE, gu_allowed = ! params->nogu, num_gu = 0;

  while ( ! done ) {

    lce = vtree_lce( v, ii, jj );

    symbol_t a = v->text[ ii + lce ];
    symbol_t b = v->text[ jj + lce ];

    /* GU pair implies G matches A in the reverse complement
     * UG pair implies U matches C in the reverse complement
     */

    if ( ( gu_allowed ) && 
	 ( num_gu < params->stem_max_gu ) &&
	 ( ( a == SYM_NUC_G && b == SYM_NUC_A ) || ( a == SYM_NUC_U && b == SYM_NUC_C ) ) ) {

      lce++;
      num_gu++;
      
    } else {

      done = TRUE;

    }

    ii += lce;
    jj += lce;

    size += lce;

  }

  return size;
}

/*****************************************************************
 * find_all_stems -                                              *
 *****************************************************************/

list_t *
find_all_stems( dstring_t *forward, param_t *params )
{
  list_t *motifs = dev_new_list();

  dev_log( 1, "[ find_all_stems ]" );

  int n = forward->length;

  dstring_t *dstring = make_dpalindrome( forward );

  vtree_t *v = vtree_create( dstring );

  pos_t mindist = 2 * params->stem_min_len + params->loop_min_len - 1;

  for ( int i=0; i<( n - mindist ); i++ ) {

    pos_t j0 = params->stem_max_separation == 0 ? n - 2 : MIN( n - 2, i + params->stem_max_separation );

    for ( pos_t j=j0; j-i >= mindist; j-- ) {

      int size = 0, m = 0, ii = i, jj = j, okay = TRUE;

      while ( m <= params->max_mismatch && jj - ii >= mindist && okay ) {

	pos_t offset = 2 * ( n - 1 ) - jj - 1, lce;
      
	lce = get_lce( v, ii, offset, params );

	if ( lce < params->stem_min_len ) {

	  okay = FALSE; /* extension is too short */

	} else {

	  while ( ( jj - lce ) - ( ii + lce ) + 1 < params->loop_min_len )
	    lce--;

	  if ( lce >= params->stem_min_len ) {

	    size = ( ii + lce ) - i;

	    ii = i + size + 1;
	    jj = j - size - 1;

	    m++;

	  } else {

	    okay = FALSE; /* extension is too short */

	  }
	}
      }

      if ( size >= params->stem_min_len ) {

	int min_size = params->skip_keep_longest_stems ? params->stem_min_len : size;

	while ( min_size <= size ) {

	  motif_t *new = new_stem_motif( i, j, min_size, m-1, forward );

	  if ( dev_get_debug_level() >= 2 )
	    report_motif( new );

	  dev_list_add( motifs, new );

	  min_size++;
	}
      }
    }
  }

  vtree_free( v );

  dev_free_dstring( dstring );

  dev_log( 1, "[ size of the motif list is %d ]", dev_list_size( motifs ) );

  return motifs;
}
