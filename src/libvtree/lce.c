/*                               -*- Mode: C -*- 
 * lce.c --- longest common extension
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Mon Jul  4 14:25:05 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Wed Jul 13 15:48:12 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 *
 * Truong Ngyen's original work included an implementation of lce
 * based on LCA and RMQ.
 *
 * MT: I haven't had the time to integrate this into the new version
 * of Seed.
 */

#include "libdev.h"
#include "libvtree.h"

/*****************************************************************
 * vtree_lce - naive implementation of longest common extentions *
 * algorithm.                                                    *
 * v : input enhanced suffix array                               *
 * i : index                                                     *
 * j : index                                                     *
 *****************************************************************/

pos_t
vtree_lce( vtree_t *v, pos_t i, pos_t j )
{
  pos_t result, min, max, ri = v->isuftab[ i ], rj = v->isuftab[ j ];

  assert( i != j );

  min = MIN( ri, rj );
  max = MAX( ri, rj );

  result = v->lcptab[ min+1 ];

  for ( pos_t k=min+2; k<=max; k++ )
    if ( v->lcptab[ k ] == 0 )
      return 0;
    else if ( v->lcptab[ k ] < result )
      result = v->lcptab[ k ];

  return result;
}
