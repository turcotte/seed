/*                               -*- Mode: C -*- 
 * stems.h --- functions related to the enumeration of stems
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Mon Jul  4 13:28:09 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Wed Jul 13 15:44:52 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 */

#ifndef STEMS_H
#define STEMS_H

#include "list.h"
#include "seed.h"

extern list_t *find_all_stems( dstring_t *seed, param_t *params );

#endif
