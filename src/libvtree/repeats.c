/*                               -*- Mode: C -*- 
 * repeats.c --- finding repeats
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Sat Jun 18 17:10:53 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Wed Jul 13 15:48:44 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 */

#include "libdev.h"
#include "vector.h"
#include "libvtree.h"

#include <string.h>

/*****************************************************************
 * vtree_findall_smax_repeats - finds all the super maximal      *
 * repeats of the text.                                          *
 *                                                               *
 * Note: Not thourougly tested                                   *
 *****************************************************************/

vector_t *
vtree_findall_smax_repeats( vtree_t *v )
{
  int i=0;
  vector_t *rs = dev_new_vector();

  while ( i < ( v->length-1 ) ) {

    int j = i+1, l = v->lcptab[ j ];

    if ( v->lcptab[ i ] >= l ) {
      i++;
    } else { /* start of an lcp interval? */

      while ( j < ( v->length-1 ) && v->lcptab[ j+1 ] == l )
	j++;

      if ( v->lcptab[ j+1 ] >= l ) {

	i = j;

      } else { /* local maximum */

	int distinct = TRUE;
	size_t size = v->alphabet_size * sizeof( symbol_t );
	symbol_t *seen = ( symbol_t * ) dev_malloc( size );
	memset( seen, FALSE, size );

	for ( int k=i; k<=j && distinct; k++ ) {
	  symbol_t c = v->bwtab[ k ];
	  if ( seen[ c ] )
	    distinct = FALSE;
	  else
	    seen[ c ] = TRUE;
	}

	if ( distinct ) {
	  interval2_t *r = ( interval2_t * ) dev_malloc( sizeof( interval2_t ) );
	  r->i = i;
	  r->j = j;
	  dev_vector_add( rs, r );
	}
	i = j+1;
      }
    }
  }
  dev_vector_trim( rs );
  return rs;
}

/*****************************************************************
 * vtree_findall_mums - finds all maximum unique matches (MUM)   *
 *                                                               *
 *****************************************************************/

void
vtree_findall_mums( vtree_t *v )
{
  dev_die( "vtree_findall_mums: not implemented yet" );
}
