/*                               -*- Mode: C -*- 
 * str_types.h --- data types for manipulating strings
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Mon Jun 13 11:52:42 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Tue Jul 19 15:37:40 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 *
 * __References__
 *
 * @article{985389,
 *  author = {Mohamed Ibrahim Abouelhoda and Stefan Kurtz and Enno Ohlebusch},
 *  title = {Replacing suffix trees with enhanced suffix arrays},
 *  journal = {J. of Discrete Algorithms},
 *  volume = {2},
 *  number = {1},
 *  year = {2004},
 *  issn = {1570-8667},
 *  pages = {53--86},
 *  doi = {http://dx.doi.org/10.1016/S1570-8667(03)00065-0},
 *  publisher = {Elsevier Science Publishers B. V.},
 *  address = {Amsterdam, The Netherlands, The Netherlands}
 *  }
 *
 *  See also http://www.zbh.uni-hamburg.de/vmatch
 */

#ifndef LIBVTREE_TYPES_H
#define LIBVTREE_TYPES_H

#include "libdev.h"
#include "vector.h"

/*****************************************************************
 * Child table                                                   *
 *                                                               *
 * The space can be reduced to one field, see Ref.               *
 *****************************************************************/

typedef struct {
  pos_t up;
  pos_t down;
  pos_t next;
} node_t;

/*****************************************************************
 * Enhanced suffix array (vtree)                                 *
 *****************************************************************/

typedef struct {
  pos_t *suftab;  /* suffix array */
  pos_t *isuftab; /* inverse suffix array, suftab^-1, rank */
  pos_t *lcptab;  /* longest common prefix of S_suftab[i-1] and S_suftab[i] */
  symbol_t *bwtab;  /* Burrows and Wheeler transformation */
  node_t *childtab; /* child-table */ 
  symbol_t *text;
  pos_t length;
  pos_t alphabet_size;
  int id;
} vtree_t;

/*****************************************************************
 * Interface                                                     *
 *****************************************************************/

/* construct.c */

extern void vtree_set_id( vtree_t *v, int id );

extern int vtree_get_id( vtree_t *v );

extern vtree_t *vtree_create( dstring_t *text );

extern void vtree_free( vtree_t *vtree );

/* repeats.c */

typedef struct {
  pos_t i;
  pos_t j;
} interval2_t;

extern interval2_t * new_interval2( pos_t i, pos_t j );
extern vector_t *vtree_findall_smax_repeats( vtree_t *v );

/* access.c */

typedef struct {
  pos_t lcp;
  pos_t lb;
  pos_t rb;
} interval3_t;

typedef struct {
  pos_t lcp;
  pos_t lb;
  pos_t rb;
  vector_t *childList;
} interval4_t;

extern void vtree_traverse_with_array( vtree_t *v, void ( *f )( vtree_t *, interval3_t * ) );

extern void vtree_traverse_and_process( vtree_t *v, void ( *f )( vtree_t *, interval4_t * ) );

extern vector_t *vtree_getChildIntervals( vtree_t *v, interval2_t *interval );

extern interval2_t *vtree_getInterval( vtree_t *v, pos_t i, pos_t j, symbol_t a, int ( *cmp )( symbol_t, symbol_t ) );

extern pos_t vtree_getlcp( vtree_t *v, pos_t i, pos_t j );

extern void vtree_find_exact_match( vtree_t *v, dstring_t *p );

/*****************************************************************
 * vtree_get_childtab_up - wrapper returning childtab.up         *
 * vtree_get_childtab_down - wrapper returning childtab.down     *
 * vtree_get_childtab_next - wrapper returning childtab.next     *
 * v : a pointer to an enhanced suffix array                     *
 * i : index into the childtab                                   *
 *                                                               *
 * Future versions may implement the space reduction technique   *
 * proposed by Abouelhoda, replacing up, down and next by a      *
 * a single value. Those wrappers will facilitate this change.   *
 *****************************************************************/

#define vtree_get_childtab_up( v, i ) v->childtab[ i ].up
#define vtree_get_childtab_down( v, i ) v->childtab[ i ].down
#define vtree_get_childtab_next( v, i ) v->childtab[ i ].next

/* lce.c */

extern pos_t vtree_lce( vtree_t *v, pos_t i, pos_t j );

/* debug.c */

extern void vtree_print_tables( alphabet_t *a, vtree_t *v );

#endif
