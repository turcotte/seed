/*                               -*- Mode: C -*- 
 * seed.h --- RNA secondary structure motif inference
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Thu Jul  7 13:31:36 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Fri Aug  5 18:01:08 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 */

#ifndef SEED_H
#define SEED_H

#include "libdev.h"

#include <sys/types.h>

#define VERSION "1.0"

/*****************************************************************
 * param_t -                                                     *
 *****************************************************************/

typedef struct {
  int seed;
  int stem_min_len;
  int min_num_stem;
  int max_num_stem;
  int stem_max_gu;
  int stem_max_separation;
  int skip_keep_longest_stems;
  int loop_min_len;
  int nogu;
  int range;
  int max_mismatch;
  int max_fixed_pos;
  int min_base_pair;
  float min_support;
  int time_limit;
  int save_all_matches;
  int save_as_ct;
  int save_motifs;
  char *match_file;
  char *destination;
  char *filename;
  int print_level;
  char *version;
  /* instrumentation
   */
  time_t start_time;
  long match_count;
} param_t;

/*****************************************************************
 * Default values for the parameters                             *
 *****************************************************************/

#define DEFAULT_SEED 0
#define STEM_MIN_LEN 3
#define MIN_NUM_STEM 1
#define MAX_NUM_STEM 2
#define STEM_MAX_GU 100
#define STEM_MAX_SEPARATION 150
#define SKIP_KEEP_LONGEST_STEMS FALSE
#define LOOP_MIN_LEN 4
#define NOGU FALSE
#define RANGE 1
#define MAX_MISMATCH 1
#define MAX_FIXED_POS 100
#define MIN_BASE_PAIR 5
#define MIN_SUPPORT 0.70
#define TIME_LIMIT 0
#define SAVE_ALL_MATCHES FALSE
#define SAVE_AS_CT FALSE
#define SAVE_MOTIFS FALSE
#define MATCH_FILE NULL
#define DESTINATION NULL
#define FILENAME NULL
#define PRINT_LEVEL 1
#define QUIET FALSE

/*****************************************************************
 * Interface                                                     *
 *****************************************************************/

extern void save_params( FILE *fh, const char *indent, param_t *params );

#endif
