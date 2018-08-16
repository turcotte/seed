/*                               -*- Mode: C -*- 
 * seed.c --- RNA secondary structure motif inference
 * Author          : Truong Nguyen and Marcel Turcotte
 * Created On      : Wed Jun  8 11:00:14 2005
 * Last Modified By: turcotte
 * Last Modified On: Thu Aug 16 08:41:27 2018
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 */

#include "libdev.h"
#include "seq.h"
#include "ida.h"
#include "stems.h"
#include "seed.h"

#include <string.h>
#include <sys/types.h>
#include <time.h>

/*****************************************************************
 * display_banner - displays the version of the program          *
 *****************************************************************/

static void
display_banner( param_t *params )
{
  printf( "Seed %s - RNA secondary structure motif inference\n\n", params->version );

  printf( "Copyright (C) 2003-18 University of Ottawa\n" );
  printf( "All Rights Reserved\n" );
  printf( "\n" );
  printf( "This program is distributed under the terms of the\n" );
  printf( "GNU General Public License. See the source code\n" );
  printf( "for details.\n\n" );
}

/*****************************************************************
 * display_usage - general instructions for running the program  *
 *****************************************************************/

static char usage[]  = "\
Usage: seed [options] file\n\
where file is a FASTA file that contains k input RNA sequences.\n\
\n\
Options:\n\
     --seed <n>                (default 0)\n\
     --stem_min_len <n>        (default 3)\n\
     --stem_max_gu <n>         (default 100)\n\
     --min_num_stem <n>        (default 1)\n\
     --max_num_stem <n>        (default 2)\n\
     --stem_max_separation <n> (default 150)\n\
     --skip_keep_longest_stems (default false)\n\
     --loop_min_len <n>        (default 4)\n\
     --nogu                    (default false)\n\
     --range <n>               (default 1)\n\
     --max_mismatch <n>        (default 2)\n\
     --max_fixed_pos <n>       (default 100)\n\
     --min_base_pair <n>       (default 5)\n\
     --min_support <n>         (default 0.70)\n\
  -t --time_limit <n>          (default 0)\n\
     --save_all_matches        (default false)\n\
     --save_as_ct              (default false)\n\
     --save_motifs             (default false)\n\
  -m --match_file <file>       (no default)\n\
  -d --destination <dir>       (default .)\n\
  -p --print_level <n>         (default 1)\n\
  -q --quiet                   (default false)\n\
  -v --version\n\
  -h --help\n\
\n\
";

static void
display_usage_and_exit()
{
  printf( "%s", usage );
  exit( EXIT_SUCCESS );
}

/*****************************************************************
 * param_init -                                                  *
 *****************************************************************/

static void
param_init( param_t *params )
{
  ( void ) time( &params->start_time );

  params->seed = DEFAULT_SEED;
  params->stem_min_len = STEM_MIN_LEN;
  params->min_num_stem = MIN_NUM_STEM;
  params->max_num_stem = MAX_NUM_STEM;
  params->stem_max_gu = STEM_MAX_GU;
  params->stem_max_separation = STEM_MAX_SEPARATION;
  params->skip_keep_longest_stems = SKIP_KEEP_LONGEST_STEMS;
  params->loop_min_len = LOOP_MIN_LEN;
  params->nogu = NOGU;
  params->range = RANGE;
  params->max_mismatch = MAX_MISMATCH;
  params->max_fixed_pos = MAX_FIXED_POS;
  params->min_base_pair = MIN_BASE_PAIR;
  params->min_support = MIN_SUPPORT;
  params->time_limit = TIME_LIMIT;
  params->save_all_matches = SAVE_ALL_MATCHES;
  params->save_as_ct = SAVE_AS_CT;
  params->save_motifs = SAVE_MOTIFS;
  params->match_file = MATCH_FILE;
  params->destination = DESTINATION;
  params->filename = FILENAME;
  params->print_level = PRINT_LEVEL;

  char *buffer = dev_malloc( 64 );
  sprintf( buffer, "%s [%s]", VERSION, __DATE__ );
  params->version = buffer;

  params->match_count = 0;
}

/*****************************************************************
 * process_argv - process the command line arguments             *
 *****************************************************************/

static void
process_argv( int argc, char *argv[], param_t *params )
{
  int i = 1;

  while ( i < argc ) {

    if ( argv[ i ][ 0 ] != '-' ) {

      if ( params->filename != NULL )
	dev_die( "not a valid argument %s", argv[ i ] );

      params->filename = argv[ i ];

    } else if ( strcmp( "--seed", argv[ i ] ) == 0 ) {

      params->seed = dev_parse_int( argv[ ++i ] );

    } else if ( strcmp( "--stem_min_len", argv[ i ] ) == 0 ) {

      params->stem_min_len = dev_parse_int( argv[ ++i ] );

    } else if ( strcmp( "--stem_max_gu", argv[ i ] ) == 0 ) {

      params->stem_max_gu = dev_parse_int( argv[ ++i ] );

    } else if ( strcmp( "--min_num_stem", argv[ i ] ) == 0 ) {

      params->min_num_stem = dev_parse_int( argv[ ++i ] );

    } else if ( strcmp( "--max_num_stem", argv[ i ] ) == 0 ) {

      params->max_num_stem = dev_parse_int( argv[ ++i ] );

    } else if ( strcmp( "--range", argv[ i ] ) == 0 ) {

      params->range = dev_parse_int( argv[ ++i ] );

    } else if ( strcmp( "--stem_max_separation", argv[ i ] ) == 0 ) {

      params->stem_max_separation = dev_parse_int( argv[ ++i ] );

    } else if ( strcmp( "--skip_keep_longest_stems", argv[ i ] ) == 0 ) {

      params->skip_keep_longest_stems = TRUE;

    } else if ( strcmp( "--loop_min_len", argv[ i ] ) == 0 ) {

      params->loop_min_len = dev_parse_int( argv[ ++i ] );

    } else if ( strcmp( "--nogu", argv[ i ] ) == 0 ) {

      params->nogu = TRUE;

    } else if ( strcmp( "--max_mismatch", argv[ i ] ) == 0 ) {

      params->max_mismatch = dev_parse_int( argv[ ++i ] );

    } else if ( strcmp( "--max_fixed_pos", argv[ i ] ) == 0 ) {

      params->max_fixed_pos = dev_parse_int( argv[ ++i ] );

    } else if ( strcmp( "--min_base_pair", argv[ i ] ) == 0 ) {

      params->min_base_pair = dev_parse_int( argv[ ++i ] );

    } else if ( strcmp( "--min_support", argv[ i ] ) == 0 ) {

      params->min_support = dev_parse_float( argv[ ++i ] );

    } else if ( strcmp( "-t", argv[ i ] ) == 0 || strcmp( "--time_limit", argv[ i ] ) == 0 ) {

      params->time_limit = dev_parse_int( argv[ ++i ] );

    } else if ( strcmp( "--save_all_matches", argv[ i ] ) == 0 ) {

      params->save_all_matches = TRUE;

    } else if ( strcmp( "--save_as_ct", argv[ i ] ) == 0 ) {

      params->save_as_ct = TRUE;

    } else if ( strcmp( "--save_motifs", argv[ i ] ) == 0 ) {

      params->save_motifs = TRUE;

    } else if ( strcmp( "-m", argv[ i ] ) == 0 || strcmp( "--match_file", argv[ i ] ) == 0 ) {

      params->match_file = argv[ ++i ];

    } else if ( strcmp( "-d", argv[ i ] ) == 0 || strcmp( "--destination", argv[ i ] ) == 0 ) {

      params->destination = argv[ ++i ];

      dev_trim_dirname( params->destination );

      if ( ! dev_isdir( params->destination ) )
	dev_die( "no such directory %s", params->destination );

    } else if ( strcmp( "-p", argv[ i ] ) == 0 || strcmp( "--print_level", argv[ i ] ) == 0 ) {

      params->print_level = dev_parse_int( argv[ ++i ] );

    } else if ( strcmp( "-q", argv[ i ] ) == 0 || strcmp( "--quiet", argv[ i ] ) == 0 ) {

      params->print_level = 0;

    } else if ( strcmp( "-v", argv[ i ] ) == 0 || strcmp( "--version", argv[ i ] ) == 0 ) {

      display_banner( params );
      exit( EXIT_SUCCESS );

    } else if ( strcmp( "-h", argv[ i ] ) == 0 || strcmp( "--help", argv[ i ] ) == 0 ) {

      display_usage_and_exit();

    } else {

      dev_die( "not a valid argument %s", argv[ i ] );
    }

    i++;
  }

  /* validating */

  if ( ( params->stem_max_separation != 0 ) &&
       ( params->stem_max_separation < 2 * params->stem_min_len + params->loop_min_len - 1 ) )
    dev_die( "stem_max_separation < 2 * stem_min_len + loop_min_len - 1" );

  if ( params->filename == NULL )
    dev_die( "please specify an input file" );

  if ( params->destination != NULL && ( ! dev_isdir( params->destination ) ) )
    dev_die( "not a valid directory: %s", params->destination );

  dev_set_debug_level( params->print_level );
}

/*****************************************************************
 * save_params -                                                 *
 *****************************************************************/

void
save_params( FILE *fh, const char *indent, param_t *params )
{

  fprintf( fh, "%s<params>\n", indent );

  fprintf( fh, "%s  <param name=\"seed\">%d</param>\n", indent, params->seed );
  fprintf( fh, "%s  <param name=\"stem_min_len\">%d</param>\n", indent, params->stem_min_len );
  fprintf( fh, "%s  <param name=\"stem_max_gu\">%d</param>\n", indent, params->stem_max_gu );
  fprintf( fh, "%s  <param name=\"min_num_stem\">%d</param>\n", indent, params->min_num_stem );
  fprintf( fh, "%s  <param name=\"max_num_stem\">%d</param>\n", indent, params->max_num_stem );
  fprintf( fh, "%s  <param name=\"stem_max_separation\">%d</param>\n", indent, params->stem_max_separation );
  fprintf( fh, "%s  <param name=\"skip_keep_longest_stems\">%d</param>\n", indent, params->skip_keep_longest_stems );
  fprintf( fh, "%s  <param name=\"loop_min_len\">%d</param>\n", indent, params->loop_min_len );
  fprintf( fh, "%s  <param name=\"nogu\">%d</param>\n", indent, params->nogu );
  fprintf( fh, "%s  <param name=\"range\">%d</param>\n", indent, params->range );
  fprintf( fh, "%s  <param name=\"max_mismatch\">%d</param>\n", indent, params->max_mismatch );
  fprintf( fh, "%s  <param name=\"max_fixed_pos\">%d</param>\n", indent, params->max_fixed_pos );
  fprintf( fh, "%s  <param name=\"min_base_pair\">%d</param>\n", indent, params->min_base_pair );
  fprintf( fh, "%s  <param name=\"min_support\">%f</param>\n", indent, params->min_support );
  fprintf( fh, "%s  <param name=\"time_limit\">%d</param>\n", indent, params->time_limit );
  fprintf( fh, "%s  <param name=\"save_all_matches\">%d</param>\n", indent, params->save_all_matches );
  fprintf( fh, "%s  <param name=\"save_as_ct\">%d</param>\n", indent, params->save_as_ct );
  fprintf( fh, "%s  <param name=\"save_motifs\">%d</param>\n", indent, params->save_motifs );

  if ( params->match_file != MATCH_FILE )
    fprintf( fh, "%s  <param name=\"match_file\">%s</param>\n", indent, params->match_file );

  if ( params->destination != DESTINATION )
    fprintf( fh, "%s  <param name=\"destination\">%s</param>\n", indent, params->destination );

  if ( params->filename != FILENAME )
    fprintf( fh, "%s  <param name=\"filename\">%s</param>\n", indent, params->filename );

  fprintf( fh, "%s  <param name=\"version\">%s</param>\n", indent, params->version );

  fprintf( fh, "%s</params>\n", indent );

}

/*****************************************************************
 * main - process arguments, initialization, generate motifs     *
 *****************************************************************/

int
main( int argc, char *argv[] )
{

  int num_seqs;
  char **seqs;
  char **descs;
  param_t params;

  if ( argc == 1 )
    display_usage_and_exit();

  /* initialisations */

  dev_init();

  param_init( &params );

  /* processing arguments */

  process_argv( argc, argv, &params );

  /* reading data */

  num_seqs = bio_read_fasta( params.filename, &seqs, &descs, isnuc );

  assert( params.seed >= 0 && params.seed < num_seqs );

  /* greetings */

  if ( dev_get_debug_level() > 0 )
    display_banner( &params );

  /* running ida motif discovery algorithm */

  ida_discover( seqs, num_seqs, &params );

  /* post-processings */

  dev_free_array( (void **) descs, num_seqs );
  dev_free_array( (void **) seqs, num_seqs );

  exit( EXIT_SUCCESS );
}
