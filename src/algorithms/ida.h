/*                               -*- Mode: C -*- 
 * ida.h --- iterative deepening algorithm
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Mon Jun 13 11:21:02 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Wed Jul 13 15:43:26 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 */

#ifndef IDA_H
#define IDA_H

#include "seed.h"

extern void ida_discover( char *seqs[], int num_seqs, param_t *params );

#endif
