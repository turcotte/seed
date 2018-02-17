/*                               -*- Mode: C -*- 
 * ivector.h --- Truong Nguyen and growable array of pos_t values
 * Author          : Marcel Turcotte
 * Created On      : Wed Jun 22 17:23:15 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Wed Jul 13 15:46:05 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 */

#ifndef IVECTOR_H
#define IVECTOR_H

#include "libdev.h"

/*****************************************************************
 * Constants                                                     *
 *****************************************************************/

#define DEV_IVECTOR_CAPACITY_INCREMENT 10
#define DEV_IVECTOR_INITIAL_CAPACITY 10

/*****************************************************************
 * ivector_t - growable array of index values                    *
 *****************************************************************/

typedef struct {
  pos_t *elems;
  int capacity;
  int increment;
  int count;
} ivector_t;

/*****************************************************************
 * Interface (exported)                                          *
 *****************************************************************/

extern ivector_t *dev_new_ivector();

extern ivector_t *dev_new_ivector2( int c, int i );

extern int dev_ivector_size( ivector_t *xs );

extern int dev_ivector_is_empty( ivector_t *xs );

extern pos_t dev_ivector_get( ivector_t *xs, int index );

extern pos_t dev_ivector_get_first( ivector_t *xs );

extern pos_t dev_ivector_get_last( ivector_t *xs );

extern void dev_ivector_set( ivector_t *xs, int index, pos_t elem );

extern void dev_ivector_add( ivector_t *xs, pos_t elem );

extern pos_t dev_ivector_remove( ivector_t *xs );

extern pos_t dev_ivector_serve( ivector_t *xs );

extern pos_t *dev_ivector_to_array( ivector_t *xs );

extern void dev_free_ivector( ivector_t *xs );

extern void dev_ivector_trim( ivector_t *xs );

#endif
