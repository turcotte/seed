/*                               -*- Mode: C -*- 
 * bitset.h --- implements a vector of bits
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Thu Jul  7 16:08:15 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Wed Jul 13 15:45:53 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 */

#ifndef BITSET_H
#define BITSET_H

/*****************************************************************
 * unit_t -                                                      *
 *****************************************************************/

typedef unsigned int unit_t;

/*****************************************************************
 * bitset_t -                                                    *
 *****************************************************************/

typedef struct {
  int size;       /* number of bits that can bet set */
  int length;     /* number of units (private) */
  unit_t *units;  /* array of units (private) */
} bitset_t;

/*****************************************************************
 * interface                                                     *
 *****************************************************************/

extern bitset_t *dev_new_bitset( int size );

extern bitset_t *dev_clone_bitset( bitset_t *b );

extern void dev_free_bitset( bitset_t *b );

extern int dev_bitset_cardinality( bitset_t *b );

extern int dev_bitset_size( bitset_t *b );

extern int dev_bitset_equals( bitset_t *a, bitset_t *b );

extern void dev_bitset_set( bitset_t *b, int i );

extern int dev_bitset_get( bitset_t *b, int i );

extern int dev_bitset_get( bitset_t *b, int i );

extern void dev_bitset_clear( bitset_t *b, int i );

extern int dev_bitset_rightmost_one( bitset_t *b );

extern int dev_bitset_leftmost_one( bitset_t *b );

extern char * dev_bitset_tostring( bitset_t *b );

#endif

