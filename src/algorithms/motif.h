/*                               -*- Mode: C -*- 
 * motif.h --- secondary structure expressions and motifs
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Thu Jul  7 14:11:27 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Fri Aug  5 16:14:39 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 */

#ifndef MOTIF_H
#define MOTIF_H

#include "libdev.h"
#include "bitset.h"
#include "libvtree.h"
#include "motif.h"
#include "seed.h"

/*****************************************************************
 * element_enum_t                                                *
 *****************************************************************/

typedef enum element { left, right, unpaired, range } element_t;

/*****************************************************************
 * secondary structure expression                                *
 *****************************************************************/

typedef struct expression_s {
  element_t type;
  pos_t start;
  pos_t length;
  pos_t range;
  dstring_t *dstring; /* shared! */
  bitset_t *mask;
  int mismatch;  
  struct expression_s *nested;
  struct expression_s *adjacent;
} expression_t;

/*****************************************************************
 * secondary structure motif                                     *
 *****************************************************************/

typedef struct {
  expression_t *expression;
  int num_fixed_pos;
  int num_stem;
  int next;
  float support;
} motif_t;

/*****************************************************************
 * match_t                                                       *
 *****************************************************************/

typedef struct {
  int id;
  pos_t offset;
  pos_t length;
  char *sequence;
  char *structure;
} match_t;

/*****************************************************************
 * Interface                                                     *
 *****************************************************************/

extern motif_t *new_stem_motif( int i, int j, int length, int m, dstring_t *ds );

extern void free_motif( motif_t *m );

extern motif_t *clone_motif( motif_t *m );

extern motif_t *combine( motif_t *a, motif_t *b );

extern list_t *match( vtree_t *v, motif_t *m, int save_all, param_t *params );

extern int occurs( vtree_t *v, motif_t *m, param_t *params );

extern void free_match( match_t *m );

extern int motif_num_base_pair( motif_t *m );

extern void motif_to_string( motif_t *m, char **sbuf, char **bbuf );

extern int stem_within( motif_t *a, motif_t *b );

extern int motif_is_equivalent( motif_t *a, motif_t *b );

extern void report_motif( motif_t *m );

extern void save_matches( vector_t *ms, vector_t *vs, param_t *params );

extern void save_matches_as_ct( vector_t *ms, vector_t *vs, param_t *params );

extern void save_motifs( vector_t *ms, vector_t *vs, param_t *params );

#endif
